#pragma once
#include "game.hpp"
#include "juice_levels.hpp"
#include <queue>
#include <set>
namespace fmg {
inline int juiceLevel(const State &s) {
  return s.game == 0 && s.a[60] == 1701 && s.a[61] >= 0 && s.a[61] < juiceLevelCount ? s.a[61] : -1;
}
inline void startJuiceLevel(Engine &e, int level) {
  level = std::max(0, std::min(juiceLevelCount - 1, level));
  e.s = State{};
  e.undo.clear();
  e.s.a[60] = 1701;
  e.s.a[61] = level;
  for (int i = 0; i < 24; ++i) {
    e.s.a[i] = juiceBoards[level][i];
    if (e.s.a[i])
      ++e.s.a[40 + i / 4];
  }
}
struct JuiceProgress {
  std::array<int, juiceLevelCount> best{};
  int unlocked() const {
    int n = 0;
    while (n < juiceLevelCount - 1 && best[n])
      ++n;
    return n;
  }
  int stars(int level) const {
    if (!best[level])
      return 0;
    return best[level] <= juicePar[level] ? 3 : best[level] <= juicePar[level] + 3 ? 2 : 1;
  }
  void record(const State &s) {
    int n = juiceLevel(s);
    if (n >= 0 && s.phase == 1 && s.score > 0 && (!best[n] || s.score < best[n]))
      best[n] = s.score;
  }
  bool read(const std::string &path) {
    State s;
    if (!load(path, s) || s.game != 0 || s.a[90] != 1702)
      return false;
    for (int i = 0; i < juiceLevelCount; ++i)
      if (s.a[100 + i] < 0 || s.a[100 + i] > 10000000)
        return false;
    for (int i = 0; i < juiceLevelCount; ++i)
      best[i] = s.a[100 + i];
    return true;
  }
  bool write(const std::string &path, const State &board) const {
    State s = board;
    s.a[90] = 1702;
    for (int i = 0; i < juiceLevelCount; ++i)
      s.a[100 + i] = best[i];
    return save(path, s);
  }
};
// A bounded breadth-first search. Canonical tube ordering removes equivalent boards.
// The returned indices always refer to the original board, never a sorted board.
inline std::string juiceSearchKey(const State &s) {
  std::array<std::string, 6> tubes;
  for (int i = 0; i < 6; ++i)
    for (int j = 0; j < 4; ++j)
      tubes[i] += char('0' + s.a[i * 4 + j]);
  std::sort(tubes.begin(), tubes.end());
  std::string key;
  for (const auto &t : tubes)
    key += t;
  return key;
}
inline std::pair<int, int> juiceHint(const State &initial, int limit = 12000) {
  struct Node {
    std::array<unsigned char, 24> board;
    std::pair<int, int> first;
  };
  auto pack = [](const State &s, std::pair<int, int> first) {
    Node n;
    n.first = first;
    for (int i = 0; i < 24; ++i)
      n.board[i] = static_cast<unsigned char>(s.a[i]);
    return n;
  };
  std::queue<Node> pending;
  std::set<std::string> seen;
  pending.push(pack(initial, {-1, -1}));
  seen.insert(juiceSearchKey(initial));
  int expanded = 0;
  Engine e(1);
  while (!pending.empty() && expanded++ < limit) {
    Node n = pending.front();
    pending.pop();
    e.s = State{};
    for (int i = 0; i < 24; ++i) {
      e.s.a[i] = n.board[i];
      if (n.board[i])
        ++e.s.a[40 + i / 4];
    }
    State state = e.s;
    if (e.juiceWon())
      return n.first;
    for (int i = 0; i < 6; ++i) {
      int count = state.a[40 + i];
      if (!count)
        continue;
      bool uniform = true, triedEmpty = false;
      for (int k = 1; k < count; ++k)
        if (state.a[i * 4 + k] != state.a[i * 4])
          uniform = false;
      for (int j = 0; j < 6; ++j) {
        if (!state.a[40 + j]) {
          if (uniform || triedEmpty)
            continue;
          triedEmpty = true;
        }
        e.s = state;
        if (!e.pour(i, j))
          continue;
        auto first = n.first.first < 0 ? std::make_pair(i, j) : n.first;
        if (e.juiceWon())
          return first;
        if (seen.size() < size_t(limit) && seen.insert(juiceSearchKey(e.s)).second)
          pending.push(pack(e.s, first));
      }
    }
  }
  return {-1, -1};
}
inline bool juiceHasMove(const State &s) {
  Engine e(1);
  for (int i = 0; i < 6; ++i)
    for (int j = 0; j < 6; ++j) {
      e.s = s;
      if (e.pour(i, j))
        return true;
    }
  return false;
}
} // namespace fmg
