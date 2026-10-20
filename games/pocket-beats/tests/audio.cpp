#include "../src/synth.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
void check(bool x, const char *m) {
  if (!x)
    throw std::runtime_error(m);
}
int main() {
  using namespace pocketbeats;
  std::array<long long, 4> hashes{};
  for (int t = 0; t < 4; ++t) {
    Synth s;
    Pattern p;
    p.notes[t][0] = t ? 4 : 4;
    s.sync(p, true);
    std::vector<int16_t> audio(Synth::rate);
    s.render(audio.data(), audio.size());
    long long energy = 0, hash = 0;
    for (int i = 0; i < int(audio.size()); ++i) {
      energy += int(audio[i]) * int(audio[i]);
      hash += audio[i] * (i % 101);
    }
    check(energy > 100000, "silent instrument");
    hashes[t] = hash;
    for (int other = 0; other < t; ++other)
      check(hashes[other] != hash, "identical instrument voices");
  }
  Synth a, b;
  auto p = preset(0);
  a.sync(p, true);
  b.sync(p, true);
  std::vector<int16_t> x(10000), y(10000);
  a.render(x.data(), x.size());
  b.render(y.data(), y.size());
  check(x == y, "non deterministic audio");
  p.volume.fill(0);
  a.sync(p, true, true);
  a.render(x.data(), x.size());
  for (auto n : x)
    check(n == 0, "volume zero");
  p = preset(1);
  p.mute.fill(true);
  a.sync(p, true, true);
  a.render(x.data(), x.size());
  for (auto n : x)
    check(n == 0, "mute leakage");
  for (int len : {4, 8, 16, 32}) {
    Synth s;
    p = preset(0);
    p.resize(len);
    p.bpm = 120;
    s.sync(p, true);
    std::vector<int16_t> block(Synth::rate * len / 8);
    s.render(block.data(), block.size());
    check(s.step == len - 1, "sequencer skipped steps");
    int16_t sample;
    s.render(&sample, 1);
    check(s.step == 0, "loop wrap");
  }
  a.sync(p, false);
  a.render(x.data(), x.size());
  for (auto n : x)
    check(n == 0, "stop leakage");
  // Preview is audible without starting transport, and then decays to silence.
  Synth preview;
  preview.audition(2, 4, 1, 80);
  preview.render(x.data(), x.size());
  long long energy = 0;
  for (auto v : x)
    energy += int(v) * int(v);
  check(energy > 100000 && !preview.playing && preview.step == -1,
        "preview started transport or silent");
  std::vector<int16_t> tail(Synth::rate * 2);
  preview.render(tail.data(), tail.size());
  check(tail.back() == 0, "preview never ends");
  for (int track = 0; track < 4; ++track) {
    std::array<long long, 3> h{};
    for (int style = 0; style < 3; ++style) {
      Synth voice;
      voice.audition(track, track ? 3 : 1, style, 80);
      voice.render(x.data(), x.size());
      for (int i = 0; i < int(x.size()); ++i)
        h[style] += x[i] * (i % 83);
      for (int j = 0; j < style; ++j)
        check(h[j] != h[style], "timbres identical");
    }
  }
  Synth drums;
  Pattern d;
  d.notes[0][0] = 1;
  d.notes[0][1] = 4;
  drums.sync(d, true);
  int16_t sample;
  drums.render(&sample, 1);
  int kick = drums.voices[0].position;
  drums.samplesToStep = 0;
  drums.render(&sample, 1);
  check(drums.voices[0].position == kick + 1 && drums.voices[2].note == 3,
        "hat cuts kick");
  d.notes[0][0] = 7;
  drums.sync(d, true, true);
  drums.render(&sample, 1);
  check(drums.voices[0].note == 1 && drums.voices[1].note == 2 &&
            drums.voices[2].note == 3,
        "simultaneous percussion missing");
  Session song;
  song.songMode = true;
  song.orderLength = 3;
  song.order = {{2, 0, 1, 0, 0, 0, 0, 0}};
  song.playing = true;
  for (auto &q : song.patterns) {
    q.length = 4;
    q.bpm = 120;
  }
  Synth chain;
  chain.syncSession(song, true);
  chain.render(&sample, 1);
  check(chain.activeSlot == 2, "song starts on wrong beat");
  for (int expected : {0, 1, 2}) {
    chain.step = chain.pattern.length - 1;
    chain.samplesToStep = 0;
    chain.render(&sample, 1);
    check(chain.step == 0 && chain.activeSlot == expected,
          "song boundary order");
  }
  std::cout << "PASS four distinct audible instruments, deterministic mix, "
               "volume/mute, all lengths timing and stop\n";
}
