#pragma once
#include "game_logic.hpp"
#include <cmath>
#include <cstdint>
namespace pocketbeats {
// Original deterministic synthesis. No samples, assets or audio from other
// games.
struct Voice {
  double age = 10, frequency = 110;
  int note = 0;
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
      for (auto &v : voices)
        v.age = 10;
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
        static const int scale[] = {0, 0, 3, 5, 7, 10, 12, 15, 19};
        v.frequency = 55 * std::pow(2., scale[n] / 12.);
      }
    }
  }
  double voice(int t) {
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
  void render(int16_t *out, int count) {
    for (int i = 0; i < count; ++i) {
      if (playing && samplesToStep <= 0) {
        trigger();
        samplesToStep += rate * 60. / (pattern.bpm * 4.);
      }
      if (playing)
        --samplesToStep;
      double sum = 0;
      if (playing)
        for (int t = 0; t < 4; ++t)
          sum += voice(t);
      out[i] = int16_t(std::max(-1., std::min(1., sum * .25)) * 30000);
    }
  }
};
} // namespace pocketbeats
