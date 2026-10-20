#include "audio.hpp"
#include "input.hpp"
#include "renderer.hpp"
#include <csignal>
#include <iostream>
#include <sys/stat.h>
namespace {
volatile std::sig_atomic_t stopped = 0;
void stop(int) { stopped = 1; }
} // namespace
int main(int argc, char **argv) {
  using namespace blockout;
  bool smoke = false;
  std::string shot;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--smoke")
      smoke = true;
    if (a == "--screenshot" && i + 1 < argc)
      shot = argv[++i];
  }
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK |
               SDL_INIT_GAMECONTROLLER) != 0) {
    std::cerr << SDL_GetError() << '\n';
    return 1;
  }
  SDL_InitSubSystem(SDL_INIT_AUDIO);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  bool fullscreen = std::getenv("FMG_FULLSCREEN");
  auto *window = SDL_CreateWindow(
      "Block Out", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 272,
      fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_RESIZABLE);
  auto *renderer = window ? SDL_CreateRenderer(window, -1,
                                               SDL_RENDERER_ACCELERATED |
                                                   SDL_RENDERER_PRESENTVSYNC)
                          : nullptr;
  if (window && !renderer)
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  if (!renderer) {
    std::cerr << SDL_GetError() << '\n';
    if (window)
      SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  SDL_RenderSetLogicalSize(renderer, 240, 136);
  char *pref = SDL_GetPrefPath("FakeMobileGames", "BlockOut");
  const char *custom = std::getenv("FMG_SAVE_DIR");
  std::string folder = custom ? custom : pref ? pref : ".";
  SDL_free(pref);
  mkdir(folder.c_str(), 0755);
  std::string save = folder + "/block-out.sav";
  Game game;
  if (!smoke)
    game.load(save);
  if (smoke)
    game.scene = Scene::Playing;
  bool running = true;
  std::signal(SIGTERM, stop);
  std::signal(SIGINT, stop);
  Uint32 last = SDL_GetTicks(), repeat = 0, lastSave = last;
  int frames = 0;
  {
    Input input;
    Audio audio;
    auto persist = [&]() {
      if (!smoke && !game.save(save))
        std::cerr << "Block Out save failed\n";
    };
    auto press = [&](Control key) {
      if (key == None || key == X)
        return;
      if (key == Back) {
        persist();
        running = false;
        return;
      }
      if (key == Start || key == Y) {
        if (game.scene == Scene::Paused)
          game.scene = won(game.board)                      ? Scene::Won
                       : game.tutorial && game.current == 0 ? Scene::Tutorial
                                                            : Scene::Playing;
        else {
          game.scene = Scene::Paused;
          game.menu = 0;
        }
        return;
      }
      if (game.scene == Scene::Menu) {
        if (key == Up)
          game.menu = (game.menu + 3) % 4;
        if (key == Down)
          game.menu = (game.menu + 1) % 4;
        if (key == A) {
          if (game.menu == 0)
            game.scene = won(game.board)                      ? Scene::Won
                         : game.tutorial && game.current == 0 ? Scene::Tutorial
                                                              : Scene::Playing;
          if (game.menu == 1) {
            game.tutorial = true;
            game.reset(0);
          }
          if (game.menu == 2)
            game.muted = !game.muted;
          if (game.menu == 3) {
            persist();
            running = false;
          }
        }
        return;
      }
      if (game.scene == Scene::Paused) {
        if (key == Up)
          game.menu = (game.menu + 4) % 5;
        if (key == Down)
          game.menu = (game.menu + 1) % 5;
        if (key == B)
          game.scene = won(game.board)                      ? Scene::Won
                       : game.tutorial && game.current == 0 ? Scene::Tutorial
                                                            : Scene::Playing;
        if (key == A) {
          int choice = game.menu;
          if (choice == 0)
            game.scene = won(game.board) ? Scene::Won : Scene::Playing;
          if (choice == 1)
            game.undo();
          if (choice == 2)
            game.reset(game.current);
          if (choice == 3)
            game.muted = !game.muted;
          if (choice == 4) {
            persist();
            running = false;
          }
        }
        return;
      }
      if (game.scene == Scene::Won) {
        if (game.animation > 0)
          return;
        if (key == A) {
          if (game.current < levelCount - 1)
            game.reset(game.current + 1);
          else
            game.scene = Scene::GameOver;
        }
        if (key == B)
          game.reset(game.current);
        persist();
        return;
      }
      if (game.scene == Scene::GameOver) {
        if (key == A) {
          game.scene = Scene::Menu;
          game.menu = 0;
        }
        return;
      }
      if (game.animation > 0 && game.exiting)
        return;
      if (key == A) {
        game.action();
        audio.play(game.selected < 0 ? 1 : 0, game.muted);
      }
      if (key == B)
        game.selected = -1;
      if (key < 4) {
        int dx = (key == Right) - (key == Left),
            dy = (key == Down) - (key == Up);
        int before = game.board.moves;
        bool ok = game.move(dx, dy);
        if (before != game.board.moves) {
          audio.play(game.exiting ? 2 : 0, game.muted);
          persist();
        } else if (!ok)
          audio.play(1, game.muted);
      }
    };
    while (running && !stopped) {
      Uint32 now = SDL_GetTicks();
      float dt = std::min(.05f, float(now - last) / 1000);
      last = now;
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
          running = false;
        else
          press(input.event(event));
      }
      auto d = input.direction();
      if ((d.x || d.y) && now >= repeat) {
        if (d.x)
          press(d.x > 0 ? Right : Left);
        else
          press(d.y > 0 ? Down : Up);
        repeat = now + 150;
      }
      if (!d.x && !d.y)
        repeat = now;
      game.tick(dt);
      render(renderer, game);
      if (!smoke && now - lastSave > 5000) {
        persist();
        lastSave = now;
      }
      if (smoke && ++frames == 3) {
        if (!shot.empty()) {
          int w, h;
          SDL_GetRendererOutputSize(renderer, &w, &h);
          auto *s = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32,
                                                   SDL_PIXELFORMAT_ARGB8888);
          if (s) {
            SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_ARGB8888,
                                 s->pixels, s->pitch);
            SDL_SaveBMP(s, shot.c_str());
            SDL_FreeSurface(s);
          }
        }
        running = false;
      }
      Uint32 elapsed = SDL_GetTicks() - now;
      if (elapsed < 16)
        SDL_Delay(16 - elapsed);
    }
    persist();
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
