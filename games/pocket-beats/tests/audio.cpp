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
  std::cout << "PASS four distinct audible instruments, deterministic mix, "
               "volume/mute, all lengths timing and stop\n";
}
