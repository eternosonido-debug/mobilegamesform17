#include "audio.hpp"
#include <cassert>
#include <iostream>
int main() {
  using namespace tiles;
  for (int song = 0; song < 3; ++song) {
    auto events = music(song);
    for (auto n : chart(song, 2)) {
      int frame = int(int64_t(n.ms) * Rate / 1000);
      bool found = false;
      for (auto e : events)
        if (e.frame == frame && e.sample == n.sample) {
          found = true;
          break;
        }
      assert(found);
    }
    Synth synth;
    synth.start(song);
    std::vector<int16_t> buf(Rate * 4);
    synth.render(buf.data(), int(buf.size()));
    int64_t power = 0;
    for (auto n : buf)
      power += int64_t(n) * n;
    assert(power > 1000000 && synth.frame == int(buf.size()));
  }
  assert(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER) == 0);
  {
    Audio a;
    assert(a.device);
    a.start(0, false);
    SDL_Delay(250);
    assert(a.clock() > 100 && SDL_AtomicGet(&a.callbacks) > 2);
    a.pause(true);
    int time = a.clock(), samples = SDL_AtomicGet(&a.frames);
    SDL_Delay(100);
    assert(a.clock() == time && SDL_AtomicGet(&a.frames) == samples);
    a.pause(false);
    SDL_Delay(100);
    assert(a.clock() > time + 40);
    a.pause(true);
    SDL_LockAudioDevice(a.device);
    a.synth.frame = a.synth.end;
    a.synth.event = a.synth.events.size();
    SDL_AtomicSet(&a.frames, a.synth.end);
    SDL_UnlockAudioDevice(a.device);
    a.pause(false);
    assert(a.clock() == duration(0));
    a.pause(true);
  }
  SDL_Quit();
  std::cout
      << "PASS original audio, chart sync, independent callback and pause\n";
}
