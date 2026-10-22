#include "../src/renderer.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
void check(bool x, const char *m) {
  if (!x)
    throw std::runtime_error(m);
}
std::vector<Uint32> capture(SDL_Renderer *r) {
  std::vector<Uint32> pixels(480 * 272);
  check(SDL_RenderReadPixels(r, nullptr, SDL_PIXELFORMAT_ARGB8888,
                             pixels.data(), 480 * 4) == 0,
        "capture");
  return pixels;
}
int main() {
  using namespace pocketbeats;
  check(SDL_Init(SDL_INIT_VIDEO) == 0, "SDL");
  auto *w = SDL_CreateWindow("full grid", 0, 0, 480, 272, SDL_WINDOW_HIDDEN);
  auto *r = SDL_CreateRenderer(w, -1, SDL_RENDERER_SOFTWARE);
  check(r, "renderer");
  Controller app;
  app.state.scene = Scene::Playing;
  app.state.track = 1;
  for (int length : {4, 8, 16, 32}) {
    auto &p = app.state.current();
    p.resize(length);
    p.notes = {};
    app.state.cursor = 0;
    render(r, app, -1, true);
    auto baseline = capture(r);
    // Every step must update the visible grid even with the cursor kept at step
    // one.
    for (int n = 0; n < length; ++n) {
      p.notes[3][n] = 3;
      render(r, app, -1, true);
      auto changed = capture(r);
      int differences = 0;
      for (int y = 82; y < 223; ++y)
        for (int x = 90; x < 480; ++x)
          differences += baseline[y * 480 + x] != changed[y * 480 + x];
      check(differences > 0, "a step is hidden behind pagination");
      p.notes[3][n] = 0;
    }
    // Moving selection must not scroll or replace notes elsewhere in the grid.
    p.notes[3][0] = 4;
    render(r, app, -1, true);
    auto first = capture(r);
    app.state.cursor = length - 1;
    render(r, app, -1, true);
    auto last = capture(r);
    for (int y = 180; y < 211; ++y)
      for (int x = 94; x < 480; ++x)
        check(first[y * 480 + x] == last[y * 480 + x],
              "cursor movement scrolls grid");
    app.state.scene = Scene::Paused;
    render(r, app, -1, true);
    app.state.scene = Scene::Playing;
    render(r, app, -1, true);
    check(capture(r) == last, "returning from actions changes grid scaling");
  }
  SDL_DestroyRenderer(r);
  SDL_DestroyWindow(w);
  SDL_Quit();
  std::cout << "PASS all steps visible at 4/8/16/32, no cursor scrolling, "
               "action-panel scaling restored\n";
}
