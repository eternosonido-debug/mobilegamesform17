#include "audio.hpp"
#include "renderer.hpp"
#include <csignal>
#include <iostream>
#include <sys/stat.h>
namespace {
volatile std::sig_atomic_t stopped = 0;
void stop(int) { stopped = 1; }
} // namespace
int main(int argc, char **argv) {
  using namespace pocketbeats;
  stopped = 0;
  bool smoke = false;
  std::string screenshot;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--smoke")
      smoke = true;
    if (arg == "--screenshot" && i + 1 < argc)
      screenshot = argv[++i];
  }
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK |
               SDL_INIT_GAMECONTROLLER)) {
    std::cerr << SDL_GetError() << '\n';
    return 1;
  }
  SDL_InitSubSystem(SDL_INIT_AUDIO);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  SDL_Window *w = SDL_CreateWindow(
      "Pocket Beats", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 272,
      std::getenv("FMG_FULLSCREEN") ? SDL_WINDOW_FULLSCREEN_DESKTOP
                                    : SDL_WINDOW_RESIZABLE);
  SDL_Renderer *r =
      w ? SDL_CreateRenderer(
              w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
        : nullptr;
  if (w && !r)
    r = SDL_CreateRenderer(w, -1, SDL_RENDERER_SOFTWARE);
  if (!r) {
    std::cerr << SDL_GetError() << '\n';
    if (w)
      SDL_DestroyWindow(w);
    SDL_Quit();
    return 1;
  }
  SDL_RenderSetLogicalSize(r, 240, 136);
  char *pref = SDL_GetPrefPath("FakeMobileGames", "PocketBeats");
  const char *custom = std::getenv("FMG_SAVE_DIR");
  std::string folder = custom ? custom : pref ? pref : ".";
  SDL_free(pref);
  mkdir(folder.c_str(), 0755);
  std::string path = folder + "/pocket-beats.sav";
  int result = 0;
  {
    Controller app;
    if (!smoke)
      app.state.load(path);
    else {
      app.state.scene = Scene::Playing;
      app.state.playing = true;
    }
    Input input;
    Audio audio;
    audio.sync(app.state.current(), app.state.playing);
    std::signal(SIGINT, stop);
    std::signal(SIGTERM, stop);
    Uint32 repeat = 0, lastSave = 0;
    int oldx = 0, oldy = 0, frames = 0;
    while (!app.quit && !stopped) {
      SDL_Event e;
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
          app.quit = true;
          break;
        }
        Control key = input.event(e);
        if (key != None) {
          app.action(key);
          audio.sync(app.state.current(), app.state.playing, app.restart);
        }
      }
      auto d = input.direction();
      Uint32 now = SDL_GetTicks();
      if (d.x || d.y) {
        if (d.x != oldx || d.y != oldy || now >= repeat) {
          app.action(d.x ? (d.x > 0 ? Right : Left) : (d.y > 0 ? Down : Up));
          audio.sync(app.state.current(), app.state.playing, app.restart);
          repeat = now + ((d.x != oldx || d.y != oldy) ? 260 : 100);
        }
      }
      oldx = d.x;
      oldy = d.y;

      if (!smoke && app.dirty && !app.state.playing && now - lastSave >= 1500) {
        bool ok = app.state.save(path);
        app.message = ok ? "SAVED" : "SAVE FAILED";
        if (ok)
          app.dirty = false;
        lastSave = now;
      }
      if (!app.message.empty() && now - lastSave > 1400)
        app.message.clear();
      render(r, app, audio.audibleStep(), audio.device != 0);
      if (smoke && frames == 10 && !screenshot.empty()) {
        int width, height;
        SDL_GetRendererOutputSize(r, &width, &height);
        auto *surface = SDL_CreateRGBSurfaceWithFormat(
            0, width, height, 32, SDL_PIXELFORMAT_ARGB8888);
        if (!surface ||
            SDL_RenderReadPixels(r, nullptr, SDL_PIXELFORMAT_ARGB8888,
                                 surface->pixels, surface->pitch) ||
            SDL_SaveBMP(surface, screenshot.c_str()))
          result = 1;
        if (surface)
          SDL_FreeSurface(surface);
      }
      SDL_RenderPresent(r);
      SDL_Delay(8);
      if (smoke && ++frames >= 24)
        app.quit = true;
    }
    if (!smoke && !app.state.save(path)) {
      std::cerr << "Save failed: " << path << '\n';
      result = 2;
    }
  }
  SDL_DestroyRenderer(r);
  SDL_DestroyWindow(w);
  SDL_Quit();
  return result;
}
