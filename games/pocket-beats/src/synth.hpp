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
  int style = 0, volume = 80;
};
struct Synth {
  static constexpr int rate = 22050;
  Pattern pattern;
  std::array<Voice, 6> voices, previewVoices;
  std::array<Pattern, slots> sequence;
  std::array<int, 8> order{{0, 1, 2, 1, 0, 0, 0, 0}};
  int orderLength = 4, orderIndex = 0, activeSlot = 0;
  bool songMode = false;
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
  void syncSession(const Session &s, bool restart = false) {
    bool reset = restart || s.playing != playing || songMode != s.songMode;
    sequence = s.patterns;
    order = s.order;
    orderLength = s.orderLength;
    songMode = s.songMode;
    if (reset)
      orderIndex = 0;
    else
      orderIndex %= orderLength;
    activeSlot = songMode ? order[orderIndex] : s.slot;
    sync(sequence[activeSlot], s.playing, reset);
  }
  void startVoice(Voice &v, int note, int style, int volume) {
    v.note = note;
    v.position = 0;
    v.style = style;
    v.volume = volume;
  }
  void audition(int track, int note, int style, int volume) {
    if (track < 0 || track >= tracks || !note)
      return;
    if (track == 0) {
      for (int i = 0; i < 3; ++i)
        if (note & (1 << i))
          startVoice(previewVoices[i], i + 1, style, volume);
    } else
      startVoice(previewVoices[track + 2], note, style, volume);
  }
  void trigger() {
    if (step + 1 >= pattern.length) {
      step = -1;
      if (songMode) {
        orderIndex = (orderIndex + 1) % orderLength;
        activeSlot = order[orderIndex];
        pattern = sequence[activeSlot];
      }
    }
    ++step;
    if (pattern.audible(0)) {
      int mask = pattern.notes[0][step];
      for (int i = 0; i < 3; ++i)
        if (mask & (1 << i))
          startVoice(voices[i], i + 1, pattern.timbre[0], pattern.volume[0]);
    }
    for (int t = 1; t < 4; ++t) {
      int n = pattern.notes[t][step];
      if (n && pattern.audible(t))
        startVoice(voices[t + 2], n, pattern.timbre[t], pattern.volume[t]);
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
      std::vector<int16_t> samples(3 * 4 * 9 * duration, 0);
      Synth generator;
      generator.pattern.volume.fill(100);
      static const int scale[] = {0, 0, 3, 5, 7, 10, 12, 15, 19};
      for (int style = 0; style < 3; ++style)
        for (int t = 0; t < 4; ++t)
          for (int n = 1; n <= 8; ++n) {
            auto &v = generator.voices[t];
            v.age = 0;
            v.note = n;
            v.frequency = 55 * std::pow(2., scale[n] / 12.);
            generator.noise = 89123;
            double low = 0;
            for (int i = 0; i < duration; ++i) {
              double sample = generator.rawVoice(t);
              double age = double(i) / rate;
              if (style == 1) {
                double hz = t == 0 ? 3700
                                   : v.frequency * (t == 1   ? 3
                                                    : t == 2 ? 12
                                                             : 16);
                sample = sample * .8 +
                         .2 * std::sin(6.283185307179586 * hz * age) *
                             std::exp(-8 * age) * std::min(1., age * 300);
              }
              if (style == 2) {
                low += .14 * (sample - low);
                sample = low * 1.15 * std::exp(-2 * age);
              }
              samples[((style * 4 + t) * 9 + n) * duration + i] =
                  int16_t(std::max(-1., std::min(1., sample)) * 24000);
            }
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
      for (int channel = 0; channel < 6; ++channel) {
        int t = channel < 3 ? 0 : channel - 2;
        auto &v = voices[channel];
        if (playing && v.note && v.position < duration) {
          if (pattern.audible(t))
            sum += samples[((v.style * 4 + t) * 9 + v.note) * duration +
                           v.position] *
                   pattern.volume[t] / 100;
          ++v.position;
        }
        auto &pv = previewVoices[channel];
        if (pv.note && pv.position < duration) {
          sum += samples[((pv.style * 4 + t) * 9 + pv.note) * duration +
                         pv.position] *
                 pv.volume / 100;
          ++pv.position;
        }
      }
      out[i] = int16_t(std::max(-30000, std::min(30000, sum / 3)));
    }
  }
};
} // namespace pocketbeats
