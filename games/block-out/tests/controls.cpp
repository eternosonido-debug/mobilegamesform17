#define main blockout_application_main
#include "../src/main.cpp"
#undef main
#include <thread>
#include <unistd.h>
void press(SDL_Keycode c) {
  SDL_Event e{};
  e.type = SDL_KEYDOWN;
  e.key.keysym.sym = c;
  SDL_PushEvent(&e);
  SDL_Delay(35);
  e.type = SDL_KEYUP;
  SDL_PushEvent(&e);
  SDL_Delay(85);
}
int main() {
  char folder[] = "/tmp/block-out-ui-XXXXXX";
  if (!mkdtemp(folder))
    return 1;
  setenv("FMG_SAVE_DIR", folder, 1);
  SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_EVENTS);
  int index = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_UNKNOWN, 4, 6, 0);
  auto *j = SDL_JoystickOpen(index);
  std::thread input([&]() {
    SDL_Delay(350);
    press(SDLK_z);
    press(SDLK_z);
    for (int axis : {0, 2}) {
      SDL_JoystickSetVirtualAxis(j, axis, 30000);
      SDL_Delay(85);
      SDL_JoystickSetVirtualAxis(j, axis, 0);
      SDL_Delay(90);
    }
    press(SDLK_v);
    press(SDLK_DOWN);
    press(SDLK_z);
    SDL_JoystickClose(j);
    press(SDLK_ESCAPE);
  });
  char name[] = "block-out-ui";
  char *args[] = {name};
  int result = blockout_application_main(1, args);
  input.join();
  blockout::Game saved;
  std::string path = std::string(folder) + "/block-out.sav";
  bool ok = result == 0 && saved.load(path) && saved.board.moves == 1 &&
            saved.board.blocks[0].x == 1;
  std::remove(path.c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL")
            << " A selection, both sticks, Y extras, undo, exit and resume\n";
  return ok ? 0 : 1;
}
