#pragma once
#include "game.hpp"
#include <SDL.h>
#include <cstdlib>
#include <string>
namespace fmg {
namespace boca_ground {
static const int width = 1840, height = 1042;
static const float bakeScale = .5f, minimumX = -1415.f;
inline Uint32 colorAt(float wx, float wy) {
  auto rgb = [](int r, int g, int b) { return Uint32(0xff000000u | (r << 16) | (g << 8) | b); };
  if (islandMetric(wx, wy, -32) > 1) {
    bool wave = (wy - std::floor(wy / 140) * 140 < 3 && wx - std::floor(wx / 220) * 220 < 42);
    return wave ? rgb(86, 167, 187) : rgb(55, 136, 165);
  }
  if (islandMetric(wx, wy, -12) > 1)
    return rgb(104, 192, 195);
  if (islandMetric(wx, wy, 0) > 1)
    return rgb(218, 236, 209);
  if (islandMetric(wx, wy, 46) > 1)
    return rgb(232, 212, 155);
  float mx = cityModulo(wx), my = cityModulo(wy);
  if (mx < cityRoadWidth || my < cityRoadWidth) {
    if ((mx >= 54 && mx < 58 && int(wy) % 64 < 28) || (my >= 54 && my < 58 && int(wx) % 64 < 28))
      return rgb(244, 234, 198);
    if ((mx > 88 && mx < 112 && my < 112 && int(my) % 16 < 7) ||
        (my > 88 && my < 112 && mx < 112 && int(mx) % 16 < 7))
      return rgb(224, 229, 211);
    return rgb(141, 158, 160);
  }
  if (mx < citySidewalk || my < citySidewalk)
    return rgb(222, 216, 190);
  if (mx < citySidewalk + 3 || my < citySidewalk + 3)
    return rgb(151, 176, 145);
  return (int(wx / 400) + int(wy / 400)) % 2 ? rgb(177, 200, 151) : rgb(184, 206, 158);
}
struct Cache {
  SDL_Texture *texture = nullptr;
  bool attempted = false;
};
inline Cache &cache() {
  static Cache value;
  return value;
}
inline void release() {
  auto &c = cache();
  if (c.texture)
    SDL_DestroyTexture(c.texture);
  c.texture = nullptr;
  c.attempted = false;
}
inline void initialize(SDL_Renderer *renderer) {
  auto &c = cache();
  if (c.attempted)
    return;
  c.attempted = true;
  const char *folder = std::getenv("FMG_ASSET_DIR");
  std::string path;
  if (folder)
    path = std::string(folder) + "/boca-ground.bmp";
  else {
    char *base = SDL_GetBasePath();
    path = base ? std::string(base) + "../assets/boca-ground.bmp" : "assets/boca-ground.bmp";
    SDL_free(base);
  }
  SDL_Surface *surface = SDL_LoadBMP(path.c_str());
  if (!surface && !folder)
    surface = SDL_LoadBMP("/sdcard/fmg-test/assets/boca-ground.bmp");
  if (!surface) {
    SDL_Log("Boca ground cache unavailable; procedural fallback: %s", SDL_GetError());
    return;
  }
  if (surface->w != width || surface->h != height) {
    SDL_FreeSurface(surface);
    return;
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  c.texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  SDL_FreeSurface(surface);
  if (c.texture)
    SDL_SetTextureBlendMode(c.texture, SDL_BLENDMODE_NONE);
}
inline bool draw(SDL_Renderer *renderer, float playerX, float playerY, float zoom) {
  initialize(renderer);
  if (!cache().texture)
    return false;
  float px = (playerX - playerY) * .70710678f, py = (playerX + playerY) * .4f;
  SDL_FRect target{240 + (minimumX - px) * zoom, 138 - py * zoom, width / bakeScale * zoom,
                   height / bakeScale * zoom};
  // Crop before scaling: SDL's software renderer otherwise scales the whole island.
  float scale = zoom / bakeScale;
  int left = std::max(0, int(std::floor((0 - target.x) / scale)));
  int top = std::max(0, int(std::floor((44 - target.y) / scale)));
  int right = std::min(width, int(std::ceil((480 - target.x) / scale)));
  int bottom = std::min(height, int(std::ceil((233 - target.y) / scale)));
  if (right > left && bottom > top) {
    SDL_Rect source{left, top, right - left, bottom - top};
    SDL_FRect visible{target.x + left * scale, target.y + top * scale, source.w * scale,
                      source.h * scale};
    SDL_RenderCopyF(renderer, cache().texture, &source, &visible);
  }
  return true;
}
} // namespace boca_ground
} // namespace fmg
