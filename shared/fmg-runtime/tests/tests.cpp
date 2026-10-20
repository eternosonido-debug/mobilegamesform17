#include "game.hpp"
#include "juice_motion.hpp"
#include <functional>
#include <iostream>
#include <queue>
#include <set>
#include <stdexcept>
using namespace fmg;
// Keep validating the FMG 1 arena rules for legacy saves.
void legacyBoca(Engine &e) {
  e.s = State{};
  e.s.game = 6;
  e.undo.clear();
  e.s.f[0] = 240;
  e.s.f[1] = 136;
  e.s.f[2] = 9;
  e.s.f[3] = 120;
  for (int i = 0; i < 60; ++i) {
    e.s.a[i] = 1;
    e.s.f[8 + i * 4] = 20 + e.roll(440);
    e.s.f[9 + i * 4] = 48 + e.roll(180);
    e.s.f[10 + i * 4] = 3 + i / 6;
  }
}
int checks = 0;
void check(bool ok, const char *msg) {
  ++checks;
  if (!ok)
    throw std::runtime_error(msg);
}
std::string trafficKey(const State &s) {
  std::string k;
  for (int i = 0; i < 8; i++) {
    k += char(s.a[i * 4]);
    k += char(s.a[i * 4 + 1]);
  }
  return k;
}
int solutions(Engine &e) {
  int pos = -1;
  for (int i = 0; i < 81; i++)
    if (!e.s.a[i]) {
      pos = i;
      break;
    }
  if (pos < 0)
    return e.sudokuWon() ? 1 : 0;
  int total = 0;
  for (int n = 1; n <= 9; n++)
    if (e.sudokuValid(pos, n)) {
      e.s.a[pos] = n;
      total += solutions(e);
      if (total >= 2)
        break;
    }
  e.s.a[pos] = 0;
  return total;
}
std::string juiceKey(const State &s) {
  std::vector<std::string> tubes;
  for (int i = 0; i < 6; i++) {
    std::string t;
    for (int j = 0; j < s.a[40 + i]; j++)
      t += char(s.a[i * 4 + j]);
    tubes.push_back(t);
  }
  std::sort(tubes.begin(), tubes.end());
  std::string key;
  for (auto t : tubes) {
    key += t;
    key += char(9);
  }
  return key;
}
bool juiceSolve(Engine &e, std::set<std::string> &seen, int depth) {
  if (e.juiceWon())
    return true;
  if (depth > 80 || seen.size() > 100000)
    return false;
  if (!seen.insert(juiceKey(e.s)).second)
    return false;
  State before = e.s;
  for (int i = 0; i < 6; i++)
    for (int j = 0; j < 6; j++) {
      e.s = before;
      if (e.pour(i, j) && juiceSolve(e, seen, depth + 1)) {
        e.s = before;
        return true;
      }
    }
  e.s = before;
  return false;
}
int main() {
  try {
    Engine e(7);
    JuiceProgress progress;
    check(progress.unlocked() == 0, "first campaign level unlocked");
    for (int level = 0; level < juiceLevelCount; ++level) {
      startJuiceLevel(e, level);
      check(validState(e.s) && juiceLevel(e.s) == level, "campaign board valid");
      int moves = 0;
      while (!e.s.phase && moves < 40) {
        auto hint = juiceHint(e.s);
        check(hint.first >= 0, "campaign solution available within hint budget");
        check(e.pour(hint.first, hint.second), "hint is a legal pour");
        ++moves;
      }
      check(e.juiceWon() && moves == juicePar[level], "campaign verified par solution");
      progress.record(e.s);
      check(progress.stars(level) == 3, "par awards three medals");
      check(progress.unlocked() == std::min(level + 1, juiceLevelCount - 1), "sequential unlock");
      State solved = e.s;
      e.s.score += 5;
      progress.record(e.s);
      check(progress.best[level] == moves, "worse replay keeps best score");
      check(progress.write("juice-progress-test.sav", solved), "campaign progress save");
      JuiceProgress restored;
      check(restored.read("juice-progress-test.sav") && restored.best == progress.best,
            "campaign progress roundtrip");
    }
    std::remove("juice-progress-test.sav");
    startJuiceLevel(e, 0);
    e.input(A);
    startJuiceLevel(e, 1);
    check(e.undo.empty() && e.s.selected == -1 && e.s.score == 0, "restart clears transient state");
    check(juiceLevel(State{}) == -1, "legacy board remains classic");
    e.reset(0);
    e.s = State{};
    for (int i = 0; i < 4; ++i) {
      e.s.a[40 + i] = 4;
      for (int j = 0; j < 4; ++j)
        e.s.a[i * 4 + j] = (i + j) % 4 + 1;
    }
    e.s.cursor = 0;
    e.input(A);
    e.s.cursor = 4;
    State pourBefore = e.s;
    e.input(A);
    State pourAfter = e.s;
    JuiceMotion motion;
    check(motion.start(pourBefore, pourAfter), "animate valid pour");
    float previous = 0;
    for (int frame = 0; frame < 120; ++frame) {
      motion.update(1.f / 60);
      check(motion.poured() >= previous && motion.poured() <= 1, "monotonic liquid animation");
      previous = motion.poured();
      check(e.s.a == pourAfter.a && e.s.score == pourAfter.score,
            "animation preserves committed rules");
    }
    check(!motion.active && motion.poured() == 1, "animation completes");
    e.input(Y);
    check(e.s.a == pourBefore.a && e.s.score == pourBefore.score, "animated move undo");
    check(!motion.start(e.s, e.s), "reject animation without move");
    e.reset(0);
    for (int seed = 0; seed < 100; seed++) {
      e.rng.seed(unsigned(seed));
      for (int g = 0; g < 7; g++) {
        e.reset(g);
        check(validState(e.s), "initial state valid");
        check(save("test.sav", e.s), "save");
        State t;
        check(load("test.sav", t), "load");
        check(t.a == e.s.a && t.f == e.s.f && t.game == g, "roundtrip");
      }
    }
    for (int seed = 0; seed < 25; seed++) {
      e.rng.seed(unsigned(seed));
      e.reset(0);
      std::set<std::string> seen;
      check(juiceSolve(e, seen, 0), "generated Juices Sort solvable");
    }
    e.reset(0);
    e.s.a.fill(0);
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        e.s.a[i * 4 + j] = i + 1;
    for (int i = 0; i < 4; i++)
      e.s.a[40 + i] = 4;
    check(e.juiceWon(), "sorted juices");
    check(!e.pour(0, 1), "different colors denied");
    check(e.pour(0, 4), "pour into empty");
    check(e.s.a[40] == 0 && e.s.a[44] == 4, "contiguous pour");
    e.s.phase = 0;
    check(!e.pour(0, 5), "empty denied");
    e.reset(1);
    e.s.a.fill(0);
    e.s.choice = 0;
    e.s.a[100] = 0;
    e.s.a[101] = 1;
    e.s.a[102] = 2;
    for (int i = 0; i < 7; i++)
      e.s.a[i] = 1;
    e.s.cursor = 7;
    check(e.place(), "place block");
    for (int i = 0; i < 8; i++)
      check(!e.s.a[i], "row clears");
    check(!e.fits(4, 7, 7), "bounds");
    e.s.a[0] = 1;
    check(!e.fits(0, 0, 0), "overlap denied");
    e.reset(2);
    e.s.a.fill(0);
    for (int i = 0; i < 3; i++)
      e.mix(1, 0);
    check(e.mixWon(), "color cycle");
    e.mix(1, 1);
    check(!e.mixWon(), "color changes");
    e.mix(1, 1);
    e.mix(1, 1);
    check(e.mixWon(), "color column solution");
    for (int seed = 0; seed < 12; seed++) {
      e.rng.seed(unsigned(seed));
      e.reset(3);
      check(solutions(e) == 1, "unique Sudoku");
      for (int i = 0; i < 81; i++)
        e.s.a[i] = e.s.a[162 + i];
      check(e.sudokuWon(), "Sudoku solution accepted");
      e.s.a[1] = e.s.a[0];
      check(!e.sudokuWon(), "duplicate Sudoku denied");
    }
    e.reset(4);
    check(!e.moveCar(0, 1, 0), "blocked car");
    check(!e.moveCar(0, 0, 1), "car axis");
    std::queue<State> q;
    q.push(e.s);
    std::set<std::string> seen;
    seen.insert(trafficKey(e.s));
    bool solved = false;
    int nodes = 0;
    while (!q.empty() && nodes < 150000) {
      State cur = q.front();
      q.pop();
      nodes++;
      if (cur.phase == 1) {
        solved = true;
        break;
      }
      for (int i = 0; i < 8; i++)
        for (int d = -1; d <= 1; d += 2) {
          e.s = cur;
          bool v = e.s.a[i * 4 + 3];
          if (e.moveCar(i, v ? 0 : d, v ? d : 0) && seen.insert(trafficKey(e.s)).second)
            q.push(e.s);
        }
    }
    check(solved, "Traffic Jam solvable");
    std::cout << "Traffic solver visited " << nodes << " states\n";
    for (int seed = 0; seed < 100; seed++) {
      e.rng.seed(unsigned(seed));
      e.reset(5);
      for (int n = 1; n <= 16; n++) {
        int i = -1, j = -1;
        for (int k = 0; k < 32; k++)
          if (e.s.a[k] == n) {
            if (i < 0)
              i = k;
            else
              j = k;
          }
        check(i >= 0 && j >= 0 && e.tileFree(i) && e.tileFree(j), "Mahjong guaranteed solution");
        e.s.a[i] = e.s.a[j] = 0;
      }
    }
    legacyBoca(e);
    e.s.f[0] = e.s.f[8];
    e.s.f[1] = e.s.f[9];
    e.tick(.01f, 0, 0);
    check(e.s.score > 0, "Boca absorbs");
    e.s.f[3] = .001f;
    e.tick(.01f, 0, 0);
    check(e.s.phase == 2, "Boca timer");
    for (unsigned seed = 1; seed <= 20; ++seed) {
      Engine route(seed);
      legacyBoca(route);
      for (int frame = 0; frame < 6001 && !route.s.phase; ++frame) {
        int target = -1;
        float closest = 1e9;
        for (int i = 0; i < 60; ++i)
          if (route.s.a[i] && route.s.f[2] > route.s.f[10 + i * 4] + 2) {
            float x = route.s.f[8 + i * 4] - route.s.f[0], y = route.s.f[9 + i * 4] - route.s.f[1];
            float distance = x * x + y * y;
            if (distance < closest) {
              closest = distance;
              target = i;
            }
          }
        check(target >= 0, "Boca always has a reachable growth target");
        float x = route.s.f[8 + target * 4] - route.s.f[0],
              y = route.s.f[9 + target * 4] - route.s.f[1];
        route.tick(.02f, (x > 1) - (x < -1), (y > 1) - (y < -1));
      }
      check(route.s.phase == 1 && route.s.score == 60 && validState(route.s),
            "Boca can clear arena within time using continuous movement");
    }
    legacyBoca(e);
    State stationary = e.s;
    e.tick(-1, 1, 1);
    check(e.s.f == stationary.f, "negative Boca time step ignored");
    e.tick(.05f, 1, 0);
    check(e.s.f[0] > stationary.f[0] && e.s.f[4] > 0 && e.s.f[4] < 150,
          "Boca accelerates progressively");
    float velocity = e.s.f[4];
    e.tick(.05f, 0, 0);
    check(e.s.f[4] < velocity, "Boca releases movement smoothly");
    e.input(A);
    check(e.s.f[6] > 0 && e.s.f[7] > 0, "Boca impulse starts with cooldown");
    float cooldown = e.s.f[7];
    e.input(A);
    check(e.s.f[7] == cooldown, "Boca impulse cannot spam cooldown");
    legacyBoca(e);
    e.s.a.fill(0);
    e.s.a[0] = 1;
    e.s.f[8] = e.s.f[0] + 17;
    e.s.f[9] = e.s.f[1];
    e.s.f[10] = 3;
    float objectX = e.s.f[8];
    e.tick(.05f, 0, 0);
    check(e.s.f[8] < objectX && e.s.a[0], "Boca attracts nearby small objects before swallowing");
    for (int frame = 0; frame < 60; ++frame)
      e.tick(.02f, 0, 0);
    check(!e.s.a[0] && e.s.score == 1 && e.s.f[2] > 9, "Boca suction absorbs and grows");
    legacyBoca(e);
    e.s.a.fill(0);
    e.s.a[0] = 1;
    e.s.f[8] = e.s.f[0];
    e.s.f[9] = e.s.f[1];
    e.s.f[10] = 12;
    e.tick(.05f, 0, 0);
    check(e.s.a[0] && e.s.score == 0, "Boca rejects objects too large");
    legacyBoca(e);
    for (int i = 0; i < 60; i++) {
      e.s.f[0] = e.s.f[8 + i * 4];
      e.s.f[1] = e.s.f[9 + i * 4];
      e.tick(.01f, 0, 0);
    }
    check(e.s.phase == 1 && e.s.score == 60, "Boca all objects reachable by growth");
    e.reset(2);
    State before = e.s;
    e.input(A);
    e.input(Y);
    check(e.s.a == before.a, "undo");
    State t = e.s;
    {
      std::ofstream out("bad.sav");
      out << "FMG 1 999";
    }
    check(!load("bad.sav", t) && t.a == e.s.a, "truncated load transactional");
    e.s.game = 7;
    check(!save("bad.sav", e.s), "reject invalid game");
    e.reset(5);
    e.s.selected = 999;
    check(!validState(e.s), "invalid selection");
    for (int g = 0; g < 7; g++) {
      e.reset(g);
      for (int k = 0; k < 3000; k++) {
        e.input(Key(e.roll(8)));
        e.tick(.02f, e.roll(3) - 1, e.roll(3) - 1);
        check(validState(e.s), "random input invariant");
      }
    }
    std::remove("test.sav");
    std::remove("bad.sav");
    std::cout << "PASS " << checks << " checks\n";
    return 0;
  } catch (const std::exception &ex) {
    std::cerr << "FAIL: " << ex.what() << " after " << checks << " checks\n";
    return 1;
  }
}
