#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
namespace pop {
constexpr int W = 7, H = 6, N = W * H, LevelCount = 20;
struct Level {
  uint32_t seed;
  int colors, quota, moves;
};
inline const std::array<Level, LevelCount> &levels() {
  static const std::array<Level, LevelCount> l = []() {
    std::array<Level, LevelCount> a{};
    const uint32_t seeds[] = {7139, 7312, 7485,  7658,  7831,  8004, 8177,
                              8350, 8523, 8696,  8869,  9042,  9215, 9388,
                              9561, 9734, 10338, 10080, 10253, 10857};
    for (int i = 0; i < LevelCount; ++i)
      a[i] = {seeds[i], 3 + (i >= 5) + (i >= 12), 6 + i, 12 + i / 2};
    return a;
  }();
  return l;
}
struct State {
  std::array<int, N> board{};
  std::array<int, 5> collected{{0, 0, 0, 0, 0}};
  uint32_t random = 1;
  int moves = 0, score = 0, charge = 0, combo = 1, shuffles = 0;
};
inline uint32_t next(uint32_t &r) {
  r ^= r << 13;
  r ^= r >> 17;
  r ^= r << 5;
  return r;
}
inline std::vector<int> group(const State &s, int cell) {
  if (cell < 0 || cell >= N)
    return {};
  std::vector<int> list{cell};
  std::array<bool, N> seen{};
  seen[cell] = true;
  for (size_t i = 0; i < list.size(); ++i) {
    int p = list[i];
    for (int n : {p % W ? p - 1 : -1, p % W < W - 1 ? p + 1 : -1, p - W, p + W})
      if (n >= 0 && n < N && !seen[n] && s.board[n] == s.board[cell]) {
        seen[n] = true;
        list.push_back(n);
      }
  }
  return list;
}
inline bool hasMove(const State &s) {
  for (int p = 0; p < N; ++p)
    if ((p % W < W - 1 && s.board[p] == s.board[p + 1]) ||
        (p + W < N && s.board[p] == s.board[p + W]))
      return true;
  return false;
}
inline bool won(const State &s, int level) {
  auto &l = levels()[level];
  return s.collected[level % l.colors] >= l.quota &&
         s.collected[(level + 1) % l.colors] >= l.quota;
}
inline State initial(int level) {
  State s;
  auto &l = levels().at(level);
  s.random = l.seed;
  s.moves = l.moves;
  for (int &c : s.board)
    c = int(next(s.random) % l.colors);
  s.board[1] = s.board[0];
  return s;
}
struct Effect {
  uint64_t removed = 0;
  std::array<int, N> from{};
  int count = 0, points = 0;
  bool shuffled = false;
};
inline bool act(State &s, int level, int cell, bool pulse,
                Effect *effect = nullptr) {
  if (cell < 0 || cell >= N || s.moves <= 0 || won(s, level) ||
      (pulse && s.charge < 12))
    return false;
  auto cells = group(s, cell);
  if (!pulse && cells.size() < 2)
    return false;
  if (pulse) {
    cells.clear();
    for (int i = 0; i < N; ++i)
      if (i % W == cell % W || i / W == cell / W)
        cells.push_back(i);
  }
  Effect e;
  for (int p : cells) {
    e.removed |= uint64_t(1) << p;
    ++s.collected[s.board[p]];
  }
  e.count = int(cells.size());
  if (pulse) {
    s.charge -= 12;
    s.combo = 1;
  } else {
    if (cells.size() >= 4) {
      s.combo = std::min(4, s.combo + 1);
      s.charge = std::min(12, s.charge + int(cells.size()));
    } else
      s.combo = 1;
  }
  e.points = e.count * e.count * s.combo;
  s.score += e.points;
  --s.moves;
  auto old = s.board;
  for (int x = 0; x < W; ++x) {
    int dest = H - 1;
    for (int y = H - 1; y >= 0; --y) {
      int p = y * W + x;
      if (!(e.removed & (uint64_t(1) << p))) {
        s.board[dest * W + x] = old[p];
        e.from[dest * W + x] = p;
        --dest;
      }
    }
    while (dest >= 0) {
      int p = dest * W + x;
      s.board[p] = int(next(s.random) % levels()[level].colors);
      e.from[p] = -1;
      --dest;
    }
  }
  if (!hasMove(s) && !won(s, level) && s.moves > 0) {
    ++s.shuffles;
    e.shuffled = true;
    for (int attempt = 0; attempt < 8 && !hasMove(s); ++attempt)
      for (int i = N - 1; i > 0; --i)
        std::swap(s.board[i], s.board[next(s.random) % (i + 1)]);
    if (!hasMove(s))
      s.board[1] = s.board[0];
  }
  if (effect)
    *effect = e;
  return true;
}
struct Game {
  int level = 0, selected = 0, unlocked = 0;
  bool muted = false, pulse = false;
  State state = initial(0);
  std::vector<State> history;
  void begin(int n) {
    level = std::max(0, std::min(LevelCount - 1, n));
    selected = 0;
    pulse = false;
    state = initial(level);
    history.clear();
  }
  bool action(Effect *e = nullptr) {
    auto before = state;
    if (!act(state, level, selected, pulse, e))
      return false;
    history.push_back(before);
    if (history.size() > 64)
      history.erase(history.begin());
    pulse = false;
    if (won(state, level))
      unlocked = std::max(unlocked, std::min(LevelCount - 1, level + 1));
    return true;
  }
  void undo() {
    if (history.empty())
      return;
    state = history.back();
    history.pop_back();
    pulse = false;
  }
  void navigate(int dx, int dy) {
    selected = std::max(0, std::min(H - 1, selected / W + dy)) * W +
               std::max(0, std::min(W - 1, selected % W + dx));
  }
  bool save(const std::string &path) const {
    std::string tmp = path + ".tmp";
    std::ofstream f(tmp);
    f << "POP1 " << level << ' ' << selected << ' ' << unlocked << ' ' << muted
      << ' ' << state.random << ' ' << state.moves << ' ' << state.score << ' '
      << state.charge << ' ' << state.combo << ' ' << state.shuffles;
    for (int c : state.collected)
      f << ' ' << c;
    for (int c : state.board)
      f << ' ' << c;
    f << '\n';
    f.close();
    return bool(f) && std::rename(tmp.c_str(), path.c_str()) == 0;
  }
  bool load(const std::string &path) {
    std::ifstream f(path);
    std::string magic;
    Game g;
    auto &s = g.state;
    if (!(f >> magic >> g.level >> g.selected >> g.unlocked >> g.muted >>
          s.random >> s.moves >> s.score >> s.charge >> s.combo >>
          s.shuffles) ||
        magic != "POP1" || g.level < 0 || g.level >= LevelCount ||
        g.selected < 0 || g.selected >= N || g.unlocked < g.level ||
        g.unlocked >= LevelCount || !s.random || s.moves < 0 ||
        s.moves > levels()[g.level].moves || s.score < 0 ||
        s.score > N * N * 4 * levels()[g.level].moves || s.charge < 0 ||
        s.charge > 12 || s.combo < 1 || s.combo > 4 || s.shuffles < 0)
      return false;
    for (int &c : s.collected)
      if (!(f >> c) || c < 0 || c > N * levels()[g.level].moves)
        return false;
    for (int &c : s.board)
      if (!(f >> c) || c < 0 || c >= levels()[g.level].colors)
        return false;
    *this = g;
    return true;
  }
};
} // namespace pop
