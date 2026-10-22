#define main tower_application_main
#include "../src/main.cpp"
#undef main
#include <thread>
#include <unistd.h>
void tap(SDL_Keycode c, int hold = 65) {
  SDL_Event e{};
  e.type = SDL_KEYDOWN;
  e.key.keysym.sym = c;
  SDL_PushEvent(&e);
  SDL_Delay(hold);
  e.type = SDL_KEYUP;
  SDL_PushEvent(&e);
  SDL_Delay(100);
}
int main() {
  char folder[] = "/tmp/tower-win-XXXXXX";
  if (!mkdtemp(folder))
    return 1;
  setenv("FMG_SAVE_DIR", folder, 1);
  std::string path = std::string(folder) + "/tower-break.save";
  tower::Game fixture;
  fixture.state.layer = 17;
  fixture.state.score = 170;
  fixture.state.z = 1;
  fixture.state.velocity = -100;
  fixture.state.angle = 0;
  if (!fixture.save(path))
    return 2;
  std::thread worker([&]() {
    SDL_Delay(350);
    tap(SDLK_z);
    tap(SDLK_z, 300);
    SDL_Delay(700);
    tap(SDLK_z);
    tap(SDLK_ESCAPE);
    tap(SDLK_z);
  });
  char name[] = "tower-win";
  char *args[] = {name};
  int result = tower_application_main(1, args);
  worker.join();
  tower::Game g;
  bool ok = result == 0 && g.load(path) && g.stage == 1 && g.unlocked == 1 &&
            !g.state.dead;
  std::remove(path.c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL")
            << " resume near base, win, unlock and next tower\n";
  return ok ? 0 : 1;
}
