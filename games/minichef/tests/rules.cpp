#include "game_logic.hpp"
#include <cassert>
#include <iostream>
void solve(chef::Game &g) {
  using namespace chef;
  while (g.s.step != Served) {
    assert(g.save("chef-phase.save"));
    Game copy;
    assert(copy.load("chef-phase.save") && copy.s.step == g.s.step &&
           copy.s.ticks == g.s.ticks && copy.s.lastCut == g.s.lastCut);
    switch (g.s.step) {
    case Pantry:
      for (int i = 0; i < 8 && g.s.step == Pantry; ++i)
        if (recipe(g.s.level).ingredients & (1u << i)) {
          g.s.selected = i;
          assert(g.action());
        }
      break;
    case Chop:
      while (std::abs(g.marker() - 50) > 2 || g.s.phaseTicks - g.s.lastCut < 20)
        g.tick();
      assert(g.action());
      g.tick();
      break;
    case Mix:
      assert(g.direction(g.nextDirection()));
      break;
    case Cook:
      g.s.heat = g.s.temperature < 520 ? 4 : 3;
      if (g.warm() && g.s.phaseTicks - g.s.lastStir >= 60)
        g.action();
      g.tick();
      break;
    case Plate:
      g.s.selected = g.s.garnish;
      assert(g.action());
      break;
    }
    assert(g.s.ticks < 60000);
  }
}
int main() {
  using namespace chef;
  Game g;
  for (int l = 0; l < RecipeCount; ++l) {
    g.begin(l);
    assert(g.s.level == l);
    int wrong = 0;
    while (recipe(l).ingredients & (1u << wrong))
      ++wrong;
    g.s.selected = wrong;
    assert(!g.action() && g.s.basket == 0 && g.s.errors == 1);
    solve(g);
    assert(g.s.step == Served && g.stars[l] == 3);
    assert(g.save("chef-test.save"));
    Game load;
    assert(load.load("chef-test.save"));
    assert(load.s.ticks == g.s.ticks && load.stars == g.stars);
    std::cout << recipe(l).name << ": todas as etapas, " << g.s.ticks / 60
              << " segundos ativos na simulacao\n";
  }
  Game locked;
  locked.begin(2);
  assert(locked.s.level == 0);
  Game cook;
  cook.s.step = Cook;
  cook.s.basket = 7;
  cook.s.cuts = 6;
  cook.s.turns = 8;
  cook.s.heat = 4;
  for (int i = 0; i < 600; ++i)
    cook.tick();
  assert(cook.s.step == Cook); // Cannot finish without stirring.
  assert(cook.save("chef-test.save"));
  Game resumed;
  assert(resumed.load("chef-test.save"));
  for (int i = 0; i < 600; ++i) {
    cook.tick();
    resumed.tick();
  }
  assert(cook.s.temperature == resumed.s.temperature &&
         cook.s.goodTicks == resumed.s.goodTicks);
  auto original = resumed.s;
  std::ofstream("chef-test.save") << "CHEF1 99";
  assert(!resumed.load("chef-test.save") && resumed.s.ticks == original.ticks);
  std::remove("chef-test.save");
  std::remove("chef-phase.save");
  Game invalid = cook;
  invalid.s.step = Plate;
  invalid.s.garnish = 3;
  assert(invalid.save("chef-test.save"));
  assert(!resumed.load("chef-test.save"));
  std::remove("chef-test.save");
  Game mix;
  mix.s.step = Mix;
  int turns = mix.s.turns;
  assert(!mix.direction((mix.nextDirection() + 1) % 4) && mix.s.turns == turns);
  Game timing;
  timing.s.step = Chop;
  assert(!timing.action() && timing.s.cuts == 0);
  Game a, b;
  for (int frame = 0; frame < 300; ++frame) {
    a.tick();
    a.tick();
  }
  for (int frame = 0; frame < 600; ++frame)
    b.tick();
  assert(a.s.ticks == b.s.ticks && a.marker() == b.marker());
}
