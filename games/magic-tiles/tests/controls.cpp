#define main tiles_application_main
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
  SDL_Delay(85);
}
int main() {
  char folder[] = "/tmp/tiles-ui-XXXXXX";
  if (!mkdtemp(folder))
    return 1;
  setenv("FMG_SAVE_DIR", folder, 1);
  SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_EVENTS);
  int index = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_UNKNOWN, 4, 8, 0);
  auto *j = SDL_JoystickOpen(index);
  if (!j)
    return 2;
  {
    tiles::Input input;
    for (int axis : {0, 2}) {
      SDL_JoystickSetVirtualAxis(j, axis, 30000);
      SDL_JoystickUpdate();
      if (tiles::directionalLane(input.direction().x, 0) != 3)
        return 3;
      SDL_JoystickSetVirtualAxis(j, axis, 0);
      SDL_JoystickUpdate();
    }
  }
  std::thread worker([&]() {
    SDL_Delay(400);
    tap(SDLK_DOWN);
    tap(SDLK_z);
    tap(SDLK_DOWN);
    tap(SDLK_z);
    tap(SDLK_DOWN);
    tap(SDLK_RIGHT);
    tap(SDLK_DOWN);
    tap(SDLK_z);
    tap(SDLK_DOWN);
    tap(SDLK_z);
    for (int i = 0; i < 5; ++i)
      tap(SDLK_UP);
    tap(SDLK_z);
    tap(SDLK_z);
    SDL_Delay(300);
    tap(SDLK_x);
    tap(SDLK_c);
    tap(SDLK_v);
    tap(SDLK_RETURN);
    tap(SDLK_x);
    SDL_JoystickClose(j);
    tap(SDLK_ESCAPE);
  });
  char name[] = "tiles-ui";
  char *args[] = {name};
  int result = tiles_application_main(1, args);
  worker.join();
  tiles::Settings g;
  std::string path = std::string(folder) + "/magic-tiles.save";
  bool ok = result == 0 && g.load(path) && g.song == 1 && g.difficulty == 1 &&
            g.offset == 5 && g.practice && g.muted;
  std::remove(path.c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL")
            << " song, difficulty, calibration, both sticks, four lanes, pause "
               "and save\n";
  return ok ? 0 : 1;
}
