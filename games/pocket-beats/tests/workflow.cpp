#include "../src/controller.hpp"
#include <iostream>
#include <stdexcept>
#include <unistd.h>
void check(bool x, const char *m) {
  if (!x)
    throw std::runtime_error(m);
}
int main() {
  using namespace pocketbeats;
  Controller c;
  c.action(A);
  c.state.track = 0;
  c.state.cursor = 1;
  c.state.current().notes[0][1] = 0;
  Control keys[] = {ToneDo, ToneRe, ToneMi, ToneFa, X, Y, B};
  for (int i = 0; i < 7; ++i) {
    c.action(keys[i]);
    check(c.state.current().notes[0][1] == 0 &&
              c.state.current().chosen[0][1] == 1 << i && c.previewRequested,
          "seven percussion choices");
  }
  c.action(A);
  check(c.state.current().notes[0][1] == 64, "crash on");
  c.action(A);
  check(c.state.current().notes[0][1] == 0 &&
            c.state.current().chosen[0][1] == 64,
        "off remembers crash");
  auto original = c.state.current().notes;
  c.vary();
  check(c.state.current().notes != original, "variation unchanged");
  c.undo();
  check(c.state.current().notes == original, "undo variation");
  std::string path = "/tmp/pocket-workflow-" + std::to_string(getpid());
  check(c.state.save(path), "save3");
  Session loaded;
  check(loaded.load(path) && loaded.current().notes == original &&
            loaded.current().chosen[0][1] == 64 && !loaded.playing,
        "inactive assignment saved");
  Session old;
  std::ofstream f(path);
  f << "POCKETBEATS1\n0 0 0\n";
  for (auto &p : old.patterns) {
    f << p.length << ' ' << p.bpm << ' ' << p.solo << '\n';
    for (int t = 0; t < 4; ++t) {
      f << p.volume[t] << ' ' << p.mute[t];
      for (int i = 0; i < 32; ++i)
        f << ' ' << (t == 0 ? i % 9 : p.notes[t][i]);
      f << '\n';
    }
  }
  f.close();
  Session migrated;
  check(migrated.load(path), "legacy save");
  int masks[] = {0, 1, 2, 4, 5, 6, 4, 4, 4};
  for (int i = 0; i < 32; ++i)
    check(migrated.current().notes[0][i] == masks[i % 9], "legacy percussion");
  std::ofstream modern(path);
  modern << "POCKETBEATS2\n0 1 0\n0 1 0 4 0 1 2 1 0 0 0 0\n";
  for (auto &p : old.patterns) {
    modern << p.length << ' ' << p.bpm << ' ' << p.solo << '\n';
    for (int style : p.timbre)
      modern << style << ' ';
    modern << '\n';
    for (int t = 0; t < 4; ++t) {
      modern << p.volume[t] << ' ' << p.mute[t];
      for (int n : p.notes[t])
        modern << ' ' << n;
      modern << '\n';
    }
  }
  modern.close();
  Session version2;
  check(version2.load(path) &&
            version2.current().chosen[1][0] == old.current().notes[1][0],
        "version2 assignment migration");
  std::remove(path.c_str());
  std::cout << "PASS seven drums, off-step assignment, variation/undo and save "
               "migration\n";
}
