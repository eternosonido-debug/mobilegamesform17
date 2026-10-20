#define main fmg_application_main
#include "../src/main.cpp"
#undef main
#include <thread>
#include <unistd.h>
static void press(SDL_Keycode key, int delay = 100) {
  SDL_Event ev{};
  ev.type = SDL_KEYDOWN;
  ev.key.keysym.sym = key;
  SDL_PushEvent(&ev);
  SDL_Delay(30);
  ev.type = SDL_KEYUP;
  SDL_PushEvent(&ev);
  SDL_Delay(delay);
}
int main() {
  char folder[] = "/tmp/fmg-juice-ui-XXXXXX";
  if (!mkdtemp(folder))
    return 1;
  setenv("FMG_SAVE_DIR", folder, 1);
  bool observedPour = false;
  auto run = [&](int mode) {
    std::thread input([&, mode]() {
      SDL_Delay(400);
      if (mode == 1) {
        press(SDLK_v); // Options.
        press(SDLK_DOWN);
        press(SDLK_z); // Restart confirmation.
        press(SDLK_x); // Cancel.
        press(SDLK_DOWN);
        press(SDLK_z); // Level selector.
        press(SDLK_z); // Start level one.
        press(SDLK_v);
        press(SDLK_z);       // Hint selects source and destination.
        press(SDLK_z, 1500); // Complete animated pour.
        State poured;
        observedPour = load(std::string(folder) + "/game-0.sav", poured) && poured.score == 1;
        press(SDLK_v); // Extras: undo.
        for (int i = 0; i < 4; ++i)
          press(SDLK_DOWN);
        press(SDLK_z);
        press(SDLK_RETURN);
        press(SDLK_x); // Pause/resume.
      }
      if (mode == 2)
        press(SDLK_z); // Advance from a restored victory.
      press(SDLK_ESCAPE);
    });
    char name[] = "fmg-ui", flag[] = "--game", game[] = "juices-sort";
    char *args[] = {name, flag, game};
    int result = fmg_application_main(3, args);
    input.join();
    return result;
  };
  int result = run(1);
  State saved;
  Engine expected(1);
  startJuiceLevel(expected, 0);
  std::string savePath = std::string(folder) + "/game-0.sav";
  bool ok = observedPour && result == 0 && load(savePath, saved) && saved.a == expected.s.a &&
            saved.score == 0;
  // Reopen the real application to check restoration after an exit.
  result = run(0);
  State resumed;
  ok = ok && result == 0 && load(savePath, resumed) && resumed.a == saved.a && resumed.score == 0;
  // Resume a completed board, advance, and verify unlocks reached disk.
  startJuiceLevel(expected, 0);
  while (!expected.s.phase) {
    auto hint = juiceHint(expected.s);
    if (hint.first < 0) {
      ok = false;
      break;
    }
    expected.pour(hint.first, hint.second);
  }
  ok = save(savePath, expected.s) && ok;
  result = run(2);
  JuiceProgress progress;
  ok = ok && result == 0 && load(savePath, resumed) && juiceLevel(resumed) == 1 &&
       resumed.score == 0 && progress.read(std::string(folder) + "/juices-progress.sav") &&
       progress.best[0] == juicePar[0] && progress.unlocked() == 1;
  std::remove(savePath.c_str());
  std::remove((std::string(folder) + "/juices-progress.sav").c_str());
  rmdir(folder);
  std::cout << (ok ? "PASS" : "FAIL") << " campaign controls, animation undo and resume\n";
  return ok ? 0 : 1;
}
