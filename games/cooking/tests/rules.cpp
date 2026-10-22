#include "demo.hpp"
#include <cassert>
#include <iostream>
int main() {
  using namespace cooking;
  for (int flavor = 0; flavor < FlavorCount; ++flavor) {
    for (int stage = 0; stage <= Photo; ++stage) {
      Game g = demo(flavor, stage);
      assert(g.valid() && g.s.stage == stage);
      assert(g.save("cooking-test.save"));
      Game loaded;
      assert(loaded.load("cooking-test.save"));
      assert(loaded.s.cakes == g.s.cakes && loaded.s.used == g.s.used &&
             loaded.s.stage == stage);
    }
    Game g = demo(flavor, Mixing);
    int count = g.s.mixed;
    g.direction(0);
    assert(!g.direction(2) && g.s.mixed == count + 1);
    g = demo(flavor, Baking);
    assert(!g.golden());
    assert(g.action());
    while (g.s.pending != Idle)
      g.tick();
    assert(!g.action()); // Cannot remove raw batter.
    for (int i = 0; i < g.bakeGoal() / 2 - 1; ++i)
      g.tick();
    assert(!g.golden());
    g.tick();
    assert(g.golden());
    assert(g.action());
    while (g.s.pending != Idle)
      g.tick();
    assert(g.s.stage == Decorating);
    g.chooseTool(0);
    g.s.option = 6;
    assert(g.action());
    assert(g.s.cakes[0].icing == 6);
    assert(g.undo() && g.s.cakes[0].icing == 0 && !g.undo());
    std::cout << flavorName(flavor)
              << ": 6 etapas, forno, decoracao e album verificados\n";
  }
  Game g;
  g.begin(0);
  assert(g.action());
  for (int i = 0; i < 33; ++i)
    g.tick();
  assert(!g.action() && !g.direction(1));
  assert(g.save("cooking-test.save"));
  Game resumed;
  assert(resumed.load("cooking-test.save"));
  for (int i = 0; i < 33; ++i) {
    g.tick();
    resumed.tick();
  }
  assert(g.s.used == 1 && resumed.s.used == 1);
  assert(!g.action()); // Do not add the same flour twice.
  Game full = demo(1, Decorating);
  for (int i = 0; i < 10; ++i) {
    full.chooseTool(5);
    assert(full.action());
    if (i < 9) {
      auto album = full.album;
      auto count = full.albumCount;
      full.begin(i % 3);
      Game next = demo(i % 3, Decorating);
      full.s = next.s;
      assert(full.album[0].serial == album[0].serial &&
             full.album[0].cakes == album[0].cakes);
      assert(full.albumCount == count);
    }
  }
  assert(full.albumCount == 8 && full.albumNext == 2 && full.serial == 10 &&
         full.valid());
  assert(full.save("cooking-test.save") && resumed.load("cooking-test.save"));
  assert(resumed.serial == 10);
  auto original = resumed.s;
  std::ofstream("cooking-test.save") << "COOK1 0 99";
  assert(!resumed.load("cooking-test.save") &&
         resumed.s.stage == original.stage);
  for (int bad = 0; bad < 7; ++bad) {
    Game invalid = demo(0, Decorating);
    if (bad == 0)
      invalid.s.cake = 4;
    if (bad == 1)
      invalid.s.tool = 6;
    if (bad == 2)
      invalid.s.option = 7;
    if (bad == 3)
      invalid.s.cakes[0].fruit = -1;
    if (bad == 4)
      invalid.s.used = 0;
    if (bad == 5)
      invalid.s.flavor = 99;
    if (bad == 6)
      invalid.s.pending = 0;
    assert(invalid.save("cooking-test.save") &&
           !resumed.load("cooking-test.save"));
  }
  std::remove("cooking-test.save");
}
