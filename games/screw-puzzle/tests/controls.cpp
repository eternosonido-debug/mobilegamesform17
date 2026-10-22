#define main screw_application_main
#include "../src/main.cpp"
#undef main
#include <thread>
#include <unistd.h>
void press(SDL_Keycode c) {
  SDL_Event e{};
  e.type = SDL_KEYDOWN;
  e.key.keysym.sym = c;
  SDL_PushEvent(&e);
  SDL_Delay(55);
  e.type = SDL_KEYUP;
  SDL_PushEvent(&e);
  SDL_Delay(90);
}
int main() {
  char folder[] = "/tmp/screw-ui-XXXXXX";
  if (!mkdtemp(folder))
    return 1;
  setenv("FMG_SAVE_DIR", folder, 1);
  SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_EVENTS);
  int index = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_UNKNOWN, 4, 8, 0);
  auto *j = SDL_JoystickOpen(index);
  if (!j)
    return 2;
  std::thread worker([&]() {
    SDL_Delay(350);
    press(SDLK_z);
    press(SDLK_z);
    press(SDLK_z);
    SDL_Delay(550);
    for (int axis : {0, 2}) {
      SDL_JoystickSetVirtualAxis(j, axis, axis == 0 ? 30000 : -30000);
      SDL_Delay(80);
      SDL_JoystickSetVirtualAxis(j, axis, 0);
      SDL_Delay(180);
    }
    press(SDLK_DOWN);
    press(SDLK_DOWN);
    press(SDLK_z);
    SDL_Delay(600);
    press(SDLK_v);
    press(SDLK_x);
    press(SDLK_RETURN);
    press(SDLK_x);
    SDL_JoystickClose(j);
    press(SDLK_ESCAPE);
  });
  char name[] = "screw-ui";
  char *args[] = {name};
  int result = screw_application_main(1, args);
  worker.join();
  screw::Game saved;
  std::string path = std::string(folder) + "/screw-puzzle.save";
  bool ok = result == 0 && saved.load(path) && saved.selected == 8 &&
            !saved.state.hand && saved.state.moves == 0;
  std::remove(path.c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL")
            << " both sticks, keyboard, undo, pause, save and resume\n";
  return ok ? 0 : 1;
}
