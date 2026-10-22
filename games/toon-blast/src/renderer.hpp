#pragma once
#include "game_logic.hpp"
#include "pixel.hpp"
namespace pop {
struct Renderer {
  ui::Canvas c;
  float timer = 1, error = 0;
  State before;
  Effect effect;
  explicit Renderer(SDL_Renderer *r) : c(r) {}
  static ui::Paint color(int i) {
    const ui::Paint colors[] = {
        ui::Paint(242, 112, 137), ui::Paint(71, 183, 230),
        ui::Paint(248, 194, 73), ui::Paint(95, 208, 160),
        ui::Paint(176, 131, 236)};
    return colors[i];
  }
  void tile(int x, int y, int v, bool lit = true, int size = 13) {
    using namespace ui;
    auto p = color(v);
    if (!lit) {
      p.r = Uint8(p.r * .8);
      p.g = Uint8(p.g * .8);
      p.b = Uint8(p.b * .8);
    }
    c.round(x, y + 2, size, size, 3, Paint(10, 22, 40));
    c.round(x, y, size, size, 3, p);
    if (size < 10)
      return;
    c.line(x + 3, y + 1, x + size - 4, y + 1,
           Paint(std::min(255, p.r + 40), std::min(255, p.g + 40),
                 std::min(255, p.b + 40)));
    int cx = x + size / 2, cy = y + size / 2;
    Paint ink(35, 53, 78);
    if (v == 0) {
      c.rect(cx - 2, cy - 1, 2, 2, ink);
      c.rect(cx + 1, cy - 1, 2, 2, ink);
      c.line(cx - 2, cy + 1, cx, cy + 3, ink);
      c.line(cx + 2, cy + 1, cx, cy + 3, ink);
    } else if (v == 1) {
      c.line(cx - 2, cy, cx + 2, cy, ink);
      c.line(cx, cy - 2, cx, cy + 2, ink);
    } else if (v == 2) {
      for (int dy = -2; dy <= 2; ++dy)
        c.line(cx - 2 + std::abs(dy), cy + dy, cx + 2 - std::abs(dy), cy + dy,
               ink);
    } else if (v == 3) {
      c.line(cx - 2, cy - 1, cx + 2, cy - 1, ink);
      c.line(cx - 2, cy + 2, cx + 2, cy + 2, ink);
    } else {
      c.disc(cx, cy, 2, ink);
      c.disc(cx, cy, 0, p);
    }
  }
  void animate(const State &old, const Effect &e) {
    before = old;
    effect = e;
    timer = 0;
  }
  float duration() const { return .42f; }
  void draw(const Game &g) {
    using namespace ui;
    c.rect(0, 0, 240, 136, Paint(22, 31, 51));
    c.text(9, 5, "COLOR POP", paper);
    c.text(159, 5, "FASE " + std::to_string(g.level + 1) + "/20", paper);
    c.round(10, 18, 115, 96, 5, Paint(12, 21, 38));
    std::array<bool, N> highlight{};
    if (g.pulse) {
      for (int i = 0; i < N; ++i)
        highlight[i] = i % W == g.selected % W || i / W == g.selected / W;
    } else
      for (int p : group(g.state, g.selected))
        highlight[p] = true;
    bool anim = timer < duration();
    for (int i = 0; i < N; ++i) {
      int x = 16 + 15 * (i % W), y = 21 + 15 * (i / W);
      if (anim && timer < .12f) {
        if (effect.removed & (uint64_t(1) << i)) {
          int shrink = int(timer / .12f * 11);
          tile(x + shrink / 2, y + shrink / 2, before.board[i], true,
               13 - shrink);
        } else
          tile(x, y, before.board[i]);
      } else {
        float t = std::min(1.f, std::max(0.f, (timer - .12f) / .30f));
        if (anim && !effect.shuffled) {
          int source = effect.from[i];
          int start = source >= 0 ? 21 + 15 * (source / W) : -15 - 15 * (i / W);
          y = int(start + (y - start) * (1 - (1 - t) * (1 - t)));
        }
        if (y >= 18)
          tile(x, y, g.state.board[i], highlight[i]);
      }
    }
    if (anim) {
      for (int i = 0; i < N; ++i)
        if (effect.removed & (uint64_t(1) << i)) {
          int cx = 22 + 15 * (i % W), cy = 27 + 15 * (i / W);
          for (int d = 0; d < 4; ++d) {
            float a = d * 1.5708f + .4f;
            c.disc(cx + float(std::cos(a)) * timer * 26,
                   cy + float(std::sin(a)) * timer * 26 + timer * timer * 22, 1,
                   color(before.board[i]));
          }
        }
      c.centered(68, 40 - int(timer * 25), "+" + std::to_string(effect.points),
                 paper);
    }
    int x = 15 + 15 * (g.selected % W), y = 20 + 15 * (g.selected / W);
    Paint sel = error > 0 ? Paint(255, 78, 91)
                : g.pulse ? Paint(255, 218, 102)
                          : paper;
    for (int d = 0; d < 2; ++d) {
      c.line(x - d, y - d, x + 14 + d, y - d, sel);
      c.line(x - d, y + 14 + d, x + 14 + d, y + 14 + d, sel);
      c.line(x - d, y - d, x - d, y + 14 + d, sel);
      c.line(x + 14 + d, y - d, x + 14 + d, y + 14 + d, sel);
    }
    c.text(136, 22, "PTS " + std::to_string(g.state.score), paper);
    auto &l = levels()[g.level];
    for (int j = 0; j < 2; ++j) {
      int v = (g.level + j) % l.colors;
      tile(136, 36 + 16 * j, v);
      c.text(155, 39 + 16 * j,
             std::to_string(std::min(l.quota, g.state.collected[v])) + "/" +
                 std::to_string(l.quota),
             paper);
    }
    c.text(136, 72, "JOGADAS", Paint(172, 190, 211));
    c.text(136, 82, std::to_string(g.state.moves), paper);
    c.text(177, 82, "X" + std::to_string(g.state.combo), color(2));
    c.text(136, 97, g.pulse ? "PULSO ATIVO" : "Y PULSO",
           g.state.charge >= 12 ? color(2) : Paint(172, 190, 211));
    c.round(136, 108, 86, 4, 1, Paint(51, 65, 88));
    c.rect(136, 108, 86 * g.state.charge / 12, 4, color(2));
    c.text(8, 117, "A ESTOURA  B DESFAZ  Y PULSO", paper);
    c.text(8, 127, "START PAUSA   SELECT SAIR", Paint(166, 185, 202));
  }
  void panel(const std::string &title, const std::vector<std::string> &rows,
             int chosen) {
    using namespace ui;
    c.rect(0, 0, 240, 136, Paint(22, 31, 51));
    c.centered(120, 12, title, paper);
    for (size_t i = 0; i < rows.size(); ++i) {
      int y = 33 + int(i) * 17;
      if (int(i) == chosen)
        c.round(19, y - 3, 202, 14, 3, Paint(58, 80, 110));
      c.text(26, y, rows[i], paper);
    }
  }
};
} // namespace pop
