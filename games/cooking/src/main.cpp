#include "audio.hpp"
#include "demo.hpp"
#include "input.hpp"
#include "renderer.hpp"
#include <csignal>
#include <iostream>
#include <sys/stat.h>
namespace {
volatile std::sig_atomic_t stopping = 0;
void stop(int) { stopping = 1; }
} // namespace
std::string findAssets(const std::string &overridePath) {
  std::vector<std::string> dirs;
  if (!overridePath.empty())
    dirs.push_back(overridePath);
  if (std::getenv("FMG_ASSET_DIR"))
    dirs.push_back(std::getenv("FMG_ASSET_DIR"));
  char *base = SDL_GetBasePath();
  if (base) {
    dirs.push_back(std::string(base) + "../share/cooking");
    SDL_free(base);
  }
  dirs.push_back("assets");
  dirs.push_back("games/cooking/assets");
#ifdef COOKING_ASSET_DIR
  dirs.push_back(COOKING_ASSET_DIR);
#endif
  for (const auto &d : dirs) {
    std::ifstream f(d + "/kitchen.bmp", std::ios::binary);
    if (f)
      return d;
  }
  return dirs.empty() ? "." : dirs[0];
}
int main(int argc, char **argv) {
  using namespace cooking;
  bool smoke = false;
  int preview = 0, previewStage = 0;
  std::string screenshot, assets;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--smoke")
      smoke = true;
    else if (a == "--screenshot" && i + 1 < argc)
      screenshot = argv[++i];
    else if (a == "--flavor" && i + 1 < argc)
      preview = std::atoi(argv[++i]);
    else if (a == "--stage" && i + 1 < argc)
      previewStage = std::atoi(argv[++i]);
    else if (a == "--assets" && i + 1 < argc)
      assets = argv[++i];
  }
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK |
               SDL_INIT_GAMECONTROLLER) != 0) {
    std::cerr << SDL_GetError();
    return 1;
  }
  SDL_InitSubSystem(SDL_INIT_AUDIO);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  SDL_Window *w = SDL_CreateWindow(
      "Cooking", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 272,
      std::getenv("FMG_FULLSCREEN") ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
  SDL_Renderer *r = w ? SDL_CreateRenderer(w, -1,
                                           SDL_RENDERER_ACCELERATED |
                                               SDL_RENDERER_PRESENTVSYNC |
                                               SDL_RENDERER_TARGETTEXTURE)
                      : nullptr;
  if (w && !r)
    r = SDL_CreateRenderer(w, -1,
                           SDL_RENDERER_SOFTWARE | SDL_RENDERER_TARGETTEXTURE);
  if (!r) {
    std::cerr << SDL_GetError();
    SDL_DestroyWindow(w);
    SDL_Quit();
    return 1;
  }
  SDL_RenderSetLogicalSize(r, 480, 272);
  char *pref = SDL_GetPrefPath("FakeMobileGames", "Cooking");
  std::string dir = std::getenv("FMG_SAVE_DIR") ? std::getenv("FMG_SAVE_DIR")
                                                : (pref ? pref : ".");
  SDL_free(pref);
  mkdir(dir.c_str(), 0755);
  std::string save = dir + "/cooking.save";
  Game g;
  bool loaded = !smoke && g.load(save);
  if (smoke)
    g = demo(preview, std::max(0, std::min(5, previewStage)));
  enum Scene { Menu, Tutorial, Playing, Paused, Help, Gallery, Finished, Exit };
  Scene scene = smoke ? (g.s.stage == Photo ? Finished : Playing) : Menu,
        previousScene = Menu;
  int choice = 0, menuFlavor = g.s.flavor, gallery = 0, frames = 0;
  bool running = true, saveError = false, assetError = false;
  std::signal(SIGINT, stop);
  std::signal(SIGTERM, stop);
  {
    Input input;
    Audio audio;
    Renderer render(r, findAssets(assets));
    assetError = render.missing;
    Uint32 before = SDL_GetTicks(), repeat = 0;
    int oldx = 0, oldy = 0;
    unsigned accumulator = 0;
    auto persist = [&]() {
      if (!smoke)
        saveError = !g.save(save);
    };
    auto startRecipe = [&]() {
      g.begin(menuFlavor);
      scene = Tutorial;
      loaded = true;
      persist();
    };
    auto command = [&](Control a) {
      if (a == None)
        return;
      if (a == Back) {
        if (scene == Menu) {
          persist();
          running = false;
        } else {
          previousScene = scene;
          scene = Exit;
        }
        return;
      }
      if (scene == Exit) {
        if (a == A) {
          persist();
          scene = Menu;
          choice = 0;
        } else if (a == B || a == Start)
          scene = previousScene;
        return;
      }
      if (scene == Menu || scene == Paused) {
        if (a == Up)
          choice = (choice + 5) % 6;
        if (a == Down)
          choice = (choice + 1) % 6;
        if (choice == 1 && (a == Left || a == Right))
          menuFlavor = (menuFlavor + (a == Right ? 1 : 2)) % 3;
        if (a == B && scene == Paused) {
          scene = g.s.stage == Photo ? Finished : Playing;
          return;
        }
        if (a == A || a == Start) {
          if (choice == 0) {
            scene = g.s.stage == Photo ? Finished : loaded ? Playing : Tutorial;
            loaded = true;
          } else if (choice == 1)
            startRecipe();
          else if (choice == 2) {
            scene = Gallery;
            gallery = 0;
          } else if (choice == 3) {
            g.muted = !g.muted;
            persist();
          } else if (choice == 4) {
            previousScene = scene;
            scene = Help;
          } else {
            persist();
            running = false;
          }
        }
        return;
      }
      if (scene == Tutorial || scene == Help) {
        if (a == A || a == B || a == Start)
          scene = scene == Tutorial ? Playing : previousScene;
        return;
      }
      if (scene == Gallery) {
        if (a == Left || a == Up)
          gallery = (gallery + std::max(1, g.albumCount) - 1) %
                    std::max(1, g.albumCount);
        if (a == Right || a == Down)
          gallery = (gallery + 1) % std::max(1, g.albumCount);
        if (a == A || a == B) {
          scene = Menu;
          choice = 0;
        }
        return;
      }
      if (scene == Finished) {
        if (a == A) {
          menuFlavor = (g.s.flavor + 1) % 3;
          startRecipe();
        } else if (a == B) {
          scene = Menu;
          choice = 0;
        }
        return;
      }
      if (a == Start || (a == B && g.s.stage != Decorating)) {
        scene = Paused;
        choice = 0;
        persist();
        return;
      }
      if (a == Y) {
        if (g.s.stage == Decorating) {
          g.nextCake();
          persist();
        } else {
          previousScene = Playing;
          scene = Help;
        }
        return;
      }
      if (a == B && g.s.stage == Decorating) {
        bool ok = g.undo();
        if (ok) {
          audio.play(0, g.muted);
          render.response(true);
          persist();
        }
        return;
      }
      bool ok = false, feedback = false;
      int oldPending = g.s.pending;
      if (a == Left || a == Right || a == Down || a == Up) {
        ok = g.direction(a == Right ? 0 : a == Down ? 1 : a == Left ? 2 : 3);
        feedback = g.s.stage == Mixing;
      } else if (a == A) {
        ok = g.action();
        feedback = g.s.pending == Idle || oldPending == Idle;
      }
      if (ok) {
        if (feedback)
          audio.play(0, g.muted);
        if (a == A || g.s.stage == Mixing)
          render.response(true);
        if (a == A || g.s.pending != oldPending)
          persist();
      } else if (feedback && oldPending == Idle) {
        render.response(false);
        audio.play(1, g.muted);
      }
    };
    while (running && !stopping) {
      Uint32 now = SDL_GetTicks(),
             elapsed = std::min(Uint32(250), now - before);
      before = now;
      render.update(float(elapsed) / 1000);
      SDL_Event e;
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
          running = false;
          continue;
        }
        if (e.type == SDL_WINDOWEVENT &&
            e.window.event == SDL_WINDOWEVENT_FOCUS_LOST && scene == Playing) {
          scene = Paused;
          choice = 0;
          persist();
        }
        command(input.event(e));
      }
      auto d = input.direction();
      if (d.x != oldx || d.y != oldy) {
        repeat = 0;
        oldx = d.x;
        oldy = d.y;
      }
      if ((d.x || d.y) && now >= repeat) {
        if (g.s.stage != Mixing || scene != Playing || repeat == 0)
          command(d.x < 0 ? Left : d.x > 0 ? Right : d.y < 0 ? Up : Down);
        repeat = now + (repeat ? 160 : 330);
      }
      if (scene == Playing) {
        accumulator += elapsed * 60;
        while (accumulator >= 1000) {
          int pending = g.s.pending, stage = g.s.stage;
          bool golden = g.golden();
          g.tick();
          accumulator -= 1000;
          if (pending != Idle && g.s.pending == Idle) {
            audio.play(stage != g.s.stage ? 2 : 0, g.muted);
            persist();
          }
          if (g.golden() && !golden)
            audio.play(2, g.muted);
          if (!smoke && g.s.ticks % 300 == 0)
            persist();
        }
        if (g.s.stage == Photo) {
          scene = Finished;
          audio.play(2, g.muted);
          persist();
        }
      } else
        accumulator = 0;
      if (scene == Playing)
        render.draw(g);
      else if (scene == Finished)
        render.photo(g.s.cakes, g.s.flavor, g.serial);
      else if (scene == Gallery) {
        if (!g.albumCount)
          render.panel("SEU ALBUM",
                       {"AINDA ESTA VAZIO", "ASSE E DECORE OS DOCES",
                        "CADA CONJUNTO FICA SALVO", "A/B PARA VOLTAR"});
        else {
          int index = (g.albumNext - 1 - gallery + AlbumSize) % AlbumSize;
          const auto &c = g.album[index];
          render.photo(c.cakes, c.flavor, c.serial, true, g.albumCount);
        }
      } else if (scene == Menu || scene == Paused)
        render.panel(
            scene == Menu ? "COOKING" : "PAUSA",
            {"CONTINUAR", std::string("< ") + flavorName(menuFlavor) + " >",
             "ALBUM DE DOCES", g.muted ? "SOM: DESLIGADO" : "SOM: LIGADO",
             "COMO JOGAR", "SALVAR E SAIR"},
            choice);
      else if (scene == Tutorial || scene == Help) {
        const char *help[] = {
            "A LEVA O INGREDIENTE A TIGELA", "GIRE O DIRECIONAL EM CIRCULOS",
            "ESCOLHA A FORMA E APERTE A",    "A COLOCA/TIRA / < > FOGO",
            "< > FERRAMENTA / CIMA OPCAO",   "DOCES SALVOS NO ALBUM"};
        render.panel(scene == Tutorial ? "VAMOS FAZER DOCES!" : "UMA AJUDINHA",
                     {flavorName(g.s.flavor), help[g.s.stage],
                      g.s.stage == Decorating ? "A APLICA / Y MUDA O DOCE"
                                              : "Y AJUDA / START PAUSA",
                      g.s.stage == Decorating ? "B DESFAZ / PRONTO GUARDA"
                                              : "SEM PRESSA E SEM GAME OVER",
                      "DOIS MANCHES E D-PAD", "A PARA CONTINUAR"});
      } else
        render.panel("VOLTAR AO MENU?",
                     {"SUA RECEITA FICA SALVA", "A SALVAR E VOLTAR",
                      "B CONTINUAR BRINCANDO"});
      if (saveError)
        render.c.text(10, 34, "NAO FOI POSSIVEL SALVAR", Paint(155, 30, 54));
      if (assetError)
        render.c.text(10, 45, "CENARIO AUSENTE: CONFIRA ASSETS",
                      Paint(155, 30, 54));
      if (smoke && frames == 24 && !screenshot.empty()) {
        auto *surface = SDL_CreateRGBSurfaceWithFormat(
            0, 480, 272, 32, SDL_PIXELFORMAT_ARGB8888);
        if (!surface ||
            SDL_RenderReadPixels(r, nullptr, SDL_PIXELFORMAT_ARGB8888,
                                 surface->pixels, surface->pitch) ||
            SDL_SaveBMP(surface, screenshot.c_str()))
          saveError = true;
        SDL_FreeSurface(surface);
      }
      SDL_RenderPresent(r);
      SDL_Delay(8);
      if (smoke && ++frames > 24)
        running = false;
    }
    persist();
  }
  SDL_DestroyRenderer(r);
  SDL_DestroyWindow(w);
  SDL_Quit();
  return saveError ? 2 : assetError ? 3 : 0;
}
