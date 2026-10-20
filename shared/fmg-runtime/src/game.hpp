#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <vector>
namespace fmg {
enum Key { Left, Right, Up, Down, A, B, X, Y, Start, Back };
static const char *const names[] = {"JUICES SORT", "NEW BLOCKS", "COLOR MIX", "SUDOKU",
                                    "TRAFFIC JAM", "MAHJONG",    "BOCA",      "COLOR LOOP"};
struct State {
  int game = 0, cursor = 0, selected = -1, score = 0, phase = 0, choice = 1;
  std::array<int, 1024> a{};
  std::array<float, 1024> f{};
};
} // namespace fmg
#include "boca_city.hpp"
#include "loop_game.hpp"
namespace fmg {
struct Engine {
  State s;
  std::vector<State> undo;
  std::mt19937 rng;
  explicit Engine(unsigned seed = 12345) : rng(seed) {
    reset(0);
  }
  int roll(int n) {
    return int(rng() % unsigned(n));
  }
  void checkpoint() {
    undo.push_back(s);
    if (undo.size() > 100)
      undo.erase(undo.begin());
  }
  void revert() {
    if (!undo.empty()) {
      int best = s.game == 1 ? s.a[110] : 0;
      s = undo.back();
      if (s.game == 1)
        s.a[110] = std::max(best, s.a[110]);
      undo.pop_back();
    }
  }
  void reset(int game) {
    int loopLevel = s.game == 7 ? s.a[200] : 0, loopUnlocked = s.game == 7 ? s.a[201] : 0;
    int blocksBest = s.game == 1 ? std::max(s.score, s.a[110]) : 0;
    s = State{};
    s.game = game;
    if (game == 1)
      s.a[110] = blocksBest;
    undo.clear();
    if (game == 0) { // Reachable by reversing legal pours from a sorted board.
      for (int i = 0; i < 4; i++) {
        s.a[40 + i] = 4;
        for (int j = 0; j < 4; j++)
          s.a[i * 4 + j] = i + 1;
      }
      for (int k = 0; k < 120; k++) {
        int from = roll(6), to = roll(6);
        int &n = s.a[40 + from], &m = s.a[40 + to];
        if (from != to && n > 0 && m < 4 &&
            (n == 1 || s.a[from * 4 + n - 2] == s.a[from * 4 + n - 1])) {
          s.a[to * 4 + m++] = s.a[from * 4 + --n];
          s.a[from * 4 + n] = 0;
        }
      }
      if (juiceWon())
        reset(0);
    }
    if (game == 1) {
      deal();
    }
    if (game == 2) {
      for (int k = 0; k < 18; k++)
        mix(roll(3), roll(2));
      if (mixWon())
        mix(0, 0);
    }
    if (game == 3) {
      const std::string puzzle =
          "530070000600195000098000060800060003400803001700020006060000280000419005000080079";
      const std::string solution =
          "534678912672195348198342567859761423426853791713924856961537284287419635345286179";
      std::array<int, 9> digits{{1, 2, 3, 4, 5, 6, 7, 8, 9}};
      std::shuffle(digits.begin(), digits.end(), rng);
      bool transpose = roll(2);
      int shiftR = roll(3) * 3, shiftC = roll(3) * 3;
      for (int r = 0; r < 9; r++)
        for (int c = 0; c < 9; c++) {
          int rr = (r + shiftR) % 9, cc = (c + shiftC) % 9;
          int i = transpose ? cc * 9 + rr : rr * 9 + cc;
          int p = puzzle[size_t(i)] - '0';
          s.a[r * 9 + c] = p ? digits[size_t(p - 1)] : 0;
          s.a[81 + r * 9 + c] = s.a[r * 9 + c];
          s.a[162 + r * 9 + c] = digits[size_t(solution[size_t(i)] - '1')];
        }
    }
    if (game == 4) { // x,y,length,vertical; target car is 0, exit at right of row 2.
      const int cars[] = {0, 2, 2, 0, 2, 1, 2, 1, 0, 0, 2, 0, 4, 0, 3, 1,
                          0, 3, 3, 1, 1, 4, 2, 0, 3, 3, 2, 0, 5, 3, 3, 1};
      std::copy(cars, cars + 32, s.a.begin());
      s.a[100] = 8;
    }
    if (game == 5) {
      std::array<int, 32> alive{};
      alive.fill(1);
      for (int pair = 1; pair <= 16; pair++) {
        std::vector<int> free;
        for (int i = 0; i < 32; i++)
          if (alive[i] && (i % 8 == 0 || !alive[i - 1] || i % 8 == 7 || !alive[i + 1]))
            free.push_back(i);
        int pick = roll(int(free.size()));
        int i = free[size_t(pick)];
        free.erase(free.begin() + pick);
        int j = free[size_t(roll(int(free.size())))];
        alive[i] = alive[j] = 0;
        s.a[i] = s.a[j] = pair;
      }
    }
    if (game == 6)
      initCity(s, rng);
    if (game == 7)
      initLoop(s, loopLevel, loopUnlocked);
  }

