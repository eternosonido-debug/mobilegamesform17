#include "game_logic.hpp"
#include <cassert>
#include <iostream>
int main() {
  using namespace tiles;
  for (int song = 0; song < 3; ++song)
    for (int difficulty = 0; difficulty < 3; ++difficulty) {
      State s(song, difficulty);
      assert(duration(song) >= 180000 && !s.notes.empty());
      int previous = -1;
      for (auto n : s.notes) {
        assert(n.ms >= previous && n.lane >= 0 && n.lane < 4);
        previous = n.ms;
        assert(s.hit(n.lane, n.ms) == 1);
        s.advance(n.ms);
      }
      s.advance(duration(song) - 1);
      assert(!s.won());
      s.advance(duration(song));
      assert(s.won() && s.score() == 1000000 && s.misses == 0);
      std::cout << title(song) << " modo " << difficulty << ": "
                << s.notes.size() << " notas verificadas\n";
    }
  for (int offset : {-150, 150}) {
    State timing(0, 0, true);
    timing.advance(duration(0) - 1 + offset, duration(0) - 1);
    assert(!timing.won());
    timing.advance(duration(0) + offset, duration(0));
    assert(timing.won());
  }
  for (int error : {0, 55, 56, 105, 106, 140}) {
    State s;
    assert(s.hit(s.notes[0].lane, s.notes[0].ms + error) == (error <= 55 ? 1
                                                             : error <= 105
                                                                 ? 2
                                                                 : 3));
    assert(s.hit(-1, 0) == 0);
  }
  State fail;
  fail.advance(30000);
  assert(fail.failed() && !fail.won());
  State practice(0, 0, true);
  practice.advance(duration(0));
  assert(!practice.failed() && practice.won());
  Settings cfg;
  cfg.offset = -75;
  cfg.song = 2;
  cfg.practice = true;
  assert(cfg.save("tiles-test.save"));
  Settings loaded;
  assert(loaded.load("tiles-test.save") && loaded.offset == -75 &&
         loaded.song == 2);
  std::ofstream("tiles-test.save") << "TILES1 9";
  assert(!loaded.load("tiles-test.save") && loaded.song == 2);
  std::remove("tiles-test.save");
}
