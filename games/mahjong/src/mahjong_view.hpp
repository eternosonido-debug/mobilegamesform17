#pragma once
#include "juice_view.hpp"
namespace fmg {
namespace mahjong_art {
using juice_art::Canvas;
using juice_art::Paint;
inline std::pair<float, float> location(int i) {
  return {17.f + i % 8 * 40, 57.f + i / 8 * 44};
}
inline void tile(Canvas &p, float x, float y, int symbol, bool free, bool cursor, bool selected,
                 float opacity = 1) {
  auto c = [&](Paint a) {
    a.a = Uint8(a.a * opacity);
    return a;
  };
  int xx = int(x), yy = int(y);
  if (cursor || selected) {
    p.round(xx - 3, yy - 3, 40, 44, 6, c(Paint(244, 192, 80)));
    p.round(xx - 1, yy - 1, 36, 40, 5, c(Paint(255, 232, 169)));
  }
  p.round(xx + 2, yy + 5, 34, 38, 5, c(Paint(13, 43, 40, 125)));
  p.round(xx, yy + 3, 34, 38, 5, c(Paint(133, 150, 122)));
  p.round(xx, yy, 34, 37, 5, c(free ? Paint(255, 249, 225) : Paint(201, 210, 184)));
  p.line(xx + 5, yy + 2, xx + 28, yy + 2, c(Paint(255, 255, 246)));
  if (symbol <= 6) {
    static const int points[6][2] = {{0, 0}, {1, 2}, {0, 2}, {1, 0}, {0, 1}, {1, 1}};
    for (int i = 0; i < symbol; ++i) {
      int dx = xx + 11 + points[i][0] * 12, dy = yy + 8 + points[i][1] * 7;
      p.disc(dx, dy, 3, c(i % 2 ? Paint(192, 76, 67) : Paint(46, 115, 147)));
      p.disc(dx, dy, 1, c(Paint(249, 232, 191)));
    }
  } else if (symbol <= 12) {
    int n = symbol - 6;
    for (int i = 0; i < n; ++i) {
      int dx = xx + 9 + i % 3 * 8, dy = yy + 6 + i / 3 * 12;
      p.rect(dx, dy, 3, 10, c(Paint(44, 119, 88)));
      p.line(dx - 1, dy + 3, dx + 3, dy + 3, c(Paint(126, 167, 96)));
    }
  } else {
    const char *winds[] = {"N", "L", "S", "O"};
    p.centered(xx + 17, yy + 7, winds[(symbol - 13) % 4], c(Paint(182, 67, 61)), 2);
    p.line(xx + 10, yy + 25, xx + 24, yy + 25, c(Paint(182, 67, 61)));
  }
  p.centered(xx + 17, yy + 29,
             std::to_string(symbol <= 6    ? symbol
                            : symbol <= 12 ? symbol - 6
                                           : symbol - 12),
             c(Paint(106, 119, 102)));
}
inline void render(SDL_Renderer *r, const Engine &e, bool help, const std::string &notice) {
  const auto &s = e.s;
  Canvas p(r);
  SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
  p.paint(Paint(29, 74, 65));
  SDL_RenderClear(r);
  p.round(7, 43, 333, 196, 12, Paint(23, 62, 56));
  p.text(17, 12, "MAHJONG", Paint(251, 234, 190), 2);
  p.text(351, 13, "PARES", Paint(176, 202, 176));
  p.text(351, 27, std::to_string(s.score / 2) + "/16", Paint(251, 234, 190));
  int available = 0;
  for (int i = 0; i < 32; ++i)
    for (int j = i + 1; j < 32; ++j)
      if (e.tileFree(i) && e.tileFree(j) && s.a[i] == s.a[j])
        ++available;
  p.text(351, 59, "DISPONIVEIS", Paint(176, 202, 176));
  p.text(351, 76, std::to_string(available), Paint(251, 234, 190), 2);
  p.round(351, 101, 108, 5, 2, Paint(58, 105, 88));
  p.round(351, 101, std::max(1, s.score * 108 / 32), 5, 2, Paint(230, 185, 86));
  p.text(351, 124, "A MARCAR", Paint(239, 234, 207));
  p.text(351, 141, "B CANCELAR", Paint(239, 234, 207));
  p.text(351, 158, "Y EXTRAS", Paint(239, 234, 207));
  p.text(351, 175, "", Paint(239, 234, 207));
  p.text(351, 204, "PARES IGUAIS", Paint(176, 202, 176));
  p.text(351, 219, "LADO LIVRE", Paint(176, 202, 176));
  // Draw the raised selection last so its rim stays visible.
  for (int pass = 0; pass < 2; ++pass)
    for (int i = 0; i < 32; ++i) {
      if (!s.a[i] || ((i == s.selected) != (pass == 1)))
        continue;
      auto xy = location(i);
      bool selected = i == s.selected, focus = i == s.cursor;
      float lift = selected ? 6 : focus ? 2 : 0;
      if (focus && s.f[1] > 0)
        xy.first += std::sin(s.f[1] * 60) * 2;
      bool hint = s.f[2] > 0 && (i == s.a[104] || i == s.a[105]);
      tile(p, xy.first, xy.second - lift, s.a[i], e.tileFree(i), focus || hint, selected);
    }
  if (!s.a[s.cursor]) {
    auto xy = location(s.cursor);
    p.round(int(xy.first) - 2, int(xy.second) - 2, 38, 42, 5, Paint(247, 213, 132, 90));
  }
  if (s.f[0] > 0 && s.a[102] > 0) {
    float t = 1 - s.f[0] / .42f, ease = t * t * (3 - 2 * t);
    auto a = location(s.a[100]), b = location(s.a[101]);
    float middleX = (a.first + b.first) * .5f, middleY = (a.second + b.second) * .5f - 12;
    float fade = std::min(1.f, s.f[0] / .13f);
    tile(p, a.first + (middleX - 19 - a.first) * ease,
         a.second + (middleY - a.second) * ease - std::sin(t * 3.14159f) * 13, s.a[102], true,
         false, true, fade);
    tile(p, b.first + (middleX + 19 - b.first) * ease,
         b.second + (middleY - b.second) * ease - std::sin(t * 3.14159f) * 13, s.a[102], true,
         false, true, fade);
    if (t > .45f)
      for (int i = 0; i < 7; ++i) {
        float angle = i * .8976f;
        float radius = (t - .45f) * 55;
        p.disc(middleX + 17 + std::cos(angle) * radius, middleY + 18 + std::sin(angle) * radius, 2,
               Paint(255, 218, 117, int(230 * fade)));
      }
  }
  std::string message = s.f[1] > 0 ? (s.a[103] == 1 ? "PECA BLOQUEADA: LIBERE UM DOS LADOS"
                                                    : "ESCOLHA DUAS PECAS COM O MESMO SIMBOLO")
                        : !available && !s.phase ? "SEM PARES LIVRES: Y EXTRAS OU START MENU"
                        : s.f[2] > 0             ? "DICA: ESTE PAR ESTA LIVRE"
                                                 : "COMBINE PECAS IGUAIS COM UM LADO LIVRE";
  p.centered(240, 244, message, Paint(239, 224, 183));
  p.centered(240, 260, "START MENU   SELECT SALVAR E SAIR", Paint(160, 191, 166));
  if (s.phase && s.f[0] < .04f) {
    p.round(63, 87, 354, 119, 12, Paint(13, 45, 42, 248));
    p.centered(240, 101, "MESA COMPLETA!", Paint(248, 212, 126), 2);
    p.centered(240, 137, "16 PARES ENCONTRADOS", Paint(245, 240, 216));
    p.centered(240, 165, "A NOVA MESA   Y EXTRAS", Paint(189, 219, 185));
  }
  if (help) {
    p.round(39, 67, 402, 150, 12, Paint(13, 45, 42, 248));
    p.centered(240, 81, "COMO JOGAR", Paint(248, 212, 126), 2);
    p.centered(240, 114, "ESCOLHA DUAS PECAS IGUAIS COM A", Paint(245, 240, 216));
    p.centered(240, 136, "UM LADO DA PECA PRECISA ESTAR LIVRE", Paint(245, 240, 216));
    p.centered(240, 158, "Y EXTRAS      B CANCELAR", Paint(245, 240, 216));
  }
  if (!notice.empty())
    p.centered(240, 230, notice, Paint(248, 212, 126));
  SDL_RenderPresent(r);
}
} // namespace mahjong_art
} // namespace fmg
