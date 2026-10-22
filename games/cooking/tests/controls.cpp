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
  char folder[] = "/tmp/cooking-ui-XXXXXX";
  if (!mkdtemp(folder))
    return 1;
  setenv("FMG_SAVE_DIR", folder, 1);
  SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_EVENTS);
  int idx = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_UNKNOWN, 4, 8, 0);
  auto *j = SDL_JoystickOpen(idx);
  if (!j)
    return 2;
  std::thread worker([&]() {
    SDL_Delay(350);
    press(SDLK_z);
    press(SDLK_z);
    for (int axis : {0, 2}) {
      SDL_JoystickSetVirtualAxis(j, axis, 30000);
      SDL_Delay(100);
      SDL_JoystickSetVirtualAxis(j, axis, 0);
      SDL_Delay(180);
    }
    press(SDLK_z);
    SDL_Delay(1250);
    press(SDLK_v);
    press(SDLK_x);
    press(SDLK_RETURN);
    press(SDLK_x);
    press(SDLK_ESCAPE);
    press(SDLK_z);
    press(SDLK_ESCAPE);
    SDL_JoystickClose(j);
  });
  char name[] = "cooking-ui";
  char *args[] = {name};
  int result = cooking_application_main(1, args);
  worker.join();
  Game saved;
  std::string path = std::string(folder) + "/cooking.save";
  bool ok = result == 0 && saved.load(path) && saved.s.selected == 2 &&
            saved.s.used == 4 && saved.s.pending == Idle;
  std::remove(path.c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL")
            << " two sticks, pour animation, help, pause and saved exit\n";
  return ok ? 0 : 1;
}
