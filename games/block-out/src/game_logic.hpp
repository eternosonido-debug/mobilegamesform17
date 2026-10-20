#pragma once
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
namespace blockout {
constexpr int side = 6, levelCount = 9;
constexpr float exitDuration = .65f;
enum class Scene { Menu, Tutorial, Playing, Paused, Won, GameOver };
struct Block {
  int x, y, w, h, color;
  bool alive;
  Block(int px = 0, int py = 0, int width = 1, int height = 1, int c = 1,
        bool live = false)
      : x(px), y(py), w(width), h(height), color(c), alive(live) {}
};
struct Door {
  int edge, pos, span, color;
}; // left, right, top, bottom
struct SolutionMove {
  int block, dx, dy;
};
struct Level {
  int size = 6;
  std::array<int, 64> walls{};
  std::vector<SolutionMove> solution;
  std::vector<Block> blocks;
  std::vector<Door> doors;
};
inline Level basicLevel(int id) {
  Level l;
  if (id == 0) {
    l.blocks = {{0, 1, 2, 1, 1, true}, {4, 3, 1, 2, 2, true}};
    l.doors = {{1, 1, 1, 1}, {3, 4, 1, 2}};
    l.walls[20] = l.walls[26] = 1;
  }
  if (id == 1) {
    l.blocks = {
        {1, 2, 2, 1, 1, true}, {3, 1, 1, 2, 2, true}, {0, 4, 2, 1, 3, true}};
    l.doors = {{1, 2, 1, 1}, {3, 3, 1, 2}, {0, 4, 1, 3}};
    l.walls[0] = l.walls[1] = l.walls[28] = l.walls[29] = 1;
  }
  if (id == 2) {
    l.blocks = {{0, 2, 2, 1, 1, true},
                {3, 2, 1, 2, 2, true},
                {2, 4, 2, 1, 3, true},
                {1, 0, 1, 2, 4, true}};
    l.doors = {{1, 2, 1, 1}, {3, 3, 1, 2}, {0, 4, 1, 3}, {2, 1, 1, 4}};
    l.walls[4] = l.walls[10] = l.walls[30] = l.walls[35] = 1;
  }
  return l;
}
inline Level generatedLevel(int id) {
  for (int attempt = 0; attempt < 80; ++attempt) {
    Level l;
    l.size = id >= 6 ? 8 : 6;
    int count = id < 6 ? 5 + (id == 5) : 7 + (id == 8);
    int wallCount = id < 6 ? 4 + (id - 3) : 12 + (id - 6) * 2;
    unsigned seed = 12371u + id * 919u + attempt * 1709u;
    auto roll = [&](int n) {
      seed = seed * 1664525u + 1013904223u;
      return int((seed >> 8) % unsigned(n));
    };
    for (int placed = 0; placed < wallCount;) {
      int x = 1 + roll(l.size - 2), y = 1 + roll(l.size - 2);
      if (!l.walls[y * l.size + x]) {
        l.walls[y * l.size + x] = 1;
        ++placed;
      }
    }
    bool complete = true;
    for (int index = 0; index < count; ++index) {
      Block shape(0, 0, index % 2 ? 1 : 2, index % 2 ? 2 : 1, index % 4 + 1,
                  true);
      if (id >= 6 && index % 4 == 0)
        shape.w = shape.h = 2;
      auto free = [&](int x, int y) {
        if (x < 0 || y < 0 || x + shape.w > l.size || y + shape.h > l.size)
          return false;
        for (int yy = y; yy < y + shape.h; ++yy)
          for (int xx = x; xx < x + shape.w; ++xx) {
            if (l.walls[yy * l.size + xx])
              return false;
            for (auto &b : l.blocks)
              if (xx >= b.x && xx < b.x + b.w && yy >= b.y && yy < b.y + b.h)
                return false;
          }
        return true;
      };
      int bestScore = -1;
      Block chosen;
      Door chosenDoor{};
      std::vector<SolutionMove> chosenPath;
      for (int edge = 0; edge < 4; ++edge) {
        int span = edge < 2 ? shape.h : shape.w;
        for (int pos = 0; pos + span <= l.size; ++pos) {
          bool overlap = false;
          for (auto &d : l.doors)
            if (d.edge == edge && pos < d.pos + d.span && pos + span > d.pos)
              overlap = true;
          if (overlap)
            continue;
          int startX = edge == 0   ? 0
                       : edge == 1 ? l.size - shape.w
                                   : pos,
              startY = edge == 2   ? 0
                       : edge == 3 ? l.size - shape.h
                                   : pos;
          if (!free(startX, startY))
            continue;
          std::array<int, 64> dist, parent, dir;
          dist.fill(-1);
          parent.fill(-1);
          dir.fill(-1);
          std::vector<int> queue;
          int start = startY * l.size + startX;
          dist[start] = 0;
          queue.push_back(start);
          const int dx[] = {-1, 1, 0, 0}, dy[] = {0, 0, -1, 1};
          for (size_t head = 0; head < queue.size(); ++head) {
            int current = queue[head];
            for (int step = 0; step < 4; ++step) {
              int x = current % l.size + dx[step],
                  y = current / l.size + dy[step];
              if (!free(x, y))
                continue;
              int next = y * l.size + x;
              if (dist[next] >= 0)
                continue;
              dist[next] = dist[current] + 1;
              parent[next] = current;
              dir[next] = step;
              queue.push_back(next);
            }
          }
          for (int cell : queue) {
            if (dist[cell] < 2)
              continue;
            int score = dist[cell] * 30 + roll(20);
            if (score <= bestScore)
              continue;
            bestScore = score;
            chosen = shape;
            chosen.x = cell % l.size;
            chosen.y = cell / l.size;
            chosenDoor = {edge, pos, span, shape.color};
            chosenPath.clear();
            for (int current = cell; current != start;
                 current = parent[current])
              chosenPath.push_back(
                  {index, -dx[dir[current]], -dy[dir[current]]});
            chosenPath.push_back({index,
                                  edge == 0   ? -1
                                  : edge == 1 ? 1
                                              : 0,
                                  edge == 2   ? -1
                                  : edge == 3 ? 1
                                              : 0});
          }
        }
      }
      if (bestScore < 0) {
        complete = false;
        break;
      }
      l.blocks.push_back(chosen);
      l.doors.push_back(chosenDoor);
      chosenPath.insert(chosenPath.end(), l.solution.begin(), l.solution.end());
      l.solution = chosenPath;
    }
    if (complete)
      return l;
  }
  throw std::runtime_error("Could not construct Block Out stage");
}
inline Level level(int id) {
  static const std::array<Level, levelCount> levels = []() {
    std::array<Level, levelCount> result;
    for (int i = 0; i < levelCount; ++i)
      result[i] = i < 3 ? basicLevel(i) : generatedLevel(i);
    return result;
  }();
  return levels.at(size_t(id));
}
struct Board {
  std::vector<Block> blocks;
  int moves = 0;
};
inline int at(const Board &b, int x, int y, int skip = -1) {
  for (int i = 0; i < int(b.blocks.size()); ++i) {
    const auto &p = b.blocks[i];
    if (i != skip && p.alive && x >= p.x && x < p.x + p.w && y >= p.y &&
        y < p.y + p.h)
      return i;
  }
  return -1;
}
inline bool won(const Board &b) {
  for (auto &p : b.blocks)
    if (p.alive)
      return false;
  return true;
}
inline bool slide(Board &b, const Level &l, int index, int dx, int dy) {
  if (index < 0 || index >= int(b.blocks.size()) || !b.blocks[index].alive ||
      std::abs(dx) + std::abs(dy) != 1)
    return false;
  auto p = b.blocks[index];
  int xx = p.x + dx, yy = p.y + dy;
  if (xx < 0 || yy < 0 || xx + p.w > l.size || yy + p.h > l.size) {
    int edge = xx < 0 ? 0 : xx + p.w > l.size ? 1 : yy < 0 ? 2 : 3;
    for (auto &d : l.doors) {
      int along = edge < 2 ? p.y : p.x, span = edge < 2 ? p.h : p.w;
      if (d.edge == edge && d.color == p.color && along >= d.pos &&
          along + span <= d.pos + d.span) {
        b.blocks[index].alive = false;
        ++b.moves;
        return true;
      }
    }
    return false;
  }
  for (int y = yy; y < yy + p.h; ++y)
    for (int x = xx; x < xx + p.w; ++x)
      if (l.walls[y * l.size + x] || at(b, x, y, index) >= 0)
        return false;
  b.blocks[index].x = xx;
  b.blocks[index].y = yy;
  ++b.moves;
  return true;
}
inline bool valid(const Board &b, int id) {
  auto l = level(id);
  if (b.blocks.size() != l.blocks.size() || b.moves < 0 || b.moves > 1000000)
    return false;
  for (int i = 0; i < int(b.blocks.size()); ++i) {
    auto &p = b.blocks[i];
    auto &original = l.blocks[i];
    if (p.w != original.w || p.h != original.h || p.color != original.color ||
        p.x < 0 || p.y < 0 || p.x + p.w > l.size || p.y + p.h > l.size)
      return false;
    if (p.alive)
      for (int y = p.y; y < p.y + p.h; ++y)
        for (int x = p.x; x < p.x + p.w; ++x)
          if (l.walls[y * l.size + x] || at(b, x, y, i) >= 0)
            return false;
  }
  return true;
}
struct Game {
  Board board, previous;
  int current = 0, unlocked = 0, cursor = 6, selected = -1;
  bool canUndo = false, muted = false, tutorial = true;
  Scene scene = Scene::Menu;
  int menu = 0;
  float clock = 0, invalid = 0, animation = 0;
  int animated = -1;
  int exitEdge = 0;
  Block oldBlock;
  bool exiting = false;
  Game() {
    reset(0);
    scene = Scene::Menu;
  }
  void reset(int id) {
    current = std::max(0, std::min(levelCount - 1, id));
    board.blocks = level(current).blocks;
    board.moves = 0;
    previous = Board{};
    canUndo = false;
    selected = -1;
    cursor = board.blocks[0].y * level(current).size + board.blocks[0].x;
    animation = 0;
    scene = tutorial && current == 0 ? Scene::Tutorial : Scene::Playing;
  }
  bool move(int dx, int dy) {
    if (scene != Scene::Playing && scene != Scene::Tutorial)
      return false;
    if (selected < 0) {
      int size = level(current).size;
      cursor = ((cursor / size + dy + size) % size) * size +
               (cursor % size + dx + size) % size;
      return true;
    }
    Board next = board;
    auto before = board.blocks[selected];
    if (!slide(next, level(current), selected, dx, dy)) {
      invalid = .25f;
      return false;
    }
    previous = board;
    canUndo = true;
    board = next;
    animated = selected;
    oldBlock = before;
    animation = .13f;
    exiting = !board.blocks[selected].alive;
    if (exiting) {
      animation = exitDuration;
      exitEdge = dx < 0 ? 0 : dx > 0 ? 1 : dy < 0 ? 2 : 3;
    }
    if (exiting)
      selected = -1;
    else
      cursor = board.blocks[animated].y * level(current).size +
               board.blocks[animated].x;
    if (won(board)) {
      scene = Scene::Won;
      unlocked = std::max(unlocked, std::min(levelCount - 1, current + 1));
    }
    return true;
  }
  void action() {
    if (selected >= 0) {
      selected = -1;
      return;
    }
    selected =
        at(board, cursor % level(current).size, cursor / level(current).size);
    if (selected < 0)
      invalid = .25f;
  }
  void undo() {
    if (canUndo) {
      board = previous;
      canUndo = false;
      selected = -1;
      animation = 0;
      scene = tutorial && current == 0 ? Scene::Tutorial : Scene::Playing;
    }
  }
  void tick(float dt) {
    if (scene == Scene::Paused || scene == Scene::Menu)
      return;
    clock += dt;
    animation = std::max(0.f, animation - dt);
    invalid = std::max(0.f, invalid - dt);
  }
  bool save(const std::string &path) const {
    std::ofstream o(path + ".tmp");
    o << "BLOCKOUT 1\n"
      << current << ' ' << unlocked << ' ' << cursor << ' ' << selected << ' '
      << muted << ' ' << tutorial << ' ' << canUndo << '\n';
    auto write = [&](const Board &b) {
      o << b.moves << ' ' << b.blocks.size() << '\n';
      for (auto &p : b.blocks)
        o << p.x << ' ' << p.y << ' ' << p.w << ' ' << p.h << ' ' << p.color
          << ' ' << p.alive << '\n';
    };
    write(board);
    if (canUndo)
      write(previous);
    o.flush();
    bool ok = bool(o);
    o.close();
    return ok && std::rename((path + ".tmp").c_str(), path.c_str()) == 0;
  }
  bool load(const std::string &path) {
    std::ifstream in(path);
    std::string magic;
    int version;
    Game temp;
    if (!(in >> magic >> version) || magic != "BLOCKOUT" || version != 1)
      return false;
    if (!(in >> temp.current >> temp.unlocked >> temp.cursor >> temp.selected >>
          temp.muted >> temp.tutorial >> temp.canUndo) ||
        temp.current < 0 || temp.current >= levelCount ||
        temp.unlocked < temp.current || temp.unlocked >= levelCount ||
        temp.cursor < 0 ||
        temp.cursor >= level(temp.current).size * level(temp.current).size ||
        temp.selected < -1)
      return false;
    auto read = [&](Board &b) {
      int count;
      if (!(in >> b.moves >> count) || count < 2 || count > 10)
        return false;
      b.blocks.resize(count);
      for (auto &p : b.blocks)
        if (!(in >> p.x >> p.y >> p.w >> p.h >> p.color >> p.alive))
          return false;
      return valid(b, temp.current);
    };
    if (!read(temp.board) || (temp.canUndo && !read(temp.previous)) ||
        temp.selected >= int(temp.board.blocks.size()) ||
        (temp.selected >= 0 && !temp.board.blocks[temp.selected].alive))
      return false;
    temp.scene = Scene::Menu;
    *this = temp;
    return true;
  }
};
} // namespace blockout
