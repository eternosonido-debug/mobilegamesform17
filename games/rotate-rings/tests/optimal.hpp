#pragma once
#include "game_logic.hpp"
#include <cstdint>
#include <iostream>
#include <queue>
#include <unordered_map>
namespace verification {
using namespace rings;
uint64_t key(State s) {
  uint64_t k = s.active;
  for (int i = 0; i < 5; ++i)
    k |= uint64_t(s.active & (1u << i) ? s.angle[i] : 0) << (5 + 4 * i);
  return k;
}
int h(const Level &l, State s) {
  int n = 0;
  for (size_t i = 0; i < l.rings.size(); ++i)
    if (s.active & (1u << i)) {
      int dmin = 6;
      for (int d = -6; d <= 6; ++d)
        if (opening((s.angle[i] + d + 12) % 12, radians(l.rings[i].pin)))
          dmin = std::min(dmin, std::abs(d));
      n += dmin;
    }
  return n;
}
struct Node {
  State s;
  int g, f;
  bool operator<(const Node &b) const { return f != b.f ? f > b.f : g < b.g; }
};
int optimal(const Level &l, State start) {
  std::priority_queue<Node> q;
  q.push({start, 0, h(l, start)});
  std::unordered_map<uint64_t, int> seen;
  seen[key(start)] = 0;
  while (!q.empty() && seen.size() < 2000000) {
    auto n = q.top();
    q.pop();
    if (seen[key(n.s)] != n.g)
      continue;
    if (!n.s.active) {
      return n.g;
    }
    for (size_t r = 0; r < l.rings.size(); ++r)
      for (int d : {-1, 1}) {
        auto s = n.s;
        if (!turn(l, s, int(r), d))
          continue;
        auto k = key(s);
        auto it = seen.find(k);
        if (it == seen.end() || it->second > n.g + 1) {
          seen[k] = n.g + 1;
          q.push({s, n.g + 1, n.g + 1 + h(l, s)});
        }
      }
  }
  return -1;
}

} // namespace verification
