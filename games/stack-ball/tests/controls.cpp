#define main tower_application_main
#include "../src/main.cpp"
#undef main
#include <thread>
#include <unistd.h>
void tap(SDL_Keycode c, int hold = 55) {
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
  char folder[] = "/tmp/tower-ui-XXXXXX";
  if (!mkdtemp(folder))
    return 1;
  setenv("FMG_SAVE_DIR", folder, 1);
  SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_EVENTS);
  int index = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_UNKNOWN, 4, 8, 0);
  auto *j = SDL_JoystickOpen(index);
  if (!j)
    return 2;
  {
    tower::Input input;
    SDL_JoystickSetVirtualButton(j, 0, 1);
    SDL_JoystickUpdate();
    if (!input.primary())
      return 3;
    SDL_JoystickSetVirtualButton(j, 0, 0);
    SDL_JoystickUpdate();
    if (input.primary())
      return 4;
    setenv("FMG_BUTTON_A", "3", 1);
    SDL_JoystickSetVirtualButton(j, 3, 1);
    SDL_JoystickUpdate();
    if (!input.primary())
      return 6;
    SDL_JoystickSetVirtualButton(j, 3, 0);
    SDL_JoystickUpdate();
    unsetenv("FMG_BUTTON_A");
    setenv("FMG_KEY_A", "Q", 1);
    SDL_Event held{};
    held.type = SDL_KEYDOWN;
    held.key.keysym.sym = SDLK_q;
    input.event(held);
    if (!input.primary())
      return 7;
    held.type = SDL_WINDOWEVENT;
    held.window.event = SDL_WINDOWEVENT_FOCUS_LOST;
    input.event(held);
    if (input.primary())
      return 8;
    unsetenv("FMG_KEY_A");
    for (int axis : {0, 2}) {
      SDL_JoystickSetVirtualAxis(j, axis, 30000);
      SDL_JoystickUpdate();
      if (input.direction().x != 1)
        return 5;
      SDL_JoystickSetVirtualAxis(j, axis, 0);
      SDL_JoystickUpdate();
    }
  }
  std::thread worker([&]() {
    SDL_Delay(350);
    tap(SDLK_z);
    tap(SDLK_z);
    tap(SDLK_z, 230);
    SDL_Delay(250);
    tap(SDLK_RETURN);
    tap(SDLK_x);
    tap(SDLK_ESCAPE);
    tap(SDLK_z);
    SDL_JoystickClose(j);
  });
  char name[] = "tower-ui";
  char *args[] = {name};
  int result = tower_application_main(1, args);
  worker.join();
  tower::Game g;
  std::string path = std::string(folder) + "/tower-break.save";
  bool ok = result == 0 && g.load(path) && g.state.layer >= 1 && !g.state.dead;
  std::remove(path.c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL")
            << " held A, both sticks, pause, exit confirmation and save\n";
  return ok ? 0 : 1;
}
