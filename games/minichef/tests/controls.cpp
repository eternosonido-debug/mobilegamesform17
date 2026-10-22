#define main minichef_application_main
#include "../src/main.cpp"
#undef main
#include <thread>
#include <unistd.h>
void press(SDL_Keycode key) {
  SDL_Event e{};
  e.type = SDL_KEYDOWN;
  e.key.keysym.sym = key;
  SDL_PushEvent(&e);
  SDL_Delay(55);
  e.type = SDL_KEYUP;
  SDL_PushEvent(&e);
  SDL_Delay(100);
}
int main() {
  char folder[] = "/tmp/minichef-ui-XXXXXX";
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
    press(SDLK_z); // Menu, tutorial, pantry.
    for (int axis : {0, 2}) {
      SDL_JoystickSetVirtualAxis(j, axis, 30000);
      SDL_Delay(90);
      SDL_JoystickSetVirtualAxis(j, axis, 0);
      SDL_Delay(180);
    }
    press(SDLK_z); // Potato, selected by two sticks.
    press(SDLK_v);
    press(SDLK_x); // Help and back.
    press(SDLK_RETURN);
    SDL_Delay(120);
    press(SDLK_x); // Pause, resume.
    press(SDLK_ESCAPE);
    press(SDLK_z); // Confirm saved return to menu.
    press(SDLK_ESCAPE);
    SDL_JoystickClose(j);
  });
  char name[] = "chef-ui";
  char *args[] = {name};
  int result = minichef_application_main(1, args);
  worker.join();
  chef::Game g;
  std::string save = std::string(folder) + "/minichef.save";
  bool ok = result == 0 && g.load(save) && g.s.selected == 2 &&
            g.s.basket == 4 && g.s.step == chef::Pantry;
  std::remove(save.c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL")
            << " both sticks, A, help, pause, confirmed exit and save\n";
  return ok ? 0 : 1;
}
