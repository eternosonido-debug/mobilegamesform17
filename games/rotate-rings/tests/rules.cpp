#include "game_logic.hpp"
#include <cassert>
#include <iostream>
int main() {
  using namespace rings;
  assert(opening(0, 0));
  assert(opening(0, 2 * pi - .1));
  assert(!opening(0, pi));
  assert(!opening(0, gapHalf + .01));
  for (int i = 0; i < int(levels().size()); ++i) {
    auto s = initial(i);
    auto before = s;
    assert(release(levels()[i], s) == 0);
    auto seq = solution(i);
    assert(!seq.empty());
    for (auto m : seq) {
      assert(turn(levels()[i], s, m.ring, m.direction));
      assert((s.active | before.active) == before.active);
      before = s;
    }
    assert(!s.active);
    assert(!turn(levels()[i], s, 0, 1));
    std::cout << "Fase " << i + 1 << ": " << seq.size()
              << " giros verificados\n";
  }
  State s = initial(0);
  assert(!turn(levels()[0], s, -1, 1));
  assert(!turn(levels()[0], s, 0, 0));
  assert(s.moves == 0);
  for (int n = 0; n < 5; ++n)
    assert(turn(levels()[0], s, 0, 1));
  assert(s.active == 3);
  assert(turn(levels()[0], s, 0, 1));
  assert(s.active == 2);
  Game g;
  g.action(1);
  g.undo();
  assert(g.state.angle == initial(0).angle && g.state.moves == 0);
  g.action(-1);
  g.muted = true;
  assert(g.save("rings-test.save"));
  Game restored;
  assert(restored.load("rings-test.save"));
  assert(restored.state.angle == g.state.angle && restored.muted);
  std::ofstream("rings-test.save") << "RINGS1 99";
  assert(!restored.load("rings-test.save"));
  assert(restored.state.angle == g.state.angle);
  std::remove("rings-test.save");
  // An opening on either ring can clear their shared crossing; solid material
  // on both blocks it.
  auto custom = levels()[3];
  auto t = initial(3);
  t.angle[0] = 0;
  assert(!releasable(custom, t, 0));
  t.angle[3] = 9;
  assert(releasable(custom, t, 0));
}
