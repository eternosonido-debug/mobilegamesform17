#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
namespace rings {
constexpr double pi = 3.14159265358979323846;
constexpr double radius = 17.0;
constexpr double gapHalf =
    pi / 4; // A 90-degree opening, in twelve 30-degree steps.
struct Ring {
  double x, y;
  int pin, initial;
};
struct Level {
  std::vector<Ring> rings;
};
inline const std::vector<Level> &levels() {
  static const std::vector<Level> l = {
      {{{60, 65, 0, 6}, {92, 65, 6, 3}}},
      {{{40, 65, 0, 5}, {72, 65, 0, 4}, {104, 65, 6, 9}}},
      {{{44, 43, 0, 6}, {76, 43, 3, 9}, {76, 75, 0, 6}, {108, 75, 6, 0}}},
      {{{58, 43, 0, 6}, {90, 43, 3, 9}, {90, 75, 6, 0}, {58, 75, 9, 3}}}};
  return l;
}
inline double radians(int step) { return step * pi / 6; }
inline double distanceAngle(double a, double b) {
  return std::abs(std::atan2(std::sin(a - b), std::cos(a - b)));
}
inline bool opening(int orientation, double angle) {
  return distanceAngle(radians(orientation), angle) <= gapHalf + 1e-8;
}
struct State {
  std::array<int, 4> angle{{0, 0, 0, 0}};
  unsigned active = 0;
  int moves = 0;
};
inline State initial(int level) {
  State s;
  auto &l = levels().at(level);
  s.active = (1u << l.rings.size()) - 1;
  for (size_t i = 0; i < l.rings.size(); ++i)
    s.angle[i] = l.rings[i].initial;
  return s;
}
// At each circle/circle crossing, at least one ring must have an opening.
// The board pin must also fit through the selected ring's opening.
inline bool releasable(const Level &l, const State &s, int selected) {
  if (selected < 0 || selected >= int(l.rings.size()) ||
      !(s.active & (1u << selected)))
    return false;
  const auto &a = l.rings[selected];
  if (!opening(s.angle[selected], radians(a.pin)))
    return false;
  for (size_t j = 0; j < l.rings.size(); ++j) {
    if (int(j) == selected || !(s.active & (1u << j)))
      continue;
    auto &b = l.rings[j];
    double dx = b.x - a.x, dy = b.y - a.y, d = std::hypot(dx, dy);
    if (d >= 2 * radius)
      continue;
    if (d < 1e-6)
      return false;
    double center = std::atan2(dy, dx), delta = std::acos(d / (2 * radius));
    for (int sign : {-1, 1}) {
      double theta = center + sign * delta;
      double x = a.x + radius * std::cos(theta),
             y = a.y + radius * std::sin(theta);
      double other = std::atan2(y - b.y, x - b.x);
      if (!opening(s.angle[selected], theta) && !opening(s.angle[j], other))
        return false;
    }
  }
  return true;
}
inline unsigned release(const Level &l, State &s) {
  unsigned before = s.active;
  bool changed = true;
  while (changed) {
    changed = false;
    for (size_t i = 0; i < l.rings.size(); ++i)
      if (releasable(l, s, int(i))) {
        s.active &= ~(1u << i);
        changed = true;
      }
  }
  return before & ~s.active;
}
inline bool turn(const Level &l, State &s, int selected, int direction) {
  if (selected < 0 || selected >= int(l.rings.size()) ||
      !(s.active & (1u << selected)) || (direction != 1 && direction != -1))
    return false;
  s.angle[selected] = (s.angle[selected] + direction + 12) % 12;
  ++s.moves;
  release(l, s);
  return true;
}
struct Move {
  int ring, direction;
};
// Constructive level verification: orient each surviving opening toward its
// pin. Replaying the resulting list must release every ring under the geometry
// above.
inline std::vector<Move> solution(int level) {
  auto s = initial(level);
  auto &l = levels()[level];
  std::vector<Move> result;
  for (size_t i = 0; i < l.rings.size(); ++i) {
    while ((s.active & (1u << i)) && s.angle[i] != l.rings[i].pin) {
      int diff = (l.rings[i].pin - s.angle[i] + 12) % 12;
      int d = diff <= 6 ? 1 : -1;
      result.push_back({int(i), d});
      turn(l, s, int(i), d);
    }
  }
  return s.active ? std::vector<Move>{} : result;
}
struct Game {
  int level = 0, selected = 0, unlocked = 0;
  bool muted = false;
  State state = initial(0);
  std::vector<State> history;
  void selectAlive() {
    if (state.active & (1u << selected))
      return;
    for (int i = 0; i < int(levels()[level].rings.size()); ++i)
      if (state.active & (1u << i)) {
        selected = i;
        return;
      }
  }
  void begin(int n) {
    level = std::max(0, std::min(n, int(levels().size()) - 1));
    state = initial(level);
    history.clear();
    selected = 0;
  }
  bool action(int direction) {
    State before = state;
    if (!turn(levels()[level], state, selected, direction))
      return false;
    history.push_back(before);
    if (history.size() > 128)
      history.erase(history.begin());
    if (!state.active)
      unlocked =
          std::max(unlocked, std::min(int(levels().size()) - 1, level + 1));
    return true;
  }
  void undo() {
    if (history.empty())
      return;
    state = history.back();
    history.pop_back();
    selectAlive();
  }
  void navigate(int dx, int dy) {
    selectAlive();
    auto &a = levels()[level].rings[selected];
    int best = -1;
    double score = 1e9;
    for (size_t i = 0; i < levels()[level].rings.size(); ++i)
      if ((state.active & (1u << i)) && int(i) != selected) {
        auto &b = levels()[level].rings[i];
        double x = b.x - a.x, y = b.y - a.y;
        if (x * dx + y * dy <= 0)
          continue;
        double cost = std::hypot(x, y) + 2 * std::abs(x * dy - y * dx);
        if (cost < score) {
          score = cost;
          best = int(i);
        }
      }
    if (best >= 0)
      selected = best;
  }
  bool save(const std::string &path) const {
    std::string tmp = path + ".tmp";
    std::ofstream f(tmp);
    f << "RINGS2 " << level << ' ' << selected << ' ' << unlocked << ' '
      << muted << ' ' << state.active << ' ' << state.moves;
    for (int a : state.angle)
      f << ' ' << a;
    f << '\n';
    f.close();
    return bool(f) && std::rename(tmp.c_str(), path.c_str()) == 0;
  }
  bool load(const std::string &path) {
    std::ifstream f(path);
    std::string magic;
    Game g;
    if (!(f >> magic >> g.level >> g.selected >> g.unlocked >> g.muted >>
          g.state.active >> g.state.moves) ||
        (magic != "RINGS1" && magic != "RINGS2") || g.level < 0 ||
        g.level >= int(levels().size()) || g.selected < 0 ||
        g.selected >= int(levels()[g.level].rings.size()) ||
        g.unlocked < g.level || g.unlocked >= int(levels().size()) ||
        g.state.moves < 0 ||
        g.state.active & ~((1u << levels()[g.level].rings.size()) - 1))
      return false;
    for (int &a : g.state.angle)
      if (!(f >> a) || a < 0 || a >= 12)
        return false;
    auto normalized = g.state;
    release(levels()[g.level], normalized);
    if (normalized.active != g.state.active && magic == "RINGS2")
      return false;
    // Preserve old progress when the wider spacing clears a connection.
    g.state = normalized;
    if (!g.state.active)
      g.unlocked =
          std::max(g.unlocked, std::min(int(levels().size()) - 1, g.level + 1));
    g.selectAlive();
    *this = g;
    return true;
  }
};
} // namespace rings
