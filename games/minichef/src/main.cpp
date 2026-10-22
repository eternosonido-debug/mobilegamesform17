#include "audio.hpp"
#include "input.hpp"
#include "renderer.hpp"
#include <csignal>
#include <iostream>
#include <sys/stat.h>
namespace {
volatile std::sig_atomic_t stopping = 0;
void stop(int) { stopping = 1; }
} // namespace
int main(int argc, char **argv) {
  using namespace chef;
  bool smoke = false;
  std::string screenshot;
  int preview = 0, previewStep = 0;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--smoke")
      smoke = true;
    else if (a == "--screenshot" && i + 1 < argc)
      screenshot = argv[++i];
    else if (a == "--level" && i + 1 < argc)
      preview = std::atoi(argv[++i]);
    else if (a == "--step" && i + 1 < argc)
      previewStep = std::atoi(argv[++i]);
  }
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK |
               SDL_INIT_GAMECONTROLLER) != 0) {
    std::cerr << SDL_GetError();
    return 1;
  }
  SDL_InitSubSystem(SDL_INIT_AUDIO);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  SDL_Window *w = SDL_CreateWindow(
      "Minichef", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 272,
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
  char *pref = SDL_GetPrefPath("FakeMobileGames", "Minichef");
  std::string dir = std::getenv("FMG_SAVE_DIR") ? std::getenv("FMG_SAVE_DIR")
                                                : (pref ? pref : ".");
  SDL_free(pref);
  mkdir(dir.c_str(), 0755);
  std::string save = dir + "/minichef.save";
  Game g;
  bool loaded = !smoke && g.load(save);
  if (smoke) {
    g.unlocked = 2;
    g.begin(preview);
    g.s.step = std::max(0, std::min(4, previewStep));
  }
  enum Scene { Menu, Tutorial, Playing, Paused, Help, Won, Exit };
  Scene scene = smoke ? Playing : Menu, previousScene = Menu;
  int choice = 0, menuRecipe = g.s.level, frames = 0;
  bool running = true, saveError = false;
  std::signal(SIGINT, stop);
  std::signal(SIGTERM, stop);
  {
    Input input;
    Audio audio;
    Renderer render(r);
    Uint32 before = SDL_GetTicks(), repeat = 0;
    int oldx = 0, oldy = 0;
    unsigned accumulator = 0;
    auto persist = [&]() {
      if (!smoke)
        saveError = !g.save(save);
    };
    auto begin = [&]() {
      g.begin(menuRecipe);
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
          menuRecipe =
              (menuRecipe + (a == Right ? 1 : g.unlocked)) % (g.unlocked + 1);
        if (a == B && scene == Paused) {
          scene = g.s.step == Served ? Won : Playing;
          return;
        }
        if (a == A || a == Start) {
          if (choice == 0) {
            scene = g.s.step == Served ? Won : loaded ? Playing : Tutorial;
            loaded = true;
          } else if (choice == 1)
            begin();
          else if (choice == 2) {
            menuRecipe = g.s.level;
            begin();
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
      if (scene == Won) {
        if (a == A) {
          if (g.s.level < 2) {
            menuRecipe = g.s.level + 1;
            begin();
          } else {
            scene = Menu;
            choice = 0;
          }
        } else if (a == B) {
          menuRecipe = g.s.level;
          begin();
        }
        return;
      }
      if (a == Start || a == B) {
        scene = Paused;
        choice = 0;
        persist();
        return;
      }
      if (a == Y) {
        previousScene = Playing;
        scene = Help;
        return;
      }
      int oldStage = g.s.step;
      bool active = false, ok = false;
      if (a == Left || a == Right || a == Down || a == Up) {
        active = g.s.step == Mix;
        ok = g.direction(a == Left ? 0 : a == Right ? 1 : a == Down ? 2 : 3);
      } else if (a == A) {
        if ((g.s.step == Chop && g.s.phaseTicks - g.s.lastCut < 20) ||
            (g.s.step == Cook && g.s.phaseTicks - g.s.lastStir < 60))
          return;
        active = true;
        ok = g.action();
      }
      if (active) {
        render.feedbackAction(ok);
        audio.play(ok ? 0 : 1, g.muted);
      }
      if (g.s.step != oldStage) {
        persist();
        audio.play(2, g.muted);
      }
    };
    while (running && !stopping) {
      Uint32 now = SDL_GetTicks();
      Uint32 elapsed = std::min(Uint32(250), now - before);
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
        // Stirring requires a fresh direction, not automatic repeats.
        if (g.s.step != Mix || scene != Playing || repeat == 0)
          command(d.x < 0 ? Left : d.x > 0 ? Right : d.y < 0 ? Up : Down);
        repeat = now + (repeat ? 160 : 330);
      }
      if (scene == Playing) {
        accumulator += elapsed * 60;
        while (accumulator >= 1000) {
          int oldStage = g.s.step;
          g.tick();
          accumulator -= 1000;
          if (oldStage != g.s.step) {
            persist();
            audio.play(2, g.muted);
          }
          if (!smoke && g.s.ticks % 900 == 0)
            persist();
        }
        if (g.s.step == Served) {
          scene = Won;
          persist();
        }
      } else
        accumulator = 0;
      if (scene == Playing)
        render.draw(g);
      else if (scene == Won)
        render.served(g);
      else if (scene == Menu || scene == Paused) {
        render.panel(
            g, scene == Menu ? "MINICHEF" : "PAUSA",
            {"CONTINUAR", std::string("< ") + recipe(menuRecipe).name + " >",
             "RECOMECAR RECEITA", g.muted ? "SOM: DESLIGADO" : "SOM: LIGADO",
             "COMO JOGAR", "SALVAR E SAIR"},
            choice);
      } else if (scene == Tutorial || scene == Help) {
        const char *help[] = {
            "ESCOLHA OS ITENS DA LISTA",   "APERTE A NA FAIXA VERDE",
            "SIGA AS SETAS NO DIRECIONAL", "< > FOGO / A MEXE NO VERDE",
            "ESCOLHA E DECORE COM A",      "PRATO CONCLUIDO"};
        render.panel(g, scene == Tutorial ? "VAMOS COZINHAR!" : "UMA AJUDINHA",
                     {recipe(g.s.level).name, help[g.s.step],
                      "ERROU? TENTE OUTRA VEZ!", "SEM PRESSA, SEM GAME OVER",
                      "START PAUSA / Y AJUDA", "A PARA CONTINUAR"});
      } else
        render.panel(g, "VOLTAR AO MENU?",
                     {"SUA RECEITA FICA SALVA", "A SALVAR E VOLTAR",
                      "B CONTINUAR COZINHANDO"});
      if (saveError)
        render.c.text(10, 31, "NAO FOI POSSIVEL SALVAR",
                      ui::Paint(170, 48, 50));
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
  return saveError ? 2 : 0;
}
