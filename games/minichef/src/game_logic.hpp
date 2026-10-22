#pragma once
#include <algorithm>
#include <array>
#include <cstdio>
#include <fstream>
#include <string>
namespace chef {
constexpr int RecipeCount = 3;
enum Step { Pantry, Chop, Mix, Cook, Plate, Served };
struct Recipe {
  const char *name;
  unsigned ingredients;
  int cuts, turns, cooking;
};
inline const Recipe &recipe(int id) {
  static const Recipe data[] = {{"SOPA DA HORTA", 7, 6, 8, 600},
                                {"PANQUECA DO SOL", 56, 8, 10, 780},
                                {"TORTINHA BERRY", 112, 10, 12, 960}};
  return data[std::max(0, std::min(2, id))];
}
inline const char *ingredient(int id) {
  const char *names[] = {"CENOURA", "TOMATE", "BATATA",  "BANANA",
                         "FARINHA", "OVO",    "MORANGO", "MANJERICAO"};
  return names[std::max(0, std::min(7, id))];
}
struct State {
  int level = 0, step = Pantry, selected = 0;
  unsigned basket = 0;
  int cuts = 0, turns = 0, goodTicks = 0, stirs = 0, garnish = 0;
  int heat = 2, temperature = 200, phaseTicks = 0, ticks = 0;
  int errors = 0, lastStir = -120, lastCut = -120;
  std::array<int, 3> decorations{{0, 0, 0}};
};
struct Game {
  State s;
  int unlocked = 0;
  std::array<int, RecipeCount> stars{{0, 0, 0}};
  bool muted = false;
  void begin(int level) {
    s = State();
    s.level = std::max(0, std::min(unlocked, level));
  }
  int marker() const {
    int pos = (s.phaseTicks * (2 + s.level)) % 200;
    return pos > 100 ? 200 - pos : pos;
  }
  int nextDirection() const {
    const int pattern[] = {3, 1, 2, 0}; // up, right, down, left
    return pattern[(s.turns + s.level) % 4];
  }
  bool warm() const { return s.temperature >= 420 && s.temperature <= 620; }
  int rating() const { return s.errors <= 2 ? 3 : s.errors <= 8 ? 2 : 1; }
  void stage(int step) {
    s.step = step;
    s.phaseTicks = 0;
    s.selected = 0;
  }
  void tick() {
    if (s.step == Served)
      return;
    ++s.ticks;
    ++s.phaseTicks;
    if (s.step == Cook) {
      if (s.phaseTicks % 6 == 0)
        s.temperature = std::max(
            0, std::min(900, s.temperature + s.heat * 5 - s.temperature / 30));
      if (warm())
        ++s.goodTicks;
      if (s.temperature > 650 && s.phaseTicks % 120 == 0)
        ++s.errors;
      if (s.goodTicks >= recipe(s.level).cooking && s.stirs >= 4)
        stage(Plate);
    }
  }
  // Directions: left=0, right=1, down=2, up=3. Returns true for accepted
  // action.
  bool direction(int dir) {
    if (dir < 0 || dir > 3)
      return false;
    if (s.step == Pantry) {
      int x = s.selected % 4, y = s.selected / 4;
      if (dir < 2)
        x = (x + (dir == 1 ? 1 : 3)) % 4;
      else
        y = 1 - y;
      s.selected = y * 4 + x;
      return true;
    }
    if (s.step == Mix) {
      if (dir != nextDirection()) {
        ++s.errors;
        return false;
      }
      if (++s.turns >= recipe(s.level).turns)
        stage(Cook);
      return true;
    }
    if (s.step == Cook && dir < 2) {
      s.heat = std::max(0, std::min(4, s.heat + (dir == 1 ? 1 : -1)));
      return true;
    }
    if (s.step == Plate && dir < 2) {
      s.selected = (s.selected + (dir == 1 ? 1 : 2)) % 3;
      return true;
    }
    return false;
  }
  bool action() {
    if (s.step == Pantry) {
      unsigned bit = 1u << s.selected;
      if (!(recipe(s.level).ingredients & bit)) {
        ++s.errors;
        return false;
      }
      s.basket |= bit;
      if (s.basket == recipe(s.level).ingredients)
        stage(Chop);
      return true;
    }
    if (s.step == Chop) {
      int tolerance = 20 - s.level * 3;
      if (s.phaseTicks - s.lastCut < 20)
        return false;
      if (std::abs(marker() - 50) > tolerance) {
        ++s.errors;
        return false;
      }
      s.lastCut = s.phaseTicks;
      if (++s.cuts >= recipe(s.level).cuts)
        stage(Mix);
      return true;
    }
    if (s.step == Cook) {
      if (!warm() || s.phaseTicks - s.lastStir < 60)
        return false;
      s.lastStir = s.phaseTicks;
      s.stirs = std::min(4, s.stirs + 1);
      return true;
    }
    if (s.step == Plate) {
      s.decorations[s.garnish++] = s.selected;
      if (s.garnish == 3) {
        stage(Served);
        stars[s.level] = std::max(stars[s.level], rating());
        unlocked = std::max(unlocked, std::min(2, s.level + 1));
      }
      return true;
    }
    return false;
  }
  bool save(const std::string &path) const {
    std::string temp = path + ".tmp";
    std::ofstream f(temp);
    f << "CHEF1 " << unlocked << ' ' << muted;
    for (int n : stars)
      f << ' ' << n;
    f << ' ' << s.level << ' ' << s.step << ' ' << s.selected << ' ' << s.basket
      << ' ' << s.cuts << ' ' << s.turns << ' ' << s.goodTicks << ' ' << s.stirs
      << ' ' << s.garnish << ' ' << s.heat << ' ' << s.temperature << ' '
      << s.phaseTicks << ' ' << s.ticks << ' ' << s.errors << ' ' << s.lastStir
      << ' ' << s.lastCut;
    for (int n : s.decorations)
      f << ' ' << n;
    f << '\n';
    f.close();
    return bool(f) && std::rename(temp.c_str(), path.c_str()) == 0;
  }
  bool load(const std::string &path) {
    std::ifstream f(path);
    std::string magic;
    Game g;
    if (!(f >> magic >> g.unlocked >> g.muted) || magic != "CHEF1" ||
        g.unlocked < 0 || g.unlocked >= RecipeCount)
      return false;
    for (int &n : g.stars)
      if (!(f >> n) || n < 0 || n > 3)
        return false;
    State &v = g.s;
    if (!(f >> v.level >> v.step >> v.selected >> v.basket >> v.cuts >>
          v.turns >> v.goodTicks >> v.stirs >> v.garnish >> v.heat >>
          v.temperature >> v.phaseTicks >> v.ticks >> v.errors >> v.lastStir >>
          v.lastCut))
      return false;
    for (int &n : v.decorations)
      if (!(f >> n) || n < 0 || n > 2)
        return false;
    if (v.level < 0 || v.level > g.unlocked || v.step < Pantry ||
        v.step > Served || v.selected < 0 ||
        v.selected >= (v.step == Pantry ? 8 : 3) ||
        (v.basket & ~recipe(v.level).ingredients) || v.cuts < 0 ||
        v.cuts > recipe(v.level).cuts || v.turns < 0 ||
        v.turns > recipe(v.level).turns || v.goodTicks < 0 ||
        v.goodTicks > 100000000 || v.stirs < 0 || v.stirs > 4 ||
        v.garnish < 0 || v.garnish > 3 || v.heat < 0 || v.heat > 4 ||
        v.temperature < 0 || v.temperature > 900 || v.phaseTicks < 0 ||
        v.ticks < v.phaseTicks || v.ticks > 100000000 || v.errors < 0 ||
        v.errors > 100000000 || v.lastStir < -120 || v.lastStir > v.ticks ||
        v.lastCut < -120 || v.lastCut > v.ticks ||
        (v.step > Pantry && v.basket != recipe(v.level).ingredients) ||
        (v.step > Chop && v.cuts != recipe(v.level).cuts) ||
        (v.step > Mix && v.turns != recipe(v.level).turns) ||
        (v.step > Cook &&
         (v.goodTicks < recipe(v.level).cooking || v.stirs != 4)) ||
        (v.step == Plate && v.garnish >= 3) ||
        (v.step == Served && v.garnish != 3) ||
        (v.step < Plate && v.garnish != 0))
      return false;
    *this = g;
    return true;
  }
};
} // namespace chef
