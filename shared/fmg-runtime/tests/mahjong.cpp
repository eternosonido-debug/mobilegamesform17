#include "game.hpp"
#include <iostream>
#include <stdexcept>
using namespace fmg;
void require(bool b, const char *m) {
  if (!b)
    throw std::runtime_error(m);
}
int main() {
  for (int seed = 0; seed < 60; ++seed) {
    Engine e(seed);
    e.reset(5);
    for (int symbol = 1; symbol <= 16; ++symbol) {
      int first = -1, last = -1;
      for (int i = 0; i < 32; ++i)
        if (e.s.a[i] == symbol) {
          if (first < 0)
            first = i;
          else
            last = i;
        }
      require(first >= 0 && last >= 0 && e.tileFree(first) && e.tileFree(last),
              "generated removal order solvable");
      e.s.cursor = first;
      e.input(A);
      e.s.cursor = last;
      e.input(A);
      require(e.s.f[0] > 0 && e.s.a[102] == symbol && e.s.a[first] == 0 && e.s.a[last] == 0,
              "animated pair removal");
      require(validState(e.s), "state valid during pair removal");
      if (symbol == 1) {
        std::string path = "/tmp/fmg-mahjong-test.sav";
        require(save(path, e.s), "save animation");
        State loaded;
        require(load(path, loaded) && loaded.a == e.s.a && loaded.f == e.s.f, "resume animation");
        std::remove(path.c_str());
        e.input(Y);
        require(e.s.a[first] == symbol && e.s.a[last] == symbol && e.s.score == 0, "undo pair");
        e.s.cursor = last;
        e.input(A);
        require(e.s.score == 2, "reapply pair after undo");
      }
      e.tick(.05f, 0, 0);
      require(e.s.f[0] < .42f, "animation advances");
    }
    require(e.s.phase == 1 && e.s.score == 32, "all 60 boards cleared");
  }
  Engine e;
  e.reset(5);
  e.s.cursor = 1;
  e.input(A);
  require(e.s.selected == -1 && e.s.f[1] > 0, "blocked feedback");
  e.input(X);
  require(e.tileFree(e.s.cursor) && e.tileFree(e.s.selected) && e.s.f[2] > 0,
          "hint highlights legal pair");
  std::cout << "PASS Mahjong 60 boards, animation, hint, blocked feedback, undo and save\n";
}
