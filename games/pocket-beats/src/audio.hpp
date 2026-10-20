#pragma once
#include "synth.hpp"
#include <SDL.h>
namespace pocketbeats {
struct Audio {
  SDL_AudioDeviceID device = 0;
  Synth synth;
  Audio() {
    SDL_AudioSpec spec{};
    spec.freq = Synth::rate;
    spec.format = AUDIO_S16SYS;
    spec.channels = 1;
    spec.samples = 512;
    device = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, 0);
    if (device)
      SDL_PauseAudioDevice(device, 0);
  }
  ~Audio() {
    if (device)
      SDL_CloseAudioDevice(device);
  }
  void sync(const Pattern &p, bool run, bool restart = false) {
    if (device && (restart || synth.playing != run))
      SDL_ClearQueuedAudio(device);
    synth.sync(p, run, restart);
  }
  void pump() {
    if (!device)
      return;
    int16_t samples[256];
    while (SDL_GetQueuedAudioSize(device) < 2048) {
      synth.render(samples, 256);
      SDL_QueueAudio(device, samples, sizeof(samples));
    }
  }
  int audibleStep() const {
    if (!device || !synth.playing)
      return -1;
    double samples = SDL_GetQueuedAudioSize(device) / 2.;
    double duration = Synth::rate * 60. / (synth.pattern.bpm * 4.);
    int behind =
        int(std::ceil(std::max(0., samples - synth.samplesToStep) / duration));
    return (synth.step - behind + synth.pattern.length * 4) %
           synth.pattern.length;
  }
};
} // namespace pocketbeats
