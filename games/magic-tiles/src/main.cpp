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
  using namespace tiles;
  bool smoke = false;
  std::string screenshot;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--smoke")
      smoke = true;
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
      "Magic Tiles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 272,
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
  char *pref = SDL_GetPrefPath("FakeMobileGames", "MagicTiles");
  std::string dir = std::getenv("FMG_SAVE_DIR") ? std::getenv("FMG_SAVE_DIR")
                                                : (pref ? pref : ".");
  SDL_free(pref);
  mkdir(dir.c_str(), 0755);
  std::string path = dir + "/magic-tiles.save";
  Settings cfg;
  if (!smoke)
    cfg.load(path);
  State game(cfg.song, cfg.difficulty, cfg.practice);
  enum Scene { Menu, Tutorial, Playing, Paused, Won, Lost };
  Scene scene = Menu;
  int choice = 0, frames = 0;
  bool running = true, saveError = false, audioError = false;
  std::signal(SIGINT, stopping);
  std::signal(SIGTERM, stopping);
  {
    Input input;
    Audio audio;
    Renderer render(r);
    Uint32 previous = SDL_GetTicks(), nextRepeat = 0;
    int oldx = 0, oldy = 0;
    auto persist = [&]() {
      if (!smoke)
        saveError = !cfg.save(path);
    };
    auto begin = [&]() {
      if (!audio.device) {
        audioError = true;
        scene = Menu;
        return;
      }
      game = State(cfg.song, cfg.difficulty, cfg.practice);
      audio.start(cfg.song, cfg.muted);
      scene = Playing;
    };
    auto judge = [&](int lane, int lag) {
      int now = std::max(0, audio.clock() - lag + cfg.offset);
      render.hit(lane, game.hit(lane, now));
    };
    auto command = [&](Control a, int lag) {
      if (a == None)
        return;
      if (a == Back) {
        if (scene == Menu) {
          persist();
          running = false;
        } else {
          audio.pause(true);
          scene = Menu;
          choice = 0;
          persist();
        }
        return;
      }
      if (scene == Menu) {
        if (a == Up)
          choice = (choice + 6) % 7;
        if (a == Down)
          choice = (choice + 1) % 7;
        if (choice == 3 && (a == Left || a == Right)) {
          cfg.offset =
              std::max(-150, std::min(150, cfg.offset + (a == Right ? 5 : -5)));
          persist();
        }
        if (a == A) {
          if (choice == 0)
            scene = Tutorial;
          else if (choice == 1)
            cfg.song = (cfg.song + 1) % SongCount;
          else if (choice == 2)
            cfg.difficulty = (cfg.difficulty + 1) % 3;
          else if (choice == 3)
            cfg.offset = 0;
          else if (choice == 4)
            cfg.practice = !cfg.practice;
          else if (choice == 5)
            cfg.muted = !cfg.muted;
          else
            running = false;
          persist();
        }
        return;
      }
      if (scene == Tutorial) {
        if (a == A || a == Start)
          begin();
        else if (a == B)
          scene = Menu;
        return;
      }
      if (scene == Paused) {
        if (a == A || a == Start) {
          audio.pause(false);
          scene = Playing;
        } else if (a == B) {
          scene = Menu;
          choice = 0;
        }
        return;
      }
      if (scene == Won || scene == Lost) {
        if (a == A)
          begin();
        else if (a == B) {
          scene = Menu;
          choice = 0;
        }
        return;
      }
      if (a == Start) {
        audio.pause(true);
        scene = Paused;
      } else if (a >= A && a <= Y)
        judge(int(a) - int(A), lag);
    };
    if (smoke)
      begin();
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
            audio.pause(true);
            scene = Paused;
          }
          int lag = e.common.timestamp && now >= e.common.timestamp
                        ? std::min(200, int(now - e.common.timestamp))
                        : 0;
          command(input.event(e), lag);
        }
      }
      auto d = input.direction();
      if (scene == Playing) {
        if ((d.x || d.y) && (d.x != oldx || d.y != oldy)) {
          int lane = directionalLane(d.x, d.y);
          if (lane >= 0)
            judge(lane, 0);
        }
      } else {
        if (d.x != oldx || d.y != oldy)
          nextRepeat = 0;
        if ((d.x || d.y) && now >= nextRepeat) {
          command(d.x < 0 ? Left : d.x > 0 ? Right : d.y < 0 ? Up : Down, 0);
          nextRepeat = now + (nextRepeat ? 120 : 260);
        }
      }
      oldx = d.x;
      oldy = d.y;
      int rawTime = audio.clock();
      int songTime = std::max(0, rawTime + cfg.offset);
      if (scene == Playing) {
        game.advance(songTime, rawTime);
        if (game.failed() || game.won()) {
          audio.pause(true);
          scene = game.failed() ? Lost : Won;
          cfg.finish(game);
          persist();
        }
      }
      render.update(dt);
      if (scene == Playing)
        render.draw(game, songTime);
      else if (scene == Menu) {
        const char *diff[] = {"FACIL", "NORMAL", "DIFICIL"};
        render.panel("MAGIC TILES",
                     {"JOGAR", std::string("MUSICA: ") + title(cfg.song),
                      std::string("NIVEL: ") + diff[cfg.difficulty],
                      "ATRASO: " + std::to_string(cfg.offset) + " MS < >",
                      cfg.practice ? "TREINO: LIGADO" : "TREINO: DESLIGADO",
                      cfg.muted ? "SOM: DESLIGADO" : "SOM: LIGADO",
                      "SALVAR E SAIR"},
                     choice);
      } else if (scene == Tutorial)
        render.panel("TOQUE NO RITMO",
                     {"PISTAS: A  B  X  Y", "OU ESQ BAIXO CIMA DIR",
                      "TOQUE NA LINHA DE BAIXO", "START PAUSA  SELECT MENU",
                      "A PARA COMECAR"},
                     -1);
      else if (scene == Paused)
        render.panel("PAUSA", {"A OU START CONTINUA", "B OU SELECT MENU"}, -1);
      else
        render.panel(scene == Won ? "MUSICA CONCLUIDA!" : "RITMO PERDIDO",
                     {"PONTOS: " + std::to_string(game.score()),
                      "COMBO MAX: " + std::to_string(game.maxCombo),
                      "PERFEITOS: " + std::to_string(game.perfect),
                      "A REPETIR  B MENU"},
                     -1);
      if (scene == Menu)
        render.c.text(
            17, 129,
            "RECORDE: " +
                std::to_string(cfg.best[cfg.song * 3 + cfg.difficulty]),
            ui::Paint(167, 189, 210));
      if (saveError)
        render.c.text(2, 2, "ERRO AO SALVAR", ui::Paint(255, 85, 100));
      if (audioError)
        render.c.text(2, 129, "AUDIO INDISPONIVEL", ui::Paint(255, 85, 100));
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
    audio.pause(true);
    persist();
  }
  SDL_DestroyRenderer(r);
  SDL_DestroyWindow(w);
  SDL_Quit();
  return saveError ? 2 : 0;
}
