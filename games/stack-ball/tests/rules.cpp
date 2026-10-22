#include "game_logic.hpp"
#include <cassert>
#include <iostream>
using namespace tower;
bool policy(const State &s, int stage) {
  auto copy = s;
  return step(copy, stage, true).broken;
}
State simulate(int fps) {
  State s;
  int accumulator = 0;
  for (int frame = 0; frame < fps * 2; ++frame) {
    accumulator += 120;
    while (accumulator >= fps) {
      step(s, 0, policy(s, 0));
      accumulator -= fps;
    }
  }
  return s;
}
int main() {
  for (int stage = 0; stage < 3; ++stage) {
    Game g;
    g.begin(stage);
    for (int tick = 0; tick < 120000 && !won(g.state, stage); ++tick)
      g.advance(policy(g.state, stage));
    assert(won(g.state, stage) && !g.state.dead);
    assert(g.unlocked == std::min(2, stage + 1));
    std::cout << "Torre " << stage + 1 << ": " << g.state.layer << " andares, "
              << g.state.ticks << " passos fixos verificados\n";
  }
  State left, right;
  step(left, 0, false, -1);
  step(right, 0, false, 1);
  assert(left.angle == 1196 && right.angle == 4);
  State dead;
  dead.z = 1;
  dead.velocity = -100;
  dead.angle = 898;
  auto bounce = dead;
  assert(step(dead, 0, true).died && dead.dead);
  assert(step(bounce, 0, false).bounced && !bounce.dead && bounce.velocity > 0);
  auto ticks = dead.ticks;
  step(dead, 0, true);
  assert(dead.ticks == ticks);
  State safe;
  safe.z = 1;
  safe.velocity = -100;
  safe.angle = 0;
  assert(step(safe, 0, true).broken && safe.layer == 1 && safe.combo == 1 &&
         safe.score == 10);
  auto second = safe;
  second.z = 1;
  second.angle = 0;
  assert(step(second, 0, true).broken && second.combo == 2 &&
         second.score == 30);
  auto a = simulate(30), b = simulate(60);
  assert(a.z == b.z && a.angle == b.angle && a.ticks == b.ticks &&
         a.layer == b.layer && a.score == b.score);
  for (int stage = 0; stage < 3; ++stage)
    for (int row = 0; row < level(stage).layers; ++row) {
      int safeCount = 0;
      for (int angle = 0; angle < 1200; ++angle)
        safeCount += !dangerAt(stage, row, angle);
      assert(safeCount > 0 && safeCount < 1200);
    }
  Game g;
  g.state = safe;
  assert(g.save("tower-test.save"));
  Game restored;
  assert(restored.load("tower-test.save"));
  assert(restored.state.layer == g.state.layer &&
         restored.state.z == g.state.z &&
         restored.state.angle == g.state.angle);
  auto original = restored.state;
  std::ofstream("tower-test.save") << "TOWER1 99";
  assert(!restored.load("tower-test.save") &&
         restored.state.layer == original.layer);
  std::remove("tower-test.save");
}
