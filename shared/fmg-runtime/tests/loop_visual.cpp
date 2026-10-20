#include "loop_view.hpp"
#include <iostream>
#include <stdexcept>
void require(bool b, const char *m) {
  if (!b)
    throw std::runtime_error(m);
}
int main() {
  for (int h : {6, 8, 10}) {
    auto previous = fmg::loop_art::position(0, h);
    float minimum = 1000, maximum = 0;
    for (int i = 1; i <= 2000; ++i) {
      auto point = fmg::loop_art::position((12 + 2 * h) * i / 2000.f, h);
      float step = std::hypot(point.first - previous.first, point.second - previous.second);
      minimum = std::min(minimum, step);
      maximum = std::max(maximum, step);
      previous = point;
    }
    require(maximum / minimum < 1.02f, "constant speed through rounded corners and seam");
  }
  SDL_Init(SDL_INIT_VIDEO);
  auto *s = SDL_CreateRGBSurfaceWithFormat(0, 480, 272, 32, SDL_PIXELFORMAT_ARGB8888);
  auto *r = SDL_CreateSoftwareRenderer(s);
  fmg::Engine e;
  e.reset(7);
  fmg::loop_art::render(r, e, false, "");
  std::vector<unsigned char> first(static_cast<unsigned char *>(s->pixels),
                                   static_cast<unsigned char *>(s->pixels) + s->pitch * s->h);
  e.tick(.05f, 0, 0);
  fmg::loop_art::render(r, e, false, "");
  int changed = 0;
  for (int y = 49; y < 226; ++y)
    for (int x = 27; x < 247; ++x) {
      auto i = y * s->pitch + x * 4;
      if (first[i] != static_cast<unsigned char *>(s->pixels)[i])
        ++changed;
    }
  require(changed > 150, "belt visibly moves even without collectors");
  SDL_DestroyRenderer(r);
  SDL_FreeSurface(s);
  SDL_Quit();
  std::cout << "PASS continuous belt motion and rounded path\n";
}
