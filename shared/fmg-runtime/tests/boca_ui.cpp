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
  char folder[] = "/tmp/fmg-boca-ui-XXXXXX";
  if (!mkdtemp(folder))
    return 1;
  setenv("FMG_SAVE_DIR", folder, 1);
  Engine initial(17);
  initial.reset(6);
  std::string path = std::string(folder) + "/game-6.sav";
  if (!save(path, initial.s))
    return 1;
  std::thread input([]() {
    SDL_Delay(400);
    key(SDLK_RIGHT, true);
    SDL_Delay(350);
    key(SDLK_RIGHT, false);
    press(SDLK_z);
    press(SDLK_v);
    press(SDLK_DOWN);
    press(SDLK_z);
    SDL_Delay(350);
    press(SDLK_x);
    press(SDLK_ESCAPE);
  });
  char name[] = "boca-ui", flag[] = "--game", game[] = "boca";
  char *args[] = {name, flag, game};
  int result = fmg_application_main(3, args);
  input.join();
  State saved;
  bool ok = result == 0 && load(path, saved) && saved.game == 6 &&
            saved.f[0] > initial.s.f[0] + 20 && saved.f[1] < initial.s.f[1] - 20 &&
            std::abs((saved.f[0] + saved.f[1]) - (initial.s.f[0] + initial.s.f[1])) < 1 &&
            saved.f[7] > 0 && saved.f[3] < 600 && validState(saved);
  std::remove(path.c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL") << " Boca movement, impulse, help, save\n";
  return ok ? 0 : 1;
}
