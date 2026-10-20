#include "renderer.hpp"
#include <iostream>
#include <stdexcept>
void require(bool b, const char *m) {
  if (!b)
    throw std::runtime_error(m);
}
int main() {
  SDL_Init(SDL_INIT_VIDEO);
  auto *s =
      SDL_CreateRGBSurfaceWithFormat(0, 240, 136, 32, SDL_PIXELFORMAT_ARGB8888);
  auto *r = SDL_CreateSoftwareRenderer(s);
  SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
  blockout::ui::Canvas p(r);
  for (int boardSize : {6, 8})
    for (int edge = 0; edge < 4; ++edge) {
      int cell = 96 / boardSize;
      int dx = edge == 0   ? -1
               : edge == 1 ? 1
                           : 0,
          dy = edge == 2   ? -1
               : edge == 3 ? 1
                           : 0;
      blockout::Block b(edge == 0   ? 0
                        : edge == 1 ? boardSize - 2
                                    : 2,
                        edge == 2   ? 0
                        : edge == 3 ? boardSize - 2
                                    : 2,
                        edge < 2 ? 2 : 1, edge < 2 ? 1 : 2, 1, true);
      blockout::Level l;
      l.size = boardSize;
      l.doors.push_back({edge, 2, 1, 1});
      l.doors.push_back({(edge + 1) % 4, 0, 1, 1});
      float t = .35f,
            travel = std::min(1.f, t / .78f) * ((dx ? b.w : b.h) * cell + 6);
      p.paint(blockout::ui::Paint(20, 30, 40));
      SDL_RenderClear(r);
      blockout::exitBrick(p, r, b, l, t, edge);
      SDL_RenderPresent(r);
      std::vector<Uint32> actual(static_cast<Uint32 *>(s->pixels),
                                 static_cast<Uint32 *>(s->pixels) + 240 * 136);
      p.paint(blockout::ui::Paint(20, 30, 40));
      SDL_RenderClear(r);
      SDL_Rect clip{12, 25, 96, 96};
      SDL_RenderSetClipRect(r, &clip);
      blockout::brick(p, 12 + b.x * cell + dx * travel,
                      25 + b.y * cell + dy * travel, b.w * cell - 1,
                      b.h * cell - 1, b.color, false, 1, cell);
      SDL_RenderSetClipRect(r, nullptr);
      SDL_RenderPresent(r);
      auto *reference = static_cast<Uint32 *>(s->pixels);
      int outsideChanges = 0;
      for (int y = 0; y < 136; ++y)
        for (int x = 0; x < 240; ++x) {
          if (x >= 12 && x < 108 && y >= 25 && y < 121)
            require(actual[y * 240 + x] == reference[y * 240 + x],
                    "inside remains exact opaque brick");
          else
            outsideChanges += actual[y * 240 + x] != reference[y * 240 + x];
        }
      require(outsideChanges > 0, "fragments appear beyond gate");
      require(!SDL_RenderIsClipEnabled(r), "clip state restored");
    }
  SDL_DestroyRenderer(r);
  SDL_FreeSurface(s);
  SDL_Quit();
  std::cout << "PASS four exit directions: intact inside and fragments only "
               "outside\n";
}
