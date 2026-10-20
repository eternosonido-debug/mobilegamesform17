#define main fmg_application_main
#include "../src/main.cpp"
#undef main
#include <thread>
#include <unistd.h>
static void key(SDL_Keycode code, bool down) {
  SDL_Event event{};
  event.type = down ? SDL_KEYDOWN : SDL_KEYUP;
  event.key.keysym.sym = code;
  SDL_PushEvent(&event);
}
static void press(SDL_Keycode code) {
  key(code, true);
  SDL_Delay(40);
  key(code, false);
  SDL_Delay(80);
}
int main() {
  char folder[] = "/tmp/fmg-blocks-ui-XXXXXX";
  if (!mkdtemp(folder))
    return 1;
  setenv("FMG_SAVE_DIR", folder, 1);
  Engine initial(17);
  initial.reset(1);
  std::string path = std::string(folder) + "/game-1.sav";
  if (!save(path, initial.s))
    return 1;
  std::thread input([]() {
    SDL_Delay(400);
    press(SDLK_z);
    press(SDLK_v);
    press(SDLK_z); // Undo, then place again; record must survive.
    press(SDLK_z);
    press(SDLK_x);
    press(SDLK_v);
    press(SDLK_DOWN);
    press(SDLK_z);
    press(SDLK_x);
    press(SDLK_ESCAPE);
  });
  char name[] = "blocks-ui", flag[] = "--game", game[] = "new-blocks";
  char *args[] = {name, flag, game};
  int result = fmg_application_main(3, args);
  input.join();
  State saved;
  bool ok = result == 0 && load(path, saved) && saved.game == 1 && saved.score > 0 &&
            saved.a[110] >= saved.score && validState(saved);
  std::remove(path.c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL")
            << " New Blocks placement, undo, piece selection, help and save\n";
  return ok ? 0 : 1;
}
