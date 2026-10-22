#pragma once
#include "game_logic.hpp"
namespace cooking {
// Deterministic preview/test setup through the same actions used by real play.
inline Game demo(int flavor, int targetStage) {
  Game g;
  g.begin(flavor);
  auto finish = [&]() {
    while (g.s.pending != Idle)
      g.tick();
  };
  if (targetStage <= Ingredients)
    return g;
  for (int i = 0; i < 6; ++i) {
    g.s.selected = i;
    g.action();
    finish();
  }
  if (targetStage <= Mixing)
    return g;
  int direction = 0;
  while (g.s.stage == Mixing) {
    g.direction(direction++ % 4);
    finish();
  }
  if (targetStage <= Filling)
    return g;
  for (int i = 0; i < 4; ++i) {
    g.s.selected = i;
    g.action();
    finish();
  }
  if (targetStage <= Baking)
    return g;
  g.action();
  finish();
  while (!g.golden())
    g.tick();
  g.action();
  finish();
  for (int i = 0; i < 4; ++i) {
    g.s.cake = i;
    for (int t = 0; t < 5; ++t) {
      g.chooseTool(t);
      g.s.option = t == 0   ? (i + flavor) % 6 + 1
                   : t == 1 ? (i % 3) + 1
                   : t == 2 ? i + 1
                   : t == 3 ? (i % 3) + 1
                            : (i + flavor) % 6;
      g.action();
    }
  }
  g.s.cake = 0;
  g.chooseTool(0);
  if (targetStage <= Decorating)
    return g;
  g.chooseTool(5);
  g.action();
  return g;
}
} // namespace cooking
