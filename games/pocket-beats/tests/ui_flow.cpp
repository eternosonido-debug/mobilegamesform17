#define main pocket_application_main
#include "../src/main.cpp"
#undef main
#include <thread>
#include <unistd.h>
void press(SDL_Keycode key) {
  SDL_Event e{};
  e.type = SDL_KEYDOWN;
  e.key.keysym.sym = key;
  SDL_PushEvent(&e);
  SDL_Delay(45);
  e.type = SDL_KEYUP;
  SDL_PushEvent(&e);
  SDL_Delay(80);
}
int main() {
  char dir[] = "/tmp/pocket-beats-flow-XXXXXX";
  if (!mkdtemp(dir))
    return 1;
  setenv("FMG_SAVE_DIR", dir, 1);
  std::thread user([] {
    SDL_Delay(2400);
    press(SDLK_z);
    press(SDLK_x);
    press(SDLK_z);
    press(SDLK_RIGHT);
    press(SDLK_c);
    press(SDLK_z);
    press(SDLK_RETURN);
    SDL_Delay(200);
    press(SDLK_ESCAPE);
    for (int i = 0; i < 7; ++i)
      press(SDLK_DOWN);
    press(SDLK_z);
  });
  char name[] = "pocket-ui";
  char *args[] = {name};
  int result = pocket_application_main(1, args);
  user.join();
  pocketbeats::Session loaded;
  std::string path = std::string(dir) + "/pocket-beats.sav";
  bool ok = result == 0 && loaded.load(path) &&
            loaded.current().notes[0][0] == 0 &&
            loaded.current().chosen[0][0] == 64 &&
            loaded.current().notes[0][1] == 16 && loaded.cursor == 1;
  std::remove(path.c_str());
  rmdir(dir);
  std::cout
      << (ok ? "PASS" : "FAIL")
      << " SDL window flow: choose percussion, A off/on, play, menu and save\n";
  return ok ? 0 : 1;
}
