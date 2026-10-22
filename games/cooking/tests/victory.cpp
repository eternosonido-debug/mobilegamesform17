#define main cooking_application_main
#include "../src/main.cpp"
#undef main
#include <thread>
#include <unistd.h>
using namespace cooking;
void press(SDL_Keycode key) {
  SDL_Event e{};
  e.type = SDL_KEYDOWN;
  e.key.keysym.sym = key;
  SDL_PushEvent(&e);
  SDL_Delay(65);
  e.type = SDL_KEYUP;
  SDL_PushEvent(&e);
  SDL_Delay(110);
}
int main() {
  char folder[] = "/tmp/cooking-win-XXXXXX";
  if (!mkdtemp(folder))
    return 1;
  setenv("FMG_SAVE_DIR", folder, 1);
  Game g = demo(2, Decorating);
  std::string path = std::string(folder) + "/cooking.save";
  if (!g.save(path))
    return 2;
  std::thread worker([]() {
    SDL_Delay(350);
    press(SDLK_z);
    press(SDLK_UP);
    press(SDLK_z);
    press(SDLK_x);
    press(SDLK_v);
    for (int i = 0; i < 5; ++i)
      press(SDLK_RIGHT);
    press(SDLK_z);
    SDL_Delay(250);
    press(SDLK_ESCAPE);
    press(SDLK_z);
    press(SDLK_ESCAPE);
  });
  char name[] = "cooking-win";
  char *args[] = {name};
  int result = cooking_application_main(1, args);
  worker.join();
  Game saved;
  bool ok = result == 0 && saved.load(path) && saved.s.stage == Photo &&
            saved.serial == 1 && saved.albumCount == 1 &&
            saved.s.cakes[0].icing == 3 && saved.made[2] == 1;
  std::remove(path.c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL")
            << " decoration, undo, switch cake, photo and album saved\n";
  return ok ? 0 : 1;
}
