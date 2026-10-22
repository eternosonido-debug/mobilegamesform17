#include "audio.hpp"
#include "input.hpp"
#include "renderer.hpp"
#include <csignal>
#include <iostream>
#include <sys/stat.h>
namespace {
volatile std::sig_atomic_t stop = 0;
void stopping(int) { stop = 1; }
} // namespace
int main(int argc, char **argv) {
  using namespace tower;
  bool smoke = false;
  int preview = 0;
  std::string screenshot;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--smoke")
      smoke = true;
    else if (a == "--level" && i + 1 < argc)
      preview = std::atoi(argv[++i]);
    else if (a == "--screenshot" && i + 1 < argc)
      screenshot = argv[++i];
  }
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK |
               SDL_INIT_GAMECONTROLLER)) {
    std::cerr << SDL_GetError();
    return 1;
  }
  SDL_InitSubSystem(SDL_INIT_AUDIO);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  auto *w = SDL_CreateWindow(
      "Tower Break", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 272,
      std::getenv("FMG_FULLSCREEN") ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
  auto *r = w ? SDL_CreateRenderer(
                    w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
              : nullptr;
  if (w && !r)
    r = SDL_CreateRenderer(w, -1, SDL_RENDERER_SOFTWARE);
  if (!r) {
    std::cerr << SDL_GetError();
    SDL_Quit();
    return 1;
  }
  SDL_RenderSetLogicalSize(r, 240, 136);
  char *pref = SDL_GetPrefPath("FakeMobileGames", "TowerBreak");
  std::string dir = std::getenv("FMG_SAVE_DIR") ? std::getenv("FMG_SAVE_DIR")
                                                : (pref ? pref : ".");
  SDL_free(pref);
  mkdir(dir.c_str(), 0755);
  std::string path = dir + "/tower-break.save";
  Game g;
  bool loaded = !smoke && g.load(path);
  if (smoke)
    g.begin(preview);
  enum Scene { Menu, Tutorial, Playing, Paused, Won, Lost, Exit };
  Scene scene = smoke ? Playing : Menu, returnScene = Menu;
  int choice = 0, frames = 0;
  bool running = true, saveError = false, needRelease = true, dirty = false;
  float accumulator = 0, settle = 0;
  std::signal(SIGINT, stopping);
  std::signal(SIGTERM, stopping);
  {
    Input input;
    Audio audio;
    Renderer render(r);
    render.camera = float(g.state.layer);
    Uint32 previous = SDL_GetTicks(), nextRepeat = 0, lastSave = previous;
    int oldx = 0, oldy = 0;
    auto persist = [&]() {
      if (!smoke) {
        saveError = !g.save(path);
        dirty = saveError;
        lastSave = SDL_GetTicks();
      }
    };
    auto resume = [&]() {
      scene = Playing;
      needRelease = true;
      accumulator = 0;
    };
    auto command = [&](Control a) {
      if (a == None)
        return;
      if (a == Back) {
        returnScene = scene;
        scene = Exit;
        choice = 0;
        needRelease = true;
        return;
      }
      if (scene == Exit) {
        if (a == B) {
          scene = returnScene;
          needRelease = true;
        } else if (a == A) {
          persist();
          running = false;
        }
        return;
      }
      if (scene == Menu || scene == Paused) {
        if (a == Up)
          choice = (choice + 4) % 5;
        if (a == Down)
          choice = (choice + 1) % 5;
        if (a == B && scene == Paused) {
          resume();
          return;
        }
        if (a == A || a == Start) {
          if (choice == 0) {
            if (g.state.dead)
              scene = Lost;
            else if (won(g.state, g.stage))
              scene = Won;
            else if (!loaded) {
              scene = Tutorial;
              loaded = true;
            } else
              resume();
          } else if (choice == 1 || choice == 2) {
            g.begin(choice == 1 ? g.stage : (g.stage + 1) % (g.unlocked + 1));
            render.restart();
            settle = 0;
            scene = Tutorial;
            persist();
          } else if (choice == 3) {
            g.muted = !g.muted;
            persist();
          } else {
            persist();
            running = false;
          }
        }
        return;
      }
      if (scene == Tutorial) {
        if (a == A || a == B || a == Start)
          resume();
        return;
      }
      if (scene == Won || scene == Lost) {
        if (a == A) {
          if (scene == Won && g.stage == 2) {
            scene = Menu;
            choice = 0;
          } else {
            if (scene == Won)
              g.begin(g.stage + 1);
            else
              g.retry();
            render.restart();
            settle = 0;
            resume();
          }
          persist();
        } else if (a == B) {
          scene = Menu;
          choice = 0;
        }
        return;
      }
      if (a == Start || a == B) {
        scene = Paused;
        choice = 0;
        needRelease = true;
        persist();
      }
    };
    while (running && !stop) {
      Uint32 now = SDL_GetTicks();
      float dt = std::min(.1f, float(now - previous) / 1000);
      previous = now;
      SDL_Event e;
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
          running = false;
        else {
          if (e.type == SDL_WINDOWEVENT &&
              e.window.event == SDL_WINDOWEVENT_FOCUS_LOST &&
              scene == Playing) {
            scene = Paused;
            choice = 0;
            needRelease = true;
            persist();
          }
          command(input.event(e));
        }
      }
      bool held = input.primary();
      if (!held)
        needRelease = false;
      auto d = input.direction();
      if (scene != Playing) {
        if (d.x != oldx || d.y != oldy)
          nextRepeat = 0;
        if ((d.x || d.y) && now >= nextRepeat) {
          command(d.y < 0 ? Up : d.y > 0 ? Down : d.x < 0 ? Up : Down);
          nextRepeat = now + (nextRepeat ? 120 : 260);
        }
      }
      oldx = d.x;
      oldy = d.y;
      if (scene == Playing) {
        accumulator += dt;
        while (accumulator >= 1.f / 120) {
          auto ev = g.advance(held && !needRelease, d.x);
          accumulator -= 1.f / 120;
          if (ev.broken) {
            render.broken(ev.row);
            audio.play(won(g.state, g.stage) ? 2 : 0, g.muted);
            dirty = true;
          }
          if (ev.died) {
            audio.play(1, g.muted);
            dirty = true;
          }
        }
        if (g.state.dead || won(g.state, g.stage)) {
          settle += dt;
          if (settle >= .55f) {
            scene = g.state.dead ? Lost : Won;
            persist();
          }
        } else
          settle = 0;
        if (dirty && !held && now - lastSave >= 1000)
          persist();
      }
      render.update(dt, g);
      if (scene == Playing)
        render.draw(g, held && !needRelease);
      else if (scene == Menu || scene == Paused)
        render.panel(scene == Menu ? "TOWER BREAK" : "PAUSA",
                     {"CONTINUAR", "REINICIAR TORRE", "TROCAR TORRE",
                      g.muted ? "SOM: DESLIGADO" : "SOM: LIGADO",
                      "SALVAR E SAIR"},
                     choice);
      else if (scene == Tutorial)
        render.panel("CHEGUE A BASE",
                     {"SEGURE A PARA QUEBRAR", "SOLTE PARA REBATER",
                      "NAO ATAQUE O VERMELHO", "DIRECIONAIS GIRAM A TORRE",
                      "A PARA COMECAR"},
                     -1);
      else if (scene == Exit)
        render.panel("SALVAR E SAIR?", {"A CONFIRMA", "B VOLTA AO JOGO"}, 0);
      else
        render.panel(scene == Lost  ? "BOLA QUEBRADA!"
                     : g.stage == 2 ? "CAMPANHA CONCLUIDA!"
                                    : "TORRE CONCLUIDA!",
                     {scene == Lost  ? "A TENTAR NOVAMENTE"
                      : g.stage == 2 ? "A MENU"
                                     : "A PROXIMA TORRE",
                      "PONTOS: " + std::to_string(g.state.score), "B MENU"},
                     -1);
      if (saveError)
        render.c.text(4, 3, "ERRO AO SALVAR", ui::Paint(255, 90, 80));
      if (smoke && frames == 24 && !screenshot.empty()) {
        int width, height;
        SDL_GetRendererOutputSize(r, &width, &height);
        auto *s = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32,
                                                 SDL_PIXELFORMAT_ARGB8888);
        if (!s ||
            SDL_RenderReadPixels(r, nullptr, SDL_PIXELFORMAT_ARGB8888,
                                 s->pixels, s->pitch) ||
            SDL_SaveBMP(s, screenshot.c_str())) {
          saveError = true;
          running = false;
        }
        SDL_FreeSurface(s);
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
