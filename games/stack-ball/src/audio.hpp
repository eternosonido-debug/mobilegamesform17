#pragma once
#include <SDL.h>
#include <cmath>
#include <vector>
namespace tower {
struct Audio {
  SDL_AudioDeviceID device = 0;
  SDL_AudioSpec spec{};
  Audio() {
    SDL_AudioSpec wanted{};
    wanted.freq = 22050;
    wanted.format = AUDIO_S16SYS;
    wanted.channels = 1;
    wanted.samples = 512;
    device = SDL_OpenAudioDevice(nullptr, 0, &wanted, &spec, 0);
    if (device)
      SDL_PauseAudioDevice(device, 0);
  }
  ~Audio() {
    if (device)
      SDL_CloseAudioDevice(device);
  }
  void play(int kind, bool muted) {
    if (!device || muted)
      return;
    SDL_ClearQueuedAudio(device);
    int samples = spec.freq * (kind == 2 ? .18f : .065f);
    std::vector<Sint16> data(samples);
    for (int i = 0; i < samples; ++i) {
      float t = float(i) / spec.freq, decay = 1 - float(i) / samples;
      float frequency = kind == 2 ? 660 + 600 * t : kind == 1 ? 175 : 410;
      data[i] = Sint16(std::sin(t * frequency * 6.2831853f) * 1900 * decay);
    }
    SDL_QueueAudio(device, data.data(), Uint32(data.size() * sizeof(Sint16)));
  }
};
} // namespace tower
