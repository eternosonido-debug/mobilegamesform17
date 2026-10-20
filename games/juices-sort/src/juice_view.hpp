#pragma once
#include "juice_motion.hpp"
#include <SDL.h>
namespace fmg {
namespace juice_art {
struct Paint {
  Uint8 r, g, b, a;
  Paint(int red, int green, int blue, int alpha = 255)
      : r(Uint8(red)), g(Uint8(green)), b(Uint8(blue)), a(Uint8(alpha)) {}
};
static const Paint cream(247, 244, 232), paper(255, 253, 247), ink(32, 67, 62), mint(29, 130, 108),
    muted(104, 125, 114), orange(230, 121, 53),
    liquids[] = {Paint(0, 0, 0), Paint(228, 88, 101), Paint(70, 162, 192), Paint(245, 186, 56),
                 Paint(145, 100, 178)};
struct Canvas {
  SDL_Renderer *r;
  explicit Canvas(SDL_Renderer *renderer) : r(renderer) {}
  void paint(Paint p) {
    SDL_SetRenderDrawColor(r, p.r, p.g, p.b, p.a);
  }
  void rect(int x, int y, int w, int h, Paint p) {
    SDL_Rect a{x, y, w, h};
    paint(p);
    SDL_RenderFillRect(r, &a);
  }
  void line(float x, float y, float xx, float yy, Paint p) {
    paint(p);
    SDL_RenderDrawLine(r, int(std::round(x)), int(std::round(y)), int(std::round(xx)),
                       int(std::round(yy)));
  }
  void disc(float x, float y, int radius, Paint p) {
    for (int dy = -radius; dy <= radius; ++dy) {
      int dx = int(std::sqrt(float(radius * radius - dy * dy)));
      line(x - dx, y + dy, x + dx, y + dy, p);
    }
  }
  void round(int x, int y, int w, int h, int radius, Paint p) {
    radius = std::min(radius, std::min(w, h) / 2);
    for (int row = 0; row < h; ++row) {
      int offset = 0;
      if (row < radius) {
        float dy = float(radius - row) - .5f;
        offset = radius - int(std::sqrt(std::max(0.f, float(radius * radius) - dy * dy)));
      } else if (row >= h - radius) {
        float dy = float(row - (h - radius)) + .5f;
        offset = radius - int(std::sqrt(std::max(0.f, float(radius * radius) - dy * dy)));
      }
      line(x + offset, y + row, x + w - 1 - offset, y + row, p);
    }
  }
  static const unsigned char *font(char ch) {
    static const unsigned char letters[][7] = {
        {14, 17, 17, 31, 17, 17, 17}, {30, 17, 17, 30, 17, 17, 30}, {14, 17, 16, 16, 16, 17, 14},
        {30, 17, 17, 17, 17, 17, 30}, {31, 16, 16, 30, 16, 16, 31}, {31, 16, 16, 30, 16, 16, 16},
        {14, 17, 16, 23, 17, 17, 15}, {17, 17, 17, 31, 17, 17, 17}, {31, 4, 4, 4, 4, 4, 31},
        {7, 2, 2, 2, 18, 18, 12},     {17, 18, 20, 24, 20, 18, 17}, {16, 16, 16, 16, 16, 16, 31},
        {17, 27, 21, 21, 17, 17, 17}, {17, 25, 21, 19, 17, 17, 17}, {14, 17, 17, 17, 17, 17, 14},
        {30, 17, 17, 30, 16, 16, 16}, {14, 17, 17, 17, 21, 18, 13}, {30, 17, 17, 30, 20, 18, 17},
        {15, 16, 16, 14, 1, 1, 30},   {31, 4, 4, 4, 4, 4, 4},       {17, 17, 17, 17, 17, 17, 14},
        {17, 17, 17, 17, 17, 10, 4},  {17, 17, 17, 21, 21, 21, 10}, {17, 17, 10, 4, 10, 17, 17},
        {17, 17, 10, 4, 4, 4, 4},     {31, 1, 2, 4, 8, 16, 31},     {14, 17, 19, 21, 25, 17, 14},
        {4, 12, 4, 4, 4, 4, 14},      {14, 17, 1, 2, 4, 8, 31},     {30, 1, 1, 14, 1, 1, 30},
        {2, 6, 10, 18, 31, 2, 2},     {31, 16, 16, 30, 1, 1, 30},   {14, 16, 16, 30, 17, 17, 14},
        {31, 1, 2, 4, 8, 8, 8},       {14, 17, 17, 14, 17, 17, 14}, {14, 17, 17, 15, 1, 1, 14},
        {0, 0, 0, 0, 0, 0, 0},        {4, 4, 4, 4, 4, 0, 4},        {0, 4, 4, 0, 4, 4, 0},
        {0, 0, 0, 31, 0, 0, 0},       {1, 2, 2, 4, 8, 8, 16},       {0, 4, 4, 31, 4, 4, 0},
        {16, 8, 4, 2, 4, 8, 16},      {1, 2, 4, 8, 4, 2, 1}};
    int i = ch >= 'A' && ch <= 'Z' ? ch - 'A' : ch >= '0' && ch <= '9' ? 26 + ch - '0' : 36;
    switch (ch) {
    case '!':
      i = 37;
      break;
    case ':':
      i = 38;
      break;
    case '-':
      i = 39;
      break;
    case '/':
      i = 40;
      break;
    case '+':
      i = 41;
      break;
    case '<':
      i = 43;
      break;
    case '>':
      i = 42;
      break;
    default:
      break;
    }
    return letters[i];
  }
  void text(int x, int y, const std::string &s, Paint p, int scale = 1) {
    for (char ch : s) {
      auto bits = font(ch);
      for (int row = 0; row < 7; row++)
        for (int col = 0; col < 5; col++)
          if (bits[row] & (1 << (4 - col)))
            rect(x + col * scale, y + row * scale, scale, scale, p);
      x += 6 * scale;
    }
  }
  void centered(int cx, int y, const std::string &s, Paint p, int scale = 1) {
    text(cx - int(s.size()) * 3 * scale + scale / 2, y, s, p, scale);
  }
};
struct Transform {
  float cx, bottom, c, s;
  Transform(float x, float y, float angle)
      : cx(x), bottom(y), c(std::cos(angle)), s(std::sin(angle)) {}
  SDL_FPoint point(float x, float y) const {
    return {cx + x * c - y * s, bottom + x * s + y * c};
  }
  void fillLine(Canvas &p, float x, float y, float xx, float yy, Paint color) const {
    auto a = point(x, y), b = point(xx, yy);
    // Cover the pixel grid even when adjacent rotated scan lines round differently.
    p.line(a.x, a.y, b.x, b.y, color);
    p.line(a.x + 1, a.y, b.x + 1, b.y, color);
    p.line(a.x, a.y + 1, b.x, b.y + 1, color);
  }
  void line(Canvas &p, float x, float y, float xx, float yy, Paint color) const {
    auto a = point(x, y), b = point(xx, yy);
    p.line(a.x, a.y, b.x, b.y, color);
  }
};
inline bool complete(const State &s, int i) {
  if (s.a[40 + i] != 4)
    return false;
  for (int j = 1; j < 4; j++)
    if (s.a[4 * i + j] != s.a[4 * i])
      return false;
  return true;
}
inline void glass(Canvas &p, const State &s, int i, float fill, float lift, float angle,
                  float shake, int incoming = 0, float shiftX = 0) {
  float cx = 49.f + i * 76.f + shake + shiftX, bottom = 211.f - lift;
  Transform t(cx, bottom, angle);
  const Paint outline(39, 54, 51);
  p.round(49 + i * 76 - 23, 215, 46, 6, 3, Paint(204, 201, 182, 120));
  // Rounded silhouette and two-tone glass, drawn in local coordinates so the
  // markings and rim travel with the tube while pouring.
  for (int y = -124; y <= -2; ++y) {
    float half = y <= -22 ? 20.f : std::sqrt(std::max(0.f, 400.f - (y + 22.f) * (y + 22.f)));
    t.fillLine(p, -half, float(y), half, float(y), outline);
  }
  for (int y = -121; y <= -5; ++y) {
    float half = y <= -22 ? 17.f : std::sqrt(std::max(0.f, 289.f - (y + 22.f) * (y + 22.f)));
    t.fillLine(p, -half, float(y), 0, float(y), Paint(207, 210, 210));
    t.fillLine(p, 0, float(y), half, float(y), Paint(231, 233, 233));
  }
  const int units = s.a[40 + i];
  int height = int(fill * 25.f);
  for (int row = 0; row < height; ++row) {
    int segment = std::min(3, row / 25);
    int id = segment < units ? s.a[i * 4 + segment] : incoming;
    if (id < 1 || id > 4)
      continue;
    float y = -5.f - row;
    float half = y <= -22 ? 17.f : std::sqrt(std::max(0.f, 289.f - (y + 22.f) * (y + 22.f)));
    Paint base = liquids[id];
    Paint shade(int(base.r) * .78f, int(base.g) * .78f, int(base.b) * .78f);
    t.fillLine(p, -half, y, 0, y, shade);
    t.fillLine(p, 0, y, half, y, base);
  }
  // Graduation marks match the reference and remain separate from color labels.
  for (int y = -102, mark = 0; y <= -28; y += 14, ++mark)
    for (int w = 0; w < 2; ++w)
      t.line(p, mark % 2 ? 10 : 6, float(y + w), 19, float(y + w), outline);
  if (fill > 0)
    for (int w = 0; w < 2; ++w)
      t.line(p, -17, -5 - height + w, 17, -5 - height + w, outline);
  for (int segment = 0; segment < int(std::ceil(fill)); ++segment) {
    if (fill < float(segment) + .64f)
      continue;
    int id = segment < units ? s.a[i * 4 + segment] : incoming;
    if (id < 1 || id > 4)
      continue;
    auto at = t.point(-6, -18.f - segment * 25);
    p.disc(at.x, at.y, 5, Paint(255, 255, 255, 225));
    p.text(int(at.x) - 2, int(at.y) - 3, std::to_string(id), ink);
  }
  // Thick capsule lip with a dark and light grey inset.
  for (int y = -129; y <= -119; ++y) {
    float dy = y + 124.f;
    float half = 21.f + std::sqrt(std::max(0.f, 25.f - dy * dy));
    t.fillLine(p, -half, float(y), half, float(y), outline);
  }
  for (int y = -127; y <= -121; ++y) {
    float dy = y + 124.f;
    float half = 21.f + std::sqrt(std::max(0.f, 9.f - dy * dy));
    t.fillLine(p, -half, float(y), 0, float(y), Paint(88, 91, 92));
    t.fillLine(p, 0, float(y), half, float(y), Paint(151, 154, 155));
  }
}
inline void button(Canvas &p, int x, char letter, const std::string &label, Paint tint) {
  p.round(x, 241, 21, 20, 7, tint);
  p.text(x + 6, 247, std::string(1, letter), paper);
  p.text(x + 27, 247, label, ink);
}
inline void render(SDL_Renderer *renderer, const Engine &e, const JuiceMotion &motion, bool help,
                   const std::string &notice) {
  Canvas p(renderer);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  p.paint(cream);
  SDL_RenderClear(renderer);
  // Warm juice-bar palette; all graphics are procedural and remain sharp at 480 x 272.
  p.disc(464, 8, 42, Paint(249, 221, 171));
  p.disc(4, 270, 38, Paint(220, 233, 211));
  p.text(19, 15, "JUICES SORT", ink, 2);
  int level = juiceLevel(e.s);
  p.text(20, 40,
         level < 0 ? "PARTIDA CLASSICA"
                   : "FASE " + std::to_string(level + 1) + "/24  " +
                         (level < 8    ? "INICIANTE"
                          : level < 16 ? "INTERMEDIARIO"
                                       : "AVANCADO"),
         muted);
  p.disc(173, 22, 10, orange);
  p.disc(173, 22, 7, Paint(255, 211, 117));
  for (int i = 0; i < 5; i++) {
    float a = float(i) * 1.256637f;
    p.line(173, 22, 173 + 6 * std::cos(a), 22 + 6 * std::sin(a), paper);
  }
  p.line(178, 12, 184, 7, mint);
  p.line(179, 11, 184, 10, mint);
  p.round(329, 11, 136, 38, 12, Paint(230, 235, 218));
  p.text(341, 20, "JOGADAS", muted);
  p.text(420, 19, std::to_string(e.s.score), ink, 2);
  p.round(14, 67, 452, 166, 17, Paint(218, 212, 193, 140));
  p.round(12, 64, 456, 166, 17, paper);
  int selected = e.s.selected;
  if (motion.active)
    selected = motion.from;
  for (int i = 0; i < 6; i++) {
    int cx = 49 + i * 76;
    if (i == e.s.cursor) {
      p.round(cx - 30, 78, 60, 145, 15, Paint(222, 240, 227));
      p.round(cx - 12, 222, 24, 3, 1, mint);
    }
  }

  // Bring the source lip directly over the destination; share this pose with the stream.
  auto pourPose = [&]() {
    float direction = motion.to > motion.from ? 1.f : -1.f;
    float travel = motion.lift();
    float angle = direction * 1.05f * travel;
    float lipX = direction * 18 * std::cos(direction * 1.05f) + 124 * std::sin(direction * 1.05f);
    float lipY = direction * 18 * std::sin(direction * 1.05f) - 124 * std::cos(direction * 1.05f);
    float targetX = 49.f + motion.to * 76.f - lipX;
    float targetBottom = 88.f - lipY;
    float returnAmount = JuiceMotion::smooth((motion.fraction() - .76f) / .24f);
    float restBottom = 211.f - 14.f * (1 - returnAmount);
    return Transform((49.f + motion.from * 76.f) * (1 - travel) + targetX * travel,
                     restBottom * (1 - travel) + targetBottom * travel, angle);
  };
  auto draw = [&](int i) {
    float lift = selected == i ? 14.f : 0.f, shiftX = 0, angle = 0, fill = float(e.s.a[40 + i]);
    const State *shown = &e.s;
    int incoming = 0;
    if (motion.active && (i == motion.from || i == motion.to)) {
      shown = &motion.before;
      int from = motion.from;
      incoming = motion.before.a[from * 4 + motion.before.a[40 + from] - 1];
      if (i == from) {
        fill = motion.before.a[40 + i] - motion.amount * motion.poured();
        Transform pose = pourPose();
        lift = 211.f - pose.bottom;
        shiftX = pose.cx - (49.f + i * 76.f);
        angle = (motion.to > from ? 1.05f : -1.05f) * motion.lift();
      } else
        fill = motion.before.a[40 + i] + motion.amount * motion.poured();
    }
    float shake =
        motion.shakeTube == i ? std::sin(motion.shakeTime * 70) * motion.shakeTime * 9 : 0;
    glass(p, *shown, i, fill, lift, angle, shake, incoming, shiftX);
    if (complete(e.s, i) && !motion.active) {
      p.disc(49 + i * 76, 217, 5, mint);
      p.line(46 + i * 76, 217, 48 + i * 76, 219, paper);
      p.line(48 + i * 76, 219, 52 + i * 76, 215, paper);
    }
  };
  for (int i = 0; i < 6; i++)
    if (!motion.active || i != motion.from)
      draw(i);
  if (motion.active) {
    draw(motion.from);
    float progress = motion.fraction();
    if (progress > .23f && progress < .76f) {
      int from = motion.from;
      float sign = motion.to > from ? 1.f : -1.f;
      Transform t = pourPose();
      auto start = t.point(sign * 18, -124);
      float endX = 49.f + motion.to * 76.f;
      int id = motion.before.a[from * 4 + motion.before.a[40 + from] - 1];
      for (int j = 0; j < 28; j++) {
        float u = float(j) / 28, v = float(j + 1) / 28;
        auto bx = [&](float z) { return (1 - z) * start.x + z * endX; };
        auto by = [&](float z) { return (1 - z) * start.y + z * 94; };
        for (int w = -2; w <= 2; w++)
          p.line(bx(u) + w, by(u), bx(v) + w, by(v), liquids[id]);
        p.line(bx(u) - 1, by(u) - 1, bx(v) - 1, by(v) - 1, Paint(255, 255, 255, 110));
      }
      float surface =
          206 - (motion.before.a[40 + motion.to] + motion.amount * motion.poured()) * 25;
      for (int w = -2; w <= 2; w++)
        p.line(endX + w, 94, endX + w, surface - 2, liquids[id]);
      p.line(endX - 1, 94, endX - 1, surface - 2, Paint(255, 255, 255, 90));
      p.disc(endX - 3, surface - 5, 2, liquids[id]);
      p.disc(endX + 4, surface - 3, 2, liquids[id]);
    }
  }
  std::string status = motion.active ? "DESPEJANDO..."
                       : e.s.selected >= 0
                           ? "TUBO " + std::to_string(e.s.selected + 1) + " > ESCOLHA O DESTINO"
                           : "ESCOLHA UM TUBO PARA COMECAR";
  Paint statusColor = muted;
  if (motion.messageTime > 0) {
    status = motion.message;
    statusColor = orange;
  }
  p.centered(240, 231, status, statusColor);
  button(p, 13, 'A', e.s.selected >= 0 ? "DESPEJAR" : "PEGAR", mint);
  button(p, 142, 'B', "SOLTAR", Paint(126, 149, 139));
  button(p, 257, 'Y', "EXTRAS", orange);
  p.centered(240, 265, "START MENU    SELECT SALVAR E SAIR", muted);
  if (e.s.phase && !motion.active) {
    p.rect(0, 0, 480, 272, Paint(30, 53, 44, 80));
    for (int i = 0; i < 18; i++) {
      float x = 22.f + float(i * 29 % 440),
            y = 25.f + std::fmod(motion.clock * 18 + i * 37.f, 210.f);
      p.rect(int(x + std::sin(motion.clock + i) * 5), int(y), 3, 5, liquids[1 + i % 4]);
    }
    p.round(48, 65, 384, 150, 20, Paint(201, 190, 165, 160));
    p.round(48, 61, 384, 150, 20, paper);
    p.centered(240, 83, "MUITO BOM!", mint, 3);
    p.centered(240, 118, level == 23 ? "24 FASES COMPLETAS!" : "TODOS OS SABORES ORGANIZADOS", ink);
    int stars = level < 0 ? 0
                          : (e.s.score <= juicePar[level]       ? 3
                             : e.s.score <= juicePar[level] + 3 ? 2
                                                                : 1);
    p.centered(240, 137,
               std::to_string(e.s.score) + " JOGADAS" +
                   (stars ? "   " + std::to_string(stars) + "/3 MEDALHAS" : ""),
               muted);
    p.round(70, 162, 195, 29, 10, mint);
    p.text(82, 172, level == 23 ? "A VER FASES" : "A PROXIMA FASE", paper);
    p.text(280, 172, "B REPETIR", muted);
  }
  if (motion.panel && !help) {
    p.rect(0, 0, 480, 272, Paint(30, 53, 44, 100));
    p.round(45, 22, 390, 229, 18, paper);
    if (motion.panel == 1) {
      p.centered(240, 37, "PAUSA", mint, 2);
      const char *items[] = {"PEDIR DICA", "REINICIAR FASE", "ESCOLHER FASE",
                             "COMO JOGAR", "DESFAZER",       "CONTINUAR"};
      for (int i = 0; i < 6; ++i) {
        if (motion.option == i)
          p.round(70, 62 + i * 25, 340, 26, 7, mint);
        p.centered(240, 70 + i * 25, items[i], motion.option == i ? paper : ink);
      }
      p.centered(240, 229, "CIMA/BAIXO ESCOLHER  A OK  B VOLTAR", muted);
    } else if (motion.panel == 2) {
      int n = motion.browse;
      p.centered(240, 39, "ESCOLHER FASE", mint, 2);
      p.centered(240, 78, "<  FASE " + std::to_string(n + 1) + " / 24  >", ink, 2);
      p.centered(240, 112, n < 8 ? "INICIANTE" : n < 16 ? "INTERMEDIARIO" : "AVANCADO", muted);
      int best = motion.progress.best[n];
      p.centered(240, 137,
                 best ? "RECORDE: " + std::to_string(best) + " JOGADAS" : "AINDA NAO CONCLUIDA",
                 ink);
      p.centered(240, 159, "MEDALHAS: " + std::to_string(motion.progress.stars(n)) + "/3", orange);
      p.centered(240, 184, "3 MEDALHAS: ATE " + std::to_string(juicePar[n]) + " JOGADAS", muted);
      p.centered(240, 210,
                 std::to_string(motion.progress.unlocked() + 1) +
                     (motion.progress.unlocked() == 0 ? " FASE DISPONIVEL" : " FASES DISPONIVEIS"),
                 muted);
      p.centered(240, 231, "ESQUERDA/DIREITA  A JOGAR  B VOLTAR", ink);
    } else {
      p.centered(240, 64, "REINICIAR?", mint, 2);
      p.centered(240, 111, "AS JOGADAS DESTA PARTIDA SERAO APAGADAS", ink);
      p.centered(240, 139, "SEUS RECORDES SERAO MANTIDOS", muted);
      p.centered(240, 193, "A REINICIAR    B CANCELAR", ink);
    }
  }
  if (help) {
    p.rect(0, 0, 480, 272, Paint(30, 53, 44, 90));
    p.round(22, 26, 436, 220, 20, paper);
    p.text(45, 46, "COMO JOGAR", mint, 2);
    p.text(45, 81, "JUNTE AS CORES EM TUBOS CHEIOS", ink, 2);
    p.text(45, 111, "A ESCOLHE E DESPEJA", ink, 2);
    p.text(45, 141, "SO A MESMA COR NO TOPO", ink, 2);
    p.text(45, 171, "B CANCELA    Y EXTRAS", ink, 2);
    p.round(117, 207, 246, 23, 8, mint);
    p.centered(240, 215, "X OU B PARA VOLTAR", paper);
  }
  if (!notice.empty()) {
    p.rect(0, 263, 480, 9, Paint(255, 245, 225));
    p.text(5, 264, notice, Paint(170, 62, 50));
  }
  SDL_RenderPresent(renderer);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
} // namespace juice_art
} // namespace fmg
