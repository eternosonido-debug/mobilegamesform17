#define main rings_application_main
#include "../src/main.cpp"
#undef main
#include <thread>
#include <unistd.h>
void tap(SDL_Keycode c) {
  SDL_Event e{};
  e.type = SDL_KEYDOWN;
  e.key.keysym.sym = c;
  SDL_PushEvent(&e);
  SDL_Delay(65);
  e.type = SDL_KEYUP;
  SDL_PushEvent(&e);
  SDL_Delay(105);
}
int main() {
  char folder[] = "/tmp/rings-win-XXXXXX";
  if (!mkdtemp(folder))
    return 1;
  setenv("FMG_SAVE_DIR", folder, 1);
  std::thread worker([&]() {
    SDL_Delay(350);
    tap(SDLK_z);
    tap(SDLK_z);
    rings::Game model;
    for (auto move : rings::solution(0)) {
      if (model.selected != move.ring) {
        tap(move.ring == 0 ? SDLK_LEFT : SDLK_RIGHT);
        model.selected = move.ring;
      }
      tap(move.direction == 1 ? SDLK_z : SDLK_x);
      unsigned before = model.state.active;
      model.action(move.direction);
      SDL_Delay(before != model.state.active ? 700 : 80);
      model.selectAlive();
    }
    tap(SDLK_z);
    tap(SDLK_ESCAPE);
  });
  char name[] = "rings-win";
  char *args[] = {name};
  int result = rings_application_main(1, args);
  worker.join();
  rings::Game g;
  std::string path = std::string(folder) + "/rotate-rings.save";
  bool ok = result == 0 && g.load(path) && g.level == 1 && g.unlocked == 1 &&
            g.state.moves == 0;
  std::remove(path.c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL")
            << " real SDL victory, unlock and next level\n";
  return ok ? 0 : 1;
}
