#pragma once
#include <SDL.h>
#include <algorithm>
#include <cmath>
#include <string>
namespace tiles {
namespace ui {
struct Paint {
  Uint8 r, g, b, a;
  Paint(int red, int green, int blue, int alpha = 255)
      : r(Uint8(red)), g(Uint8(green)), b(Uint8(blue)), a(Uint8(alpha)) {}
};
static const Paint cream(247, 244, 232), paper(255, 253, 247), ink(32, 67, 62),
    mint(29, 130, 108), muted(104, 125, 114), orange(230, 121, 53),
    liquids[] = {Paint(0, 0, 0), Paint(228, 88, 101), Paint(70, 162, 192),
                 Paint(245, 186, 56), Paint(145, 100, 178)};
struct Canvas {
  SDL_Renderer *r;
  explicit Canvas(SDL_Renderer *renderer) : r(renderer) {}
  void paint(Paint p) { SDL_SetRenderDrawColor(r, p.r, p.g, p.b, p.a); }
  void rect(int x, int y, int w, int h, Paint p) {
    SDL_Rect a{x, y, w, h};
    paint(p);
    SDL_RenderFillRect(r, &a);
  }
  void line(float x, float y, float xx, float yy, Paint p) {
    paint(p);
    SDL_RenderDrawLine(r, int(std::round(x)), int(std::round(y)),
                       int(std::round(xx)), int(std::round(yy)));
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
        offset =
            radius -
            int(std::sqrt(std::max(0.f, float(radius * radius) - dy * dy)));
      } else if (row >= h - radius) {
        float dy = float(row - (h - radius)) + .5f;
        offset =
            radius -
            int(std::sqrt(std::max(0.f, float(radius * radius) - dy * dy)));
      }
      line(x + offset, y + row, x + w - 1 - offset, y + row, p);
    }
  }
  static const unsigned char *font(char ch) {
    static const unsigned char letters[][7] = {
        {14, 17, 17, 31, 17, 17, 17}, {30, 17, 17, 30, 17, 17, 30},
        {14, 17, 16, 16, 16, 17, 14}, {30, 17, 17, 17, 17, 17, 30},
        {31, 16, 16, 30, 16, 16, 31}, {31, 16, 16, 30, 16, 16, 16},
        {14, 17, 16, 23, 17, 17, 15}, {17, 17, 17, 31, 17, 17, 17},
        {31, 4, 4, 4, 4, 4, 31},      {7, 2, 2, 2, 18, 18, 12},
        {17, 18, 20, 24, 20, 18, 17}, {16, 16, 16, 16, 16, 16, 31},
        {17, 27, 21, 21, 17, 17, 17}, {17, 25, 21, 19, 17, 17, 17},
        {14, 17, 17, 17, 17, 17, 14}, {30, 17, 17, 30, 16, 16, 16},
        {14, 17, 17, 17, 21, 18, 13}, {30, 17, 17, 30, 20, 18, 17},
        {15, 16, 16, 14, 1, 1, 30},   {31, 4, 4, 4, 4, 4, 4},
        {17, 17, 17, 17, 17, 17, 14}, {17, 17, 17, 17, 17, 10, 4},
        {17, 17, 17, 21, 21, 21, 10}, {17, 17, 10, 4, 10, 17, 17},
        {17, 17, 10, 4, 4, 4, 4},     {31, 1, 2, 4, 8, 16, 31},
        {14, 17, 19, 21, 25, 17, 14}, {4, 12, 4, 4, 4, 4, 14},
        {14, 17, 1, 2, 4, 8, 31},     {30, 1, 1, 14, 1, 1, 30},
        {2, 6, 10, 18, 31, 2, 2},     {31, 16, 16, 30, 1, 1, 30},
        {14, 16, 16, 30, 17, 17, 14}, {31, 1, 2, 4, 8, 8, 8},
        {14, 17, 17, 14, 17, 17, 14}, {14, 17, 17, 15, 1, 1, 14},
        {0, 0, 0, 0, 0, 0, 0},        {4, 4, 4, 4, 4, 0, 4},
        {0, 4, 4, 0, 4, 4, 0},        {0, 0, 0, 31, 0, 0, 0},
        {1, 2, 2, 4, 8, 8, 16},       {0, 4, 4, 31, 4, 4, 0},
        {16, 8, 4, 2, 4, 8, 16},      {1, 2, 4, 8, 4, 2, 1}};
    int i = ch >= 'A' && ch <= 'Z'   ? ch - 'A'
            : ch >= '0' && ch <= '9' ? 26 + ch - '0'
                                     : 36;
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
} // namespace ui
} // namespace tiles
