#pragma once
#include "game_logic.hpp"
#include "pixel.hpp"
#include <vector>
namespace cooking {
using ui::Paint;
struct Renderer {
  ui::Canvas c;
  SDL_Texture *background = nullptr, *icons = nullptr;
  std::array<SDL_Texture *, 4> cakes{{nullptr, nullptr, nullptr, nullptr}};
  std::array<Design, 4> cached;
  int cachedFlavor = -1;
  bool missing = false;
  float time = 0, feedback = 0;
  bool good = true;
  Paint ink{36, 61, 79}, cream{255, 248, 220}, red{239, 74, 83},
      blue{35, 163, 224}, yellow{255, 209, 66}, green{78, 187, 118};
  static Paint color(int i) {
    const Paint colors[] = {Paint(245, 90, 106), Paint(53, 173, 232),
                            Paint(255, 205, 72), Paint(146, 105, 227),
                            Paint(75, 196, 151), Paint(246, 142, 67)};
    return colors[(i % 6 + 6) % 6];
  }
  explicit Renderer(SDL_Renderer *r, const std::string &assets) : c(r) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_Surface *s = SDL_LoadBMP((assets + "/kitchen.bmp").c_str());
    if (s) {
      background = SDL_CreateTextureFromSurface(r, s);
      SDL_FreeSurface(s);
    }
    missing = !background;
    icons = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888,
                              SDL_TEXTUREACCESS_TARGET, 480, 272);
    if (icons) {
      SDL_SetRenderTarget(r, icons);
      SDL_SetRenderDrawColor(r, 0, 0, 0, 0);
      SDL_RenderClear(r);
      for (int flavor = 0; flavor < 3; ++flavor)
        for (int i = 0; i < 6; ++i)
          ingredient(i * 76 + 38, flavor * 80 + 39, i, flavor);
      SDL_SetRenderTarget(r, nullptr);
      SDL_SetTextureBlendMode(icons, SDL_BLENDMODE_BLEND);
    }
  }
  ~Renderer() {
    if (background)
      SDL_DestroyTexture(background);
    if (icons)
      SDL_DestroyTexture(icons);
    for (auto *t : cakes)
      if (t)
        SDL_DestroyTexture(t);
  }
  void update(float dt) {
    time += dt;
    feedback = std::max(0.f, feedback - dt);
  }
  void response(bool yes) {
    good = yes;
    feedback = 1.0f;
  }
  void oval(int x, int y, int w, int h, Paint p) {
    if (w < 1 || h < 1)
      return;
    for (int yy = -h; yy <= h; ++yy) {
      int dx =
          int(w * std::sqrt(std::max(0.f, 1.f - float(yy * yy) / (h * h))));
      c.line(x - dx, y + yy, x + dx, y + yy, p);
    }
  }
  void ball(int x, int y, int w, int h, Paint top, Paint bottom,
            bool outline = true) {
    if (outline)
      oval(x, y, w + 2, h + 2, ink);
    for (int yy = -h; yy <= h; ++yy) {
      float t = float(yy + h) / (2 * h);
      int dx =
          int(w * std::sqrt(std::max(0.f, 1.f - float(yy * yy) / (h * h))));
      c.line(x - dx, y + yy, x + dx, y + yy,
             Paint(int(top.r * (1 - t) + bottom.r * t),
                   int(top.g * (1 - t) + bottom.g * t),
                   int(top.b * (1 - t) + bottom.b * t)));
    }
  }
  void box(int x, int y, int w, int h, Paint p, int radius = 8) {
    c.round(x, y + 3, w, h, radius, Paint(33, 55, 72, 70));
    c.round(x, y, w, h, radius, ink);
    c.round(x + 2, y + 2, w - 4, h - 4, std::max(1, radius - 2), p);
  }
  void shine(int x, int y, int w, int h) {
    oval(x, y, w, h, Paint(255, 255, 255, 155));
  }
  void sparkle(int x, int y, int size, Paint p) {
    c.line(x - size, y, x + size, y, p);
    c.line(x, y - size, x, y + size, p);
    c.disc(x, y, 2, p);
  }
  void star(int x, int y, int n, Paint p) {
    for (int i = 0; i < 5; ++i) {
      float a = -1.5707963f + i * 1.256637f, b = a + 2.513274f;
      c.line(x + std::cos(a) * n, y + std::sin(a) * n, x + std::cos(b) * n,
             y + std::sin(b) * n, p);
    }
    c.disc(x, y, n / 3, p);
  }
  void leaf(int x, int y) {
    oval(x - 4, y - 1, 6, 3, green);
    oval(x + 4, y - 4, 5, 3, green);
    c.line(x - 6, y + 1, x + 8, y - 6, ink);
  }
  void berry(int x, int y, int size = 8) {
    ball(x, y, size, size, Paint(255, 129, 143), Paint(208, 47, 89));
    leaf(x, y - size);
    for (int i = 0; i < 4; ++i)
      c.rect(x + (i % 2 ? 3 : -3), y + (i / 2) * 4, 1, 2, yellow);
  }
  void hand(int x, int y) {
    c.round(x - 7, y - 3, 19, 15, 6, ink);
    c.round(x - 5, y - 2, 15, 12, 5, cream);
    c.round(x - 6, y - 15, 7, 19, 3, ink);
    c.round(x - 4, y - 13, 3, 16, 2, cream);
    c.round(x + 9, y + 1, 7, 10, 3, ink);
    c.round(x + 10, y + 2, 4, 7, 2, cream);
  }
  void ingredient(int x, int y, int id, int flavor) {
    oval(x, y + 26, 24, 5, Paint(27, 44, 62, 45));
    if (id == 0 || id == 1) {
      box(x - 21, y - 22, 42, 43, id == 0 ? cream : Paint(203, 237, 248), 5);
      c.rect(x - 18, y - 16, 36, 8, id == 0 ? yellow : blue);
      c.round(x - 12, y - 1, 24, 17, 4, id == 0 ? Paint(255, 221, 154) : cream);
      if (id == 0) {
        c.line(x, y + 10, x, y - 2, ink);
        for (int i = 0; i < 3; ++i) {
          c.line(x, y + 6 - i * 4, x - 6, y + 2 - i * 4, ink);
          c.line(x, y + 6 - i * 4, x + 6, y + 2 - i * 4, ink);
        }
      } else
        sparkle(x, y + 7, 6, blue);
      shine(x - 14, y - 8, 2, 6);
    } else if (id == 2) {
      box(x - 26, y + 4, 52, 12, Paint(151, 206, 220), 5);
      box(x - 19, y - 13, 38, 22, yellow, 4);
      c.line(x - 15, y - 9, x + 12, y - 9, cream);
      c.line(x + 7, y - 10, x + 7, y + 5, Paint(221, 161, 61));
    } else if (id == 3) {
      ball(x - 10, y + 2, 12, 18, cream, Paint(228, 190, 143));
      ball(x + 13, y - 3, 12, 18, Paint(255, 253, 234), Paint(241, 211, 163));
      shine(x + 8, y - 10, 3, 5);
    } else if (id == 4) {
      box(x - 18, y - 19, 36, 46, Paint(215, 239, 246), 4);
      c.round(x - 18, y - 8, 36, 13, 2, blue);
      box(x - 11, y - 26, 21, 9, cream, 3);
      c.text(x - 15, y + 9, "LEITE", ink);
      c.disc(x, y - 1, 4, cream);
    } else if (flavor == 2) {
      berry(x, y, 18);
    } else if (flavor == 1) {
      box(x - 21, y - 19, 42, 40, Paint(112, 63, 53), 4);
      for (int i = 0; i < 4; ++i)
        box(x - 17 + (i % 2) * 18, y - 14 + (i / 2) * 17, 15, 14,
            Paint(163, 96, 66), 2);
    } else {
      box(x - 13, y - 19, 26, 42, Paint(225, 164, 69), 5);
      box(x - 12, y - 26, 24, 11, red, 3);
      star(x, y + 1, 8, cream);
      shine(x - 7, y - 9, 2, 5);
    }
  }
  void itemSprite(int x, int y, int id, int flavor, double angle = 0,
                  float scale = 1) {
    // Flavor art is rendered directly when still; moving jars share a generic
    // ingredient atlas.
    if (id == 5 && angle == 0) {
      ingredient(x, y, id, flavor);
      return;
    }
    if (!icons) {
      ingredient(x, y, id, flavor);
      return;
    }
    SDL_Rect src{id * 76, flavor * 80, 76, 80},
        dst{x - int(38 * scale), y - int(39 * scale), int(76 * scale),
            int(80 * scale)};
    SDL_RenderCopyEx(c.r, icons, &src, &dst, angle, nullptr, SDL_FLIP_NONE);
  }
  void mascot(int x, int y) {
    int bob = int(std::sin(time * 4) * 2);
    oval(x, y + 22, 27, 6, Paint(28, 53, 63, 55));
    ball(x, y + bob, 22, 25, yellow, Paint(243, 164, 58));
    oval(x - 21, y + 6 + bob, 7, 12, yellow);
    oval(x + 21, y + int(std::sin(time * 6) * 5) + bob, 7, 12, yellow);
    c.round(x - 16, y + 14 + bob, 32, 8, 3, blue);
    c.rect(x + 5, y + 19 + bob, 7, 9, blue);
    ball(x - 7, y - 4 + bob, 4, 6, cream, cream, false);
    ball(x + 8, y - 4 + bob, 4, 6, cream, cream, false);
    c.disc(x - 6, y - 3 + bob, 2, ink);
    c.disc(x + 9, y - 3 + bob, 2, ink);
    ball(x + 1, y + 5 + bob, 7, 4, Paint(255, 167, 72), Paint(235, 112, 53));
    c.round(x - 19, y - 29 + bob, 38, 8, 3, cream);
    c.disc(x - 12, y - 33 + bob, 10, cream);
    c.disc(x, y - 38 + bob, 12, cream);
    c.disc(x + 13, y - 33 + bob, 10, cream);
  }
  void bowl(int x, int y, int flavor, int amount, bool stir = false) {
    oval(x, y + 39, 74, 12, Paint(26, 56, 61, 58));
    ball(x, y + 13, 66, 34, Paint(126, 222, 226), Paint(30, 142, 178));
    ball(x, y - 3, 69, 26, Paint(215, 254, 245), Paint(101, 201, 212));
    oval(x, y - 3, 61, 20, Paint(50, 119, 136));
    if (amount) {
      Paint top = flavor == 1   ? Paint(183, 112, 79)
                  : flavor == 2 ? Paint(252, 178, 180)
                                : Paint(255, 228, 155);
      oval(x, y - 2, 58, 18, top);
      if (stir) {
        float a = time * 4;
        int px = x, py = y;
        for (int i = 0; i < 100; ++i) {
          float t = float(i) / 99, r = t * 51, ang = t * 13 + a;
          int xx = x + int(std::cos(ang) * r),
              yy = y + int(std::sin(ang) * r * .28f);
          if (i)
            c.line(px, py, xx, yy, cream);
          px = xx;
          py = yy;
        }
      } else
        for (int i = 0; i < amount; ++i) {
          int xx = x - 35 + (i * 23) % 70, yy = y - 8 + (i * 9) % 17;
          c.disc(xx, yy, 3, i % 2 ? cream : yellow);
        }
    }
    c.line(x - 56, y + 10, x - 42, y + 28, Paint(207, 255, 251));
    shine(x - 47, y + 18, 3, 8);
    c.line(x - 39, y + 39, x + 35, y + 39, Paint(53, 128, 159));
  }
  void drawCake(int x, int base, int flavor, const Design &d) {
    oval(x, base + 7, 61, 11, Paint(29, 47, 62, 55));
    ball(x, base + 1, 63, 14, cream, Paint(157, 219, 237));
    oval(x, base - 2, 54, 9, cream);
    // Pleated, bevelled paper cup.
    for (int row = 0; row < 53; ++row) {
      int extent = 43 - row * 12 / 53;
      Paint col = d.wrapper == 5 ? color(row / 9) : color(d.wrapper);
      float shade = .97f - float(row) * .16f / 53;
      c.line(x - extent - 2, base - 54 + row, x + extent + 2, base - 54 + row,
             ink);
      c.line(x - extent, base - 54 + row, x + extent, base - 54 + row,
             Paint(int(col.r * shade), int(col.g * shade), int(col.b * shade)));
      for (int k = -4; k <= 4; ++k) {
        int xx = x + k * 9;
        if (xx > x - extent + 1 && xx < x + extent - 1)
          c.line(xx, base - 54 + row, xx + 2, base - 54 + row,
                 Paint(255, 255, 255, 55));
      }
    }
    c.line(x - 31, base, x + 31, base, ink);
    ball(x, base - 58, 48, 24,
         flavor == 1   ? Paint(190, 121, 79)
         : flavor == 2 ? Paint(255, 197, 155)
                       : Paint(255, 222, 132),
         flavor == 1 ? Paint(128, 73, 53) : Paint(215, 147, 70));
    for (int i = 0; i < 8; ++i) {
      int xx = x - 34 + (i * 23) % 68, yy = base - 71 + (i * 11) % 23;
      c.disc(xx, yy, 2, flavor == 1 ? Paint(76, 48, 48) : Paint(171, 100, 64));
    }
    if (d.icing) {
      const Paint colors[] = {cream,
                              Paint(255, 244, 215),
                              Paint(130, 75, 66),
                              Paint(251, 134, 175),
                              Paint(123, 225, 172),
                              Paint(255, 221, 93),
                              Paint(146, 180, 255)};
      Paint top = colors[d.icing],
            low(int(top.r * .82f), int(top.g * .79f), int(top.b * .85f));
      ball(x, base - 76, 49, 13, top, low);
      ball(x - 4, base - 91, 39, 12, top, low);
      ball(x + 2, base - 105, 28, 11, top, low);
      ball(x - 2, base - 117, 17, 9, top, low);
      ball(x + 3, base - 126, 7, 8, top, low);
      c.line(x - 29, base - 96, x + 17, base - 96, Paint(255, 255, 255, 110));
    }
    if (d.sprinkles) {
      for (int i = 0; i < 22; ++i) {
        int xx = x - 35 + (i * 29) % 70,
            yy = base - (d.icing ? 109 : 76) + (i * 17) % (d.icing ? 38 : 22);
        if (d.icing && yy < base - 95 && std::abs(xx - x) > 25)
          continue;
        if (d.sprinkles == 3)
          star(xx, yy, 3, color(i));
        else if (d.sprinkles == 2)
          c.disc(xx, yy, 2, Paint(73, 45, 50));
        else
          c.line(xx, yy, xx + 3, yy + (i % 2 ? 2 : -2), color(i));
      }
    }
    int top = base - (d.icing ? 137 : 85);
    if (d.fruit == 1)
      berry(x, top, 8);
    else if (d.fruit == 2) {
      ball(x - 7, top + 2, 7, 7, red, Paint(180, 48, 65));
      ball(x + 8, top + 4, 7, 7, red, Paint(180, 48, 65));
      c.line(x - 7, top - 4, x + 1, top - 16, green);
      c.line(x + 8, top - 3, x + 1, top - 16, green);
    } else if (d.fruit == 3) {
      ball(x, top, 11, 10, yellow, Paint(247, 151, 56));
      for (int i = 0; i < 6; ++i) {
        float a = i * 1.0472f;
        c.line(x, top, x + std::cos(a) * 8, top + std::sin(a) * 7, cream);
      }
    } else if (d.fruit == 4)
      leaf(x, top);
    if (d.face) {
      int yy = base - 27;
      oval(x - 19, yy + 8, 5, 3, Paint(255, 182, 169));
      oval(x + 19, yy + 8, 5, 3, Paint(255, 182, 169));
      for (int side : {-1, 1}) {
        if (d.face == 3 || (d.face == 2 && side == 1)) {
          c.line(x + side * 12 - 4, yy + 1, x + side * 12, yy - 2, ink);
          c.line(x + side * 12, yy - 2, x + side * 12 + 4, yy + 1, ink);
        } else {
          oval(x + side * 12, yy, 6, 8, cream);
          oval(x + side * 12 + 1, yy + 1, 4, 6, ink);
          c.disc(x + side * 12, yy - 2, 1, cream);
        }
      }
      ball(x, yy + 12, 6, 4, ink, ink, false);
      oval(x, yy + 14, 3, 1, red);
    }
  }
  void sync(const std::array<Design, 4> &designs, int flavor) {
    for (int i = 0; i < 4; ++i) {
      if (cakes[i] && cachedFlavor == flavor && cached[i] == designs[i])
        continue;
      if (cakes[i])
        SDL_DestroyTexture(cakes[i]);
      cakes[i] = SDL_CreateTexture(c.r, SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_TARGET, 160, 180);
      if (!cakes[i])
        continue;
      SDL_SetRenderTarget(c.r, cakes[i]);
      SDL_RenderSetLogicalSize(c.r, 160, 180);
      SDL_SetRenderDrawColor(c.r, 0, 0, 0, 0);
      SDL_RenderClear(c.r);
      drawCake(80, 155, flavor, designs[i]);
      SDL_SetRenderTarget(c.r, nullptr);
      SDL_RenderSetLogicalSize(c.r, 480, 272);
      SDL_SetTextureBlendMode(cakes[i], SDL_BLENDMODE_BLEND);
      cached[i] = designs[i];
    }
    cachedFlavor = flavor;
  }
  void cake(int i, int x, int base, float scale = 1) {
    if (!cakes[i])
      return;
    SDL_Rect dst{x - int(80 * scale), base - int(155 * scale), int(160 * scale),
                 int(180 * scale)};
    SDL_RenderCopy(c.r, cakes[i], nullptr, &dst);
  }
  void backdrop() {
    if (background)
      SDL_RenderCopy(c.r, background, nullptr, nullptr);
    else {
      c.rect(0, 0, 480, 272, blue);
      c.rect(0, 130, 480, 142, Paint(236, 170, 98));
    }
  }
  void heading(const Game &g) {
    box(7, 5, 466, 27, cream, 10);
    c.text(20, 10, "COOKING", red, 2);
    c.text(119, 16, flavorName(g.s.flavor), ink);
    for (int i = 0; i < 5; ++i) {
      int x = 355 + i * 22;
      c.disc(x, 18, 7, i <= g.s.stage ? color(i) : Paint(214, 219, 211));
      c.centered(x, 15, std::to_string(i + 1), i <= g.s.stage ? cream : ink);
    }
  }
  void footer(const std::string &main, const std::string &extra) {
    box(7, 235, 466, 32, cream, 8);
    c.text(17, 241, main, ink);
    c.text(17, 254, extra, Paint(46, 116, 149));
  }
  void tray(const Game &g, int x, int y, float scale = 1, bool oven = false) {
    int w = int(172 * scale), h = int(101 * scale);
    box(x - w / 2, y - h / 2, w, h, blue, std::max(2, int(9 * scale)));
    for (int i = 0; i < 4; ++i) {
      int xx = x + int((i % 2 ? 40 : -40) * scale),
          yy = y + int((i / 2 ? 22 : -22) * scale);
      int w1 = int(29 * scale), h1 = int(13 * scale);
      oval(xx, yy + 7, w1 + 1, h1 + 5, ink);
      oval(xx, yy + 4, w1, h1 + 4, color(i));
      oval(xx, yy, w1, h1, cream);
      bool filled = (g.s.filled & (1u << i)) ||
                    (g.s.pending == FillCup && g.s.target == i &&
                     g.s.anim > g.s.total / 2);
      if (filled) {
        float growth = oven ? float(g.s.bake) / g.bakeGoal() : 0;
        ball(xx, yy - int(growth * 9 * scale), int(25 * scale),
             int((10 + growth * 8) * scale),
             g.s.flavor == 1   ? Paint(169, 100, 69)
             : g.s.flavor == 2 ? Paint(255, 183, 164)
                               : Paint(255, 220, 139),
             g.s.flavor == 1 ? Paint(113, 66, 57) : Paint(223, 158, 91));
      }
      if (g.s.stage == Filling && g.s.selected == i) {
        c.line(xx - w1, yy - 22 * scale, xx + w1, yy - 22 * scale, yellow);
        c.disc(xx, yy - 26 * scale, 3, yellow);
      }
    }
  }
  void ingredients(const Game &g) {
    const auto &s = g.s;
    const int xs[] = {82, 238, 397, 78, 240, 402},
              ys[] = {88, 68, 88, 176, 190, 177};
    c.round(19, 55, 442, 171, 24, Paint(255, 233, 178, 45));
    int count = 0;
    for (int i = 0; i < 6; ++i)
      if (s.used & (1u << i))
        ++count;
    bowl(240, 148, s.flavor, count);
    for (int i = 0; i < 6; ++i) {
      if ((s.used & (1u << i)) || s.pending == i) {
        if (s.used & (1u << i)) {
          c.disc(xs[i], ys[i], 14, green);
          c.line(xs[i] - 7, ys[i], xs[i] - 2, ys[i] + 6, cream);
          c.line(xs[i] - 2, ys[i] + 6, xs[i] + 8, ys[i] - 7, cream);
        }
        continue;
      }
      int bob = (s.selected == i && s.pending == Idle)
                    ? int(std::sin(time * 5) * 3) - 3
                    : 0;
      if (s.selected == i && s.pending == Idle) {
        oval(xs[i], ys[i] + 29, 31, 8, yellow);
        hand(xs[i] + 29, ys[i] + 5 + bob);
      }
      itemSprite(xs[i], ys[i] + bob, i, s.flavor);
      box(xs[i] - 35, ys[i] + 28, 70, 13, cream, 4);
      c.centered(xs[i], ys[i] + 31, itemName(i, s.flavor), ink);
    }
    if (s.pending >= 0 && s.pending < 6) {
      float t = float(s.anim) / s.total, move = std::min(1.f, t * 3),
            ease = move * move * (3 - 2 * move);
      int id = s.pending, x = int(xs[id] * (1 - ease) + 278 * ease),
          y = int(ys[id] * (1 - ease) + 92 * ease);
      float tilt = std::min(1.f, std::max(0.f, (t - .25f) * 4)) * -55;
      if (id == 3 && t > .36f) {
        int spread = int((t - .36f) * 45);
        ball(x - 10 - spread, y, 11, 14, cream, Paint(221, 193, 156));
        ball(x + 10 + spread, y, 11, 14, cream, Paint(221, 193, 156));
        int drop = std::min(47, int((t - .36f) * 110));
        ball(258, 98 + drop, 7, 5, yellow, Paint(244, 166, 66));
      } else
        itemSprite(x, y, id, s.flavor, tilt);
      if (id != 3 && t > .36f && t < .91f) {
        Paint stream = id == 0 || id == 1 || id == 4 ? cream
                       : id == 2                     ? yellow
                       : s.flavor == 1               ? Paint(110, 65, 52)
                       : s.flavor == 2               ? red
                                                     : yellow;
        for (int p = 0; p < 8; ++p) {
          int h = (s.anim * 3 + p * 7) % 49;
          c.round(252 + (p % 3) * 3, 102 + h, 3, 6, 1, stream);
        }
      }
    }
    footer("< > OU CIMA/BAIXO: ESCOLHER   A COLOCAR NA TIGELA",
           "SEM PRESSA!   Y AJUDA   B/START PAUSA   SELECT MENU");
  }
  void mixing(const Game &g) {
    bowl(251, 158, g.s.flavor, 6, true);
    float a = time * 3.5f;
    int x = 251 + int(std::cos(a) * 39), y = 154 + int(std::sin(a) * 12);
    c.line(x, y, x + 19, y - 53, ink);
    c.line(x + 2, y, x + 21, y - 53, cream);
    box(x + 13, y - 62, 13, 30, red, 5);
    for (int i = 0; i < 3; ++i) {
      oval(x + i * 2, y - 4, 7 - i, 8, cream);
    }
    mascot(72, 183);
    box(22, 93, 106, 37, cream, 9);
    c.centered(75, 100, "GIRE O", ink);
    c.centered(75, 113, "DIRECIONAL!", ink);
    const char *arrows[] = {">", "V", "<", "CIMA"};
    for (int i = 0; i < 4; ++i) {
      float angle = i * 1.570796f;
      int xx = 251 + int(std::cos(angle) * 105),
          yy = 149 + int(std::sin(angle) * 61);
      c.disc(xx, yy, 12, i == g.s.lastDirection ? yellow : cream);
      c.centered(xx, yy - 3, arrows[i], ink);
    }
    box(377, 86, 77, 74, cream, 10);
    c.centered(415, 98, "MISTURA", ink);
    c.centered(415, 118,
               std::to_string(g.s.mixed) + "/" + std::to_string(g.mixGoal()),
               blue, 2);
    c.centered(415, 145, "BEM CREMOSA", ink);
    if (g.s.pending == FinishMix)
      for (int i = 0; i < 8; ++i) {
        float a1 = i * .7854f + time;
        sparkle(251 + std::cos(a1) * 80, 154 + std::sin(a1) * 43, 5, yellow);
      }
    footer("GIRE EM CIRCULO COM QUALQUER UM DOS DOIS MANCHES",
           "D-PAD TAMBEM FUNCIONA   Y AJUDA   B/START PAUSA");
  }
  void filling(const Game &g) {
    bowl(97, 152, g.s.flavor, 6, true);
    tray(g, 333, 154);
    if (g.s.pending == FillCup) {
      float t = float(g.s.anim) / g.s.total;
      int tx = 333 + (g.s.target % 2 ? 40 : -40),
          ty = 154 + (g.s.target / 2 ? 22 : -22);
      float f = std::min(1.f, t * 3);
      int x = int(115 * (1 - f) + tx * f),
          y = int(110 * (1 - f) + (ty - 42) * f);
      c.line(x, y, x + 30, y - 29, ink);
      c.line(x + 1, y, x + 31, y - 29, cream);
      ball(x, y, 17, 7, yellow, Paint(230, 153, 72));
      if (t > .35f && t < .9f)
        c.round(x - 3, y + 3, 6, std::max(1, ty - y - 3), 3,
                g.s.flavor == 1 ? Paint(142, 86, 62) : Paint(255, 215, 141));
    } else
      hand(333 + (g.s.selected % 2 ? 40 : -40) + 31,
           154 + (g.s.selected / 2 ? 22 : -22) - 9);
    footer("DIRECIONAL ESCOLHE A FORMINHA   A COLOCA A MASSA",
           "PREENCHA AS QUATRO!   Y AJUDA   B/START PAUSA");
  }
  void oven(const Game &g) {
    const auto &s = g.s;
    oval(245, 211, 141, 15, Paint(26, 45, 57, 65));
    box(112, 68, 255, 143, red, 15);
    c.round(120, 75, 239, 9, 5, Paint(255, 160, 130));
    box(127, 93, 188, 96, Paint(49, 90, 114), 9);
    c.round(133, 99, 176, 84, 5, Paint(24, 57, 76));
    c.round(140, 103, 160, 15, 5, Paint(135, 207, 222, 80));
    for (int x = 126; x < 312; x += 17)
      c.rect(x, 193, 10, 4, Paint(149, 39, 67));
    for (int yy : {109, 151}) {
      ball(339, yy, 13, 13, yellow, Paint(227, 145, 54));
      c.line(339, yy, 339 + (s.heat - 2) * 7, yy - 8, ink);
    }
    c.line(143, 88, 294, 88, ink);
    c.line(144, 86, 293, 86, cream);
    if (s.oven || s.pending == PutIn || s.pending == TakeOut) {
      float f = 1;
      if (s.pending == PutIn)
        f = std::min(1.f, float(s.anim) / s.total);
      if (s.pending == TakeOut)
        f = 1 - float(s.anim) / s.total;
      int x = int(77 * (1 - f) + 222 * f), y = int(179 * (1 - f) + 145 * f);
      tray(g, x, y, .9f - f * .2f, true);
    } else
      tray(g, 85, 180, .7f);
    if (s.oven && g.golden()) {
      c.disc(339, 183, 6, green);
      for (int i = 0; i < 4; ++i)
        sparkle(148 + i * 43, 124 + int(std::sin(time * 4 + i) * 10), 4,
                yellow);
    } else if (s.oven)
      c.disc(339, 183, 6, yellow);
    box(379, 79, 88, 126, cream, 9);
    c.centered(423, 89, "FORNO", ink);
    c.centered(423, 108, "FOGO " + std::to_string(s.heat), red);
    c.centered(423, 131, "<   >", blue, 2);
    c.round(391, 157, 62, 10, 4, Paint(211, 218, 205));
    c.round(392, 158, std::max(1, 60 * s.bake / g.bakeGoal()), 8, 3, green);
    c.centered(423, 183, g.golden() ? "DOURADINHO!" : "CRESCENDO", ink);
    footer(!s.oven      ? "A COLOCAR A FORMA NO FORNO"
           : g.golden() ? "PRONTO! A TIRA OS DOCES DO FORNO"
                        : "< > AJUSTA O FOGO   ESPERE A MASSA CRESCER",
           "NAO QUEIMA NEM ESTRAGA   Y AJUDA   B/START PAUSA");
  }
  const char *optionName(int tool, int value) {
    const char *icing[] = {"SEM COBERTURA", "BAUNILHA", "CHOCOLATE",
                           "ROSA BERRY",    "MENTA",    "LIMAO",
                           "CEU AZUL"};
    const char *sprinkles[] = {"SEM CONFEITO", "ARCO-IRIS", "GOTAS CACAU",
                               "ESTRELINHAS"};
    const char *fruit[] = {"SEM FRUTA", "MORANGO", "CEREJAS", "LARANJA",
                           "FOLHINHAS"};
    const char *faces[] = {"SEM CARINHA", "SORRISO", "PISCADINHA", "SONINHO"};
    const char *wrappers[] = {"CORAL", "AZUL",  "AMARELO",
                              "LILAS", "MENTA", "ARCO-IRIS"};
    return tool == 0   ? icing[value]
           : tool == 1 ? sprinkles[value]
           : tool == 2 ? fruit[value]
           : tool == 3 ? faces[value]
           : tool == 4 ? wrappers[value]
                       : "GUARDAR NO ALBUM";
  }
  void decorating(const Game &g) {
    const auto &s = g.s;
    box(11, 57, 145, 157, cream, 12);
    const char *tools[] = {"COBERTURA", "CONFEITOS", "FRUTINHAS",
                           "CARINHA",   "FORMINHA",  "PRONTO!"};
    c.centered(83, 70, tools[s.tool], ink);
    c.centered(83, 89, "CIMA", blue);
    c.centered(83, 176, "BAIXO", blue);
    c.centered(83, 193, "A APLICAR", ink);
    if (s.tool == 5) {
      star(83, 127, 24, yellow);
      c.centered(83, 155, "GUARDAR DOCES", ink);
    } else {
      c.disc(83, 127, 26, color(s.option));
      shine(73, 115, 8, 4);
      if (s.tool == 0) {
        for (int i = 0; i < 3; ++i)
          oval(83, 138 - i * 10, 23 - i * 6, 6,
               s.option == 2   ? Paint(128, 76, 60)
               : s.option == 3 ? Paint(250, 161, 199)
                               : cream);
      } else if (s.tool == 1)
        for (int i = 0; i < 12; ++i)
          c.line(66 + (i * 13) % 33, 112 + (i * 17) % 29, 69 + (i * 13) % 33,
                 115 + (i * 17) % 29, cream);
      else if (s.tool == 2)
        berry(83, 127, 16);
      else if (s.tool == 3) {
        c.disc(74, 124, 3, ink);
        c.disc(92, 124, 3, ink);
        c.line(76, 136, 90, 136, ink);
      } else {
        box(66, 112, 35, 31, color(s.option), 5);
        for (int i = 0; i < 4; ++i)
          c.line(72 + i * 7, 116, 72 + i * 7, 138, cream);
      }
      c.centered(83, 158, optionName(s.tool, s.option), ink);
    }
    int bob = int(std::sin(time * 3) * 2);
    cake(s.cake, 260, 211 + bob, 1.03f);
    box(355, 58, 114, 153, cream, 10);
    c.centered(412, 68, "SEUS DOCES", ink);
    for (int i = 0; i < 4; ++i) {
      int x = 384 + (i % 2) * 55, y = 129 + (i / 2) * 66;
      if (i == s.cake)
        c.round(x - 26, y - 48, 52, 59, 7, Paint(255, 226, 126));
      cake(i, x, y, .36f);
      c.centered(x, y + 5, std::to_string(i + 1), ink);
    }
    for (int i = 0; i < 6; ++i) {
      int x = 7 + i * 78;
      box(x, 220, 75, 22, i == s.tool ? yellow : cream, 6);
      c.centered(x + 37, 228, tools[i], ink);
    }
    c.round(7, 248, 466, 20, 5, ink);
    c.text(15, 254,
           "< > FERRAMENTA  CIMA/BAIXO OPCAO  A APLICA  Y DOCE  B DESFAZ",
           cream);
  }
  void draw(const Game &g) {
    sync(g.s.cakes, g.s.flavor);
    backdrop();
    heading(g);
    if (g.s.stage == Ingredients)
      ingredients(g);
    else if (g.s.stage == Mixing)
      mixing(g);
    else if (g.s.stage == Filling)
      filling(g);
    else if (g.s.stage == Baking)
      oven(g);
    else if (g.s.stage == Decorating)
      decorating(g);
    if (feedback > 0) {
      box(168, 37, 145, 17, good ? yellow : cream, 5);
      c.centered(240, 42, good ? "QUE DELICIA!" : "TENTE OUTRO JEITO", ink);
    }
  }
  void photo(const std::array<Design, 4> &designs, int flavor, int serial,
             bool gallery = false, int count = 0) {
    sync(designs, flavor);
    backdrop();
    for (int i = 0; i < 20; ++i) {
      int x = 12 + i * 24, y = 33 + int(time * 25 + i * 29) % 190;
      c.rect(x, y, 3, 5, color(i));
    }
    box(70, 9, 340, 35, cream, 12);
    c.centered(240, 17, gallery ? "ALBUM DE DELICIAS" : "FEITO POR VOCE!", red,
               2);
    box(137, 54, 206, 22, yellow, 7);
    c.centered(240, 61, flavorName(flavor), ink);
    for (int i = 0; i < 4; ++i)
      cake(i, 68 + i * 114, 205, .78f);
    footer(gallery ? "< > ESCOLHER CRIACAO   A/B VOLTAR"
                   : "A FAZER OUTRO SABOR   B MENU",
           std::string("CRIACAO ") + std::to_string(serial) +
               (gallery ? "   " + std::to_string(count) + " CONJUNTOS NO ALBUM"
                        : "   SALVA NO SEU ALBUM!"));
  }
  void panel(const std::string &title, const std::vector<std::string> &rows,
             int choice = -1) {
    backdrop();
    mascot(53, 203);
    mascot(430, 201);
    box(96, 40, 288, 214, cream, 15);
    c.centered(240, 52, title, red, title.size() < 17 ? 2 : 1);
    for (size_t i = 0; i < rows.size(); ++i) {
      int y = 89 + int(i) * 23;
      if (int(i) == choice)
        c.round(106, y - 5, 268, 21, 6, yellow);
      c.centered(240, y, rows[i], ink);
    }
    c.centered(240, 233, "A CONFIRMA   B VOLTA", blue);
  }
};
} // namespace cooking
