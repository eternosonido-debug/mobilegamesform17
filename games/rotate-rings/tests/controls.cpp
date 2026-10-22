#define main rings_application_main
#include "../src/main.cpp"
#undef main
#include <thread>
#include <unistd.h>
void press(SDL_Keycode c) {
  SDL_Event e{};
  e.type = SDL_KEYDOWN;
  e.key.keysym.sym = c;
  SDL_PushEvent(&e);
  SDL_Delay(60);
  e.type = SDL_KEYUP;
  SDL_PushEvent(&e);
  SDL_Delay(115);
}
int main() {
  char folder[] = "/tmp/rings-ui-XXXXXX";
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
    for (int axis : {0, 2}) {
      SDL_JoystickSetVirtualAxis(j, axis, axis == 0 ? 30000 : -30000);
      SDL_Delay(85);
      SDL_JoystickSetVirtualAxis(j, axis, 0);
      SDL_Delay(180);
      press(SDLK_z);
    }
    press(SDLK_x);
    press(SDLK_v);
    press(SDLK_RETURN);
    press(SDLK_x);
    SDL_JoystickClose(j);
    press(SDLK_ESCAPE);
  });
  char name[] = "rings-ui";
  char *args[] = {name};
  int result = rings_application_main(1, args);
  worker.join();
  rings::Game saved;
  std::string path = std::string(folder) + "/rotate-rings.save";
  bool ok = result == 0 && saved.load(path) && saved.selected == 0 &&
            saved.state.moves == 2 &&
            saved.state.angle[0] == rings::initial(0).angle[0] + 1 &&
            saved.state.angle[1] == rings::initial(0).angle[1] + 1;
  std::remove(path.c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL")
            << " both sticks, A/B rotation, undo, pause, exit and save\n";
  return ok ? 0 : 1;
}
