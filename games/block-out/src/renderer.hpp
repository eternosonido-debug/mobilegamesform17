#pragma once
#include "game_logic.hpp"
#include "pixel.hpp"
namespace blockout {
inline ui::Paint color(int c) {
  static const ui::Paint colors[] = {
      ui::Paint(58, 74, 86), ui::Paint(221, 77, 69), ui::Paint(48, 141, 207),
      ui::Paint(59, 168, 112), ui::Paint(245, 188, 52)};
  return colors[c % 5];
}
inline void brick(ui::Canvas &p, float x, float y, int w, int h, int c,
                  bool selected = false, float fade = 1, int cell = 16) {
  auto paint = [&](ui::Paint a) {
    a.a = Uint8(a.a * fade);
    return a;
  };
  auto body = color(c);
  int xx = int(x), yy = int(y);
  if (selected)
    p.round(xx - 2, yy - 2, w + 4, h + 4, 2, paint(ui::Paint(255, 244, 199)));
  p.rect(xx + 1, yy + 2, w, h, paint(ui::Paint(31, 51, 64, 100)));
  p.rect(xx, yy, w, h,
         paint(ui::Paint(body.r * .66, body.g * .66, body.b * .66)));
  p.rect(xx, yy, w - 1, h - 3, paint(body));
  p.line(xx + 1, yy, xx + w - 2, yy,
         paint(ui::Paint(std::min(255, body.r + 40), std::min(255, body.g + 40),
                         std::min(255, body.b + 40))));
  for (int row = 0; row < (h + 1) / cell; ++row)
    for (int col = 0; col < (w + 1) / cell; ++col) {
      int cx = xx + cell / 2 + col * cell, cy = yy + cell / 2 - 2 + row * cell;
      p.disc(cx, cy + 1, cell / 4,
             paint(ui::Paint(body.r * .65, body.g * .65, body.b * .65)));
      p.disc(cx, cy - 1, cell / 4,
             paint(ui::Paint(std::min(255, body.r + 26),
                             std::min(255, body.g + 26),
                             std::min(255, body.b + 26))));
      p.line(cx - 2, cy - cell / 4, cx + 2, cy - cell / 4,
             paint(ui::Paint(255, 255, 245, 135)));
    }
}
inline void exitBrick(ui::Canvas &p, SDL_Renderer *renderer, const Block &b,
                      const Level &l, float t, int edgeOverride = -1) {
  int dx = 0, dy = 0;
  for (auto &d : l.doors)
    if (d.color == b.color) {
      dx = d.edge == 0 ? -1 : d.edge == 1 ? 1 : 0;
      dy = d.edge == 2 ? -1 : d.edge == 3 ? 1 : 0;
    }
  if (edgeOverride >= 0) {
    dx = edgeOverride == 0 ? -1 : edgeOverride == 1 ? 1 : 0;
    dy = edgeOverride == 2 ? -1 : edgeOverride == 3 ? 1 : 0;
  }
  int cell = 96 / l.size;
  float startX = 12 + b.x * cell, startY = 25 + b.y * cell;
  float distance = (dx ? b.w : b.h) * cell + 6;
  float travel = std::min(1.f, t / .78f) * distance;
  SDL_Rect prior;
  bool clipped = SDL_RenderIsClipEnabled(renderer);
  SDL_RenderGetClipRect(renderer, &prior);
  SDL_Rect boardClip{12, 25, 96, 96};
  SDL_RenderSetClipRect(renderer, &boardClip);
  // The portion still inside the board is always the complete, opaque brick.
  brick(p, startX + dx * travel, startY + dy * travel, b.w * cell - 1,
        b.h * cell - 1, b.color, false, 1, cell);
  SDL_Rect outside = dx > 0   ? SDL_Rect{108, 0, 132, 136}
                     : dx < 0 ? SDL_Rect{0, 0, 12, 136}
                     : dy > 0 ? SDL_Rect{0, 121, 240, 15}
                              : SDL_Rect{0, 0, 240, 25};
  SDL_RenderSetClipRect(renderer, &outside);
  auto base = color(b.color);
  for (int row = 0; row < b.h * cell / 4; ++row)
    for (int col = 0; col < b.w * cell / 4; ++col) {
      float ox = startX + col * 4 + 2, oy = startY + row * 4 + 2;
      float crossing = dx > 0   ? (108 - ox) / distance * .78f
                       : dx < 0 ? (ox - 12) / distance * .78f
                       : dy > 0 ? (121 - oy) / distance * .78f
                                : (oy - 25) / distance * .78f;
      float age = t - crossing;
      if (age <= 0 || age >= .26f)
        continue;
      int seed = (col * 17 + row * 31) % 13;
      float drift = (seed - 6) * age * 4;
      float along = travel + age * (3 + seed * .3f);
      int xx = int(ox + dx * along - dy * drift) - 1,
          yy = int(oy + dy * along + dx * drift) - 1;
      int size = age > .17f ? 2 : 3,
          alpha = int(255 * std::min(1.f, (.26f - age) / .12f));
      p.rect(xx, yy, size, size, ui::Paint(base.r, base.g, base.b, alpha));
      if (seed % 3 == 0)
        p.rect(xx, yy, 1, 1, ui::Paint(255, 246, 196, alpha));
    }
  SDL_RenderSetClipRect(renderer, clipped ? &prior : nullptr);
}
inline void render(SDL_Renderer *r, const Game &g) {
  ui::Canvas p(r);
  SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
  p.paint(ui::Paint(209, 224, 226));
  SDL_RenderClear(r);
  for (int y = 5; y < 136; y += 10)
    for (int x = 5; x < 240; x += 10) {
      p.disc(x, y, 2, ui::Paint(182, 204, 209));
      p.disc(x, y - 1, 1, ui::Paint(225, 237, 234));
    }
  p.rect(0, 0, 240, 20, ui::Paint(32, 65, 80));
  p.text(9, 6, "BLOCK OUT", ui::Paint(255, 242, 203));
  p.text(146, 6, "PECAS E PORTAS", ui::Paint(168, 219, 216));
  auto l = level(g.current);
  int cell = 96 / l.size;
  for (int y = 0; y < l.size; ++y)
    for (int x = 0; x < l.size; ++x) {
      p.rect(12 + x * cell, 25 + y * cell, cell, cell,
             (x + y) % 2 ? ui::Paint(163, 191, 196) : ui::Paint(171, 199, 202));
      p.disc(12 + cell / 2 + x * cell, 25 + cell / 2 + y * cell, 1,
             ui::Paint(194, 217, 216));
      if (l.walls[y * l.size + x])
        brick(p, 12 + x * cell, 25 + y * cell, cell, cell, 0, false, 1, cell);
    }
  for (auto &d : l.doors) {
    auto c = color(d.color);
    int x = d.edge == 0 ? 7 : d.edge == 1 ? 109 : 12 + d.pos * cell;
    int y = d.edge == 2 ? 20 : d.edge == 3 ? 122 : 25 + d.pos * cell;
    int w = d.edge < 2 ? 4 : d.span * cell, h = d.edge < 2 ? d.span * cell : 4;
    p.rect(x, y, w, h, c);
    if (d.edge < 2)
      p.line(x + 1, y + 2, x + 1, y + h - 3, ui::Paint(255, 246, 216));
    else
      p.line(x + 2, y + 1, x + w - 3, y + 1, ui::Paint(255, 246, 216));
  }
  for (int i = 0; i < int(g.board.blocks.size()); ++i) {
    auto b = g.board.blocks[i];
    if (!b.alive && !(g.animation > 0 && g.animated == i))
      continue;
    float x = b.x, y = b.y, fade = 1;
    if (g.animation > 0 && g.animated == i) {
      float t = 1 - g.animation / (g.exiting ? exitDuration : .13f);
      float ease = t * t * (3 - 2 * t);
      x = g.oldBlock.x + (b.x - g.oldBlock.x) * ease;
      y = g.oldBlock.y + (b.y - g.oldBlock.y) * ease;
      if (g.exiting) {
        for (auto &d : l.doors)
          if (d.color == b.color) {
            x += (d.edge == 0 ? -1 : d.edge == 1 ? 1 : 0) * ease;
            y += (d.edge == 2 ? -1 : d.edge == 3 ? 1 : 0) * ease;
          }
        fade = 1 - t;
      }
    }
    if (g.animation > 0 && g.animated == i && g.exiting) {
      exitBrick(p, r, g.oldBlock, l, 1 - g.animation / exitDuration,
                g.exitEdge);
      continue;
    }
    float shake =
        g.selected == i && g.invalid > 0 ? std::sin(g.invalid * 80) : 0;
    brick(p, 12 + x * cell + shake, 25 + y * cell - (g.selected == i ? 2 : 0),
          b.w * cell - 1, b.h * cell - 1, b.color, g.selected == i, fade, cell);
  }
  if (g.selected < 0) {
    int x = 12 + g.cursor % l.size * cell, y = 25 + g.cursor / l.size * cell;
    p.rect(x, y, cell - 1, 1, ui::Paint(255, 249, 213));
    p.rect(x, y, 1, cell - 1, ui::Paint(255, 249, 213));
    p.rect(x + cell - 2, y, 1, cell - 1, ui::Paint(255, 249, 213));
    p.rect(x, y + cell - 2, cell - 1, 1, ui::Paint(255, 249, 213));
  }
  p.round(122, 25, 110, 98, 4, ui::Paint(235, 241, 225));
  p.text(128, 31,
         "FASE " + std::to_string(g.current + 1) + "/" +
             std::to_string(levelCount),
         ui::Paint(42, 85, 91));
  p.text(128, 45, "MOVS " + std::to_string(g.board.moves),
         ui::Paint(42, 85, 91));
  p.text(128, 64, g.selected < 0 ? "A PEGAR" : "A SOLTAR",
         ui::Paint(42, 85, 91));
  p.text(128, 77, "B CANCELAR", ui::Paint(42, 85, 91));
  p.text(128, 90, "Y EXTRAS", ui::Paint(42, 85, 91));
  p.text(128, 109, "START PAUSA", ui::Paint(94, 128, 126));
  p.text(4, 128,
         g.invalid > 0    ? "BLOQUEADO: CONFIRA A COR E O CAMINHO"
         : g.current == 0 ? "LEVE CADA BLOCO A PORTA DA SUA COR"
                          : "SELECT SALVAR E SAIR",
         ui::Paint(35, 74, 88));
  auto panel = [&](const std::string &title) {
    p.round(28, 24, 184, 103, 6, ui::Paint(28, 61, 74, 248));
    p.centered(120, 32, title, ui::Paint(255, 224, 135));
  };
  if (g.scene == Scene::Menu) {
    panel("BLOCK OUT");
    const char *options[] = {"JOGAR / CONTINUAR", "TUTORIAL", "SOM", "SAIR"};
    for (int i = 0; i < 4; ++i) {
      if (g.menu == i)
        p.round(37, 46 + i * 17, 166, 15, 3, ui::Paint(57, 102, 112));
      p.centered(120, 50 + i * 17,
                 i == 2 ? (g.muted ? "SOM: DESLIGADO" : "SOM: LIGADO")
                        : options[i],
                 ui::Paint(244, 240, 210));
    }
  }
  if (g.scene == Scene::Paused) {
    panel("PAUSA / EXTRAS");
    const char *options[] = {"CONTINUAR", "DESFAZER", "REINICIAR", "SOM",
                             "SALVAR E SAIR"};
    for (int i = 0; i < 5; ++i) {
      if (g.menu == i)
        p.round(37, 45 + i * 15, 166, 14, 3, ui::Paint(57, 102, 112));
      p.centered(120, 48 + i * 15,
                 i == 3 ? (g.muted ? "SOM: DESLIGADO" : "SOM: LIGADO")
                        : options[i],
                 ui::Paint(244, 240, 210));
    }
  }
  if (g.scene == Scene::Won && g.animation <= 0) {
    panel("TODOS OS BLOCOS SAIRAM!");
    p.centered(120, 60, "MOVIMENTOS: " + std::to_string(g.board.moves),
               ui::Paint(244, 240, 210));
    p.centered(120, 82,
               g.current < levelCount - 1 ? "A PROXIMA FASE" : "A CONCLUIR",
               ui::Paint(244, 240, 210));
    p.centered(120, 104, "B REPETIR  Y EXTRAS", ui::Paint(185, 217, 207));
  }
  if (g.scene == Scene::GameOver) {
    panel("9 FASES CONCLUIDAS!");
    p.centered(120, 66, "OBRIGADO POR JOGAR", ui::Paint(244, 240, 210));
    p.centered(120, 95, "A VOLTAR AO MENU", ui::Paint(244, 240, 210));
  }
  SDL_RenderPresent(r);
}
} // namespace blockout
