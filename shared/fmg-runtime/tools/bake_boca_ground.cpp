#include "boca_ground.hpp"
#include <iostream>
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  auto *surface = SDL_CreateRGBSurfaceWithFormat(
      0, fmg::boca_ground::width, fmg::boca_ground::height, 32, SDL_PIXELFORMAT_ARGB8888);
  if (!surface)
    return 1;
  for (int y = 0; y < surface->h; ++y) {
    auto *row =
        reinterpret_cast<Uint32 *>(static_cast<Uint8 *>(surface->pixels) + y * surface->pitch);
    for (int x = 0; x < surface->w; ++x) {
      float px = fmg::boca_ground::minimumX + (x + .5f) / fmg::boca_ground::bakeScale,
            py = (y + .5f) / fmg::boca_ground::bakeScale;
      float wx = (px / .70710678f + py / .4f) * .5f, wy = (py / .4f - px / .70710678f) * .5f;
      row[x] = fmg::boca_ground::colorAt(wx, wy);
    }
  }
  int result = SDL_SaveBMP(surface, argv[1]);
  SDL_FreeSurface(surface);
  if (result)
    std::cerr << SDL_GetError() << "\n";
  return result ? 1 : 0;
}
