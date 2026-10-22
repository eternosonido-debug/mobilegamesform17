#define main pop_application_main
#include "../src/main.cpp"
#undef main
#include "solutions.hpp"
#include <thread>
#include <unistd.h>
void tap(SDL_Keycode c) {
  SDL_Event e{};
  e.type = SDL_KEYDOWN;
  e.key.keysym.sym = c;
  SDL_PushEvent(&e);
  SDL_Delay(55);
  e.type = SDL_KEYUP;
  SDL_PushEvent(&e);
  SDL_Delay(80);
}
int main() {
  char folder[] = "/tmp/pop-win-XXXXXX";
  if (!mkdtemp(folder))
    return 1;
  setenv("FMG_SAVE_DIR", folder, 1);
  std::thread worker([&]() {
    SDL_Delay(350);
    tap(SDLK_z);
    tap(SDLK_z);
    int selected = 0;
    for (int action : poptest::solutions()[0]) {
      int target = action % pop::N;
      while (selected % pop::W != target % pop::W) {
        bool right = selected % pop::W < target % pop::W;
        tap(right ? SDLK_RIGHT : SDLK_LEFT);
        selected += right ? 1 : -1;
      }
      while (selected / pop::W != target / pop::W) {
        bool down = selected / pop::W < target / pop::W;
        tap(down ? SDLK_DOWN : SDLK_UP);
        selected += down ? pop::W : -pop::W;
      }
      if (action >= pop::N)
        tap(SDLK_v);
      tap(SDLK_z);
      SDL_Delay(550);
    }
    tap(SDLK_z);
    tap(SDLK_ESCAPE);
  });
  char name[] = "pop-win";
  char *args[] = {name};
  int result = pop_application_main(1, args);
  worker.join();
  pop::Game g;
  std::string path = std::string(folder) + "/color-pop.save";
  bool ok = result == 0 && g.load(path) && g.level == 1 && g.unlocked == 1 &&
            g.state.moves == pop::levels()[1].moves;
  std::remove(path.c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL") << " SDL victory and next stage\n";
  return ok ? 0 : 1;
}
