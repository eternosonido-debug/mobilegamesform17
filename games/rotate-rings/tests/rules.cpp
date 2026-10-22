#include "game_logic.hpp"
#include "optimal.hpp"
#include <cassert>
#include <iostream>
int main() {
  using namespace rings;
  assert(opening(0, 0));
  assert(opening(0, 2 * pi - .1));
  assert(!opening(0, pi));
  assert(!opening(0, gapHalf + .01));
  int previousMinimum = 0;
  assert(levels().size() == 20);
  for (int i = 0; i < int(levels().size()); ++i) {
    auto s = initial(i);
    auto before = s;
    assert(release(levels()[i], s) == 0);
    auto seq = solution(i);
    assert(!seq.empty());
    int bound = 0;
    for (size_t j = 0; j < levels()[i].rings.size(); ++j) {
      int minimum = 12;
      for (int d = -6; d <= 6; ++d)
        if (opening((s.angle[j] + d + 12) % 12,
                    radians(levels()[i].rings[j].pin)))
          minimum = std::min(minimum, std::abs(d));
      bound += minimum;
    }
    int minimum = verification::optimal(levels()[i], s);
    assert(minimum > previousMinimum && minimum >= bound);
    assert(minimum == (i < 4 ? 2 + 2 * i : i + 5));
    previousMinimum = minimum;
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
  s.angle[0] = 6;
  s.angle[1] = 3;
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
  // Import the previous save format without discarding progress or angles.
  std::ofstream("rings-test.save") << "RINGS1 0 0 0 1 3 2 7 4 0 0";
  assert(restored.load("rings-test.save"));
  assert(restored.state.moves == 2 && restored.state.angle[0] == 7 &&
         restored.muted);
  std::remove("rings-test.save");
  std::ofstream("rings-test.save") << "RINGS2 0 0 0 0 3 2 7 4 0 0";
  assert(restored.load("rings-test.save"));
  Game campaign;
  for (int i = 0; i < 20; ++i) {
    campaign.begin(i);
    for (auto m : solution(i)) {
      campaign.selected = m.ring;
      assert(campaign.action(m.direction));
    }
    assert(!campaign.state.active);
    assert(campaign.unlocked == std::min(19, i + 1));
  }
  assert(campaign.save("rings-test.save"));
  assert(restored.load("rings-test.save") && restored.level == 19 &&
         restored.unlocked == 19);
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
