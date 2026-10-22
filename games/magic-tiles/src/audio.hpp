#pragma once
#include "game_logic.hpp"
#include <SDL.h>
#include <cmath>
namespace tiles {
constexpr int Rate = 22050;
struct MusicEvent {
  int frame, sample, volume;
};
inline std::vector<MusicEvent> music(int song) {
  std::vector<MusicEvent> out;
  for (auto n : melody(song))
    out.push_back(
        {int(int64_t(n.ms) * Rate / 1000), n.sample, n.harmony ? 50 : 110});

  for (int beat = 0; beat * 60000 / bpm(song) < intro(song) + 180000; ++beat) {
    int ms = beat * 60000 / bpm(song);
    out.push_back({int(int64_t(ms) * Rate / 1000), beat % 2 ? 25 : 24, 100});
    out.push_back({int(int64_t(ms) * Rate / 1000), 26, 45});
    if (ms >= intro(song) && beat % 2 == 0)
      out.push_back({int(int64_t(ms) * Rate / 1000),
                     (rootNote(song) + chordOffset(song, (beat - 4) / 16)) % 12,
                     95});
  }
  std::stable_sort(out.begin(), out.end(),
                   [](const MusicEvent &a, const MusicEvent &b) {
                     return a.frame < b.frame;
                   });
  return out;
}
struct Synth {
  struct Voice {
    int sample, pos, volume;
    Voice(int s = 0, int p = 0, int v = 0) : sample(s), pos(p), volume(v) {}
  };
  std::array<Voice, 16> voices{};
  std::vector<MusicEvent> events;
  int frame = 0;
  size_t event = 0;
  int end = 0;
  static const std::array<std::vector<int16_t>, 27> &bank() {
    static const std::array<std::vector<int16_t>, 27> samples = []() {
      std::array<std::vector<int16_t>, 27> a;
      for (int k = 0; k < 27; ++k) {
        int length = k < 24 ? Rate * 3 / 10 : k == 26 ? Rate / 14 : Rate / 5;
        a[k].resize(length);
        uint32_t noise = 1234;
        double phase = 0;
        for (int i = 0; i < length; ++i) {
          double t = double(i) / Rate, decay = 1 - double(i) / length, w = 0;
          if (k < 24) {
            double f = 130.8128 * std::pow(2., k / 12.);
            w = std::sin(t * f * 6.2831853) * .75 +
                std::sin(t * f * 12.5663706) * .15;
          } else if (k == 24) {
            phase += 6.2831853 * (55 + 110 * std::exp(-35 * t)) / Rate;
            w = std::sin(phase);
          } else {
            noise = noise * 1664525u + 1013904223u;
            double n = (double(noise >> 16) / 32768 - 1);
            w = n * (k == 25 ? .7 : .5) +
                (k == 25 ? std::sin(t * 180 * 6.2831853) * .2 : 0);
          }
          double attack = std::min(1., t * 500);
          a[k][i] = int16_t(w * 8500 * decay * decay * attack);
        }
      }
      return a;
    }();
    return samples;
  }
  void start(int song) {
    events = music(song);
    frame = 0;
    event = 0;
    end = int(int64_t(duration(song)) * Rate / 1000);
    for (auto &v : voices)
      v = Voice{};
  }
  void render(int16_t *out, int count) {
    auto &samples = bank();
    for (int i = 0; i < count; ++i) {
      while (event < events.size() && events[event].frame <= frame) {
        auto e = events[event++];
        auto target = std::find_if(voices.begin(), voices.end(),
                                   [](const Voice &v) { return !v.volume; });
        if (target == voices.end())
          target = voices.begin();
        *target = {e.sample, 0, e.volume};
      }
      int value = 0;
      for (auto &v : voices)
        if (v.volume) {
          value += samples[v.sample][v.pos++] * v.volume / 256;
          if (v.pos >= int(samples[v.sample].size()))
            v.volume = 0;
        }
      out[i] = int16_t(std::max(-30000, std::min(30000, value)));
      if (frame < end)
        ++frame;
    }
  }
};
struct Audio {
  SDL_AudioDeviceID device = 0;
  SDL_AudioSpec spec{};
  Synth synth;
  SDL_atomic_t frames{}, at{}, sequence{}, muted{}, callbacks{};
  bool paused = true;
  int frozen = 0, totalMs = 0;
  static void callback(void *ptr, Uint8 *stream, int bytes) {
    auto *self = static_cast<Audio *>(ptr);
    self->synth.render(reinterpret_cast<int16_t *>(stream), bytes / 2);
    if (SDL_AtomicGet(&self->muted))
      SDL_memset(stream, 0, size_t(bytes));
    SDL_AtomicAdd(&self->sequence, 1);
    SDL_AtomicSet(&self->frames, self->synth.frame);
    SDL_AtomicSet(&self->at, int(SDL_GetTicks()));
    SDL_AtomicAdd(&self->sequence, 1);
    SDL_AtomicAdd(&self->callbacks, 1);
  }
  Audio() {
    Synth::bank();
    SDL_AudioSpec wanted{};
    wanted.freq = Rate;
    wanted.format = AUDIO_S16SYS;
    wanted.channels = 1;
    wanted.samples = 512;
    wanted.callback = callback;
    wanted.userdata = this;
    device = SDL_OpenAudioDevice(nullptr, 0, &wanted, &spec, 0);
  }
  ~Audio() {
    if (device)
      SDL_CloseAudioDevice(device);
  }
  int clock() const {
    if (paused)
      return frozen;
    int f = 0, stamp = 0, a = 0, b = 0;
    do {
      a = SDL_AtomicGet(const_cast<SDL_atomic_t *>(&sequence));
      f = SDL_AtomicGet(const_cast<SDL_atomic_t *>(&frames));
      stamp = SDL_AtomicGet(const_cast<SDL_atomic_t *>(&at));
      b = SDL_AtomicGet(const_cast<SDL_atomic_t *>(&sequence));
    } while (a != b || (a & 1));
    if (totalMs && f >= int(int64_t(totalMs) * Rate / 1000))
      return totalMs;
    unsigned elapsed = SDL_GetTicks() - Uint32(stamp);
    int extra = std::min(int(elapsed), spec.samples * 1000 / Rate);
    return std::max(
        0,
        int(int64_t(std::max(0, f - int(spec.samples))) * 1000 / Rate) + extra);
  }
  void start(int song, bool silent) {
    if (!device)
      return;
    SDL_PauseAudioDevice(device, 1);
    SDL_LockAudioDevice(device);
    synth.start(song);
    totalMs = duration(song);
    SDL_AtomicSet(&frames, 0);
    SDL_AtomicSet(&at, int(SDL_GetTicks()));
    SDL_AtomicSet(&muted, silent);
    SDL_UnlockAudioDevice(device);
    frozen = 0;
    paused = false;
    SDL_PauseAudioDevice(device, 0);
  }
  void pause(bool value) {
    if (!device || paused == value)
      return;
    if (value) {
      frozen = clock();
      SDL_PauseAudioDevice(device, 1);
      paused = true;
    } else {
      int base =
          int(int64_t(std::max(0, SDL_AtomicGet(&frames) - int(spec.samples))) *
              1000 / Rate);
      SDL_AtomicSet(&at, int(SDL_GetTicks()) - std::max(0, frozen - base));
      paused = false;
      SDL_PauseAudioDevice(device, 0);
    }
  }
};
} // namespace tiles