  bool juiceWon() const {
    for (int i = 0; i < 6; i++) {
      int n = s.a[40 + i];
      if (n != 0 && n != 4)
        return false;
      for (int j = 1; j < n; j++)
        if (s.a[i * 4 + j] != s.a[i * 4])
          return false;
    }
    return true;
  }
  bool pour(int from, int to) {
    if (from == to)
      return false;
    int &n = s.a[40 + from], &m = s.a[40 + to];
    if (n == 0 || m == 4)
      return false;
    int color = s.a[from * 4 + n - 1];
    if (m && s.a[to * 4 + m - 1] != color)
      return false;
    do {
      s.a[to * 4 + m++] = color;
      s.a[from * 4 + --n] = 0;
    } while (n && m < 4 && s.a[from * 4 + n - 1] == color);
    s.score++;
    if (juiceWon())
      s.phase = 1;
    return true;
  }
  static std::vector<std::pair<int, int>> shape(int id) {
    switch (id) {
    case 0:
      return {{0, 0}};
    case 1:
      return {{0, 0}, {1, 0}};
    case 2:
      return {{0, 0}, {0, 1}, {1, 1}};
    case 3:
      return {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    case 4:
      return {{0, 0}, {1, 0}, {2, 0}};
    case 5:
      return {{0, 0}, {0, 1}, {0, 2}};
    case 6:
      return {{0, 0}, {1, 0}, {2, 0}, {3, 0}};
    case 7:
      return {{0, 0}, {0, 1}, {0, 2}, {0, 3}};
    case 8:
      return {{0, 0}, {1, 0}, {2, 0}, {1, 1}};
    case 9:
      return {{0, 0}, {0, 1}, {0, 2}, {1, 2}};
    case 10:
      return {{1, 0}, {1, 1}, {0, 2}, {1, 2}};
    case 11:
      return {{0, 0}, {1, 0}, {1, 1}, {2, 1}};
    case 12:
      return {{1, 0}, {2, 0}, {0, 1}, {1, 1}};
    case 13:
      return {{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}, {2, 1}};
    case 14:
      return {{0, 0}, {1, 0}, {0, 1}, {1, 1}, {0, 2}, {1, 2}};
    case 15:
      return {{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}, {2, 1}, {0, 2}, {1, 2}, {2, 2}};
    default:
      return {};
    }
  }
  void deal() {
    for (int i = 0; i < 3; i++)
      s.a[100 + i] = roll(16);
  }
  bool fits(int id, int x, int y) const {
    if (id < 0 || id >= 16)
      return false;
    for (auto p : shape(id)) {
      int xx = x + p.first, yy = y + p.second;
      if (xx >= 8 || yy >= 8 || xx < 0 || yy < 0 || s.a[yy * 8 + xx])
        return false;
    }
    return true;
  }
  bool hasMove() const {
    for (int k = 0; k < 3; k++)
      for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
          if (fits(s.a[100 + k], x, y))
            return true;
    return false;
  }
  bool place() {
    int previousScore = s.score;
    int id = s.a[100 + s.choice];
    int x = s.cursor % 8, y = s.cursor / 8;
    if (!fits(id, x, y))
      return false;
    for (auto p : shape(id)) {
      s.a[(y + p.second) * 8 + x + p.first] = id % 6 + 1;
      s.score++;
    }
    s.a[100 + s.choice] = -1;
    std::array<int, 8> rows{}, cols{};
    for (int r = 0; r < 8; r++) {
      rows[r] = cols[r] = 1;
      for (int c = 0; c < 8; c++) {
        if (!s.a[r * 8 + c])
          rows[r] = 0;
        if (!s.a[c * 8 + r])
          cols[r] = 0;
      }
    }
    s.a[113] = s.a[114] = 0;
    int lines = 0;
    for (int k = 0; k < 8; ++k) {
      if (rows[k]) {
        s.a[113] |= 1 << k;
        ++lines;
      }
      if (cols[k]) {
        s.a[114] |= 1 << k;
        ++lines;
      }
    }
    s.a[111] = lines ? s.a[111] + 1 : 0;
    s.a[112] += lines;
    s.score += lines * lines * 10 + (lines ? (s.a[111] - 1) * 10 : 0);
    s.f[0] = lines ? .45f : .12f;
    for (int r = 0; r < 8; r++)
      for (int c = 0; c < 8; c++)
        if (rows[r] || cols[c]) {
          s.a[r * 8 + c] = 0;
          s.score += 2;
        }
    if (s.a[100] < 0 && s.a[101] < 0 && s.a[102] < 0)
      deal();
    if (s.a[100 + s.choice] < 0)
      for (int k = 1; k <= 3; ++k)
        if (s.a[100 + (s.choice + k) % 3] >= 0) {
          s.choice = (s.choice + k) % 3;
          break;
        }
    s.a[115] = s.score - previousScore;
    s.f[2] = .75f;
    s.a[110] = std::max(s.a[110], s.score);
    if (!hasMove())
      s.phase = 2;
    return true;
  }
  void mix(int i, int column) {
    for (int k = 0; k < 3; k++) {
      int p = column ? k * 3 + i : i * 3 + k;
      s.a[p] = (s.a[p] + 1) % 3;
    }
  }
  bool mixWon() const {
    return std::all_of(s.a.begin(), s.a.begin() + 9, [](int n) { return n == 0; });
  }
  bool sudokuValid(int pos, int v) const {
    if (v < 1 || v > 9)
      return false;
    int r = pos / 9, c = pos % 9;
    for (int i = 0; i < 81; i++)
      if (i != pos && s.a[i] == v &&
          (i / 9 == r || i % 9 == c || (i / 27 == r / 3 && (i % 9) / 3 == c / 3)))
        return false;
    return true;
  }
  bool sudokuWon() const {
    for (int i = 0; i < 81; i++)
      if (!sudokuValid(i, s.a[i]))
        return false;
    return true;
  }
  int carAt(int x, int y) const {
    for (int i = 0; i < s.a[100]; i++) {
      int p = i * 4;
      for (int j = 0; j < s.a[p + 2]; j++)
        if (s.a[p] + (!s.a[p + 3] ? j : 0) == x && s.a[p + 1] + (s.a[p + 3] ? j : 0) == y)
          return i;
    }
    return -1;
  }
  bool moveCar(int id, int dx, int dy) {
    int p = id * 4;
    if ((s.a[p + 3] && dx) || (!s.a[p + 3] && dy))
      return false;
    int x = s.a[p] + dx, y = s.a[p + 1] + dy;
    for (int j = 0; j < s.a[p + 2]; j++) {
      int xx = x + (!s.a[p + 3] ? j : 0), yy = y + (s.a[p + 3] ? j : 0);
      if (xx < 0 || yy < 0 || xx >= 6 || yy >= 6)
        return false;
      int other = carAt(xx, yy);
      if (other >= 0 && other != id)
        return false;
    }
    s.a[p] = x;
    s.a[p + 1] = y;
    s.score++;
    if (s.a[0] == 4)
      s.phase = 1;
    return true;
  }
  bool tileFree(int i) const {
    return s.a[i] && (i % 8 == 0 || !s.a[i - 1] || i % 8 == 7 || !s.a[i + 1]);
  }
  std::pair<int, int> hint() const {
    for (int i = 0; i < 32; i++)
      for (int j = i + 1; j < 32; j++)
        if (tileFree(i) && tileFree(j) && s.a[i] == s.a[j])
          return {i, j};
    return {-1, -1};
  }
  void nav(int dx, int dy, int w, int h) {
    int x = s.cursor % w, y = s.cursor / w;
    s.cursor = ((y + dy + h) % h) * w + (x + dx + w) % w;
  }
  void input(Key k) {
    if (k == Y) {
      revert();
      return;
    }
    if (s.phase)
      return;
    if (s.game == 7) {
      if (k == B)
        s.a[220] = 0;
      if (k == Left)
        s.cursor = (s.cursor / 4) * 4 + (s.cursor + 3) % 4;
      if (k == Right)
        s.cursor = (s.cursor / 4) * 4 + (s.cursor + 1) % 4;
      if (k == Up || k == Down)
        s.cursor = (s.cursor + 4) % 8;
      if (k == A) {
        State before = s;
        if (launchLoop(s, s.cursor)) {
          undo.push_back(before);
          if (undo.size() > 100)
            undo.erase(undo.begin());
        }
      }
      return;
    }
    if (s.game == 6 && k == A && s.f[7] <= 0) {
      s.f[6] = .24f;
      s.f[7] = 1.2f;
    }
    int dx = (k == Right) - (k == Left), dy = (k == Down) - (k == Up);
    if (s.game == 0) {
      if (dx)
        s.cursor = (s.cursor + dx + 6) % 6;
      if (k == B)
        s.selected = -1;
      if (k == A) {
        if (s.selected < 0) {
          if (s.a[40 + s.cursor] > 0)
            s.selected = s.cursor;
        } else if (s.selected == s.cursor) {
          s.selected = -1;
        } else {
          int from = s.selected;
          checkpoint();
          if (!pour(from, s.cursor)) {
            undo.pop_back();
            return; // Keep the source selected when the destination is invalid.
          }
          s.selected = -1;
        }
      }
    }
    if (s.game == 1) {
      nav(dx, dy, 8, 8);
      if (k == B)
        for (int k = 1; k <= 3; ++k)
          if (s.a[100 + (s.choice + k) % 3] >= 0) {
            s.choice = (s.choice + k) % 3;
            break;
          }
      if (k == A) {
        checkpoint();
        if (!place()) {
          undo.pop_back();
          s.f[1] = .4f;
        }
      }
    }
    if (s.game == 2) {
      nav(dx, dy, 3, 3);
      if (k == A || k == B) {
        checkpoint();
        mix(k == A ? s.cursor / 3 : s.cursor % 3, k == B);
        s.score++;
        if (mixWon())
          s.phase = 1;
      }
    }
    if (s.game == 3) {
      nav(dx, dy, 9, 9);
      if (k == B)
        s.choice = s.choice % 9 + 1;
      if (k == X && !s.a[81 + s.cursor]) {
        checkpoint();
        s.a[s.cursor] = 0;
      }
      if (k == A && !s.a[81 + s.cursor] && sudokuValid(s.cursor, s.choice)) {
        checkpoint();
        s.a[s.cursor] = s.choice;
        s.score++;
        if (sudokuWon())
          s.phase = 1;
      }
    }
    if (s.game == 4) {
      if (s.selected < 0) {
        nav(dx, dy, 6, 6);
        if (k == A)
          s.selected = carAt(s.cursor % 6, s.cursor / 6);
      } else {
        if (k == B || k == A)
          s.selected = -1;
        else if (dx || dy) {
          checkpoint();
          if (!moveCar(s.selected, dx, dy))
            undo.pop_back();
        }
      }
    }
    if (s.game == 5) {
      nav(dx, dy, 8, 4);
      if (k == B)
        s.selected = -1;
      if (k == X) {
        auto p = hint();
        if (p.first >= 0) {
          s.cursor = p.first;
          s.selected = p.second;
          s.a[104] = p.first;
          s.a[105] = p.second;
          s.f[2] = 1.5f;
          s.a[103] = 3;
        }
      }
      if (k == A && tileFree(s.cursor)) {
        if (s.selected < 0 || s.selected == s.cursor)
          s.selected = s.cursor;
        else if (tileFree(s.selected) && s.a[s.selected] == s.a[s.cursor]) {
          checkpoint();
          s.a[100] = s.selected;
          s.a[101] = s.cursor;
          s.a[102] = s.a[s.cursor];
          s.f[0] = .42f;
          s.a[s.selected] = s.a[s.cursor] = 0;
          s.selected = -1;
          s.score += 2;
          if (s.score == 32)
            s.phase = 1;
        } else {
          s.selected = s.cursor;
          s.f[1] = .7f;
          s.a[103] = 2;
        }
      } else if (k == A) {
        s.f[1] = .7f;
        s.a[103] = 1;
      }
    }
  }
  void tick(float dt, int dx, int dy) {
    if (s.game == 5) {
      dt = std::max(0.f, std::min(.05f, dt));
      for (int i = 0; i < 3; ++i)
        s.f[i] = std::max(0.f, s.f[i] - dt);
      s.f[3] = std::fmod(s.f[3] + dt, 1000.f);
      return;
    }
    if (s.game == 7) {
      tickLoop(s, dt);
      return;
    }
    if (s.game == 1) {
      s.f[0] = std::max(0.f, s.f[0] - dt);
      s.f[1] = std::max(0.f, s.f[1] - dt);
      s.f[2] = std::max(0.f, s.f[2] - dt);
      return;
    }
    if (s.game != 6 || s.phase)
      return;
    if (isCity(s)) {
      float norm = std::max(1.f, std::hypot(float(dx), float(dy)));
      tickCity(s, dt, (dx * .70710678f + dy * 1.25f) / norm,
               (-dx * .70710678f + dy * 1.25f) / norm);
      return;
    }
    dt = std::max(0.f, std::min(dt, 0.05f));
    if (dt == 0)
      return;
    s.f[250] += dt;
    s.f[6] = std::max(0.f, s.f[6] - dt);
    s.f[7] = std::max(0.f, s.f[7] - dt);
    float norm = (dx && dy) ? .70710678f : 1;
    float speed = s.f[6] > 0 ? 245.f : 135.f + std::min(25.f, s.f[2]);
    float blend = 1 - std::exp(-14.f * dt);
    s.f[4] += (dx * speed * norm - s.f[4]) * blend;
    s.f[5] += (dy * speed * norm - s.f[5]) * blend;
    s.f[0] = std::max(10.f, std::min(470.f, s.f[0] + s.f[4] * dt));
    s.f[1] = std::max(50.f, std::min(224.f, s.f[1] + s.f[5] * dt));
    s.f[3] = std::max(0.f, s.f[3] - dt);
    for (int i = 0; i < 60; ++i) {
      int base = 8 + i * 4;
      s.f[base + 3] = std::max(0.f, s.f[base + 3] - dt);
      if (!s.a[i] || s.f[2] <= s.f[base + 2] + 2)
        continue;
      float x = s.f[0] - s.f[base], y = s.f[1] - s.f[base + 1];
      float distance = std::sqrt(x * x + y * y);
      float reach = s.f[2] + s.f[base + 2] + 10;
      if (distance < reach && distance > .01f) {
        float pull = std::min(distance, (25 + 90 * (1 - distance / reach)) * dt);
        s.f[base] += x / distance * pull;
        s.f[base + 1] += y / distance * pull;
        distance -= pull;
      }
      if (distance < s.f[2] - s.f[base + 2] * .45f) {
        s.a[i] = 0;
        s.f[base + 3] = .28f;
        ++s.score;
        s.f[2] += .6f;
      }
    }
    if (s.score == 60)
      s.phase = 1;
    else if (s.f[3] <= 0)
      s.phase = 2;
  }
};
inline bool validState(const State &s) {
  if (s.game < 0 || s.game > 7 || s.phase < 0 || s.phase > 2 || s.score < 0 || s.score > 10000000 ||
      s.selected < -1)
    return false;
  const int limits[] = {6, 64, 9, 81, 36, 32, 1, 8};
  if (s.cursor < 0 || s.cursor >= limits[s.game] || s.choice < 0 || s.choice > 9)
    return false;
  for (float x : s.f)
    if (!std::isfinite(x) || std::abs(x) > 1000000)
      return false;
  if (s.game == 7 && !validLoop(s))
    return false;
  if (s.game == 0) {
    if (s.selected >= 6)
      return false;
    std::array<int, 5> counts{};
    for (int i = 0; i < 6; i++) {
      int n = s.a[40 + i];
      if (n < 0 || n > 4)
        return false;
      for (int j = 0; j < 4; j++) {
        int v = s.a[i * 4 + j];
        if (v < 0 || v > 4 || (j < n && v == 0) || (j >= n && v != 0))
          return false;
        if (v)
          counts[v]++;
      }
    }
    for (int i = 1; i <= 4; i++)
      if (counts[i] != 4)
        return false;
  }
  if (s.game == 1) {
    if (s.choice > 2)
      return false;
    for (int i = 0; i < 64; i++)
      if (s.a[i] < 0 || s.a[i] > 6)
        return false;
    for (int i = 100; i < 103; i++)
      if (s.a[i] < -1 || s.a[i] > 15)
        return false;
  }
  if (s.game == 2)
    for (int i = 0; i < 9; i++)
      if (s.a[i] < 0 || s.a[i] > 2)
        return false;
  if (s.game == 3)
    for (int i = 0; i < 81; i++)
      if (s.a[i] < 0 || s.a[i] > 9 || s.a[81 + i] < 0 || s.a[81 + i] > 9 || s.a[162 + i] < 1 ||
          s.a[162 + i] > 9 || (s.a[81 + i] && s.a[81 + i] != s.a[i]))
        return false;
  if (s.game == 4) {
    if (s.a[100] != 8 || s.selected >= 8)
      return false;
    std::array<int, 36> occupied{};
    for (int i = 0; i < 8; i++) {
      int p = i * 4, x = s.a[p], y = s.a[p + 1], len = s.a[p + 2], v = s.a[p + 3];
      if (len < 2 || len > 3 || v < 0 || v > 1)
        return false;
      for (int j = 0; j < len; j++) {
        int xx = x + (!v ? j : 0), yy = y + (v ? j : 0);
        if (xx < 0 || xx >= 6 || yy < 0 || yy >= 6 || occupied[yy * 6 + xx]++)
          return false;
      }
    }
  }
  if (s.game == 5) {
    if (s.selected >= 32)
      return false;
    for (int i = 0; i < 32; i++)
      if (s.a[i] < 0 || s.a[i] > 16)
        return false;
  }
  if (s.game == 6 && isCity(s)) {
    if (s.f[901] < 0 || s.f[901] > 5 || s.f[902] < 0 || s.f[902] >= 1.01f)
      return false;
    for (int i = 0; i < cityCount; ++i)
      if (s.a[500 + i] < 0 || s.a[500 + i] > 90000 || s.a[cityOwnerSlot(i)] < 0 ||
          s.a[cityOwnerSlot(i)] > 4)
        return false;
    for (int i = 0; i < 3; ++i) {
      int b = 800 + i * 8;
      if (s.f[b + 4] < 0 || s.f[b + 4] > 12 || s.f[b + 5] < 0 || s.f[b + 5] > 5)
        return false;
    }

    if (s.f[0] < 0 || s.f[0] > cityWidth || s.f[1] < 0 || s.f[1] > cityHeight || s.f[2] < 1 ||
        s.f[2] > 2000 || s.f[3] < 0 || s.f[3] > 600 || s.a[254] < 0 || s.a[254] > 12)
      return false;
    for (int i = 0; i < cityCount; ++i) {
      int b = 8 + i * 4;
      if (s.a[i] < 0 || s.a[i] > 1 || s.a[300 + i] < 0 || s.a[300 + i] > 5 || s.f[b] < 0 ||
          s.f[b] > cityWidth || s.f[b + 1] < 0 || s.f[b + 1] > cityHeight || s.f[b + 2] < 1 ||
          s.f[b + 2] > 110 || s.f[b + 3] < 0 || s.f[b + 3] > .29f)
        return false;
    }
    for (int i = 0; i < 3; ++i) {
      int b = 800 + i * 8;
      if (s.a[800 + i] < 0 || s.a[800 + i] > 1 || s.f[b] < 0 || s.f[b] > cityWidth ||
          s.f[b + 1] < 0 || s.f[b + 1] > cityHeight || s.f[b + 2] < 1 || s.f[b + 2] > 2000)
        return false;
    }
  } else if (s.game == 6) {
    if (s.f[0] < 0 || s.f[0] > 480 || s.f[1] < 0 || s.f[1] > 272 || s.f[2] < 1 || s.f[2] > 100 ||
        s.f[3] < 0 || s.f[3] > 120)
      return false;
    for (int i = 0; i < 60; i++)
      if (s.a[i] < 0 || s.a[i] > 1 || s.f[10 + i * 4] < 1 || s.f[10 + i * 4] > 20)
        return false;
  }
  return true;
}
inline bool save(const std::string &path, const State &s) {
  if (!validState(s))
    return false;
  std::string tmp = path + ".tmp";
  std::ofstream o(tmp);
  int version = isCity(s) ? 2 : 1;
  o << "FMG " << version << "\n"
    << s.game << ' ' << s.cursor << ' ' << s.selected << ' ' << s.score << ' ' << s.phase << ' '
    << s.choice << '\n';
  for (int i = 0; i < (version == 2 ? 1024 : 256); ++i)
    o << s.a[i] << ' ';
  o << '\n';
  o.precision(9);
  for (int i = 0; i < (version == 2 ? 1024 : 256); ++i)
    o << s.f[i] << ' ';
  o << '\n';
  o.flush();
  bool ok = bool(o);
  o.close();
  return ok && std::rename(tmp.c_str(), path.c_str()) == 0;
}
inline bool load(const std::string &path, State &out) {
  std::ifstream in(path);
  std::string magic;
  int ver = 0;
  State s;
  if (!(in >> magic >> ver) || magic != "FMG" || (ver != 1 && ver != 2))
    return false;
  if (!(in >> s.game >> s.cursor >> s.selected >> s.score >> s.phase >> s.choice))
    return false;
  for (int i = 0; i < (ver == 2 ? 1024 : 256); ++i)
    if (!(in >> s.a[i]))
      return false;
  for (int i = 0; i < (ver == 2 ? 1024 : 256); ++i)
    if (!(in >> s.f[i]))
      return false;
  std::string extra;
  if (in >> extra)
    return false;
  if (!validState(s))
    return false;
  out = s;
  return true;
}
} // namespace fmg
