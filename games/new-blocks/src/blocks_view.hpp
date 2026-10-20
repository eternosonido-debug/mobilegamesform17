#pragma once
#include "juice_view.hpp"
namespace fmg {
namespace blocks_art {
using juice_art::Canvas;
using juice_art::Paint;
inline Paint blockColor(int n) {
  static const Paint colors[] = {Paint(64, 170, 180),  Paint(237, 153, 70), Paint(142, 118, 205),
                                 Paint(222, 111, 136), Paint(96, 170, 119), Paint(90, 145, 217)};
  return colors[(n + 5) % 6];
}
inline void tile(Canvas &p, int x, int y, int size, Paint c) {
  p.round(x, y + 2, size, size, 4, Paint(c.r * .6, c.g * .6, c.b * .6));
  p.round(x, y, size, size - 2, 4, c);
  p.line(x + 4, y + 2, x + size - 5, y + 2,
         Paint(std::min(255, c.r + 38), std::min(255, c.g + 38), std::min(255, c.b + 38)));
}
inline void render(SDL_Renderer *renderer, const Engine &e, bool help, const std::string &notice) {
  const State &s = e.s;
  Canvas p(renderer);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  p.paint(Paint(18, 29, 46));
  SDL_RenderClear(renderer);
  p.text(18, 12, "NEW BLOCKS", Paint(239, 244, 249), 2);
  p.text(301, 12, "PONTOS", Paint(145, 168, 190));
  p.text(301, 25, std::to_string(s.score), Paint(251, 205, 108), 2);
  p.round(15, 40, 208, 208, 9, Paint(10, 20, 34));
  int id = s.a[100 + s.choice], cx = s.cursor % 8, cy = s.cursor / 8;
  bool fits = e.fits(id, cx, cy);
  for (int y = 0; y < 8; ++y)
    for (int x = 0; x < 8; ++x) {
      int px = 20 + x * 25, py = 45 + y * 25, v = s.a[y * 8 + x];
      p.round(px, py, 23, 23, 4, Paint(34, 49, 68));
      if (v)
        tile(p, px, py, 23, blockColor(v));
      if (s.f[0] > .02f && ((s.a[113] & (1 << y)) || (s.a[114] & (1 << x))))
        p.round(px, py, 23, 23, 4, Paint(254, 225, 143, int(190 * s.f[0] / .45f)));
    }
  if (!s.phase && id >= 0) {
    for (auto cell : Engine::shape(id)) {
      int x = cx + cell.first, y = cy + cell.second;
      if (x < 8 && y < 8) {
        int px = 20 + x * 25, py = 45 + y * 25;
        p.round(px, py, 23, 23, 4, fits ? Paint(111, 225, 190, 140) : Paint(243, 100, 115, 150));
        p.rect(px + 4, py + 4, 15, 2, fits ? Paint(186, 255, 223) : Paint(255, 182, 182));
      }
    }
    p.rect(20 + cx * 25, 45 + cy * 25, 23, 2, Paint(255, 244, 207));
    p.rect(20 + cx * 25, 45 + cy * 25, 2, 23, Paint(255, 244, 207));
  }
  p.text(242, 55, "SUAS PECAS", Paint(153, 177, 198));
  for (int k = 0; k < 3; ++k) {
    int y = 73 + k * 45, sh = s.a[100 + k];
    p.round(239, y, 111, 40, 6, k == s.choice ? Paint(65, 88, 110) : Paint(30, 44, 62));
    if (k == s.choice)
      p.rect(239, y + 7, 3, 26, Paint(251, 205, 108));
    if (sh >= 0)
      for (auto cell : Engine::shape(sh))
        tile(p, 251 + cell.first * 9, y + 3 + cell.second * 9, 8, blockColor(sh % 6 + 1));
    else
      p.text(257, y + 15, "USADA", Paint(112, 132, 150));
    p.text(334, y + 15, std::to_string(k + 1), Paint(172, 190, 202));
  }
  p.text(367, 65, "RECORDE", Paint(145, 168, 190));
  p.text(367, 80, std::to_string(std::max(s.score, s.a[110])), Paint(251, 205, 108));
  p.text(367, 108, "LINHAS", Paint(145, 168, 190));
  p.text(367, 123, std::to_string(s.a[112]), Paint(231, 240, 248));
  p.text(367, 153, "A COLOCAR", Paint(197, 215, 230));
  p.text(367, 168, "B PECA", Paint(197, 215, 230));
  p.text(367, 183, "Y EXTRAS", Paint(197, 215, 230));
  p.text(367, 198, "B TROCA PECA", Paint(197, 215, 230));
  std::string message = s.f[1] > 0                   ? "NAO CABE AQUI"
                        : s.f[0] > 0 && s.a[111] > 1 ? "COMBO X" + std::to_string(s.a[111])
                                                     : "COMPLETE LINHAS OU COLUNAS";
  p.text(241, 219, message, s.f[1] > 0 ? Paint(255, 141, 144) : Paint(153, 211, 188));
  p.text(19, 258, "START MENU                  SELECT SALVAR E SAIR", Paint(145, 168, 190));
  if (s.f[2] > 0 && s.a[115] > 0) {
    float progress = 1.f - std::min(.75f, s.f[2]) / .75f;
    int y = 132 - int(progress * 25);
    int alpha = int(255 * std::min(1.f, s.f[2] / .25f));
    std::string gain = "+" + std::to_string(s.a[115]);
    int width = int(gain.size()) * 12 + 20;
    p.round(120 - width / 2, y - 7, width, 30, 8, Paint(10, 21, 36, alpha * 3 / 4));
    p.centered(120, y, gain, Paint(255, 225, 145, alpha), 2);
  }
  if (s.phase) {
    p.round(64, 84, 352, 126, 12, Paint(10, 21, 36, 245));
    p.centered(240, 99, "SEM ENCAIXES", Paint(251, 205, 108), 2);
    p.centered(240, 127, "PONTOS " + std::to_string(s.score), Paint(239, 244, 249));
    p.centered(240, 150, "A NOVA PARTIDA", Paint(159, 229, 197));
    p.centered(240, 171, "Y EXTRAS   START MENU", Paint(197, 215, 230));
  }
  if (help) {
    p.round(42, 47, 396, 190, 12, Paint(10, 21, 36, 250));
    p.centered(240, 61, "COMO JOGAR", Paint(251, 205, 108), 2);
    const char *lines[] = {"DIRECIONAL: MOVER A PREVISAO",
                           "A: ENCAIXAR   B: TROCAR PECA",
                           "COMPLETE LINHAS E COLUNAS DE 8",
                           "USE AS 3 PECAS PARA RECEBER MAIS",
                           "LIMPE EM JOGADAS SEGUIDAS: COMBO",
                           "Y: EXTRAS   SELECT: SALVAR E SAIR",
                           "B OU Y: FECHAR AJUDA"};
    for (int i = 0; i < 7; ++i)
      p.centered(240, 91 + i * 19, lines[i], Paint(215, 230, 241));
  }
  if (!notice.empty())
    p.centered(240, 240, notice, Paint(251, 205, 108));
  SDL_RenderPresent(renderer);
}
} // namespace blocks_art
} // namespace fmg
