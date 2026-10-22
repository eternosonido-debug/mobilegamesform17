#pragma once
#include "game_logic.hpp"
#include "pixel.hpp"
#include <array>
#include <vector>
namespace tower {
struct Renderer {
  ui::Canvas c;
  std::array<std::array<int, 97>, 25> lookup{};
  std::array<std::array<bool, 97>, 25> border{};
  float camera = 0, burst = 1;
  int burstRow = 0;
  float elapsed = 0;
  explicit Renderer(SDL_Renderer *r) : c(r) {
    for (int y = -12; y <= 12; ++y)
      for (int x = -48; x <= 48; ++x) {
        double nx = x / 48., ny = y / 12., rad = nx * nx + ny * ny;
        double a = std::atan2(ny, nx);
        if (a < 0)
          a += 6.28318530718;
        lookup[y + 12][x + 48] =
            rad > 1 || rad < .16 ? -1 : int(a / 6.28318530718 * 1200) % 1200;
        border[y + 12][x + 48] = rad > .93 || rad < .20;
      }
  }
  void update(float dt, const Game &g) {
    elapsed += dt;
    burst += dt;
    camera += float(g.state.layer - camera) * std::min(1.f, dt * 9);
    camera = std::max(float(g.state.layer) - 2, camera);
  }
  void restart() {
    camera = 0;
    burst = 1;
  }
  void broken(int row) {
    burstRow = row;
    burst = 0;
  }
  static ui::Paint color(int row, int stage) {
    const ui::Paint colors[] = {ui::Paint(69, 202, 193),
                                ui::Paint(245, 180, 64),
                                ui::Paint(161, 128, 230)};
    return colors[(row + stage) % 3];
  }
  void platform(int cy, int row, const Game &g) {
    using namespace ui;
    auto safe = color(row, g.stage);
    for (int pass = 0; pass < 2; ++pass)
      for (int yy = -12; yy <= 12; ++yy) {
        int run = -1, last = -1;
        auto flush = [&](int end) {
          if (run < 0 || last < 0)
            return;
          Paint p(13, 26, 42);
          if (last == 1)
            p = safe;
          if (last == 2)
            p = Paint(safe.r * 3 / 4, safe.g * 3 / 4, safe.b * 3 / 4);
          if (last == 3)
            p = Paint(244, 87, 104);
          if (last == 4)
            p = Paint(66, 39, 59);
          c.rect(78 + run - 48, cy + yy + (pass == 0 ? 4 : 0), end - run, 1, p);
        };
        for (int xx = 0; xx <= 97; ++xx) {
          int cls = -1;
          if (xx < 97 && lookup[yy + 12][xx] >= 0) {
            cls = 0;
            if (pass == 1 && !border[yy + 12][xx]) {
              int angle = (lookup[yy + 12][xx] - g.state.angle -
                           offset(row, g.stage) + 2400) %
                          1200;
              int sector = angle / 100;
              bool bad = (level(g.stage).danger & (1 << sector)) != 0;
              cls = bad ? (((xx + yy) / 3 % 2) == 0 ? 3 : 4)
                        : ((sector % 2) ? 1 : 2);
              if (angle % 100 < 4)
                cls = 0;
            }
          }
          if (cls != last) {
            flush(xx);
            run = xx;
            last = cls;
          }
        }
      }
  }
  void ellipse(float x, float y, int rx, int ry, ui::Paint p) {
    for (int dy = -ry; dy <= ry; ++dy) {
      float dx =
          rx * std::sqrt(std::max(0.f, 1 - float(dy * dy) / float(ry * ry)));
      c.line(x - dx, y + dy, x + dx, y + dy, p);
    }
  }
  void draw(const Game &g, bool held) {
    using namespace ui;
    c.rect(0, 0, 240, 136, Paint(19, 28, 47));
    c.text(8, 5, "TOWER BREAK", paper);
    c.text(161, 5, "TORRE " + std::to_string(g.stage + 1) + "/3", paper);
    c.round(7, 19, 139, 94, 5, Paint(25, 39, 60));
    for (int i = 0; i < 12; ++i)
      c.rect(15 + (i * 37) % 125, 24 + (i * 23) % 80, 1, 1, Paint(57, 74, 96));
    SDL_Rect playfield{8, 20, 138, 93};
    SDL_RenderSetClipRect(c.r, &playfield);
    c.round(70, 20, 17, 93, 4, Paint(49, 68, 94));
    c.rect(74, 20, 5, 93, Paint(90, 115, 143));
    c.line(83, 22, 83, 110, Paint(32, 48, 73));
    for (int row = std::min(level(g.stage).layers - 1, g.state.layer + 5);
         row >= g.state.layer; --row) {
      int y = int(48 + (row - camera) * 14);
      if (y < 125)
        platform(y, row, g);
    }
    int surface = int(60 + (g.state.layer - camera) * 14);
    if (won(g.state, g.stage)) {
      c.round(32, 55, 92, 12, 4, Paint(247, 192, 66));
      c.centered(78, 57, "BASE!", ink);
      surface = 55;
    }
    float y = surface - 5 - g.state.z * .0006f;
    ellipse(78, float(surface), 6, 2, Paint(13, 23, 39));
    if (held && !g.state.dead) {
      ellipse(78, y - 10, 3, 2, Paint(243, 161, 83));
      ellipse(78, y - 17, 2, 1, Paint(172, 94, 78));
    }
    ellipse(78, y + 1, g.state.z < 1500 ? 7 : 6, g.state.z < 1500 ? 4 : 6,
            Paint(16, 22, 34));
    ellipse(78, y, g.state.z < 1500 ? 6 : 5, g.state.z < 1500 ? 3 : 5,
            g.state.dead ? Paint(234, 75, 92) : Paint(255, 222, 155));
    c.disc(76, y - 2, 1, paper);
    if (burst < .55f) {
      float origin = 60 + (burstRow - camera) * 14;
      for (int i = 0; i < 10; ++i) {
        float a = i * .6283f;
        int x = int(78 + std::cos(a) * (20 + burst * 70));
        int py =
            int(origin + std::sin(a) * (5 + burst * 30) + burst * burst * 40);
        c.rect(x, py, 3, 2, color(burstRow, g.stage));
      }
    }
    SDL_RenderSetClipRect(c.r, nullptr);
    bool bad = dangerAt(g.stage, g.state.layer, g.state.angle);
    c.text(156, 27, "ANDARES", Paint(174, 193, 210));
    c.text(156, 39,
           std::to_string(g.state.layer) + "/" +
               std::to_string(level(g.stage).layers),
           paper);
    c.round(154, 54, 78, 17, 3, bad ? Paint(95, 40, 60) : Paint(28, 91, 87));
    c.centered(193, 59, bad ? "PERIGO" : "SEGURO", paper);
    c.text(156, 81, "PTS " + std::to_string(g.state.score), paper);
    c.text(156, 96, "COMBO " + std::to_string(g.state.combo), color(1, 0));
    unsigned seconds = g.state.ticks / 120;
    c.text(156, 108,
           "TEMPO " + std::to_string(seconds / 60) + ":" +
               (seconds % 60 < 10 ? "0" : "") + std::to_string(seconds % 60),
           Paint(166, 185, 202));
    c.text(8, 117, "SEGURE A: CAIR  SOLTE: FREAR", paper);
    c.text(8, 127, "START PAUSA   SELECT SAIR", Paint(166, 185, 202));
  }
  void panel(const std::string &title, const std::vector<std::string> &rows,
             int chosen) {
    using namespace ui;
    c.rect(0, 0, 240, 136, Paint(19, 28, 47));
    c.centered(120, 12, title, paper);
    for (size_t i = 0; i < rows.size(); ++i) {
      int y = 33 + int(i) * 17;
      if (int(i) == chosen)
        c.round(19, y - 3, 202, 14, 3, Paint(53, 77, 102));
      c.text(26, y, rows[i], paper);
    }
  }
};
} // namespace tower
