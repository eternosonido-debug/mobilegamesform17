#pragma once
#include "boca_ground.hpp"
#include "boca_sprites.hpp"
#include "juice_view.hpp"
namespace fmg {
namespace boca_art {
using juice_art::Canvas;
using juice_art::Paint;
inline void oval(Canvas &p, float x, float y, float rx, float ry, Paint color) {
  for (int row = -int(ry); row <= int(ry); ++row) {
    float half = rx * std::sqrt(std::max(0.f, 1 - row * row / (ry * ry)));
    p.line(x - half, y + row, x + half, y + row, color);
  }
}
inline Paint holeColor(int actor) {
  const Paint colors[] = {Paint(48, 193, 218), Paint(235, 104, 100), Paint(156, 115, 224),
                          Paint(240, 191, 70)};
  return colors[actor];
}
inline void face(Canvas &p, std::initializer_list<SDL_FPoint> corners, Paint paint) {
  std::vector<SDL_FPoint> v(corners);
  float minY = v[0].y, maxY = minY;
  for (auto point : v) {
    minY = std::min(minY, point.y);
    maxY = std::max(maxY, point.y);
  }
  for (int row = std::max(0, int(std::floor(minY))); row <= std::min(271, int(std::ceil(maxY)));
       ++row) {
    float scan = row + .5f, left = 1e9, right = -1e9;
    for (size_t i = 0; i < v.size(); ++i) {
      auto a = v[i], b = v[(i + 1) % v.size()];
      if ((a.y <= scan && b.y > scan) || (b.y <= scan && a.y > scan)) {
        float x = a.x + (scan - a.y) * (b.x - a.x) / (b.y - a.y);
        left = std::min(left, x);
        right = std::max(right, x);
      }
    }
    if (left <= right)
      p.line(std::floor(left), float(row), std::ceil(right), float(row), paint);
  }
}
inline void block(Canvas &p, float x, float y, float w, float depth, float height, Paint front,
                  Paint side, Paint roof) {
  float left = x - w * .5f, right = x + w * .5f, nearY = y + depth * .5f, farY = y - depth * .5f,
        skew = depth * .4f;
  float shadow = height * .35f;
  face(p,
       {{left, nearY},
        {right + shadow, nearY + shadow * .45f},
        {right + skew + shadow, farY + shadow * .45f},
        {left + skew, farY}},
       Paint(74, 95, 78, 90));
  face(p, {{left, nearY}, {right, nearY}, {right, nearY - height}, {left, nearY - height}}, front);
  face(p,
       {{right, nearY},
        {right + skew, farY},
        {right + skew, farY - height},
        {right, nearY - height}},
       side);
  face(p,
       {{left, nearY - height},
        {right, nearY - height},
        {right + skew, farY - height},
        {left + skew, farY - height}},
       roof);
}
inline void object(Canvas &p, int id, float x, float y, float size, bool edible, float scale = 1,
                   int kind = -1, bool flip = false) {
  const Paint dark(64, 77, 70), wood(168, 105, 63);
  float r = std::max(1.f, size * scale);
  if (kind >= 2 && boca_sprites::available(p.r, id, kind)) {
    if (edible)
      oval(p, x, y + r * .7f, r + 3, 2, Paint(255, 228, 147, 150));
    boca_sprites::draw(p.r, id, kind, x, y, r, flip);
    return;
  }
  oval(p, x + 2, y + r * .6f, r + 2, r * .38f, Paint(87, 110, 91, 100));
  if (edible)
    oval(p, x, y + r * .7f, r + 3, 2, Paint(255, 228, 147));
  int type = id % 3;
  int category = kind < 0 ? (size < 6 ? 0 : size < 9 ? 1 : 2) : kind;
  if (kind == 0)
    type = 1;
  if (kind == 1)
    type = 1;
  if (kind == 2)
    type = 2;
  if (kind >= 2) {
    if (kind == 2) {
      const Paint paintwork[] = {Paint(198, 99, 85),   Paint(77, 139, 176),  Paint(232, 185, 79),
                                 Paint(111, 166, 126), Paint(229, 232, 218), Paint(150, 121, 177)};
      int model = id % 6;
      Paint body = paintwork[model];
      float cabin = model == 0 ? .75f : model == 3 ? 1.25f : .95f;
      block(p, x, y, r * 1.9f, r * .9f, r * .4f, body,
            Paint(body.r * .7f, body.g * .7f, body.b * .7f),
            Paint(std::min(255, int(body.r) + 25), std::min(255, int(body.g) + 25),
                  std::min(255, int(body.b) + 25)));
      block(p, x - r * .08f, y - r * .4f, r * cabin, r * .65f, r * (model == 3 ? .48f : .36f),
            Paint(127, 183, 197), Paint(75, 128, 153), Paint(184, 219, 219));
      for (int side = -1; side <= 1; side += 2)
        oval(p, x + side * r * .65f, y + r * .47f, r * .17f, r * .2f, dark);
      p.line(x - r * .75f, y + r * .05f, x - r * .4f, y + r * .05f, Paint(251, 229, 159));
      if (model == 2) {
        p.rect(int(x - r * .12f), int(y - r * .93f), std::max(1, int(r * .3f)),
               std::max(1, int(r * .12f)), Paint(245, 211, 94));
      }
      if (model == 4) {
        block(p, x - r * .12f, y - r * .35f, r * 1.2f, r * .8f, r * .65f, Paint(238, 238, 222),
              Paint(185, 202, 197), Paint(252, 248, 231));
        p.rect(int(x - r * .32f), int(y - r * .54f), std::max(1, int(r * .48f)),
               std::max(1, int(r * .13f)), Paint(214, 76, 70));
        p.rect(int(x - r * .15f), int(y - r * .72f), std::max(1, int(r * .13f)),
               std::max(1, int(r * .48f)), Paint(214, 76, 70));
        p.rect(int(x - r * .25f), int(y - r * 1.08f), std::max(1, int(r * .25f)),
               std::max(1, int(r * .13f)), Paint(86, 173, 211));
        p.rect(int(x), int(y - r * 1.08f), std::max(1, int(r * .25f)), std::max(1, int(r * .13f)),
               Paint(233, 108, 81));
      }
    } else if (kind == 3) {
      int model = id % 4;
      const Paint bodies[] = {Paint(137, 169, 185), Paint(208, 151, 82), Paint(205, 76, 65),
                              Paint(129, 166, 125)};
      Paint body = bodies[model];
      block(p, x - r * .26f, y, r * 1.35f, r * .95f, r * .85f, body,
            Paint(body.r * .7f, body.g * .7f, body.b * .7f),
            Paint(std::min(255, int(body.r) + 35), std::min(255, int(body.g) + 35),
                  std::min(255, int(body.b) + 35)));
      block(p, x + r * .72f, y + r * .02f, r * .65f, r * .85f, r * .6f, Paint(67, 126, 155),
            Paint(44, 88, 119), Paint(114, 166, 188));
      p.rect(int(x + r * .48f), int(y - r * .38f), std::max(1, int(r * .43f)),
             std::max(1, int(r * .22f)), Paint(189, 224, 221));
      for (int wheel = 0; wheel < 3; ++wheel)
        oval(p, x - r * .73f + wheel * r * .68f, y + r * .5f, r * .15f, r * .18f, dark);
      for (int line = 0; line < 3; ++line)
        p.line(x - r * .8f + line * r * .35f, y - r * .7f, x - r * .8f + line * r * .35f,
               y + r * .35f, Paint(106, 139, 158));
      if (model == 2) {
        block(p, x + r * .72f, y + r * .02f, r * .65f, r * .85f, r * .6f, Paint(218, 83, 70),
              Paint(153, 56, 52), Paint(238, 131, 100));
        p.rect(int(x + r * .46f), int(y - r * .4f), std::max(1, int(r * .45f)),
               std::max(1, int(r * .22f)), Paint(174, 213, 217));
        for (int rail = -1; rail <= 1; rail += 2)
          p.line(x - r * .83f, y - r * .99f + rail * r * .12f, x + r * .25f,
                 y - r * .99f + rail * r * .12f, Paint(230, 229, 203));
        for (int rung = 0; rung < 6; ++rung)
          p.line(x - r * .8f + rung * r * .19f, y - r * 1.11f, x - r * .8f + rung * r * .19f,
                 y - r * .87f, Paint(230, 229, 203));
        p.disc(x + r * .55f, y - r * .69f, std::max(1, int(r * .08f)), Paint(96, 174, 216));
        p.rect(int(x - r * .7f), int(y - r * .08f), std::max(1, int(r * .7f)),
               std::max(1, int(r * .12f)), Paint(245, 225, 154));
      }
    } else if (kind == 4) {
      block(p, x, y, r * 1.6f, r * .9f, r * .9f, Paint(238, 206, 155), Paint(184, 151, 111),
            Paint(247, 219, 169));
      float left = x - r * .8f, right = x + r * .8f, nearY = y - r * .45f, farY = y - r * 1.35f,
            skew = r * .36f;
      face(p, {{left, nearY}, {right, nearY}, {x, nearY - r * .58f}}, Paint(220, 177, 126));
      face(p,
           {{left, nearY}, {x, nearY - r * .58f}, {x + skew, farY - r * .58f}, {left + skew, farY}},
           Paint(197, 122, 85));
      face(p,
           {{x, nearY - r * .58f},
            {right, nearY},
            {right + skew, farY},
            {x + skew, farY - r * .58f}},
           Paint(151, 84, 64));
      p.rect(int(x - r * .15f), int(y - r * .08f), std::max(1, int(r * .3f)),
             std::max(1, int(r * .52f)), wood);
      for (int side = -1; side <= 1; side += 2)
        p.rect(int(x + side * r * .5f - r * .13f), int(y - r * .22f), std::max(1, int(r * .27f)),
               std::max(1, int(r * .29f)), Paint(112, 166, 183));
    } else {
      Paint wall = id % 2 ? Paint(150, 174, 182) : Paint(184, 172, 152);
      block(p, x, y, r * 1.3f, r * .8f, r * 1.85f, wall, Paint(101, 131, 145),
            Paint(205, 215, 206));
      for (int row = 0; row < 5; ++row)
        for (int col = 0; col < 3; ++col)
          p.rect(int(x - r * .48f + col * r * .34f), int(y - r * 1.58f + row * r * .36f),
                 std::max(1, int(r * .18f)), std::max(1, int(r * .22f)), Paint(211, 232, 222));
      for (int row = 0; row < 5; ++row)
        face(p,
             {{x + r * .68f, y + r * .27f - r * 1.85f + row * r * .36f},
              {x + r * .87f, y - r * .2f - r * 1.85f + row * r * .36f},
              {x + r * .87f, y - r * .05f - r * 1.85f + row * r * .36f},
              {x + r * .68f, y + r * .42f - r * 1.85f + row * r * .36f}},
             Paint(158, 194, 200));
      block(p, x + r * .1f, y - r * 1.85f, r * .4f, r * .25f, r * .13f, Paint(137, 154, 154),
            Paint(106, 124, 130), Paint(222, 225, 211));
    }
    return;
  }
  if (category >= 3) {
    if (category == 3) {
      p.round(int(x - r), int(y - r * .6f), int(r * 2), int(r * 1.2f), 3, Paint(126, 161, 179));
      p.rect(int(x + r * .4f), int(y - r * .5f), int(r * .6f), int(r), Paint(61, 115, 139));
      p.rect(int(x + r * .5f), int(y - r * .4f), int(r * .4f), int(r * .4f), Paint(188, 224, 221));
      for (int w = -1; w <= 1; w += 2)
        p.rect(int(x + w * r * .7f - 2), int(y + r * .55f), 4, 3, dark);
      for (int n = 0; n < 3; ++n)
        p.line(x - r * .8f + n * r * .35f, y - r * .45f, x - r * .8f + n * r * .35f, y + r * .45f,
               Paint(162, 189, 196));
    } else if (category == 4) {
      p.rect(int(x - r * .8f), int(y - r * .25f), int(r * 1.6f), int(r), Paint(238, 206, 155));
      for (int row = 0; row < int(r * .9f); ++row) {
        float half = r * (1 - row / (r * 1.1f));
        p.line(x - half, y - r * .25f - row, x + half, y - r * .25f - row, Paint(179, 106, 76));
      }
      p.rect(int(x - r * .2f), int(y + r * .2f), int(r * .35f), int(r * .55f), wood);
      for (int side = -1; side <= 1; side += 2)
        p.rect(int(x + side * r * .48f - r * .15f), int(y + r * .05f), int(r * .3f), int(r * .3f),
               Paint(121, 171, 177));
    } else {
      Paint wall = id % 2 ? Paint(151, 173, 177) : Paint(180, 166, 147);
      p.rect(int(x - r * .7f), int(y - r * 1.35f), int(r * 1.4f), int(r * 2), wall);
      p.rect(int(x + r * .45f), int(y - r * 1.35f), int(r * .25f), int(r * 2),
             Paint(111, 137, 143));
      p.rect(int(x - r * .75f), int(y - r * 1.42f), int(r * 1.5f), int(r * .18f),
             Paint(88, 111, 116));
      for (int row = 0; row < 4; ++row)
        for (int col = 0; col < 3; ++col)
          p.rect(int(x - r * .52f + col * r * .34f), int(y - r * 1.1f + row * r * .38f),
                 std::max(1, int(r * .19f)), std::max(1, int(r * .23f)), Paint(214, 228, 209));
    }
  } else if (category == 0) {
    if (type == 0) {
      p.round(int(x - r), int(y - r), int(r * 2), int(r * 2), 2, Paint(108, 151, 166));
      p.line(x - r, y - r, x + r, y - r, Paint(231, 242, 232));
    }
    if (type == 1) {
      p.disc(x, y, r, Paint(232, 136, 78));
      p.line(x, y - r, x + 2, y - r - 2, Paint(67, 119, 84));
    }
    if (type == 2) {
      p.disc(x, y, r, Paint(143, 157, 146));
      p.line(x - r * .5f, y - r * .4f, x + r * .4f, y - r * .4f, Paint(194, 202, 187));
    }
  } else if (category == 1) {
    if (type == 0) {
      p.round(int(x - r), int(y - r), int(2 * r), int(2 * r), 2, wood);
      p.rect(int(x - r + 2), int(y - r + 2), int(2 * r - 4), 2, Paint(222, 170, 99));
      p.line(x - r + 2, y + r - 2, x + r - 2, y - r + 2, Paint(112, 76, 49));
    }
    if (type == 1) {
      p.rect(int(x - r), int(y + r - 3), int(2 * r), 3, dark);
      for (int row = 0; row < int(2 * r); ++row) {
        float half = (row + 1) * .42f;
        p.line(x - half, y - r + row, x + half, y - r + row,
               row > r * .8f && row < r * 1.2f ? Paint(255, 243, 204) : Paint(232, 143, 71));
      }
    }
    if (type == 2) {
      p.disc(x - r * .5f, y, r * .7f, Paint(73, 128, 89));
      p.disc(x + r * .4f, y - r * .4f, r * .8f, Paint(98, 155, 101));
      p.disc(x, y - r * .6f, r * .6f, Paint(134, 179, 112));
    }
  } else {
    if (type == 0) {
      p.rect(int(x - 2), int(y), 4, int(r), wood);
      p.disc(x, y - r * .5f, r, Paint(53, 112, 83));
      p.disc(x - r * .3f, y - r * .8f, r * .72f, Paint(82, 147, 98));
      p.disc(x + r * .25f, y - r * .95f, r * .5f, Paint(120, 174, 112));
    }
    if (type == 1) {
      p.rect(int(x - r), int(y - r * .3f), int(2 * r), int(r), wood);
      p.rect(int(x - r), int(y - r * .5f), int(2 * r), 3, Paint(216, 161, 97));
      p.rect(int(x - r + 2), int(y + r * .5f), 3, 4, dark);
      p.rect(int(x + r - 5), int(y + r * .5f), 3, 4, dark);
    }
    if (type == 2) {
      p.round(int(x - r), int(y - r * .6f), int(2 * r), int(r * 1.3f), 4, Paint(196, 104, 87));
      p.round(int(x - r * .45f), int(y - r * .5f), int(r), int(r), 2, Paint(177, 212, 206));
      p.rect(int(x - r + 2), int(y + r * .6f), 4, 3, dark);
      p.rect(int(x + r - 6), int(y + r * .6f), 4, 3, dark);
    }
  }
}
inline void renderCity(SDL_Renderer *renderer, const Engine &e, bool help,
                       const std::string &notice) {
  Canvas p(renderer);
  const State &s = e.s;
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  p.paint(Paint(55, 136, 165));
  SDL_RenderClear(renderer);
  float zoom = std::max(.18f, std::min(1.f, 27.f / std::max(9.f, s.f[2])));
  float cameraX = s.f[0];
  float cameraY = s.f[1];
  auto sx = [&](float x, float y) {
    return 240 + ((x - cameraX) - (y - cameraY)) * .70710678f * zoom;
  };
  auto sy = [&](float x, float y) { return 138 + ((x - cameraX) + (y - cameraY)) * .4f * zoom; };
  SDL_Rect clip{0, 44, 480, 189};
  SDL_RenderSetClipRect(renderer, &clip);
  if (!boca_ground::draw(renderer, cameraX, cameraY, zoom)) {
    // Ocean continues beyond every coastline; the camera never stops at the map edge.
    for (int stripe = 0; stripe < 14; ++stripe) {
      float worldY = std::floor((cameraY - 120 / zoom) / 140) * 140 + stripe * 140;
      for (int j = 0; j < 12; ++j) {
        float worldX = std::floor((cameraX - 250 / zoom) / 220) * 220 + j * 220;
        float x = sx(worldX, worldY),
              y = sy(worldX, worldY) + std::sin(s.f[900] * 1.1f + j + stripe) * 2;
        p.line(x, y, x + std::max(8.f, 42 * zoom), y, Paint(100, 183, 197, 115));
      }
    }
    auto coast = [&](float margin, Paint color, std::array<SDL_FPoint, 64> *keep) {
      std::array<SDL_FPoint, 64> points;
      for (int i = 0; i < 64; ++i) {
        float angle = i * 6.2831853f / 64, c = std::cos(angle), d = std::sin(angle),
              radial = islandWave(angle) / std::pow(c * c * c * c + d * d * d * d, .25f);
        float wx = 1600 + c * (1520 - margin) * radial, wy = 1000 + d * (920 - margin) * radial;
        points[i] = {sx(wx, wy), sy(wx, wy)};
      }
      if (keep)
        *keep = points;
      float minY = 1e9, maxY = -1e9;
      for (auto q : points) {
        minY = std::min(minY, q.y);
        maxY = std::max(maxY, q.y);
      }
      for (int row = std::max(44, int(minY)); row <= std::min(232, int(maxY)); ++row) {
        float scan = row + .5f, left = 1e9, right = -1e9;
        for (int i = 0; i < 64; ++i) {
          auto a = points[i], b = points[(i + 1) % 64];
          if ((a.y <= scan && b.y > scan) || (b.y <= scan && a.y > scan)) {
            float x = a.x + (scan - a.y) * (b.x - a.x) / (b.y - a.y);
            left = std::min(left, x);
            right = std::max(right, x);
          }
        }
        if (left > right)
          continue;
        if (!keep) {
          p.line(std::max(0.f, left), float(row), std::min(479.f, right), float(row), color);
          continue;
        }
        int begin = std::max(0, int(std::ceil(left))), end = std::min(479, int(std::floor(right)));
        auto terrain = [&](int pixel) {
          float horizontal = (pixel - 240) / (.70710678f * zoom),
                vertical = (row - 138) / (.4f * zoom);
          float wx = cameraX + (horizontal + vertical) * .5f,
                wy = cameraY + (vertical - horizontal) * .5f;
          float mx = cityModulo(wx), my = cityModulo(wy);
          bool road = mx < cityRoadWidth || my < cityRoadWidth;
          if (road) {
            bool dash = (mx >= 54 && mx < 58 && int(wy) % 64 < 28) ||
                        (my >= 54 && my < 58 && int(wx) % 64 < 28);
            if (dash)
              return Paint(244, 234, 198);
            // Mark crossings where the two diagonal streets meet.
            if ((mx > 88 && mx < 112 && my < 112 && int(my) % 16 < 7) ||
                (my > 88 && my < 112 && mx < 112 && int(mx) % 16 < 7))
              return Paint(224, 229, 211);
            return Paint(141, 158, 160);
          }
          if (mx < citySidewalk || my < citySidewalk)
            return Paint(222, 216, 190);
          if (mx < citySidewalk + 3 || my < citySidewalk + 3)
            return Paint(151, 176, 145);
          return (int(wx / 400) + int(wy / 400)) % 2 ? Paint(177, 200, 151) : Paint(184, 206, 158);
        };
        if (begin > end)
          continue;
        int run = begin;
        Paint current = terrain(begin);
        for (int pixel = begin + 1; pixel <= end + 1; ++pixel) {
          Paint next = pixel <= end ? terrain(pixel) : Paint(0, 0, 0);
          if (pixel == end + 1 || next.r != current.r || next.g != current.g ||
              next.b != current.b) {
            p.line(float(run), float(row), float(pixel - 1), float(row), current);
            run = pixel;
            current = next;
          }
        }
      }
    };
    coast(-32, Paint(104, 192, 195), nullptr);
    coast(-12, Paint(218, 236, 209), nullptr);
    coast(0, Paint(232, 212, 155), nullptr);
    std::array<SDL_FPoint, 64> land;
    coast(46, Paint(184, 206, 158), &land);
  }
  auto hole = [&](float wx, float wy, float radius, Paint rim) {
    float x = sx(wx, wy), y = sy(wx, wy), r = std::max(2.f, radius * zoom);
    if (x + r < 0 || x - r > 480 || y + r < 44 || y - r > 233)
      return;
    oval(p, x + 2, y + 4, r + 3, r * .73f + 2, Paint(136, 142, 118));
    oval(p, x, y, r + 3, r * .73f + 2, rim);
    oval(p, x, y, r, r * .73f, Paint(24, 34, 36));
    oval(p, x, y - 1, r * .73f, r * .47f, Paint(12, 23, 28));
  };
  std::vector<int> order;
  for (int i = 0; i < cityCount; ++i)
    if (s.a[i] || s.f[11 + i * 4] > 0)
      order.push_back(i);
  if (s.f[901] <= 0 || int(s.f[901] * 8) % 2 == 0)
    order.push_back(cityCount);
  for (int rival = 0; rival < 3; ++rival)
    if (s.a[800 + rival] && (s.f[805 + rival * 8] <= 0 || int(s.f[805 + rival * 8] * 8) % 2 == 0))
      order.push_back(cityCount + 1 + rival);
  auto depth = [&](int item) {
    return item < cityCount ? s.f[8 + item * 4] + s.f[9 + item * 4]
           : item == cityCount
               ? s.f[0] + s.f[1]
               : s.f[800 + (item - cityCount - 1) * 8] + s.f[801 + (item - cityCount - 1) * 8];
  };
  // Holes belong to the ground layer. Objects must remain above their opening,
  // including when approaching from the far side and during absorption.
  std::sort(order.begin(), order.end(), [&](int a, int b) {
    if ((a >= cityCount) != (b >= cityCount))
      return a >= cityCount;
    return depth(a) < depth(b);
  });
  for (int item : order) {
    if (item >= cityCount) {
      int actor = item - cityCount;
      int b = 800 + (actor - 1) * 8;
      float wx = actor ? s.f[b] : s.f[0], wy = actor ? s.f[b + 1] : s.f[1],
            radius = actor ? s.f[b + 2] : s.f[2];
      hole(wx, wy, radius, holeColor(actor));
      if (actor) {
        p.disc(sx(wx, wy) - 3, sy(wx, wy) - 1, 1, Paint(246, 225, 191));
        p.disc(sx(wx, wy) + 3, sy(wx, wy) - 1, 1, Paint(246, 225, 191));
        if (radius > s.f[2] * 1.25f)
          p.text(int(sx(wx, wy)) - 2, int(sy(wx, wy) - radius * zoom * .73f) - 11, "!",
                 Paint(214, 73, 65));
      }
      continue;
    }
    int i = item, b = 8 + i * 4;
    float scale = s.a[i] ? 1 : s.f[b + 3] / .28f;
    int owner = s.a[cityOwnerSlot(i)] - 1;
    float hx = s.f[0], hy = s.f[1];
    if (owner > 0 && owner <= 3) {
      hx = s.f[800 + (owner - 1) * 8];
      hy = s.f[801 + (owner - 1) * 8];
    }
    float x = s.a[i] ? s.f[b] : hx + (s.f[b] - hx) * scale,
          y = s.a[i] ? s.f[b + 1] : hy + (s.f[b + 1] - hy) * scale, r = s.f[b + 2] * zoom;
    if (sx(x, y) + r * 1.5f < 0 || sx(x, y) - r > 480 || sy(x, y) + r < 44 ||
        sy(x, y) - r * 2.6f > 233)
      continue;
    object(p, i, sx(x, y), sy(x, y), r, s.a[i] && s.f[2] > s.f[b + 2] + 2, scale, s.a[300 + i],
           citySpriteFlip(s, i));
  }
  // A tiny city-wide map keeps off-screen rivals and unexplored areas readable.
  p.round(393, 49, 81, 52, 4, Paint(40, 105, 134, 235));
  p.round(397, 53, 73, 44, 12, Paint(221, 208, 151));
  p.round(399, 55, 69, 40, 11, Paint(81, 117, 82));
  for (int i = 0; i < cityCount; ++i)
    if (s.a[i] && s.a[300 + i] == 5)
      p.rect(397 + int(s.f[8 + i * 4] / cityWidth * 72), 53 + int(s.f[9 + i * 4] / cityHeight * 43),
             2, 2, Paint(172, 183, 166));
  for (int i = 0; i < 3; ++i)
    if (s.a[800 + i])
      p.disc(397 + s.f[800 + i * 8] / cityWidth * 72, 53 + s.f[801 + i * 8] / cityHeight * 43, 2,
             holeColor(i + 1));
  p.disc(397 + s.f[0] / cityWidth * 72, 53 + s.f[1] / cityHeight * 43, 2, holeColor(0));
  SDL_RenderSetClipRect(renderer, nullptr);
  p.rect(0, 0, 480, 44, Paint(32, 61, 55));
  p.text(12, 11, "BOCA", Paint(245, 235, 203), 3);
  p.text(115, 8, "ILHA DA BOCA", Paint(190, 211, 177));
  p.text(115, 25, "EDIFICIOS " + std::to_string(s.a[254]) + "/3", Paint(245, 235, 203));
  int seconds = int(std::ceil(s.f[3]));
  p.text(383, 16,
         std::to_string(seconds / 60) + ":" + (seconds % 60 < 10 ? "0" : "") +
             std::to_string(seconds % 60),
         Paint(245, 235, 203), 2);
  const char *stage = s.f[2] <= 11    ? "FRUTAS"
                      : s.f[2] <= 24  ? "CONES"
                      : s.f[2] <= 42  ? "CARROS"
                      : s.f[2] <= 72  ? "CAMINHOES"
                      : s.f[2] <= 112 ? "CASAS"
                                      : "EDIFICIOS";
  p.rect(0, 233, 480, 39, Paint(245, 239, 218));
  p.text(10, 241, std::string("ESCALA: ") + stage, Paint(52, 99, 80));
  p.text(253, 241, "A IMPULSO   Y EXTRAS", Paint(52, 99, 80));
  p.centered(240, 259,
             s.f[901] > 0 ? "FUJA! PROTECAO " + std::to_string(int(std::ceil(s.f[901]))) + "S"
                          : "DOURADO CABE   ! RIVAL MAIOR   SELECT SALVAR",
             Paint(99, 104, 82));
  if (help || s.phase) {
    p.rect(0, 0, 480, 272, Paint(15, 33, 28, 150));
    p.round(22, 39, 436, 192, 15, Paint(250, 245, 226));
    p.centered(240, 56,
               help           ? "ILHA DA BOCA"
               : s.phase == 1 ? "GIGANTE DA CIDADE!"
                              : "TEMPO ESGOTADO",
               Paint(43, 104, 80), 2);
    p.centered(240, 96,
               help ? "FRUTAS > CONES > CARROS > CAMINHOES"
                    : "EDIFICIOS ABSORVIDOS: " + std::to_string(s.a[254]) + "/3",
               Paint(61, 81, 70));
    p.centered(240, 119,
               help ? "CASAS > EDIFICIOS. ABSORVA 3 PARA VENCER"
                    : "OBJETOS ABSORVIDOS: " + std::to_string(s.score),
               Paint(61, 81, 70));
    p.centered(240, 143,
               help ? "RIVAIS MAIORES ROUBAM PARTE DO SEU TAMANHO"
                    : "A CIDADE MUDA EM CADA NOVA RODADA",
               Paint(61, 81, 70));
    p.centered(240, 167,
               help ? "OBJETOS VOLTAM. RIVAIS TAMBEM SE ATACAM"
                    : "EXPLORE, CRESCA E EVITE OS RIVAIS MAIORES",
               Paint(61, 81, 70));
    p.centered(240, 207, help ? "A IMPULSO   B VOLTAR" : "B NOVA CIDADE   SELECT SALVAR E SAIR",
               Paint(43, 104, 80));
  }
  if (!notice.empty())
    p.text(5, 264, notice, Paint(168, 69, 50));
  SDL_RenderPresent(renderer);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
inline void render(SDL_Renderer *renderer, const Engine &e, bool help, const std::string &notice) {
  if (isCity(e.s)) {
    renderCity(renderer, e, help, notice);
    return;
  }
  Canvas p(renderer);
  const State &s = e.s;
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  p.paint(Paint(190, 210, 171));
  SDL_RenderClear(renderer);
  p.rect(0, 46, 480, 186, Paint(185, 205, 166));
  p.round(8, 50, 464, 179, 12, Paint(225, 219, 191));
  p.rect(0, 120, 480, 42, Paint(206, 202, 181));
  p.rect(218, 47, 44, 184, Paint(206, 202, 181));
  for (int x = 15; x < 480; x += 31)
    p.rect(x, 139, 14, 2, Paint(237, 231, 207));
  for (int y = 55; y < 229; y += 28)
    p.rect(239, y, 2, 12, Paint(237, 231, 207));
  for (int i = 0; i < 16; ++i) {
    int x = 12 + i * 31;
    p.disc(x, 55, 2, Paint(159, 183, 139));
    p.disc(x, 224, 2, Paint(159, 183, 139));
  }
  float x = s.f[0], y = s.f[1], r = s.f[2];
  if (s.f[6] > 0)
    oval(p, x, y, r + 5, r * .73f + 4, Paint(235, 184, 93));
  oval(p, x + 2, y + 4, r + 3, r * .73f + 2, Paint(148, 137, 114));
  oval(p, x, y, r + 2, r * .73f + 1, Paint(73, 83, 73));
  oval(p, x, y, r, r * .73f, Paint(25, 35, 36));
  oval(p, x, y - 1, r * .75f, r * .48f, Paint(13, 23, 27));
  for (int i = 0; i < 60; ++i) {
    int b = 8 + i * 4;
    if (!s.a[i] && s.f[b + 3] <= 0)
      continue;
    float scale = s.a[i] ? 1 : s.f[b + 3] / .28f;
    float px = s.a[i] ? s.f[b] : x + (s.f[b] - x) * scale;
    float py = s.a[i] ? s.f[b + 1] : y + (s.f[b + 1] - y) * scale;
    object(p, i, px, py, s.f[b + 2], s.a[i] && r > s.f[b + 2] + 2, scale);
  }
  p.rect(0, 0, 480, 44, Paint(32, 61, 55));
  p.text(14, 11, "BOCA", Paint(245, 235, 203), 3);
  p.text(122, 9, "PRACA DO BAIRRO", Paint(190, 211, 177));
  p.text(122, 25, "ABSORVIDOS " + std::to_string(s.score) + "/60", Paint(245, 235, 203));
  p.text(348, 10, "TEMPO", Paint(190, 211, 177));
  int seconds = int(std::ceil(s.f[3]));
  std::string timer = std::to_string(seconds / 60) + ":" + (seconds % 60 < 10 ? "0" : "") +
                      std::to_string(seconds % 60);
  p.text(390, 16, timer, s.f[3] < 20 ? Paint(255, 174, 120) : Paint(245, 235, 203), 2);
  p.rect(0, 42, int(480 * s.score / 60), 2, Paint(235, 184, 93));
  p.rect(0, 233, 480, 39, Paint(245, 239, 218));
  p.text(12, 241, s.f[7] > 0 ? "A RECARGA" : "A IMPULSO", Paint(52, 99, 80));
  p.text(143, 241, "Y EXTRAS", Paint(52, 99, 80));
  p.text(260, 241, "DOURADO = PODE ABSORVER", Paint(99, 104, 82));
  p.centered(240, 259, "DIRECIONAL MOVER   START MENU   SELECT SALVAR E SAIR", Paint(99, 104, 82));
  if (help || s.phase) {
    p.rect(0, 0, 480, 272, Paint(15, 33, 28, 140));
    p.round(32, 48, 416, 174, 15, Paint(250, 245, 226));
    p.centered(240, 66,
               help           ? "COMO JOGAR"
               : s.phase == 1 ? "PRACA LIMPA!"
                              : "TEMPO ESGOTADO",
               Paint(43, 104, 80), 2);
    p.centered(240, 103,
               help ? "APROXIME A BOCA DOS OBJETOS MENORES"
                    : std::to_string(s.score) + " DE 60 OBJETOS ABSORVIDOS",
               Paint(61, 81, 70));
    p.centered(240, 125,
               help ? "O BRILHO DOURADO MOSTRA O QUE CABE" : "A BOCA CRESCE A CADA OBJETO",
               Paint(61, 81, 70));
    p.centered(240, 147,
               help ? "CRESCA PARA PEGAR ARVORES E CARROS"
                    : "TENTE LIMPAR TODA A PRACA EM 2 MINUTOS",
               Paint(61, 81, 70));
    p.centered(240, 187, help ? "A IMPULSO   B VOLTAR" : "B NOVA RODADA   SELECT SALVAR E SAIR",
               Paint(43, 104, 80));
  }
  if (!notice.empty())
    p.text(5, 264, notice, Paint(168, 69, 50));
  SDL_RenderPresent(renderer);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
} // namespace boca_art
} // namespace fmg
