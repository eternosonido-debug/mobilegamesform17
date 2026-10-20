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
  char folder[] = "/tmp/fmg-mahjong-ui-XXXXXX";
  if (!mkdtemp(folder))
    return 1;
  setenv("FMG_SAVE_DIR", folder, 1);
  Engine initial(17);
  initial.reset(5);
  std::string path = std::string(folder) + "/game-5.sav";
  if (!save(path, initial.s))
    return 1;
  std::thread input([]() {
    SDL_Delay(400);
    press(SDLK_v);
    press(SDLK_DOWN);
    press(SDLK_z); // Hint highlights a legal pair.
    press(SDLK_z);
    press(SDLK_v);
    press(SDLK_z);
    press(SDLK_v);
    press(SDLK_DOWN);
    press(SDLK_z);
    press(SDLK_z);
    press(SDLK_ESCAPE);
  });
  char name[] = "mahjong-ui", flag[] = "--game", game[] = "mahjong";
  char *args[] = {name, flag, game};
  int result = fmg_application_main(3, args);
  input.join();
  State saved;
  bool ok =
      result == 0 && load(path, saved) && saved.game == 5 && saved.score == 2 && validState(saved);
  std::remove(path.c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL") << " Mahjong hint, pair, undo and save\n";
  return ok ? 0 : 1;
}
