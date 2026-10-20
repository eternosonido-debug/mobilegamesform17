#include "game.hpp"
#include <iostream>
#include <stdexcept>
using namespace fmg;
void require(bool v, const char *m) {
  if (!v)
    throw std::runtime_error(m);
}
void lap(State &s) {
  for (int k = 0; k < 210 && !s.phase && loopActive(s); ++k) {
    tickLoop(s, .05f);
    require(validLoop(s), "valid state during movement");
  }
}
int main() {
  for (int level = 0; level < 24; ++level) {
    State s;
    initLoop(s, level, level);
    require(validState(s), "valid initial stage");
    int steps = 0;
    while (!s.phase && steps++ < 80) {
      int best = -1;
      float value = -1e9;
      State next;
      for (int choice = 0; choice < 8; ++choice) {
        State t = s;
        if (!launchLoop(t, choice))
          continue;
        lap(t);
        float v = (t.a[203] - s.a[203]) * 100 - loopWaiting(t) * 10 + (choice >= 4 ? 1 : 0);
        if (t.phase == 2)
          v = -1e6;
        if (v > value) {
          value = v;
          best = choice;
          next = t;
        }
      }
      require(best >= 0, "solver has legal launch");
      s = next;
    }
    require(s.phase == 1 && loopItemCount(s) == 0, "all 24 stages finish");
    require(s.a[201] == std::min(23, level + 1), "unlock next stage");
  }
  State s;
  initLoop(s, 5, 5);
  require(launchLoop(s, 0), "launch front");
  require(launchLoop(s, 1) && s.a[220] == 2 && loopActive(s) == 1,
          "buffer early launch without consuming front");
  require(!launchLoop(s, 2) && s.a[220] == 2, "only one pending launch");
  int color = s.a[130], count = s.a[134];
  tickLoop(s, .05f);
  require(s.f[0] > 0, "belt moves");
  std::string path = "/tmp/fmg-loop-rules.sav";
  require(save(path, s), "save moving collector");
  State loaded;
  require(load(path, loaded), "resume moving collector");
  require(loaded.a == s.a && loaded.f == s.f, "exact in-flight persistence");
  std::remove(path.c_str());
  require(color > 0 && count == 6, "collector capacity");
  for (int i = 0; i < 6; ++i)
    tickLoop(s, .05f);
  require(s.a[220] == 0 && loopActive(s) == 2, "pending launch starts automatically");
  // Farther colors cannot be collected through the exposed blocker.
  State b;
  initLoop(b, 0, 0);
  b.a[0] = 1;
  b.a[6] = 2;
  require(loopExposed(b, 0) == 0, "only exposed block");
  b.a[0] = 0;
  require(loopExposed(b, 0) == 6, "uncover next block");
  State full;
  initLoop(full, 0, 0);
  for (int i = 0; i < 4; ++i) {
    full.a[130 + i] = 1;
    full.a[134 + i] = 6;
    full.f[i] = 3 + i * 2;
  }
  require(launchLoop(full, 0) && full.a[220] == 1 && loopActive(full) == 4,
          "full belt buffers one launch");
  State jam;
  initLoop(jam, 0, 0);
  for (int i = 0; i < 4; ++i) {
    jam.a[150 + i] = 1;
    jam.a[154 + i] = 6;
  }
  jam.a[130] = 1;
  jam.a[134] = 6;
  jam.f[0] = 23.99f;
  tickLoop(jam, .05f);
  require(jam.phase == 2, "overflow ends round");
  Engine e;
  e.reset(7);
  State before = e.s;
  e.input(A);
  e.tick(.05f, 0, 0);
  e.input(Y);
  require(e.s.a == before.a && e.s.f == before.f, "undo restores full moving state");
  e.input(A);
  e.s.cursor = 1;
  e.input(A);
  require(e.s.a[220] == 2, "engine buffers choice");
  e.input(B);
  require(!e.s.a[220], "B cancels pending launch");
  e.s.a[200] = 5;
  e.s.a[201] = 8;
  e.reset(7);
  require(e.s.a[200] == 5 && e.s.a[201] == 8, "restart preserves campaign");
  State corrupt;
  initLoop(corrupt, 0, 0);
  corrupt.a[70] = 4;
  require(!validState(corrupt), "reject inconsistent color capacity");
  std::cout
      << "PASS Color Loop 24 stages, collection, capacity, return jam, undo and in-flight save\n";
}
