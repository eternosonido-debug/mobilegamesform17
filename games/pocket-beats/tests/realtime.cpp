#include "../src/audio.hpp"
#include <iostream>
#include <stdexcept>
void check(bool x, const char *m) {
  if (!x)
    throw std::runtime_error(m);
}
int main() {
  using namespace pocketbeats;
  check(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER) == 0, "SDL init");
  {
    Audio audio;
    check(audio.device != 0, "audio device");
    audio.sync(preset(0), true, true);
    // Simulates a completely blocked UI / filesystem operation: no pump or sync
    // calls.
    SDL_Delay(350);
    int before = SDL_AtomicGet(&audio.renderedSamples);
    SDL_Delay(250);
    int after = SDL_AtomicGet(&audio.renderedSamples);
    check(before >= 4096 && after - before >= 3072, "audio depends on UI loop");
    check(audio.audibleStep() >= 1, "transport stalled");
    audio.sync(preset(0), false);
    SDL_Delay(100);
    check(audio.audibleStep() == -1, "stop marker");
  }
  SDL_Quit();
  std::cout << "PASS audio callback and transport continue while main thread "
               "is blocked\n";
}
