#pragma once
#include "game_logic.hpp"
#include "pixel.hpp"
namespace tiles {
struct Renderer {
  ui::Canvas c;
  std::array<float, 4> flash{{0, 0, 0, 0}};
  int grade = -1;
  float gradeTime = 0;
  explicit Renderer(SDL_Renderer *r) : c(r) {}
  static ui::Paint color(int i) {
    const ui::Paint colors[] = {
        ui::Paint(91, 216, 172), ui::Paint(244, 174, 80),
        ui::Paint(111, 175, 241), ui::Paint(225, 127, 207)};
    return colors[i];
  }
  void update(float dt) {
    for (float &f : flash)
      f = std::max(0.f, f - dt);
    gradeTime = std::max(0.f, gradeTime - dt);
  }
  void hit(int lane, int result) {
    flash[lane] = .18f;
    grade = result;
    gradeTime = .6f;
  }
  void draw(const State &s, int ms) {
    using namespace ui;
    c.rect(0, 0, 240, 136, Paint(17, 24, 43));
    c.text(8, 4, "MAGIC TILES", paper);
    int sec = std::max(0, ms / 1000);
    c.text(157, 4,
           std::to_string(sec / 60) + ":" + (sec % 60 < 10 ? "0" : "") +
               std::to_string(sec % 60) + " /3:0" +
               std::to_string(duration(s.song) / 1000 - 180),
           paper);
    c.rect(8, 15, 224, 2, Paint(45, 60, 84));
    c.rect(8, 15, 224 * std::min(ms, duration(s.song)) / duration(s.song), 2,
           color(0));
    for (int lane = 0; lane < 4; ++lane) {
      int x = 12 + 30 * lane;
      c.round(x, 23, 27, 91, 3,
              flash[lane] > 0 ? Paint(50, 73, 92) : Paint(25, 37, 59));
      c.line(x + 1, 100, x + 25, 100, color(lane));
      c.round(x + 2, 102, 23, 11, 2, color(lane));
      const char *names[] = {"A", "B", "X", "Y"};
      c.centered(x + 13, 104, names[lane], ink);
    }
    for (size_t i = s.cursor; i < s.notes.size(); ++i) {
      auto n = s.notes[i];
      int difference = n.ms - ms;
      if (difference > 2600)
        break;
      if (s.judged[i] || difference < -140)
        continue;
      int y = 100 - difference * 76 / 2600;
      if (y < 23 || y > 100)
        continue;
      int x = 14 + 30 * n.lane;
      c.round(x, y - 5, 23, 10, 2, color(n.lane));
      c.line(x + 3, y - 3, x + 19, y - 3, paper);
      c.disc(x + 11, y + 1, 1, Paint(34, 48, 72));
    }
    const char *song1[] = {"BRISA", "RUA", "PULSO"},
               *song2[] = {"DIGITAL", "NEON", "NOTURNO"};
    c.text(148, 27, song1[s.song], paper);
    c.text(148, 38, song2[s.song], paper);
    c.text(148, 55, s.practice ? "TREINO" : "VIDA", Paint(167, 189, 210));
    c.round(148, 66, 80, 5, 2, Paint(45, 59, 82));
    c.rect(148, 66, 80 * s.life / 100, 5, color(0));
    c.text(148, 80, "COMBO " + std::to_string(s.combo), paper);
    c.text(148, 94, "PTS", Paint(167, 189, 210));
    c.text(148, 105, std::to_string(s.score()), paper);
    const char *labels[] = {"ERRO", "PERFEITO", "BOM", "QUASE"};
    if (gradeTime > 0 && grade >= 0)
      c.text(9, 119, labels[grade], grade ? color(0) : color(1));
    c.text(148, 119, "START PAUSA", Paint(167, 189, 210));
    c.text(9, 129, "SELECT MENU", Paint(167, 189, 210));
  }
  void panel(const std::string &title, const std::vector<std::string> &rows,
             int chosen) {
    using namespace ui;
    c.rect(0, 0, 240, 136, Paint(17, 24, 43));
    c.centered(120, 10, title, paper);
    for (size_t i = 0; i < rows.size(); ++i) {
      int y = 30 + int(i) * 14;
      if (int(i) == chosen)
        c.round(10, y - 3, 220, 12, 3, Paint(48, 71, 97));
      c.text(17, y, rows[i], paper);
    }
  }
};
inline int directionalLane(int x, int y) {
  return x < 0 ? 0 : x > 0 ? 3 : y > 0 ? 1 : y < 0 ? 2 : -1;
}
} // namespace tiles
