#pragma once
#include <SDL.h>
#include <algorithm>
#include <cstdlib>
#include <string>
namespace fmg {
namespace boca_sprites {
struct Atlas {
  SDL_Texture *texture = nullptr;
  bool attempted = false;
};
inline Atlas &atlas() {
  static Atlas value;
  return value;
}
inline void release() {
  auto &a = atlas();
  if (a.texture)
    SDL_DestroyTexture(a.texture);
  a.texture = nullptr;
  a.attempted = false;
}
inline void initialize(SDL_Renderer *renderer) {
  auto &a = atlas();
  if (a.attempted)
    return;
  a.attempted = true;
  std::string path;
  const char *folder = std::getenv("FMG_ASSET_DIR");
  if (folder)
    path = std::string(folder) + "/boca-atlas-v2.bmp";
  else {
    char *base = SDL_GetBasePath();
    if (base) {
      path = std::string(base) + "../assets/boca-atlas-v2.bmp";
      SDL_free(base);
    } else
      path = "assets/boca-atlas-v2.bmp";
  }
  SDL_Surface *surface = SDL_LoadBMP(path.c_str());
  if (!surface && !folder)
    surface = SDL_LoadBMP("/sdcard/fmg-test/assets/boca-atlas-v2.bmp");
  if (!surface) {
    SDL_Log("Boca sprite atlas unavailable; using procedural artwork: %s", SDL_GetError());
    return;
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  a.texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  SDL_FreeSurface(surface);
  if (a.texture)
    SDL_SetTextureBlendMode(a.texture, SDL_BLENDMODE_BLEND);
}
inline int spriteIndex(int id, int kind) {
  if (kind == 2) {
    const int variants[] = {1, 0, 2, 8, 3, 1};
    return variants[id % 6];
  }
  if (kind == 3)
    return id % 4 == 2 ? 4 : 5;
  if (kind == 4)
    return 6;
  if (kind == 5)
    return 7;
  return -1;
}
inline bool available(SDL_Renderer *renderer, int id, int kind) {
  if (spriteIndex(id, kind) < 0)
    return false;
  initialize(renderer);
  return atlas().texture != nullptr;
}
inline bool draw(SDL_Renderer *renderer, int id, int kind, float x, float y, float radius,
                 bool flip = false) {
  int sprite = spriteIndex(id, kind);
  if (sprite < 0)
    return false;
  initialize(renderer);
  if (!atlas().texture)
    return false;
  const SDL_Rect bounds[] = {{63, 105, 330, 265}, {473, 125, 338, 244}, {884, 123, 327, 247},
                             {72, 487, 340, 297}, {470, 463, 344, 315}, {887, 474, 325, 317},
                             {73, 860, 312, 310}, {505, 836, 270, 330}, {878, 892, 339, 277}};
  SDL_Rect source = bounds[sprite];
  float width = radius * (kind == 3   ? 2.2f
                          : kind == 5 ? 1.65f
                                      : 2.1f),
        height = width * source.h / source.w;
  SDL_Rect destination{int(x - width * .5f), int(y + radius * .65f - height),
                       std::max(1, int(width)), std::max(1, int(height))};
  SDL_RenderCopyEx(renderer, atlas().texture, &source, &destination, 0, nullptr,
                   flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
  return true;
}
} // namespace boca_sprites
} // namespace fmg
