#include "game_logic.hpp"
#include <cassert>
#include <iostream>
int main() {
  using namespace screw;
  for (int i = 0; i < 3; ++i) {
    auto s = initial(i);
    auto solution = solve(i, s);
    assert(!solution.empty());
    for (int h : solution) {
      for (int invalid : {-1, 16}) {
        auto copy = s;
        assert(!act(levels()[i], copy, invalid));
        assert(key(copy) == key(s));
      }
      assert(act(levels()[i], s, h));
    }
    assert(s.active == 0);
    assert(!act(levels()[i], s, 0));
    std::cout << "Fase " << i + 1 << ": " << solution.size()
              << " acoes verificadas\n";
  }
  Game g;
  auto before = key(g.state);
  g.selected = 15;
  assert(!g.action());
  assert(key(g.state) == before);
  g.selected = 0;
  assert(g.action());
  assert(g.state.hand);
  assert(g.state.active == 3); // One screw still supports the first plate.
  g.cancel();
  assert(key(g.state) == before);
  g.selected = 0;
  g.action();
  g.selected = 8;
  assert(g.action());
  g.selected = 1;
  assert(g.action());
  assert(g.state.active == 2); // Last support removed: only first plate falls.
  g.undo();
  assert(g.state.active == 3 && !g.state.hand);
  g.undo();
  assert(g.state.hand);
  assert(g.save("screw-test.save"));
  Game restored;
  assert(restored.load("screw-test.save"));
  assert(key(g.state) == key(restored.state));
  assert(restored.state.moves == g.state.moves);
  assert(restored.history.empty()); // Undo history intentionally session-local.
  std::ofstream("screw-test.save") << "SCREW1 99 0 0 0 0 0 0 0";
  auto unchanged = key(restored.state);
  assert(!restored.load("screw-test.save"));
  assert(key(restored.state) == unchanged);
  std::remove("screw-test.save");
  assert(!restored.load("missing.save"));
  auto s = initial(1);
  assert(!visible(levels()[1], s, 5));
  assert(visible(levels()[1], s, 2));
  s.active &= ~(1u << 1);
  assert(visible(levels()[1], s,
                 5)); // Upper plate removal exposes its lower screw.
}
