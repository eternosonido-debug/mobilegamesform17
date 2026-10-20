#include "boca_ground.hpp"
#include "boca_sprites.hpp"
#include <iostream>
int main() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Surface *target = SDL_CreateRGBSurfaceWithFormat(0, 480, 272, 32, SDL_PIXELFORMAT_ARGB8888);
  SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(target);
  fmg::boca_sprites::initialize(renderer);
  bool ok = fmg::boca_sprites::atlas().texture != nullptr;
  int ids[] = {103, 100, 130, 150, 168}, kinds[] = {2, 2, 3, 4, 5};
  for (int id = 100; id < 150; ++id)
    ok = ok && fmg::boca_sprites::spriteIndex(id, id < 130 ? 2 : 3) >= 0;
  for (int id = 150; id < 180; ++id)
    ok = ok && fmg::boca_sprites::spriteIndex(id, id < 168 ? 4 : 5) >= 0;
  for (int i = 0; i < 5; ++i)
    ok = fmg::boca_sprites::draw(renderer, ids[i], kinds[i], 80 + i * 80, 150, 25) && ok;
  std::string path = std::string(std::getenv("FMG_ASSET_DIR")) + "/boca-atlas-v2.bmp";
  SDL_Surface *source = SDL_LoadBMP(path.c_str());
  if (!source)
    ok = false;
  else {
    Uint8 r, g, b, a;
    SDL_GetRGBA(*static_cast<Uint32 *>(source->pixels), source->format, &r, &g, &b, &a);
    ok = ok && source->w == 1254 && source->h == 1254 && a == 0;
    SDL_FreeSurface(source);
  }
  fmg::boca_sprites::release();
  fmg::boca_sprites::initialize(renderer);
  ok = ok && fmg::boca_sprites::atlas().texture;
  for (float zoom : {1.f, .36f, .18f}) {
    for (float x : {100.f, 1600.f, 3100.f})
      ok = fmg::boca_ground::draw(renderer, x, 1000.f, zoom) && ok;
  }
  fmg::boca_ground::release();
  ok = fmg::boca_ground::draw(renderer, 1600.f, 1000.f, 1.f) && ok;
  fmg::boca_ground::release();
  fmg::boca_sprites::release();
  SDL_DestroyRenderer(renderer);
  SDL_FreeSurface(target);
  SDL_Quit();
  std::cout << (ok ? "PASS" : "FAIL") << " atlas transparency, five sprites and reload\n";
  return ok ? 0 : 1;
}
