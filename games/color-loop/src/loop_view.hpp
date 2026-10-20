#pragma once
#include "juice_view.hpp"
namespace fmg {
namespace loop_art {
using juice_art::Canvas;
using juice_art::Paint;
inline Paint tint(int color) {
  static const Paint colors[] = {Paint(77, 95, 113), Paint(233, 114, 120), Paint(67, 175, 205),
                                 Paint(245, 193, 75), Paint(159, 125, 213)};
  return colors[color % 5];
}
inline std::pair<float, float> position(float progress, int h) {
  const float pi = 3.14159265f, radius = 14, top = 61, left = 39, right = 235, bottom = 213;
  const float horizontal = right - left - 2 * radius, vertical = bottom - top - 2 * radius,
              arc = pi * radius * .5f;
  float perimeter = 2 * (horizontal + vertical) + 4 * arc;
  float t = std::fmod(progress, float(12 + 2 * h));
  if (t < 0)
    t += 12 + 2 * h;
  float distance = t / (12 + 2 * h) * perimeter;
  for (int side = 0; side < 4; ++side) {
    float straight = side % 2 ? vertical : horizontal;
    if (distance < straight) {
      if (side == 0)
        return {left + radius + distance, top};
      if (side == 1)
        return {right, top + radius + distance};
      if (side == 2)
        return {right - radius - distance, bottom};
      return {left, bottom - radius - distance};
    }
    distance -= straight;
    if (distance < arc) {
      float angle = (-.5f + side * .5f) * pi + distance / radius;
      float cx = side == 0 || side == 1 ? right - radius : left + radius;
      float cy = side < 1 || side == 3 ? top + radius : bottom - radius;
      return {cx + std::cos(angle) * radius, cy + std::sin(angle) * radius};
    }
    distance -= arc;
  }
  return {left + radius, top};
}
inline void hopper(Canvas &p, float x, float y, int color, int amount, bool selected = false,
                   float clock = 0, float reaction = 0, float opacity = 1) {
  if (selected)
    y -= 6;
  Paint body = tint(color), dark(int(body.r * .78f), int(body.g * .78f), int(body.b * .78f));
  auto alpha = [&](Paint c) {
    c.a = Uint8(c.a * opacity);
    return c;
  };
  y += std::sin(clock * 5 + color) * .65f;
  float squash = std::sin(std::min(1.f, reaction) * 3.14159f) * 2;
  int xx = int(x), yy = int(y), w = 28 + int(squash), height = 27 - int(squash);
  if (selected) {
    p.round(xx - 21, yy - 25, 42, 50, 12, alpha(Paint(48, 76, 77)));
    p.round(xx - 19, yy - 23, 38, 46, 11, alpha(Paint(255, 198, 64)));
    p.round(xx - 16, yy - 20, 32, 40, 10, alpha(Paint(255, 248, 209)));
    int arrowY = yy - 30 - int(std::sin(clock * 5) * 2);
    for (int row = 0; row < 6; ++row)
      p.line(xx - 6 + row, arrowY + row, xx + 6 - row, arrowY + row, Paint(172, 104, 24));
  }
  p.round(xx - 12, yy + 10, 26, 8, 5, alpha(Paint(37, 65, 67, 65)));
  // Ears and feet give each collector a soft, toy-like silhouette.
  if (color == 1) {
    p.round(xx - 11, yy - 23, 7, 16, 4, alpha(dark));
    p.round(xx + 4, yy - 23, 7, 16, 4, alpha(dark));
    p.round(xx - 9, yy - 21, 3, 10, 2, alpha(Paint(255, 185, 183)));
    p.round(xx + 6, yy - 21, 3, 10, 2, alpha(Paint(255, 185, 183)));
  } else {
    p.disc(x - 10, y - 13, color == 3 ? 4 : 6, alpha(dark));
    p.disc(x + 10, y - 13, color == 3 ? 4 : 6, alpha(dark));
    p.disc(x - 10, y - 14, 3, alpha(body));
    p.disc(x + 10, y - 14, 3, alpha(body));
  }
  p.round(xx - 12, yy + 9, 9, 7, 4, alpha(dark));
  p.round(xx + 4, yy + 9, 9, 7, 4, alpha(dark));
  p.round(xx - w / 2, yy - 15, w, height, 11, alpha(dark));
  p.round(xx - w / 2, yy - 17, w, height - 2, 11, alpha(body));
  p.round(xx - 8, yy - 14, 15, 3, 2, alpha(Paint(255, 255, 240, 90)));
  bool blink = std::fmod(clock + color * .63f, 4.f) < .12f;
  if (blink) {
    p.line(x - 8, y - 6, x - 3, y - 6, alpha(Paint(37, 62, 69)));
    p.line(x + 3, y - 6, x + 8, y - 6, alpha(Paint(37, 62, 69)));
  } else
    for (int sign : {-1, 1}) {
      p.disc(x + sign * 5, y - 7, 4, alpha(Paint(255, 255, 245)));
      p.disc(x + sign * 5, y - 6, 2, alpha(Paint(36, 61, 74)));
      p.disc(x + sign * 5 - 1, y - 7, 1, alpha(Paint(255, 255, 255)));
    }
  p.disc(x - 10, y - 1, 2, alpha(Paint(255, 190, 175, 160)));
  p.disc(x + 10, y - 1, 2, alpha(Paint(255, 190, 175, 160)));
  if (reaction > .1f)
    p.disc(x, y, 3, alpha(Paint(65, 65, 79)));
  else {
    p.line(x - 3, y - 1, x, y + 1, alpha(Paint(54, 66, 76)));
    p.line(x, y + 1, x + 3, y - 1, alpha(Paint(54, 66, 76)));
  }
  p.round(xx - 6, yy + 5, 13, 12, 5, alpha(Paint(255, 249, 218)));
  p.centered(xx, yy + 7, std::to_string(amount), alpha(Paint(46, 75, 81)));
}
inline void render(SDL_Renderer *r, const Engine &e, bool help, const std::string &notice) {
  const State &s = e.s;
  Canvas p(r);
  SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
  p.paint(Paint(239, 241, 223));
  SDL_RenderClear(r);
  p.text(15, 10, "COLOR LOOP", Paint(36, 75, 78), 2);
  p.text(289, 10, "FASE " + std::to_string(s.a[200] + 1) + "/24", Paint(36, 75, 78));
  p.text(289, 25, "COLETA " + std::to_string(s.a[203]) + "/" + std::to_string(s.a[204]),
         Paint(89, 115, 112));
  p.round(22, 44, 230, 187, 22, Paint(177, 199, 185));
  p.round(27, 49, 220, 177, 18, Paint(71, 100, 99));
  p.round(50, 72, 174, 130, 7, Paint(215, 224, 201));
  int h = s.a[205], n = 12 + 2 * h;
  // Moving chevrons follow the same rounded path and direction as collectors.
  for (int j = 0; j < 40; ++j) {
    float progress = j * n / 40.f + s.f[40] * 6.f;
    auto xy = position(progress, h), ahead = position(progress + .035f, h);
    float dx = ahead.first - xy.first, dy = ahead.second - xy.second;
    float length = std::max(.001f, std::hypot(dx, dy));
    dx /= length;
    dy /= length;
    p.line(xy.first - dx * 3 - dy * 3, xy.second - dy * 3 + dx * 3, xy.first + dx * 2,
           xy.second + dy * 2, Paint(142, 173, 157));
    p.line(xy.first - dx * 3 + dy * 3, xy.second - dy * 3 - dx * 3, xy.first + dx * 2,
           xy.second + dy * 2, Paint(142, 173, 157));
  }
  float cellH = std::min(18.f, 120.f / h);
  for (int i = 0; i < h * 6; ++i)
    if (s.a[i]) {
      int x = 57 + (i % 6) * 27, y = 77 + int((i / 6) * cellH);
      auto c = tint(s.a[i]);
      p.round(x, y + 2, 24, int(cellH) - 2, 3, Paint(c.r * .7, c.g * .7, c.b * .7));
      p.round(x, y, 24, int(cellH) - 3, 3, c);
      p.rect(x + 4, y + 2, 10, 1, Paint(255, 255, 236, 150));
    }
  for (int i = 0; i < 4; ++i) {
    auto xy = position(s.f[i], h);
    if (s.f[30 + i] > 0) {
      int cell = s.a[210 + i];
      float t = 1 - s.f[30 + i] / .3f;
      float x = 69 + (cell % 6) * 27, y = 82 + (cell / 6) * cellH;
      p.disc(x + (xy.first - x) * t, y + (xy.second - y) * t, 4, tint(s.a[214 + i]));
    }
    if (s.a[130 + i]) {
      if (s.f[44 + i] > 0) {
        float t = 1 - s.f[44 + i] / .32f;
        t = t * t * (3 - 2 * t);
        int source = s.a[224 + i];
        float x = 291 + (source % 4) * 48, y = source < 4 ? 143 : 209;
        xy.first = x + (xy.first - x) * t;
        xy.second = y + (xy.second - y) * t - std::sin(t * 3.14159f) * 24;
      }
      hopper(p, xy.first, xy.second, s.a[130 + i], s.a[134 + i], false, s.f[40] + i * .23f,
             s.f[30 + i] / .3f);
    }
  }
  p.text(274, 47, "FILA: ESCOLHA A FRENTE", Paint(36, 75, 78));
  for (int lane = 0; lane < 4; ++lane) {
    int x = 291 + lane * 48;
    p.round(x - 18, 62, 36, 105, 7, Paint(214, 223, 202));
    // Back collectors are smaller and visually behind the selectable front.
    for (int depth = 3; depth >= 1; --depth) {
      int c = s.a[70 + lane * 4 + depth];
      if (c) {
        p.round(x - 11, 68 + (3 - depth) * 18, 22, 14, 4, tint(c));
        p.centered(x, 71 + (3 - depth) * 18, "6", Paint(34, 63, 70));
      }
    }
    int c = s.a[70 + lane * 4];
    if (c)
      hopper(p, x, 143, c, 6, s.cursor == lane, s.f[40] + lane * .2f, 0,
             s.cursor == lane ? 1.f : .68f);
    else {
      p.centered(x, 139, "-", Paint(133, 152, 137));
      if (s.cursor == lane)
        p.rect(x - 14, 157, 28, 2, Paint(36, 75, 78));
    }
  }
  p.text(274, 176, "RETORNO: REENVIE OS INCOMPLETOS", Paint(36, 75, 78));
  for (int i = 0; i < 4; ++i) {
    int x = 291 + i * 48;
    p.round(x - 18, 190, 36, 39, 7, Paint(214, 223, 202));
    bool arriving = false;
    for (int j = 0; j < 4; ++j)
      if (s.f[60 + j] > 0 && s.a[242 + j] == i + 1)
        arriving = true;
    if (s.a[150 + i] && !arriving)
      hopper(p, x, 209, s.a[150 + i], s.a[154 + i], s.cursor == i + 4, s.f[40] + i * .3f, 0,
             s.cursor == i + 4 ? 1.f : .68f);
    else if (s.cursor == i + 4)
      p.round(x - 14, 195, 28, 28, 7, Paint(177, 199, 185));
  }
  for (int i = 0; i < 4; ++i) {
    if (s.f[60 + i] > 0) {
      float t = 1 - s.f[60 + i] / .4f;
      auto origin = position(s.f[64 + i], h);
      if (s.a[242 + i]) {
        int slot = s.a[242 + i] - 1;
        float ease = t * t * (3 - 2 * t);
        hopper(p, origin.first + (291 + slot * 48 - origin.first) * ease,
               origin.second + (209 - origin.second) * ease - std::sin(t * 3.14159f) * 24,
               s.a[234 + i], s.a[238 + i], false, s.f[40]);
      } else {
        p.centered(int(origin.first), int(origin.second - 15 - t * 22), "+50",
                   Paint(40, 104, 83, int(255 * (1 - t))));
        for (int star = 0; star < 5; ++star) {
          float angle = star * 1.2566f;
          p.disc(origin.first + std::cos(angle) * (5 + t * 20),
                 origin.second + std::sin(angle) * (5 + t * 20), 2,
                 Paint(255, 211, 104, int(255 * (1 - t))));
        }
      }
    }
  }
  const char *messages[] = {
      "A ENVIAR   B CANCELAR   Y EXTRAS", "ESSE ESPACO ESTA VAZIO",    "ESTEIRA CHEIA: AGUARDE",
      "AGUARDE LIBERAR A ENTRADA",        "COLETOR COMPLETO!",         "RETORNO CHEIO",
      "COLETOR VOLTOU: REENVIE",          "ENVIO AGENDADO   B CANCELA"};
  p.text(20, 240,
         messages[s.a[220]      ? 7
                  : s.f[20] > 0 ? std::max(0, std::min(7, s.a[206]))
                                : 0],
         Paint(36, 75, 78));
  p.text(20, 258, "START MENU        SELECT SALVAR/SAIR", Paint(100, 123, 113));
  p.text(338, 258, std::to_string(s.score) + " PTS", Paint(36, 75, 78));
  if (s.phase) {
    p.round(54, 77, 372, 133, 13, Paint(30, 66, 70, 248));
    p.centered(240, 93, s.phase == 1 ? "FASE CONCLUIDA!" : "RETORNO LOTADO", Paint(255, 227, 146),
               2);
    p.centered(240, 127,
               s.phase == 1 ? (s.a[200] == 23 ? "CAMPANHA COMPLETA!" : "A PROXIMA FASE")
                            : "A TENTAR NOVAMENTE",
               Paint(244, 246, 226));
    p.centered(240, 151, "B REPETIR   Y EXTRAS", Paint(207, 227, 213));
    p.centered(240, 176, "START MENU", Paint(207, 227, 213));
  }
  if (help) {
    p.round(22, 40, 436, 198, 13, Paint(30, 66, 70, 250));
    p.centered(240, 52, "COMO JOGAR", Paint(255, 227, 146), 2);
    const char *lines[] = {"DIRECIONAL ESCOLHE FILA OU RETORNO", "A ENVIA O COLETOR PARA A ESTEIRA",
                           "ELE RECOLHE SO A SUA COR EXPOSTA",   "CHEIO, SAI E LIBERA UM ESPACO",
                           "INCOMPLETO, VOLTA APOS UMA VOLTA",   "REENVIE ANTES DE LOTAR O RETORNO",
                           "LIMPE TODOS OS BLOCOS PARA VENCER",  "Y EXTRAS   B FECHA A AJUDA"};
    for (int i = 0; i < 8; ++i)
      p.centered(240, 82 + i * 18, lines[i], Paint(232, 240, 219));
  }
  if (!notice.empty())
    p.centered(240, 231, notice, Paint(36, 75, 78));
  SDL_RenderPresent(r);
}
} // namespace loop_art
} // namespace fmg
