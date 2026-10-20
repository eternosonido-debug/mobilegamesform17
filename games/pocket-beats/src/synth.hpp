#pragma once
#include "game_logic.hpp"
#include <cmath>
#include <cstdint>
#include <vector>
namespace pocketbeats {
// Original deterministic synthesis. No samples, assets or audio from other
// games.
struct Voice {
  double age = 10, frequency = 110;
  int note = 0;
  int position = 0;
};
struct Synth {
  static constexpr int rate = 22050;
  Pattern pattern;
  std::array<Voice, 4> voices;
  bool playing = false;
  int step = -1;
  double samplesToStep = 0;
  uint32_t noise = 89123;
  void sync(const Pattern &p, bool run, bool restart = false) {
    pattern = p;
    if (restart || run != playing) {
      step = -1;
      samplesToStep = 0;
      for (auto &v : voices) {
        v.age = 10;
        v.note = 0;
        v.position = 0;
      }
    }
    playing = run;
  }
  void trigger() {
    step = (step + 1) % pattern.length;
    for (int t = 0; t < 4; ++t) {
      int n = pattern.notes[t][step];
      if (n && pattern.audible(t)) {
        auto &v = voices[t];
        v.age = 0;
        v.note = n;
        v.position = 0;
        static const int scale[] = {0, 0, 3, 5, 7, 10, 12, 15, 19};
        v.frequency = 55 * std::pow(2., scale[n] / 12.);
      }
    }
  }
  double rawVoice(int t) {
    auto &v = voices[t];
    double a = v.age;
    v.age += 1. / rate;
    if (a > 1.2 || !pattern.audible(t))
      return 0;
    const double pi = 3.141592653589793, w = 2 * pi * v.frequency * a;
    double out = 0;
    if (t == 0) {
      noise ^= noise << 13;
      noise ^= noise >> 17;
      noise ^= noise << 5;
      double n = (noise & 65535) / 32767.5 - 1;
      if (v.note == 1 || v.note == 4)
        out += std::sin(2 * pi * (48 * a + 9 * (1 - std::exp(-28 * a)))) *
               std::exp(-14 * a);
      if (v.note == 2 || v.note == 5)
        out += (n * .7 + std::sin(2 * pi * 180 * a) * .3) * std::exp(-22 * a);
      if (v.note >= 3)
        out += n * .25 * std::exp(-80 * a);
    } else if (t == 1)
      out = (std::sin(w) + .25 * std::sin(2 * w)) * std::exp(-5 * a) *
            std::min(1., a * 250);
    else if (t == 2) {
      double env = std::exp(-7 * a) * std::min(1., a * 400);
      out = (std::sin(4 * w) + .45 * std::sin(5 * w) + .35 * std::sin(6 * w) +
             .2 * std::sin(8 * w)) *
            env * .5;
    } else
      out = (std::sin(8 * w) + .3 * std::sin(16 * w + .7 * std::sin(3 * w))) *
            std::exp(-3 * a) * std::min(1., a * 50) * .6;
    return out * pattern.volume[t] / 100.;
  }
  static const std::vector<int16_t> &bank() {
    static const std::vector<int16_t> data = [] {
      const int duration = rate * 6 / 5;
      std::vector<int16_t> samples(4 * 9 * duration, 0);
      Synth generator;
      generator.pattern.volume.fill(100);
      static const int scale[] = {0, 0, 3, 5, 7, 10, 12, 15, 19};
      for (int t = 0; t < 4; ++t)
        for (int n = 1; n <= 8; ++n) {
          auto &v = generator.voices[t];
          v.age = 0;
          v.note = n;
          v.frequency = 55 * std::pow(2., scale[n] / 12.);
          generator.noise = 89123;
          for (int i = 0; i < duration; ++i)
            samples[(t * 9 + n) * duration + i] = int16_t(
                std::max(-1., std::min(1., generator.rawVoice(t))) * 24000);
        }
      return samples;
    }();
    return data;
  }
  void render(int16_t *out, int count) {
    const auto &samples = bank();
    const int duration = rate * 6 / 5;
    for (int i = 0; i < count; ++i) {
      if (playing && samplesToStep <= 0) {
        trigger();
        samplesToStep += rate * 60. / (pattern.bpm * 4.);
      }
      if (playing)
        --samplesToStep;
      int sum = 0;
      if (playing)
        for (int t = 0; t < 4; ++t) {
          auto &v = voices[t];
          if (v.note && v.position < duration) {
            if (pattern.audible(t))
              sum += samples[(t * 9 + v.note) * duration + v.position] *
                     pattern.volume[t] / 100;
            ++v.position;
          }
        }
      out[i] = int16_t(std::max(-30000, std::min(30000, sum / 3)));
    }
  }
};
} // namespace pocketbeats
