#include "game.hpp"
#include <iostream>
#include <set>
#include <stdexcept>
using namespace fmg;
void require(bool value, const char *message) {
  if (!value)
    throw std::runtime_error(message);
}
int main() {
  Engine e(24);
  e.reset(1);
  for (int id = 0; id < 16; ++id) {
    auto cells = Engine::shape(id);
    std::set<std::pair<int, int>> unique(cells.begin(), cells.end());
    require(!cells.empty() && unique.size() == cells.size(), "unique nonempty shapes");
    require(e.fits(id, 0, 0), "all shapes fit empty board");
  }
  e.s.a.fill(0);
  e.s.choice = 0;
  e.s.a[100] = 0;
  e.s.a[101] = 3;
  e.s.a[102] = 4;
  for (int i = 1; i < 8; ++i)
    e.s.a[i] = e.s.a[i * 8] = 1;
  e.s.cursor = 0;
  State before = e.s;
  e.input(A);
  require(e.s.a[112] == 2 && e.s.a[111] == 1, "simultaneous row and column clear");
  for (int i = 0; i < 8; ++i)
    require(!e.s.a[i] && !e.s.a[i * 8], "cross fully removed");
  require(e.s.a[115] == e.s.score && e.s.f[2] > 0, "score animation gain");
  e.tick(.05f, 0, 0);
  require(e.s.f[2] < .75f, "score animation progresses");
  int best = e.s.a[110];
  e.input(Y);
  require(e.s.a[0] == before.a[0] && e.s.a[1] == 1 && e.s.a[8] == 1 && e.s.score == before.score,
          "undo restores board and score");
  require(e.s.a[110] == best, "undo preserves record");
  e.s.cursor = 63;
  e.s.choice = 1;
  auto board = e.s.a;
  e.input(A);
  require(e.s.a == board && e.s.f[1] > 0, "invalid placement changes no board");
  e.s.a[100] = -1;
  e.s.choice = 2;
  e.input(B);
  require(e.s.choice == 1, "skip used piece");
  e.s.a.fill(1);
  e.s.a[100] = 0;
  e.s.a[101] = 3;
  e.s.a[102] = 4;
  e.s.choice = 0;
  e.s.cursor = 0;
  // A single gap clears a full board; replace other cells so no complete lines remain.
  for (int y = 0; y < 8; ++y)
    for (int x = 0; x < 8; ++x)
      e.s.a[y * 8 + x] = (x + y) % 2 ? 1 : 0;
  e.s.a[100] = 3;
  e.s.a[101] = 3;
  e.s.a[102] = 3;
  require(!e.hasMove(), "detect no remaining fit");
  e.reset(1);
  e.s.a[110] = 1234;
  e.reset(1);
  require(e.s.a[110] == 1234, "restart preserves record");
  for (int seed = 0; seed < 50; ++seed) {
    e.rng.seed(seed);
    e.reset(1);
    for (int turn = 0; turn < 60 && !e.s.phase; ++turn) {
      bool done = false;
      for (int k = 0; k < 3 && !done; ++k)
        for (int pos = 0; pos < 64 && !done; ++pos)
          if (e.fits(e.s.a[100 + k], pos % 8, pos / 8)) {
            e.s.choice = k;
            e.s.cursor = pos;
            e.input(A);
            done = true;
          }
      require(done || !e.hasMove(), "legal moves detectable");
      require(validState(e.s), "generated state save-valid");
    }
  }
  std::cout << "PASS New Blocks shapes, cross clears, undo, record, invalid placement and 50 "
               "seeded games\n";
}
