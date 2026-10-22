#define main pop_application_main
#include "../src/main.cpp"
#undef main
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
  SDL_Delay(100);
}
int main() {
  char folder[] = "/tmp/pop-ui-XXXXXX";
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
    tap(SDLK_z);
    tap(SDLK_z);
    for (int axis : {0, 2}) {
      SDL_JoystickSetVirtualAxis(j, axis, axis == 0 ? 30000 : -30000);
      SDL_Delay(85);
      SDL_JoystickSetVirtualAxis(j, axis, 0);
      SDL_Delay(180);
    }
    tap(SDLK_RIGHT);
    tap(SDLK_z);
    SDL_Delay(550);
    tap(SDLK_x);
    tap(SDLK_v);
    tap(SDLK_RETURN);
    tap(SDLK_x);
    SDL_JoystickClose(j);
    tap(SDLK_ESCAPE);
  });
  char name[] = "pop-ui";
  char *args[] = {name};
  int result = pop_application_main(1, args);
  worker.join();
  pop::Game g;
  std::string path = std::string(folder) + "/color-pop.save";
  bool ok = result == 0 && g.load(path) && g.selected == 1 &&
            g.state.board == pop::initial(0).board &&
            g.state.moves == pop::initial(0).moves;
  std::remove(path.c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL")
            << " both sticks, group, undo, pause and save\n";
  return ok ? 0 : 1;
}
