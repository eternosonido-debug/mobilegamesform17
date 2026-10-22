#pragma once
#include <SDL.h>
#include <array>
#include <cmath>
#include <vector>
namespace cooking {
struct Audio {
  SDL_AudioDeviceID device = 0;
  SDL_AudioSpec spec{};
  std::array<std::vector<Sint16>, 3> sounds;
  Audio() {
    SDL_AudioSpec wanted{};
    wanted.freq = 22050;
    wanted.format = AUDIO_S16SYS;
    wanted.channels = 1;
    wanted.samples = 512;
    device = SDL_OpenAudioDevice(nullptr, 0, &wanted, &spec, 0);
    if (!device)
      return;
    for (int kind = 0; kind < 3; ++kind) {
      int count = int(spec.freq * (kind == 2   ? .56f
                                   : kind == 1 ? .075f
                                               : .11f));
      auto &data = sounds[kind];
      data.resize(count);
      const float melody[] = {523.25f, 659.25f, 783.99f, 1046.5f};
      for (int i = 0; i < count; ++i) {
        float t = float(i) / spec.freq, phase, decay;
        if (kind == 2) {
          int note = std::min(3, int(t / .14f));
          float local = t - note * .14f;
          phase = local * melody[note];
          decay = 1 - local / .14f;
        } else {
          phase = t * (kind == 1 ? 210.f : 740.f) +
                  t * t * (kind == 1 ? -80.f : 900.f);
          decay = 1 - float(i) / count;
        }
        data[i] = Sint16((std::sin(phase * 6.2831853f) +
                          .22f * std::sin(phase * 12.56637f)) *
                         1700 * decay);
      }
    }
    SDL_PauseAudioDevice(device, 0);
  }
  ~Audio() {
    if (device)
      SDL_CloseAudioDevice(device);
  }
  void play(int kind, bool muted) {
    if (!device || muted || kind < 0 || kind > 2)
      return;
    SDL_ClearQueuedAudio(device);
    const auto &data = sounds[kind];
    SDL_QueueAudio(device, data.data(), Uint32(data.size() * sizeof(Sint16)));
  }
};
} // namespace cooking
