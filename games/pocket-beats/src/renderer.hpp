#pragma once
#include "controller.hpp"
#include "pixel.hpp"
namespace pocketbeats {
inline void render(SDL_Renderer *r, const Controller &app, int step,
                   bool audio) {
  using namespace ui;
  Canvas c(r);
  Paint bg(17, 23, 39), panel(30, 40, 60), white(231, 238, 245),
      dim(131, 149, 172), accent(101, 236, 199);
  Paint colors[] = {Paint(247, 139, 108), Paint(113, 192, 248),
                    Paint(242, 201, 94), Paint(192, 143, 244)};
  const auto &s = app.state;
  const auto &p = s.current();
  c.rect(0, 0, 240, 136, bg);
  c.text(7, 5, "POCKET BEATS", accent);
  c.text(112, 5, std::to_string(p.bpm) + " BPM", white);
  c.text(174, 5, s.playing ? "PLAY" : "STOP", s.playing ? accent : dim);
  if (s.scene == Scene::Menu) {
    c.centered(120, 27, "FOUR TRACK GROOVEBOX", white);
    const char *items[] = {"MAKE MUSIC", "HOW TO PLAY", "PLAY SAVED BEAT",
                           "SAVE AND EXIT"};
    for (int i = 0; i < 4; ++i) {
      if (s.menu == i)
        c.round(29, 44 + i * 17, 182, 14, 3, panel);
      c.centered(120, 48 + i * 17, items[i], s.menu == i ? accent : dim);
    }
    c.centered(120, 122, "A CHOOSE   SELECT SAVE EXIT", dim);
    return;
  }
  if (s.scene == Scene::Tutorial) {
    const char *lines[] = {"DIRECTIONS SELECT A PAD",
                           "A TOGGLE NOTE   START PLAY",
                           "Y CHOOSES A SETTING",
                           "LEFT RIGHT CHANGE SETTING",
                           "UP DOWN CHOOSE TRACK",
                           "B MENU   SELECT SAVE EXIT",
                           "THREE EDITABLE BEATS READY",
                           "ALL TRACKS SHARE LOOP SIZE",
                           "A OR B RETURN"};
    for (int i = 0; i < 9; ++i)
      c.centered(120, 22 + i * 12, lines[i], i == 8 ? accent : white);
    return;
  }
  if (s.scene == Scene::Paused) {
    const char *items[] = {"CONTINUE", "RESTORE THIS PRESET",
                           "CLEAR THIS PATTERN", "MAIN MENU", "SAVE AND EXIT"};
    c.centered(120, 24, "PAUSED", accent);
    for (int i = 0; i < 5; ++i) {
      if (s.pause == i)
        c.round(24, 40 + i * 16, 192, 14, 3, panel);
      c.centered(120, 44 + i * 16, items[i], s.pause == i ? accent : dim);
    }
    return;
  }
  c.text(7, 19, std::string(presets[s.slot]) + " " + std::to_string(s.slot + 1),
         dim);
  c.text(154, 19,
         std::to_string(s.page() + 1) + "/" +
             std::to_string((p.length + 7) / 8) + " PAGE",
         dim);
  for (int j = 0; j < 8; ++j) {
    int n = s.page() * 8 + j;
    if (n >= p.length)
      break;
    int x = 61 + j * 22;
    c.centered(x + 9, 31, std::to_string(n + 1), n == step ? accent : dim);
    if (n == step)
      c.rect(x, 40, 18, 2, accent);
  }
  for (int t = 0; t < 4; ++t) {
    int y = 45 + t * 17;
    c.text(3, y + 5, names[t], t == s.track ? colors[t] : dim);
    if (!p.audible(t))
      c.line(3, y + 13, 48, y + 13, colors[t]);
    for (int j = 0; j < 8; ++j) {
      int n = s.page() * 8 + j;
      if (n >= p.length)
        break;
      int x = 61 + j * 22;
      bool sel = t == s.track && n == s.cursor;
      if (sel)
        c.round(x - 2, y - 2, 22, 18, 3, white);
      c.round(x, y, 18, 14, 2, p.notes[t][n] ? colors[t] : panel);
      if (p.notes[t][n]) {
        c.rect(x + 3, y + 3, 12, 2, white);
        if (t)
          c.centered(x + 9, y + 6, std::to_string(p.notes[t][n]), bg);
      }
      if (n == step)
        c.rect(x, y + 12, 18, 2, accent);
    }
  }
  static const char *labels[] = {"PADS", "TEMPO", "LOOP", "VOLUME",
                                 "MUTE", "SOLO",  "NOTE", "PATTERN"};
  std::string value = labels[s.setting];
  switch (s.setting) {
  case 1:
    value += " " + std::to_string(p.bpm);
    break;
  case 2:
    value += " " + std::to_string(p.length);
    break;
  case 3:
    value += " " + std::to_string(p.volume[s.track]);
    break;
  case 4:
    value += p.mute[s.track] ? " ON" : " OFF";
    break;
  case 5:
    value += p.solo == s.track ? " ON" : " OFF";
    break;
  case 6:
    value += " " + std::to_string(p.notes[s.track][s.cursor]);
    break;
  case 7:
    value += " " + std::to_string(s.slot + 1);
    break;
  }
  c.text(5, 116, "Y " + value, accent);
  c.text(5, 128,
         app.message.empty()
             ? (audio ? "A PAD  START PLAY  B MENU" : "NO AUDIO DEVICE")
             : app.message,
         dim);
}
} // namespace pocketbeats
