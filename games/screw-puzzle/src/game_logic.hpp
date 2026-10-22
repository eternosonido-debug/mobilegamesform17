#pragma once
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <initializer_list>
#include <map>
#include <string>
#include <vector>
namespace screw {
inline unsigned bits(std::initializer_list<int> a) {
  unsigned b = 0;
  for (int i : a)
    b |= 1u << i;
  return b;
}
struct Plate {
  unsigned cover, pins;
  int color;
};
struct Level {
  unsigned holes;
  std::vector<Plate> plates;
};
inline const std::vector<Level> &levels() {
  static const std::vector<Level> l = {
      {bits({0, 1, 4, 5, 8, 9}),
       {{bits({0, 1}), bits({0, 1}), 0}, {bits({4, 5}), bits({4, 5}), 1}}},
      {bits({5, 9, 10, 4, 7, 2, 14, 8, 11}),
       {{bits({5, 6, 9, 10}), bits({5, 9, 10}), 1},
        {bits({4, 5, 6, 7}), bits({4, 7}), 2},
        {bits({2, 6, 10, 14}), bits({2, 14}), 0}}},
      {bits({8, 11, 13, 5, 9, 10, 4, 7, 2, 14, 0, 1, 3}),
       {{bits({8, 9, 10, 11, 12, 13, 14, 15}), bits({8, 11, 13}), 3},
        {bits({5, 6, 9, 10}), bits({5, 9, 10}), 1},
        {bits({4, 5, 6, 7}), bits({4, 7}), 2},
        {bits({2, 6, 10, 14}), bits({2, 14}), 0}}}};
  return l;
}
struct State {
  unsigned occupied = 0, active = 0;
  bool hand = false;
  int moves = 0;
};
inline State initial(int level) {
  State s;
  const auto &l = levels().at(level);
  for (auto &p : l.plates)
    s.occupied |= p.pins;
  s.active = (1u << l.plates.size()) - 1;
  return s;
}
// A screw head belongs to the highest plate drilled at that position.
// Solid material above that head blocks both pickup and placement.
inline bool visible(const Level &l, const State &s, int hole) {
  if (hole < 0 || hole >= 16 || !(l.holes & (1u << hole)))
    return false;
  int top = -1;
  for (unsigned i = 0; i < l.plates.size(); ++i)
    if ((s.active & (1u << i)) && (l.plates[i].pins & (1u << hole)))
      top = int(i);
  for (unsigned i = top + 1; i < l.plates.size(); ++i)
    if ((s.active & (1u << i)) && (l.plates[i].cover & (1u << hole)) &&
        !(l.plates[i].pins & (1u << hole)))
      return false;
  return true;
}
inline bool act(const Level &l, State &s, int h) {
  if (!s.active || !visible(l, s, h))
    return false;
  unsigned b = 1u << h;
  if (bool(s.occupied & b) == s.hand)
    return false;
  s.occupied ^= b;
  if (!s.hand)
    ++s.moves;
  s.hand = !s.hand;
  for (unsigned i = 0; i < l.plates.size(); ++i)
    if (!(s.occupied & l.plates[i].pins))
      s.active &= ~(1u << i);
  return true;
}
inline unsigned key(const State &s) {
  return s.occupied | (s.active << 16) | (unsigned(s.hand) << 24);
}
// Breadth-first search explores only rule states, independently of rendering.
inline std::vector<int> solve(int level, State start) {
  struct Node {
    State s;
    int parent, hole;
  };
  std::vector<Node> q{{start, -1, -1}};
  std::map<unsigned, int> seen;
  seen[key(start)] = 0;
  for (size_t n = 0; n < q.size() && q.size() < 500000; ++n) {
    if (!q[n].s.active) {
      std::vector<int> result;
      for (int i = int(n); q[i].parent >= 0; i = q[i].parent)
        result.push_back(q[i].hole);
      std::reverse(result.begin(), result.end());
      return result;
    }
    for (int h = 0; h < 16; ++h) {
      State s = q[n].s;
      if (act(levels()[level], s, h) && !seen.count(key(s))) {
        seen[key(s)] = int(q.size());
        q.push_back({s, int(n), h});
      }
    }
  }
  return {};
}
struct Game {
  int level = 0, selected = 0, unlocked = 0;
  bool muted = false;
  State state = initial(0);
  std::vector<State> history;
  void begin(int n) {
    level = std::max(0, std::min(n, 2));
    state = initial(level);
    history.clear();
    selected = 0;
    while (!(levels()[level].holes & (1u << selected)))
      ++selected;
  }
  bool action() {
    State before = state;
    if (!act(levels()[level], state, selected))
      return false;
    history.push_back(before);
    if (history.size() > 64)
      history.erase(history.begin());
    if (!state.active)
      unlocked = std::max(unlocked, std::min(2, level + 1));
    return true;
  }
  void undo() {
    if (!history.empty()) {
      state = history.back();
      history.pop_back();
    }
  }
  void cancel() {
    if (state.hand)
      undo();
  }
  void navigate(int dx, int dy) {
    int best = -1, score = 10000;
    for (int h = 0; h < 16; ++h)
      if (levels()[level].holes & (1u << h)) {
        int x = h % 4 - selected % 4, y = h / 4 - selected / 4;
        if (x * dx + y * dy <= 0)
          continue;
        int cost =
            (std::abs(x) + std::abs(y)) * 10 + std::abs(x * dy - y * dx) * 20;
        if (cost < score) {
          score = cost;
          best = h;
        }
      }
    if (best >= 0)
      selected = best;
  }
  bool save(const std::string &path) const {
    std::string tmp = path + ".tmp";
    std::ofstream f(tmp);
    f << "SCREW1 " << level << ' ' << selected << ' ' << unlocked << ' '
      << muted << ' ' << state.occupied << ' ' << state.active << ' '
      << state.hand << ' ' << state.moves << '\n';
    f.close();
    return bool(f) && std::rename(tmp.c_str(), path.c_str()) == 0;
  }
  bool load(const std::string &path) {
    std::ifstream f(path);
    std::string magic;
    Game g;
    if (!(f >> magic >> g.level >> g.selected >> g.unlocked >> g.muted >>
          g.state.occupied >> g.state.active >> g.state.hand >>
          g.state.moves) ||
        magic != "SCREW1" || g.level < 0 || g.level > 2 || g.selected < 0 ||
        g.selected > 15 || g.unlocked < g.level || g.unlocked > 2 ||
        g.state.moves < 0)
      return false;
    auto &l = levels()[g.level];
    if ((g.state.occupied & ~l.holes) ||
        (g.state.active & ~((1u << l.plates.size()) - 1)) ||
        !(l.holes & (1u << g.selected)))
      return false;
    for (unsigned i = 0; i < l.plates.size(); ++i)
      if ((g.state.active & (1u << i)) &&
          !(g.state.occupied & l.plates[i].pins))
        return false;
    State init = initial(g.level);
    int count = 0;
    for (unsigned b = g.state.occupied; b; b &= b - 1)
      ++count;
    int total = 0;
    for (unsigned b = init.occupied; b; b &= b - 1)
      ++total;
    if (count + int(g.state.hand) != total)
      return false;
    *this = g;
    return true;
  }
};
} // namespace screw
