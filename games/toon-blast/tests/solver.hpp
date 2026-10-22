#pragma once
#include "game_logic.hpp"
namespace poptest {
struct Node {
  pop::State s;
  std::vector<int> path;
  double rank;
};
inline std::vector<int> solve(int level, uint32_t seed = 0) {
  using namespace pop;
  auto &l = levels()[level];
  State start = initial(level);
  if (seed) {
    start.random = seed;
    for (int &c : start.board)
      c = int(next(start.random) % l.colors);
    start.board[1] = start.board[0];
  }
  std::vector<Node> beam{{start, {}, 0}};
  for (int depth = 0; depth < l.moves; ++depth) {
    std::vector<Node> children;
    for (auto &node : beam) {
      std::array<bool, N> seen{};
      for (int kind = 0; kind < 2; ++kind)
        for (int cell = 0; cell < N; ++cell) {
          if (!kind) {
            if (seen[cell])
              continue;
            auto g = group(node.s, cell);
            for (int p : g)
              seen[p] = true;
            if (g.size() < 2)
              continue;
          } else if (node.s.charge < 12)
            break;
          auto s = node.s;
          if (!act(s, level, cell, kind))
            continue;
          auto path = node.path;
          path.push_back(cell + kind * N);
          if (won(s, level))
            return path;
          int a = level % l.colors, b = (level + 1) % l.colors;
          double deficit = std::max(0, l.quota - s.collected[a]) +
                           std::max(0, l.quota - s.collected[b]);
          double rank = -deficit * 100 + s.charge * 1.5 + s.score * .002;
          children.push_back({s, path, rank});
        }
    }
    std::stable_sort(
        children.begin(), children.end(),
        [](const Node &a, const Node &b) { return a.rank > b.rank; });
    if (children.empty())
      return {};
    if (children.size() > 48)
      children.resize(48);
    beam.swap(children);
  }
  return {};
}
} // namespace poptest
