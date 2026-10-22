#define main minichef_application_main
#include "../src/main.cpp"
#undef main
#include <thread>
#include <unistd.h>
void tap(SDL_Keycode key) {
  SDL_Event e{};
  e.type = SDL_KEYDOWN;
  e.key.keysym.sym = key;
  SDL_PushEvent(&e);
  SDL_Delay(55);
  e.type = SDL_KEYUP;
  SDL_PushEvent(&e);
  SDL_Delay(130);
}
int main() {
  char folder[] = "/tmp/minichef-win-XXXXXX";
  if (!mkdtemp(folder))
    return 1;
  setenv("FMG_SAVE_DIR", folder, 1);
  std::string path = std::string(folder) + "/minichef.save";
  chef::Game g;
  g.s.step = chef::Plate;
  g.s.basket = 7;
  g.s.cuts = 6;
  g.s.turns = 8;
  g.s.goodTicks = 600;
  g.s.stirs = 4;
  g.s.ticks = 1800;
  g.s.temperature = 500;
  if (!g.save(path))
    return 2;
  std::thread worker([]() {
    SDL_Delay(350);
    tap(SDLK_z); // Continue saved plating.
    tap(SDLK_z);
    tap(SDLK_RIGHT);
    tap(SDLK_z);
    tap(SDLK_z); // Three decorations.
    tap(SDLK_z); // Next recipe, tutorial.
    tap(SDLK_ESCAPE);
    tap(SDLK_z);
    tap(SDLK_ESCAPE); // Confirm menu, exit.
  });
  char name[] = "chef-win";
  char *args[] = {name};
  int result = minichef_application_main(1, args);
  worker.join();
  chef::Game saved;
  bool ok = result == 0 && saved.load(path) && saved.unlocked == 1 &&
            saved.stars[0] == 3 && saved.s.level == 1 &&
            saved.s.step == chef::Pantry;
  std::remove(path.c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL")
            << " resume, decoration, victory, next recipe and saved unlock\n";
  return ok ? 0 : 1;
}
