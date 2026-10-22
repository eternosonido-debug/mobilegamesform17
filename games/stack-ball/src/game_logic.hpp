#pragma once
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
namespace tower {
struct Level {
  int layers, speed, danger;
};
inline Level level(int i) {
  const Level all[] = {{18, 2, (1 << 6) | (1 << 7)},
                       {24, 3, (1 << 2) | (1 << 3) | (1 << 8)},
                       {30, 4, (1 << 1) | (1 << 2) | (1 << 6) | (1 << 7)}};
  return all[std::max(0, std::min(2, i))];
}
// Angles use 1/1200 turn, height uses thousandths of a logical pixel.
// Rules advance only in fixed 1/120-second steps, independently of rendering.
struct State {
  int layer = 0, angle = 0, z = 18000, velocity = 0, combo = 0, score = 0;
  uint32_t ticks = 0;
  bool dead = false;
};
inline int offset(int row, int stage) {
  return (row * 137 + stage * 173) % 1200;
}
inline bool dangerAt(int stage, int row, int angle) {
  int sector = ((300 - angle - offset(row, stage)) % 1200 + 1200) % 1200 / 100;
  return (level(stage).danger & (1 << sector)) != 0;
}
inline bool won(const State &s, int stage) {
  return s.layer == level(stage).layers && !s.dead;
}
struct Event {
  bool broken = false, bounced = false, died = false;
  int row = -1;
};
inline Event step(State &s, int stage, bool held, int steer = 0) {
  Event e;
  if (s.dead || won(s, stage))
    return e;
  ++s.ticks;
  steer = std::max(-1, std::min(1, steer));
  int rotation = steer ? steer * (level(stage).speed + 2) : level(stage).speed;
  s.angle = (s.angle + rotation + 1200) % 1200;
  s.velocity = std::max(-1400, s.velocity - (held ? 90 : 35));
  s.z += s.velocity;
  if (s.z > 0)
    return e;
  s.z = 0;
  if (!held) {
    s.velocity = 800;
    s.combo = 0;
    e.bounced = true;
    return e;
  }
  if (dangerAt(stage, s.layer, s.angle)) {
    s.dead = true;
    s.combo = 0;
    s.velocity = 0;
    e.died = true;
    return e;
  }
  e.row = s.layer;
  e.broken = true;
  ++s.layer;
  ++s.combo;
  s.score += 10 * s.combo;
  s.z = 24000;
  s.velocity = -400;
  return e;
}
struct Game {
  int stage = 0, unlocked = 0;
  bool muted = false;
  State state;
  void begin(int n) {
    stage = std::max(0, std::min(2, n));
    state = State{};
  }
  Event advance(bool held, int steer = 0) {
    auto e = step(state, stage, held, steer);
    if (won(state, stage))
      unlocked = std::max(unlocked, std::min(2, stage + 1));
    return e;
  }
  bool save(const std::string &path) const {
    std::string tmp = path + ".tmp";
    std::ofstream f(tmp);
    auto &s = state;
    f << "TOWER1 " << stage << ' ' << unlocked << ' ' << muted << ' ' << s.layer
      << ' ' << s.angle << ' ' << s.z << ' ' << s.velocity << ' ' << s.combo
      << ' ' << s.score << ' ' << s.ticks << ' ' << s.dead << '\n';
    f.close();
    return bool(f) && std::rename(tmp.c_str(), path.c_str()) == 0;
  }
  bool load(const std::string &path) {
    std::ifstream f(path);
    std::string magic;
    Game g;
    auto &s = g.state;
    if (!(f >> magic >> g.stage >> g.unlocked >> g.muted >> s.layer >>
          s.angle >> s.z >> s.velocity >> s.combo >> s.score >> s.ticks >>
          s.dead) ||
        magic != "TOWER1" || g.stage < 0 || g.stage > 2 ||
        g.unlocked < g.stage || g.unlocked > 2 || s.layer < 0 ||
        s.layer > level(g.stage).layers || s.angle < 0 || s.angle >= 1200 ||
        s.z < 0 || s.z > 50000 || s.velocity < -1400 || s.velocity > 800 ||
        s.combo < 0 || s.combo > s.layer || s.score < 0 ||
        s.score > 5 * level(g.stage).layers * (level(g.stage).layers + 1) ||
        (s.dead && s.layer == level(g.stage).layers))
      return false;
    *this = g;
    return true;
  }
};
} // namespace tower
