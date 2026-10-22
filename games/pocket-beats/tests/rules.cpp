#include "../src/game_logic.hpp"
#include <iostream>
#include <stdexcept>
#include <unistd.h>
void check(bool x, const char *m) {
  if (!x)
    throw std::runtime_error(m);
}
int main() {
  using namespace pocketbeats;
  Pattern p;
  auto before = p.notes;
  check(!p.toggle(-1, 0) && !p.toggle(0, 32) && !p.resize(12),
        "invalid action");
  check(p.notes == before && p.length == 16, "invalid action mutated state");
  check(p.toggle(2, 3) && p.notes[2][3] == 9, "valid pad");
  p.setNote(2, 31, 8); // outside active pattern is invalid
  for (int len : {4, 8, 16, 32}) {
    check(p.resize(len), "length");
    for (int t = 0; t < 4; ++t)
      check(p.toggle(t, len - 1), "global length");
  }
  p.setNote(3, 31, 8);
  p.resize(4);
  p.resize(32);
  check(p.notes[3][31] == 8, "hidden steps preserved");
  p.solo = 1;
  check(!p.audible(0) && p.audible(1), "solo");
  p.mute[1] = true;
  check(!p.audible(1), "mute");
  Session s;
  for (int id = 0; id < 3; ++id) {
    auto q = preset(id);
    for (int t = 0; t < 4; ++t) {
      bool present = false;
      for (int n : q.notes[t])
        present |= n > 0;
      check(present, "preset has silent instrument");
    }
  }
  s.chooseSlot(2);
  s.current() = p;
  s.track = 3;
  s.cursor = 31;
  std::string path =
      "/tmp/pocket-beats-test-" + std::to_string(getpid()) + ".sav";
  check(s.save(path), "save");
  Session restored;
  check(restored.load(path) && restored.slot == 2 && restored.track == 3 &&
            restored.cursor == 31,
        "load state");
  check(restored.current().notes == p.notes &&
            restored.current().volume == p.volume &&
            restored.current().mute == p.mute && restored.current().solo == 1,
        "load sound");
  std::ofstream(path) << "POCKETBEATS1\n2 3 31\n32 999 -1\n";
  check(!restored.load(path) && restored.current().bpm == p.bpm,
        "corrupt save atomic reject");
  std::remove(path.c_str());
  std::cout << "PASS valid/invalid pads, shared lengths, three presets, "
               "mute/solo, atomic save roundtrip\n";
}
