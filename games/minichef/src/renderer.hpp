#pragma once
#include "game_logic.hpp"
#include "pixel.hpp"
#include <vector>
namespace chef {
struct Renderer {
  ui::Canvas c;
  SDL_Texture *room = nullptr, *icons = nullptr;
  float time = 0, feedback = 0, pulse = 0;
  bool happy = true;
  const ui::Paint cocoa{91, 63, 56}, cream{255, 246, 218}, rose{223, 126, 137},
      green{105, 154, 101}, mint{191, 218, 179}, wood{193, 131, 86};
  explicit Renderer(SDL_Renderer *r) : c(r) {
    room = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888,
                             SDL_TEXTUREACCESS_TARGET, 480, 272);
    if (room) {
      SDL_SetRenderTarget(r, room);
      kitchen();
      SDL_SetRenderTarget(r, nullptr);
    }
    icons = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888,
                              SDL_TEXTUREACCESS_TARGET, 480, 272);
    if (icons) {
      SDL_SetRenderTarget(r, icons);
      SDL_SetRenderDrawColor(r, 0, 0, 0, 0);
      SDL_RenderClear(r);
      for (int i = 0; i < 8; ++i)
        food(i * 40 + 20, 14, i, 1, false);
      SDL_SetRenderTarget(r, nullptr);
      SDL_SetTextureBlendMode(icons, SDL_BLENDMODE_BLEND);
    }
  }
  ~Renderer() {
    if (room)
      SDL_DestroyTexture(room);
    if (icons)
      SDL_DestroyTexture(icons);
  }
  void ellipse(int x, int y, int w, int h, ui::Paint p) {
    for (int i = -h; i <= h; ++i) {
      int dx = int(w * std::sqrt(std::max(0.f, 1.f - float(i * i) / (h * h))));
      c.line(x - dx, y + i, x + dx, y + i, p);
    }
  }
  void leaf(int x, int y, int k = 1) {
    ellipse(x - 3 * k, y, 4 * k, 2 * k, green);
    ellipse(x + 3 * k, y - 3 * k, 3 * k, 2 * k, green);
    c.line(x, y + 3 * k, x + 2 * k, y - 5 * k, ui::Paint(70, 119, 76));
  }
  void flower(int x, int y, ui::Paint color) {
    c.disc(x - 3, y, 3, color);
    c.disc(x + 3, y, 3, color);
    c.disc(x, y - 3, 3, color);
    c.disc(x, y + 3, 3, color);
    c.disc(x, y, 2, ui::Paint(248, 202, 91));
  }
  void heart(int x, int y, int size, ui::Paint p) {
    c.disc(x - size / 2, y, size / 2, p);
    c.disc(x + size / 2, y, size / 2, p);
    for (int i = 0; i < size; ++i)
      c.line(x - size + i, y + i, x + size - i, y + i, p);
  }
  void star(int x, int y, bool full) {
    ui::Paint p = full ? ui::Paint(243, 189, 73) : ui::Paint(211, 197, 168);
    c.disc(x, y, 4, p);
    c.line(x, y - 7, x, y + 6, p);
    c.line(x - 6, y - 3, x + 6, y + 4, p);
    c.line(x + 6, y - 3, x - 6, y + 4, p);
    c.rect(x - 2, y - 2, 4, 4, p);
  }
  void food(int x, int y, int id, int k = 1, bool face = true) {
    using ui::Paint;
    ellipse(x, y + 8 * k, 10 * k, 3 * k, Paint(171, 135, 100));
    if (id == 0) {
      for (int i = 0; i < 16 * k; ++i) {
        int w = (16 * k - i) / 3 + 1;
        c.line(x - w, y - 6 * k + i, x + w, y - 6 * k + i,
               Paint(237, 145 + i / k * 2, 66));
      }
      leaf(x, y - 10 * k, k);
      c.line(x + 1, y - 2 * k, x + 4 * k, y - 2 * k, Paint(211, 110, 54));
    } else if (id == 1 || id == 6) {
      ellipse(x, y, 9 * k, id == 1 ? 8 * k : 9 * k,
              id == 1 ? Paint(232, 104, 92) : Paint(222, 94, 127));
      leaf(x, y - 8 * k, k);
      ellipse(x - 4 * k, y - 3 * k, 2 * k, k, Paint(255, 175, 155));
      if (id == 6)
        for (int i = 0; i < 5; ++i)
          c.rect(x + (i % 3 - 1) * 4 * k, y + (i / 3) * 4 * k, k, k, cream);
    } else if (id == 2) {
      ellipse(x, y, 10 * k, 7 * k, Paint(225, 187, 119));
      c.disc(x - 5 * k, y - 2 * k, k, wood);
      c.disc(x + 6 * k, y + 2 * k, k, wood);
    } else if (id == 3) {
      for (int i = 0; i < 17 * k; ++i)
        c.disc(x - 8 * k + i,
               y + int(std::sin(float(i) / (16 * k) * 3.14159f) * 5 * k), 3 * k,
               Paint(248, 208, 105));
      c.disc(x - 8 * k, y, 2 * k, wood);
      c.disc(x + 8 * k, y, 2 * k, wood);
    } else if (id == 4) {
      c.round(x - 8 * k, y - 10 * k, 16 * k, 19 * k, 3 * k,
              id == 3 ? Paint(244, 246, 222) : Paint(241, 214, 160));
      c.rect(x - 8 * k, y - 6 * k, 16 * k, 3 * k,
             id == 3 ? Paint(148, 196, 208) : Paint(198, 152, 112));
      c.round(x - 4 * k, y, 8 * k, 5 * k, k, cream);
    } else if (id == 5) {
      ellipse(x, y, 7 * k, 10 * k, Paint(255, 243, 203));
      ellipse(x - 3 * k, y - 4 * k, 2 * k, 3 * k, ui::paper);
    } else {
      c.round(x - 7 * k, y, 14 * k, 10 * k, 2 * k, Paint(211, 136, 102));
      leaf(x, y - 7 * k, k);
      leaf(x - 5 * k, y - 4 * k, k);
    }
    if (face && id != 0 && id != 7) {
      c.rect(x - 3 * k, y, 1 * k, 2 * k, cocoa);
      c.rect(x + 3 * k, y, 1 * k, 2 * k, cocoa);
      c.line(x, y + 3 * k, x + k, y + 3 * k, cocoa);
    }
  }
  void bunny(int x, int y, bool chefHat, bool smile = true, int variant = 0) {
    using ui::Paint;
    Paint fur = variant ? Paint(211, 170, 132) : Paint(255, 239, 204);
    ellipse(x, y + 35, 23, 6, Paint(164, 128, 103));
    c.round(x - 16, y + 5, 32, 30, 11, variant ? Paint(180, 194, 151) : rose);
    c.round(x - 10, y + 10, 20, 22, 5, cream);
    c.line(x - 9, y + 14, x + 9, y + 14, mint);
    c.disc(x, y + 25, 3, rose);
    ellipse(x - 19, y + 15, 6, 8, fur);
    ellipse(x + 19, y + 15, 6, 8, fur);
    ellipse(x - 9, y - 29, 6, 17, fur);
    ellipse(x + 9, y - 29, 6, 17, fur);
    ellipse(x - 9, y - 30, 2, 10, Paint(240, 177, 161));
    ellipse(x + 9, y - 30, 2, 10, Paint(240, 177, 161));
    ellipse(x, y - 6, 22, 20, fur);
    ellipse(x - 17, y - 1, 5, 3, Paint(245, 176, 155));
    ellipse(x + 17, y - 1, 5, 3, Paint(245, 176, 155));
    if (smile) {
      c.line(x - 10, y - 7, x - 7, y - 9, cocoa);
      c.line(x - 7, y - 9, x - 4, y - 7, cocoa);
      c.line(x + 4, y - 7, x + 7, y - 9, cocoa);
      c.line(x + 7, y - 9, x + 10, y - 7, cocoa);
    } else {
      c.round(x - 9, y - 10, 3, 5, 1, cocoa);
      c.round(x + 6, y - 10, 3, 5, 1, cocoa);
    }
    c.disc(x, y - 2, 2, rose);
    c.line(x, y, x - 3, y + 3, cocoa);
    c.line(x, y, x + 3, y + 3, cocoa);
    if (chefHat) {
      c.round(x - 18, y - 29, 36, 10, 3, ui::paper);
      c.disc(x - 13, y - 32, 9, ui::paper);
      c.disc(x, y - 37, 11, ui::paper);
      c.disc(x + 13, y - 32, 9, ui::paper);
      c.line(x - 16, y - 22, x + 16, y - 22, Paint(224, 217, 194));
    }
  }
  void kitchen() {
    using ui::Paint;
    c.rect(0, 0, 480, 272, cream);
    c.rect(0, 0, 480, 168, Paint(246, 224, 192));
    for (int x = 0; x < 480; x += 24)
      c.rect(x, 30, 1, 107, Paint(235, 207, 179));
    c.rect(0, 133, 480, 4, wood);
    for (int y = 137; y < 207; y += 18)
      for (int x = 0; x < 480; x += 24) {
        c.rect(x, y, 23, 17,
               ((x / 24 + y / 18) % 2) ? Paint(228, 199, 154)
                                       : Paint(242, 218, 179));
        c.line(x + 4, y + 8, x + 16, y + 8, Paint(216, 182, 140));
      }
    // Window and a tiny farm beyond it.
    c.round(24, 42, 137, 79, 9, wood);
    c.round(28, 45, 129, 70, 6, Paint(166, 211, 215));
    SDL_Rect windowClip{28, 45, 129, 70};
    SDL_RenderSetClipRect(c.r, &windowClip);
    c.disc(132, 58, 9, Paint(255, 228, 139));
    ellipse(67, 104, 55, 23, Paint(155, 193, 138));
    ellipse(140, 112, 62, 28, Paint(130, 174, 119));
    for (int row = 0; row < 2; ++row)
      for (int i = 0; i < 6; ++i) {
        int x = 40 + i * 18, y = 101 + row * 9;
        c.round(x - 6, y, 13, 5, 2, Paint(150, 120, 77));
        leaf(x, y - 1);
      }
    SDL_RenderSetClipRect(c.r, nullptr);
    c.rect(89, 44, 5, 72, cream);
    c.rect(28, 77, 129, 4, cream);
    c.round(19, 112, 147, 8, 2, Paint(232, 172, 120));
    for (int x : {22, 149}) {
      c.round(x, 43, 12, 66, 4, rose);
      c.rect(x, 70, 12, 5, Paint(248, 198, 173));
      for (int y = 46; y < 103; y += 9)
        c.line(x + 4, y, x + 4, y + 5, Paint(238, 158, 161));
    }
    // Crockery shelf, jars, tea towel, hanging utensils.
    c.round(187, 48, 128, 5, 2, wood);
    for (int i = 0; i < 4; ++i) {
      c.round(195 + i * 27, 30, 17, 18, 3, Paint(248, 237, 199));
      c.rect(196 + i * 27, 39, 15, 8, i % 2 ? rose : mint);
      c.round(194 + i * 27, 28, 19, 4, 1, wood);
      c.rect(199 + i * 27, 34, 3, 3, ui::paper);
    }
    c.line(197, 66, 303, 66, cocoa);
    for (int x : {202, 232, 262, 292}) {
      c.line(x, 65, x, 84, wood);
      if (x == 232 || x == 292) {
        c.round(x - 5, 82, 10, 12, 4, Paint(169, 182, 159));
        c.line(x, 83, x, 91, cream);
      } else {
        ellipse(x, 88, 5, 7, Paint(191, 148, 108));
        c.disc(x, 88, 2, cream);
      }
    }
    c.round(329, 42, 127, 82, 7, Paint(179, 199, 153));
    c.round(333, 46, 119, 74, 5, cream);
    c.centered(392, 53, "COZINHA DA HORTA", cocoa);
    for (int y = 70; y < 112; y += 12)
      c.line(345, y, 436, y, Paint(222, 211, 182));
    heart(392, 91, 8, rose);
    leaf(350, 94, 2);
    flower(434, 94, Paint(233, 177, 133));
    // Back counter and cupboards with bevelled fronts.
    c.round(18, 132, 444, 49, 5, Paint(164, 113, 83));
    c.rect(22, 134, 436, 6, Paint(250, 218, 169));
    for (int x = 25; x < 455; x += 72) {
      c.round(x, 145, 63, 32, 3, Paint(203, 153, 105));
      c.rect(x + 3, 148, 57, 2, Paint(231, 181, 129));
      c.round(x + 27, 153, 11, 3, 1, cocoa);
    }
    // Sink, hob, herb pot, plates.
    c.round(198, 125, 62, 13, 4, Paint(132, 163, 161));
    c.round(204, 128, 50, 7, 3, Paint(184, 211, 201));
    c.line(233, 125, 233, 112, Paint(119, 151, 145));
    c.round(232, 108, 13, 6, 3, Paint(119, 151, 145));
    c.rect(298, 127, 55, 10, Paint(107, 107, 95));
    for (int x : {309, 338})
      ellipse(x, 131, 8, 3, Paint(64, 76, 72));
    food(430, 127, 7);
    c.round(373, 128, 25, 6, 3, ui::paper);
    c.line(375, 131, 396, 131, mint);
    // Kitchen island, bunting and wood legs.
    c.round(60, 167, 304, 30, 8, Paint(154, 113, 84));
    c.round(62, 165, 300, 25, 7, Paint(239, 202, 150));
    c.line(69, 168, 354, 168, Paint(255, 232, 181));
    c.rect(75, 191, 8, 15, wood);
    c.rect(341, 191, 8, 15, wood);
    c.line(83, 190, 339, 190, cream);
    for (int i = 0; i < 9; ++i) {
      int x = 88 + i * 29;
      for (int h = 0; h < 9; ++h)
        c.line(x + h, 190 + h, x + 16 - h, 190 + h, i % 2 ? mint : rose);
    }
    c.rect(0, 207, 480, 65, cream);
    c.line(0, 207, 480, 207, Paint(207, 167, 125));
  }
  void dish(int x, int y, int level, int k = 1) {
    using ui::Paint;
    ellipse(x, y + 5 * k, 22 * k, 6 * k, Paint(169, 133, 103));
    ellipse(x, y, 24 * k, 9 * k, Paint(255, 254, 232));
    ellipse(x, y, 20 * k, 6 * k, Paint(198, 220, 197));
    if (level == 0) {
      c.round(x - 18 * k, y - 7 * k, 36 * k, 14 * k, 7 * k,
              Paint(216, 147, 102));
      ellipse(x, y - 6 * k, 18 * k, 7 * k, Paint(255, 214, 124));
      for (int i = 0; i < 5; ++i)
        c.round(x + (i % 3 - 1) * 8 * k, y - 7 * k + (i / 3) * 4 * k, 5 * k,
                3 * k, k, i % 2 ? green : rose);
    } else if (level == 1) {
      for (int i = 0; i < 3; ++i) {
        ellipse(x, y - i * 4 * k, 17 * k, 5 * k, Paint(192, 124, 70));
        ellipse(x, y - i * 4 * k - 2 * k, 17 * k, 4 * k, Paint(247, 202, 126));
      }
      c.round(x - 4 * k, y - 12 * k, 9 * k, 5 * k, k, Paint(255, 232, 150));
    } else {
      c.round(x - 18 * k, y - 8 * k, 36 * k, 13 * k, 5 * k,
              Paint(214, 151, 100));
      ellipse(x, y - 9 * k, 18 * k, 6 * k, Paint(255, 234, 199));
      for (int i = 0; i < 5; ++i)
        c.disc(x + (i % 3 - 1) * 9 * k, y - 12 * k + (i / 3) * 5 * k, 4 * k,
               rose);
    }
    leaf(x + 8 * k, y - 10 * k, k);
  }
  void base() {
    if (room)
      SDL_RenderCopy(c.r, room, nullptr, nullptr);
    else
      kitchen();
  }
  void feedbackAction(bool success) {
    happy = success;
    feedback = 1.1f;
    pulse = .35f;
  }
  void update(float dt) {
    time += dt;
    feedback = std::max(0.f, feedback - dt);
    pulse = std::max(0.f, pulse - dt);
  }
  void header(const Game &g) {
    c.round(7, 5, 466, 25, 8, cream);
    leaf(22, 19);
    c.text(36, 11, "MINICHEF", cocoa, 2);
    c.text(182, 15, recipe(g.s.level).name, cocoa);
    for (int i = 0; i < 3; ++i)
      star(427 + i * 17, 17, i < g.stars[g.s.level]);
  }
  void draw(const Game &g) {
    using ui::Paint;
    base();
    header(g);
    const State &s = g.s;
    int bob = int(std::sin(time * 3) * 1.5f);
    bunny(175, 137 + bob, true, feedback > 0 ? happy : true);
    bunny(405, 169 + bob, false, true, 1);
    // Readable recipe note within the kitchen noticeboard.
    c.round(332, 46, 121, 76, 5, cream);
    c.text(344, 51, "NA RECEITA", green);
    int row = 0;
    for (int i = 0; i < 8; ++i)
      if (recipe(s.level).ingredients & (1u << i)) {
        c.text(344, 65 + row * 15, ingredient(i), cocoa);
        if (s.basket & (1u << i))
          c.text(435, 65 + row * 15, "+", green);
        ++row;
      }
    // Station activity, all artwork built from original primitives.
    if (s.step == Pantry) {
      c.round(73, 151, 61, 22, 4, wood);
      food(88, 156, 0);
      food(114, 156, 1);
      c.round(232, 151, 64, 22, 4, wood);
      food(251, 155, 5);
      food(278, 155, 6);
    } else if (s.step == Chop) {
      c.round(212, 158, 83, 20, 5, wood);
      c.round(215, 160, 77, 15, 4, Paint(255, 218, 161));
      food(242, 159, s.level == 0 ? 0 : s.level == 1 ? 3 : 6, 1, false);
      int lift = pulse > 0 ? 0 : 8;
      c.round(263, 147 - lift, 21, 5, 2, cocoa);
      c.round(258, 151 - lift, 16, 8, 2, Paint(206, 220, 199));
      for (int i = 0; i < s.cuts; ++i)
        c.rect(222 + (i % 6) * 8, 171 + (i / 6) * 3, 5, 2, rose);
    } else if (s.step == Mix) {
      ellipse(254, 169, 30, 7, Paint(178, 128, 111));
      c.round(224, 151, 60, 20, 9, rose);
      ellipse(254, 150, 30, 9, Paint(255, 201, 162));
      for (int i = 0; i < 5; ++i) {
        float a = time * 2 + i * 1.25f;
        c.disc(254 + std::cos(a) * 18, 150 + std::sin(a) * 4, 2, cream);
      }
      int xx = 254 + int(std::cos(time * 3) * 17);
      c.line(xx, 150, xx + 10, 130, wood);
      c.line(xx + 1, 150, xx + 11, 130, wood);
    } else if (s.step == Cook) {
      for (int i = 0; i < s.heat; ++i)
        c.disc(237 + i * 10, 173, 4, Paint(242, 165, 78));
      c.round(226, 150, 59, 23, 7, Paint(125, 162, 139));
      ellipse(255, 149, 29, 8, Paint(218, 230, 188));
      c.round(215, 154, 12, 6, 2, wood);
      c.round(285, 154, 12, 6, 2, wood);
      for (int i = 0; i < 4; ++i) {
        int h = int(time * 18 + i * 10) % 27;
        ellipse(239 + i * 11, 139 - h, 2 + h / 10, 2, Paint(255, 245, 220));
      }
      if (g.warm())
        for (int i = 0; i < 5; ++i)
          c.disc(236 + i * 9, 148 - int(std::sin(time * 4 + i) * 2), 2, cream);
    } else {
      dish(255, 161, s.level, 2);
      for (int i = 0; i < s.garnish; ++i) {
        int xx = 231 + i * 22;
        if (s.decorations[i] == 0)
          leaf(xx, 146);
        else if (s.decorations[i] == 1)
          flower(xx, 145, rose);
        else
          c.disc(xx, 145, 3, cream);
      }
    }
    // Five milestones make the progression obvious.
    const char *steps[] = {"COLHER", "CORTAR", "MEXER", "FOGAO", "DECORAR"};
    for (int i = 0; i < 5; ++i) {
      int x = 17 + i * 94;
      c.round(x, 190, 89, 15, 5, i == s.step ? green : cream);
      c.centered(x + 44, 194, steps[i], i == s.step ? cream : cocoa);
      if (i < s.step)
        c.disc(x + 5, 197, 2, green);
    }
    if (s.step == Pantry) {
      for (int i = 0; i < 8; ++i) {
        int x = 15 + (i % 4) * 72, y = 211 + (i / 4) * 23;
        bool chosen = s.basket & (1u << i);
        c.round(x, y, 69, 21, 5,
                i == s.selected ? green : ui::Paint(232, 218, 186));
        c.round(x + 1, y + 1, 67, 19, 4, chosen ? mint : cream);
        if (icons) {
          SDL_Rect src{i * 40, 0, 40, 32}, dst{x + 1, y + 2, 20, 16};
          SDL_RenderCopy(c.r, icons, &src, &dst);
        }
        c.text(x + 23, y + 7, i == 7 ? "ERVA" : ingredient(i), cocoa);
        if (chosen)
          c.rect(x + 24, y + 17, 38, 2, green);
        if (i == s.selected)
          c.line(x + 4, y + 19, x + 64, y + 19, green);
      }
      c.text(320, 215, ingredient(s.selected), cocoa);
      c.text(320, 231, "A PEGAR", green);
      c.text(320, 246, "SIGA A RECEITA", cocoa);
    } else if (s.step == Chop) {
      c.text(18, 214, "A CORTE NO VERDE", cocoa);
      c.round(20, 233, 290, 16, 5, Paint(221, 202, 169));
      int tol = 20 - s.level * 3;
      c.round(20 + (50 - tol) * 29 / 10, 235, tol * 58 / 10, 12, 3, mint);
      int x = 20 + g.marker() * 29 / 10;
      c.round(x - 2, 230, 5, 22, 2, cocoa);
      c.text(335, 220, "CORTES", green);
      c.text(351, 236,
             std::to_string(s.cuts) + "/" +
                 std::to_string(recipe(s.level).cuts),
             cocoa, 2);
    } else if (s.step == Mix) {
      const char *arrows[] = {"<", ">", "V", "CIMA"};
      c.text(18, 215, "SIGA AS SETAS PARA MISTURAR", cocoa);
      for (int i = 0; i < 4; ++i) {
        int dir = (s.turns + i + s.level) % 4;
        const int pat[] = {3, 1, 2, 0};
        c.round(24 + i * 58, 231, 50, 23, 7, i == 0 ? green : mint);
        c.centered(49 + i * 58, 238, arrows[pat[dir]], i == 0 ? cream : cocoa);
      }
      c.text(304, 218, "VOLTAS", green);
      c.text(306, 236,
             std::to_string(s.turns) + "/" +
                 std::to_string(recipe(s.level).turns),
             cocoa, 2);
    } else if (s.step == Cook) {
      c.text(18, 214, "< > FOGO   A MEXER", cocoa);
      c.round(18, 230, 220, 12, 4, Paint(227, 197, 160));
      c.rect(18 + 420 * 220 / 900, 232, 200 * 220 / 900, 8, mint);
      c.round(18 + s.temperature * 220 / 900 - 2, 228, 4, 16, 2, cocoa);
      c.text(18, 247, "FOGO " + std::to_string(s.heat) + "/4", green);
      c.text(126, 247, "MEXIDAS " + std::to_string(s.stirs) + "/4", cocoa);
      c.text(263, 214, g.warm() ? "PONTO CERTO!" : "AJUSTE ATE O VERDE",
             g.warm() ? green : cocoa);
      c.round(265, 233, 192, 13, 5, Paint(224, 210, 179));
      c.round(266, 234,
              std::min(190, 190 * s.goodTicks / recipe(s.level).cooking), 11, 4,
              green);
      c.text(265, 249, "COZINHANDO COM CARINHO", cocoa);
    } else if (s.step == Plate) {
      const char *names[] = {"FOLHINHA", "FLORZINHA", "CREMINHO"};
      c.text(18, 215, "< > ESCOLHER   A DECORAR", cocoa);
      for (int i = 0; i < 3; ++i) {
        int x = 20 + i * 105;
        c.round(x, 231, 100, 22, 6, i == s.selected ? mint : cream);
        c.centered(x + 50, 239, names[i], cocoa);
      }
      c.text(360, 233, std::to_string(s.garnish) + "/3", green, 2);
    }
    if (feedback > 0) {
      c.round(197, 99, 117, 20, 6, cream);
      c.centered(255, 105, happy ? "QUE CAPRICHO!" : "TENTE DE NOVO",
                 happy ? green : rose);
      if (happy)
        heart(199, 93 - int((1.1f - feedback) * 14), 4, rose);
    }
    c.rect(0, 260, 480, 12, green);
    c.text(14, 263, "DIRECIONAIS: DOIS MANCHES", cream);
    c.text(258, 263, "Y AJUDA   START PAUSA   B VOLTA", cream);
  }
  void panel(const Game &g, const std::string &title,
             const std::vector<std::string> &rows, int choice = -1) {
    base();
    bunny(73, 151, true, true);
    bunny(410, 160, false, true, 1);
    dish(76, 186, g.s.level);
    flower(429, 54, rose);
    flower(448, 68, cream);
    c.round(116, 36, 251, 213, 13, ui::Paint(167, 118, 85));
    c.round(120, 39, 243, 205, 11, cream);
    leaf(142, 57);
    leaf(342, 57);
    c.centered(241, 50, title, cocoa, title.size() < 15 ? 2 : 1);
    for (size_t i = 0; i < rows.size(); ++i) {
      int y = 85 + int(i) * 23;
      if (int(i) == choice)
        c.round(129, y - 4, 225, 21, 6, mint);
      c.centered(241, y, rows[i], cocoa);
    }
    c.centered(241, 230, "A CONFIRMA   B VOLTA", green);
  }
  void served(const Game &g) {
    base();
    header(g);
    bunny(102, 142, true, true);
    bunny(385, 147, false, true, 1);
    dish(241, 136, g.s.level, 3);
    for (int i = 0; i < 3; ++i) {
      int x = 216 + i * 25;
      if (g.s.decorations[i] == 0)
        leaf(x, 104, 2);
      else if (g.s.decorations[i] == 1)
        flower(x, 104, rose);
      else
        c.disc(x, 104, 4, cream);
    }
    for (int i = 0; i < 3; ++i)
      star(218 + i * 23, 64, i < g.rating());
    c.round(99, 201, 282, 54, 9, cream);
    c.centered(240, 210, "FEITO COM CARINHO!", cocoa, 2);
    c.centered(240, 236,
               g.s.level < 2 ? "A PROXIMA RECEITA   B REFAZER"
                             : "A MENU   B REFAZER",
               green);
    for (int i = 0; i < 12; ++i) {
      int x = 18 + i * 39, y = 40 + int(time * 23 + i * 17) % 140;
      if (i % 3 == 0)
        heart(x, y, 3, rose);
      else
        c.rect(x, y, 3, 5, i % 2 ? mint : wood);
    }
  }
};
} // namespace chef
