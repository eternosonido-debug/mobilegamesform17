#include "audio.hpp"
#include "input.hpp"
#include "renderer.hpp"
#include <csignal>
#include <iostream>
#include <sys/stat.h>
namespace {
volatile std::sig_atomic_t stop = 0;
void signalStop(int) { stop = 1; }
} // namespace
int main(int argc, char **argv) {
  using namespace pop;
  bool smoke = false;
  int previewLevel = 0;
  std::string screenshot;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--smoke")
      smoke = true;
    else if (a == "--level" && i + 1 < argc)
      previewLevel = std::atoi(argv[++i]);
    else if (a == "--screenshot" && i + 1 < argc)
      screenshot = argv[++i];
  }
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK |
               SDL_INIT_GAMECONTROLLER) != 0) {
    std::cerr << SDL_GetError();
    return 1;
  }
  SDL_InitSubSystem(SDL_INIT_AUDIO);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  auto *w = SDL_CreateWindow(
      "Color Pop", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 272,
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
  char *pref = SDL_GetPrefPath("FakeMobileGames", "ColorPop");
  std::string dir = std::getenv("FMG_SAVE_DIR") ? std::getenv("FMG_SAVE_DIR")
                                                : (pref ? pref : ".");
  SDL_free(pref);
  mkdir(dir.c_str(), 0755);
  std::string save = dir + "/color-pop.save";
  Game g;
  bool loaded = !smoke && g.load(save);
  if (smoke)
    g.begin(previewLevel);
  enum Scene { Menu, Tutorial, Playing, Paused, Won, Lost, Finished };
  Scene scene = smoke ? Playing : Menu;
  int choice = 0, frames = 0;
  bool running = true, saveError = false;
  std::signal(SIGINT, signalStop);
  std::signal(SIGTERM, signalStop);
  {
    Input input;
    Audio audio;
    Renderer render(r);
    Uint32 previous = SDL_GetTicks(), nextRepeat = 0;
    int oldx = 0, oldy = 0;
    auto persist = [&]() {
      if (!smoke)
        saveError = !g.save(save);
    };
    auto command = [&](Control a) {
      if (a == Back) {
        persist();
        running = false;
        return;
      }
      if (a == None)
        return;
      if (scene == Menu || scene == Paused) {
        int count = 5;
        if (a == Up)
          choice = (choice + count - 1) % count;
        if (a == Down)
          choice = (choice + 1) % count;
        if (a == B && scene == Paused) {
          scene = Playing;
          return;
        }
        if (a == A || a == Start) {
          if (choice == 0) {
            scene =
                won(g.state, g.level) ? Won : (!loaded ? Tutorial : Playing);
            loaded = true;
          } else if (choice == 1) {
            g.begin(g.level);
            scene = Tutorial;
            persist();
          } else if (choice == 2) {
            g.begin((g.level + 1) % (g.unlocked + 1));
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
          scene = Playing;
        return;
      }
      if (scene == Won || scene == Finished) {
        if (a == A) {
          if (g.level + 1 < LevelCount) {
            g.begin(g.level + 1);
            scene = Tutorial;
          } else {
            scene = Menu;
            choice = 0;
          }
          persist();
        }
        if (a == B) {
          scene = Menu;
          choice = 0;
        }
        return;
      }
      if (scene == Lost) {
        if (a == A) {
          g.begin(g.level);
          scene = Playing;
          persist();
        } else if (a == Y) {
          g.undo();
          scene = Playing;
          persist();
        } else if (a == B) {
          scene = Menu;
          choice = 0;
        }
        return;
      }
      if (a == Start) {
        scene = Paused;
        choice = 0;
        persist();
        return;
      }
      if (render.timer < render.duration())
        return;
      if (a == Left || a == Right || a == Up || a == Down)
        g.navigate((a == Right) - (a == Left), (a == Down) - (a == Up));
      else if (a == B) {
        if (g.pulse)
          g.pulse = false;
        else
          g.undo();
        render.timer = 1;
        persist();
      } else if (a == Y) {
        if (g.state.charge >= 12)
          g.pulse = !g.pulse;
        else {
          render.error = .3f;
          audio.play(1, g.muted);
        }
      } else if (a == A) {
        State before = g.state;
        Effect e;
        if (g.action(&e)) {
          render.animate(before, e);
          audio.play(won(g.state, g.level) ? 2 : 0, g.muted);
          persist();
        } else {
          render.error = .3f;
          audio.play(1, g.muted);
        }
      }
    };
    while (running && !stop) {
      Uint32 now = SDL_GetTicks();
      float dt = std::min(.05f, float(now - previous) / 1000);
      previous = now;
      render.timer += dt;
      render.error = std::max(0.f, render.error - dt);
      SDL_Event e;
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
          running = false;
        else
          command(input.event(e));
      }
      auto d = input.direction();
      if (d.x != oldx || d.y != oldy) {
        nextRepeat = 0;
        oldx = d.x;
        oldy = d.y;
      }
      if ((d.x || d.y) && now >= nextRepeat) {
        command(d.x < 0 ? Left : d.x > 0 ? Right : d.y < 0 ? Up : Down);
        nextRepeat = now + (nextRepeat ? 110 : 260);
      }
      if (scene == Playing && render.timer >= render.duration()) {
        if (won(g.state, g.level))
          scene = g.level + 1 == LevelCount ? Finished : Won;
        else if (g.state.moves == 0)
          scene = Lost;
      }
      if (scene == Playing)
        render.draw(g);
      else if (scene == Menu || scene == Paused)
        render.panel(scene == Menu ? "COLOR POP" : "PAUSA",
                     {"CONTINUAR", "REINICIAR FASE", "TROCAR FASE",
                      g.muted ? "SOM: DESLIGADO" : "SOM: LIGADO",
                      "SALVAR E SAIR"},
                     choice);
      else if (scene == Tutorial)
        render.panel("JUNTE AS CORES",
                     {"A ESTOURA GRUPOS DE 2+", "CUMPRA AS DUAS METAS",
                      "B DESFAZ A ULTIMA JOGADA", "Y PULSO QUANDO CARREGAR",
                      "A PARA COMECAR"},
                     -1);
      else if (scene == Lost)
        render.panel("SEM JOGADAS",
                     {"A TENTAR DE NOVO", "Y DESFAZER", "B MENU"}, -1);
      else
        render.panel(
            scene == Finished ? "CAMPANHA CONCLUIDA!" : "CORES COMPLETAS!",
            {"FASE CONCLUIDA", "PONTOS: " + std::to_string(g.state.score),
             g.level + 1 < LevelCount ? "A PROXIMA FASE" : "A VOLTAR AO MENU",
             "B MENU"},
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
          std::cerr << SDL_GetError();
          running = false;
          saveError = true;
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
