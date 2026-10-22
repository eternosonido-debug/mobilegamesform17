#pragma once
#include "game_logic.hpp"
#include "pixel.hpp"
namespace screw {
inline int hx(int h) { return 26 + 34 * (h % 4); }
inline int hy(int h) { return 33 + 22 * (h / 4); }
struct Renderer {
  ui::Canvas c;
  float timer = 1;
  unsigned falling = 0;
  int flight = -1;
  bool pickup = true;
  float error = 0;
  explicit Renderer(SDL_Renderer *r) : c(r) {}
  void bolt(float x, float y, float angle = 0) {
    using namespace ui;
    c.disc(x + 1, y + 2, 6, Paint(24, 36, 49));
    c.disc(x, y, 5, Paint(130, 150, 165));
    c.disc(x - 1, y - 1, 4, Paint(216, 227, 231));
    c.line(x - 3 * std::cos(angle), y - 3 * std::sin(angle),
           x + 3 * std::cos(angle), y + 3 * std::sin(angle), ink);
    c.line(x + 2 * std::sin(angle), y - 2 * std::cos(angle),
           x - 2 * std::sin(angle), y + 2 * std::cos(angle), ink);
  }
  void draw(const Game &g) {
    using namespace ui;
    c.rect(0, 0, 240, 136, Paint(22, 31, 44));
    c.text(9, 5, "SCREW PUZZLE", paper);
    c.text(166, 5, "FASE " + std::to_string(g.level + 1), paper);
    c.round(8, 20, 140, 92, 5, Paint(10, 17, 27));
    c.round(9, 18, 138, 91, 4, Paint(97, 70, 52));
    c.rect(13, 22, 130, 83, Paint(126, 93, 63));
    auto &l = levels()[g.level];
    for (int h = 0; h < 16; ++h)
      if (l.holes & (1u << h)) {
        c.disc(hx(h), hy(h), 5, Paint(68, 48, 36));
        c.disc(hx(h), hy(h) + 1, 3, Paint(30, 30, 32));
      }
    const Paint colors[] = {Paint(220, 91, 91), Paint(57, 155, 191),
                            Paint(232, 179, 62), Paint(85, 173, 121)};
    for (unsigned i = 0; i < l.plates.size(); ++i) {
      bool drop = falling & (1u << i);
      if (!(g.state.active & (1u << i)) && !(drop && timer < .55f))
        continue;
      auto &p = l.plates[i];
      int left = 150, right = 0, top = 110, bottom = 0;
      for (int h = 0; h < 16; ++h)
        if (p.cover & (1u << h)) {
          left = std::min(left, hx(h) - 10);
          right = std::max(right, hx(h) + 10);
          top = std::min(top, hy(h) - 8);
          bottom = std::max(bottom, hy(h) + 8);
        }
      int offset = drop ? int(390 * timer * timer) : 0;
      top += offset;
      bottom += offset;
      auto col = colors[p.color];
      c.round(left + 1, top + 3, right - left, bottom - top, 3,
              Paint(col.r / 2, col.g / 2, col.b / 2));
      c.round(left, top, right - left, bottom - top, 3, col);
      c.line(left + 3, top + 1, right - 4, top + 1,
             Paint(std::min(255, int(col.r) + 35),
                   std::min(255, int(col.g) + 35),
                   std::min(255, int(col.b) + 35)));
      for (int h = 0; h < 16; ++h)
        if (p.pins & (1u << h)) {
          c.disc(hx(h), hy(h) + offset, 5, Paint(35, 46, 52));
          c.disc(hx(h), hy(h) + offset, 3, Paint(83, 72, 59));
        }
    }
    for (int h = 0; h < 16; ++h)
      if ((g.state.occupied & (1u << h)) && visible(l, g.state, h) &&
          !(flight == h && !pickup && timer < .25f))
        bolt(hx(h), hy(h));
    int x = hx(g.selected), y = hy(g.selected);
    Paint sel = error > 0 ? Paint(255, 90, 80) : Paint(255, 250, 185);
    for (int d = -1; d <= 1; ++d) {
      c.line(x - 9, y - 9 + d, x - 5, y - 9 + d, sel);
      c.line(x + 5, y - 9 + d, x + 9, y - 9 + d, sel);
      c.line(x - 9, y + 9 + d, x - 5, y + 9 + d, sel);
      c.line(x + 5, y + 9 + d, x + 9, y + 9 + d, sel);
    }
    c.text(159, 24, "PLACAS", paper);
    int remaining = 0;
    for (unsigned b = g.state.active; b; b &= b - 1)
      ++remaining;
    c.text(159, 35, std::to_string(remaining) + " RESTAM",
           Paint(115, 221, 184));
    c.text(159, 49, "GIROS " + std::to_string(g.state.moves), paper);
    c.round(173, 65, 41, 22, 4, Paint(45, 59, 76));
    if (g.state.hand && !(pickup && timer < .25f))
      bolt(193, 75);
    c.centered(193, 93, g.state.hand ? "ENCAIXE" : "RETIRE", paper);
    if (flight >= 0 && timer < .25f) {
      float t = timer / .25f;
      if (!pickup)
        t = 1 - t;
      bolt(hx(flight) * (1 - t) + 193 * t,
           hy(flight) * (1 - t) + 75 * t - 15 * std::sin(t * 3.14159f),
           timer * 35);
    }
    c.text(8, 117, "A MOVER  B VOLTAR  Y DESFAZER", paper);
    c.text(8, 127, "START PAUSA   SELECT SAIR", Paint(166, 185, 202));
  }
  void panel(const std::string &title, const std::vector<std::string> &rows,
             int chosen) {
    using namespace ui;
    c.rect(0, 0, 240, 136, Paint(22, 31, 44));
    c.centered(120, 12, title, paper);
    for (size_t i = 0; i < rows.size(); ++i) {
      int y = 33 + int(i) * 17;
      if (int(i) == chosen)
        c.round(19, y - 3, 202, 14, 3, Paint(56, 88, 105));
      c.text(26, y, rows[i], paper);
    }
  }
};
} // namespace screw
