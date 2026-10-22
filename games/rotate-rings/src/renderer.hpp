#pragma once
#include "game_logic.hpp"
#include "pixel.hpp"
namespace rings {
struct Renderer {
  ui::Canvas c;
  std::array<SDL_Texture *, 5> textures{
      {nullptr, nullptr, nullptr, nullptr, nullptr}};
  float timer = 1, error = 0;
  unsigned released = 0;
  int turning = -1, direction = 0;
  State before;
  static ui::Paint color(int i) {
    const ui::Paint colors[] = {
        ui::Paint(240, 109, 125), ui::Paint(69, 193, 215),
        ui::Paint(249, 194, 78), ui::Paint(158, 130, 240),
        ui::Paint(98, 210, 141)};
    return colors[i];
  }
  explicit Renderer(SDL_Renderer *r) : c(r) {
    for (int k = 0; k < 5; ++k) {
      auto *s = SDL_CreateRGBSurfaceWithFormat(0, 48, 48, 32,
                                               SDL_PIXELFORMAT_ARGB8888);
      if (!s)
        continue;
      SDL_FillRect(s, nullptr, SDL_MapRGBA(s->format, 0, 0, 0, 0));
      for (int y = 0; y < 48; ++y)
        for (int x = 0; x < 48; ++x) {
          double dx = x - 23.5, dy = y - 23.5;
          double d = std::hypot(dx, dy);
          double theta = std::atan2(dy, dx);
          double end = gapHalf + std::asin(3.0 / radius);
          double stroke =
              distanceAngle(0, theta) > end ? std::abs(d - radius) : 100;
          for (int sign : {-1, 1})
            stroke = std::min(stroke,
                              std::hypot(dx - radius * std::cos(sign * end),
                                         dy - radius * std::sin(sign * end)));
          if (stroke > 3)
            continue;
          auto col = stroke > 2 ? ui::Paint(9, 17, 28) : color(k);
          double light =
              stroke > 2
                  ? 0
                  : std::max(-.18, std::min(.18, (17 - d) * .10 - dy * .004));
          auto channel = [&](int v) {
            return Uint8(std::max(0., std::min(255., v + light * 160)));
          };
          Uint32 pixel = SDL_MapRGBA(s->format, channel(col.r), channel(col.g),
                                     channel(col.b), 255);
          auto *row = reinterpret_cast<Uint32 *>(
              static_cast<Uint8 *>(s->pixels) + y * s->pitch);
          row[x] = pixel;
        }
      textures[k] = SDL_CreateTextureFromSurface(r, s);
      SDL_FreeSurface(s);
      if (textures[k])
        SDL_SetTextureBlendMode(textures[k], SDL_BLENDMODE_BLEND);
    }
  }
  ~Renderer() {
    for (auto *t : textures)
      SDL_DestroyTexture(t);
  }
  void animate(const State &old, const Game &g, int d) {
    before = old;
    timer = 0;
    released = old.active & ~g.state.active;
    turning = g.selected;
    direction = d;
  }
  float duration() const { return released ? .65f : .13f; }
  void draw(const Game &g) {
    using namespace ui;
    c.rect(0, 0, 240, 136, Paint(19, 27, 41));
    c.text(8, 5, "ROTATE RINGS", paper);
    c.text(165, 5, "FASE " + std::to_string(g.level + 1) + "/20", paper);
    c.round(7, 20, 142, 91, 5, Paint(30, 43, 60));
    for (int y = 26; y < 109; y += 8)
      for (int x = 13; x < 146; x += 8)
        c.rect(x, y, 1, 1, Paint(41, 56, 73));
    auto &l = levels()[g.level];
    // Keep layer order stable when the selection changes.
    for (int i = 0; i < int(l.rings.size()); ++i) {
      bool drop = (released & (1u << i)) && timer < .65f;
      if (!(g.state.active & (1u << i)) && !drop)
        continue;
      auto &a = l.rings[i];
      double rotation = g.state.angle[i] * 30.;
      if (i == turning && timer < .13f)
        rotation = before.angle[i] * 30. + direction * 30. * (timer / .13f);
      float t = drop ? std::max(0.f, (timer - .13f) / .52f) : 0;
      int x = int(a.x - 24 - 100 * t * std::cos(radians(a.pin))),
          y = int(a.y - 24 - 100 * t * std::sin(radians(a.pin)) - 18 * t * t);
      int size = 48 - int(t * 12);
      SDL_Rect dst{x + 2, y + 3, size, size};
      auto *tex = textures[i];
      if (tex) {
        SDL_SetTextureColorMod(tex, 15, 22, 32);
        SDL_SetTextureAlphaMod(tex, Uint8(200 * (1 - t)));
        SDL_RenderCopyEx(c.r, tex, nullptr, &dst, rotation, nullptr,
                         SDL_FLIP_NONE);
        dst.x -= 2;
        dst.y -= 3;
        Uint8 brightness = i == g.selected ? 255 : 190;
        SDL_SetTextureColorMod(tex, brightness, brightness, brightness);
        SDL_SetTextureAlphaMod(tex, Uint8(255 * (1 - t)));
        SDL_RenderCopyEx(c.r, tex, nullptr, &dst, rotation, nullptr,
                         SDL_FLIP_NONE);
      }
    }
    // Only remaining rings have targets; each target uses its ring's color.
    for (size_t i = 0; i < l.rings.size(); ++i) {
      if (!(g.state.active & (1u << i)))
        continue;
      auto &a = l.rings[i];
      float px = float(a.x + radius * std::cos(radians(a.pin)));
      float py = float(a.y + radius * std::sin(radians(a.pin)));
      auto col = color(int(i));
      c.disc(px, py, 3, Paint(9, 17, 28));
      c.disc(px, py, 2, col);
      if (int(i) == g.selected) {
        c.disc(px, py, 1, paper);
        for (int step = 5; step < 14; step += 4)
          c.disc(float(a.x + step * std::cos(radians(a.pin))),
                 float(a.y + step * std::sin(radians(a.pin))), 0, col);
        c.disc(float(a.x), float(a.y), 3, Paint(9, 17, 28));
        c.disc(float(a.x), float(a.y), 2, col);
      }
    }
    if (g.state.active & (1u << g.selected)) {
      auto &a = l.rings[g.selected];
      Paint sel = error > 0 ? Paint(255, 90, 80) : Paint(255, 246, 180);
      int x = int(a.x), y = int(a.y);
      for (int sign : {-1, 1}) {
        c.line(x - 23, y + 23 * sign, x - 17, y + 23 * sign, sel);
        c.line(x + 17, y + 23 * sign, x + 23, y + 23 * sign, sel);
      }
      double angle = radians(g.state.angle[g.selected]);
      // An arrow identifies the opening; dots are reserved for the posts.
      for (int side : {-1, 1})
        c.line(float(a.x + 20 * std::cos(angle)),
               float(a.y + 20 * std::sin(angle)),
               float(a.x + 25 * std::cos(angle) - side * 3 * std::sin(angle)),
               float(a.y + 25 * std::sin(angle) + side * 3 * std::cos(angle)),
               paper);
    }
    int remaining = 0;
    for (unsigned b = g.state.active; b; b &= b - 1)
      ++remaining;
    c.text(158, 25, "ANEIS", paper);
    c.text(158, 36, std::to_string(remaining) + " RESTAM",
           Paint(112, 225, 193));
    c.text(158, 53, "GIROS " + std::to_string(g.state.moves), paper);
    c.text(158, 68, "SELECIONADO", paper);
    const char *names[] = {"ROSA", "AZUL", "AMARELO", "ROXO", "VERDE"};
    c.text(158, 80, names[g.selected], color(g.selected));
    c.text(158, 96, "PINO", Paint(167, 186, 209));
    c.disc(190, 99, 3, color(g.selected));
    c.disc(190, 99, 1, paper);
    c.text(8, 117, "A GIRA >  B GIRA <  Y DESFAZ", paper);
    c.text(8, 127, "START PAUSA   SELECT SAIR", Paint(166, 185, 202));
  }
  void panel(const std::string &title, const std::vector<std::string> &rows,
             int chosen) {
    using namespace ui;
    c.rect(0, 0, 240, 136, Paint(19, 27, 41));
    c.centered(120, 12, title, paper);
    for (size_t i = 0; i < rows.size(); ++i) {
      int y = 33 + int(i) * 17;
      if (int(i) == chosen)
        c.round(19, y - 3, 202, 14, 3, Paint(56, 88, 105));
      c.text(26, y, rows[i], paper);
    }
  }
};
} // namespace rings
