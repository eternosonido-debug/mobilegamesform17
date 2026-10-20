#pragma once
#include "controller.hpp"
#include "pixel.hpp"
namespace pocketbeats {
inline void render(SDL_Renderer *r, const Controller &app, int step, bool audio,
                   int learning = 0) {
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
  if (learning) {
    c.centered(120, 37, "CONFIGURAR GATILHOS", accent);
    c.centered(120, 64,
               learning == 1 ? "APERTE L2 PARA DESCER" : "APERTE R2 PARA SUBIR",
               white);
    c.centered(120, 90, "B CANCELA", dim);
    return;
  }
  if (s.scene == Scene::Menu) {
    c.centered(120, 27, "FOUR TRACK GROOVEBOX", white);
    const char *items[] = {"CONTINUE EDITING", "NIGHT DRIVE", "SUNNY POP",
                           "BROKEN BEAT",      "HOW TO PLAY", "SAVE AND EXIT"};
    for (int i = 0; i < 6; ++i) {
      if (s.menu == i)
        c.round(25, 39 + i * 13, 190, 12, 2, panel);
      c.centered(120, 42 + i * 13, items[i], s.menu == i ? accent : dim);
    }
    c.centered(120, 124, "A CHOOSE BEAT", dim);
    return;
  }
  if (s.scene == Scene::Tutorial) {
    const char *lines[] = {"DIRECTIONS SELECT A PAD",
                           "A TOGGLE NOTE   START PLAY",
                           "Y OPENS SETTINGS PANEL",
                           "UP DOWN CHOOSE SETTING",
                           "LEFT RIGHT CHANGE VALUE",
                           "B MENU   SELECT SAVE EXIT",
                           "THREE EDITABLE BEATS READY",
                           "L2 R2 CHANGE PAD NOTE",
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
  if (app.settings) {
    c.centered(120, 21, std::string("SETTINGS - ") + names[s.track], accent);
    std::string rows[] = {
        "TEMPO       " + std::to_string(p.bpm) + " BPM",
        "LOOP        " + std::to_string(p.length) + " STEPS",
        "VOLUME      " + std::to_string(p.volume[s.track]),
        std::string("MUTE        ") + (p.mute[s.track] ? "ON" : "OFF"),
        std::string("SOLO        ") + (p.solo == s.track ? "ON" : "OFF"),
        "PAD NOTE    " + fullNoteName(s.track, p.notes[s.track][s.cursor]),
        "PATTERN     " + std::to_string(s.slot + 1),
        "MAP L2 R2   A START"};
    for (int i = 0; i < 8; ++i) {
      if (s.setting == i)
        c.round(14, 32 + i * 11, 212, 11, 2, panel);
      c.text(22, 34 + i * 11, rows[i], s.setting == i ? accent : dim);
    }
    c.centered(120, 124, "LEFT RIGHT CHANGE  B BACK", white);
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
          c.centered(x + 9, y + 6, noteName(t, p.notes[t][n]), bg);
      }
      if (n == step)
        c.rect(x, y + 12, 18, 2, accent);
    }
  }
  c.text(5, 116,
         "L2 - " + fullNoteName(s.track, p.notes[s.track][s.cursor]) + " + R2",
         accent);
  c.text(5, 128,
         app.message.empty()
             ? (audio ? "START PLAY   B MENU" : "NO AUDIO DEVICE")
             : app.message,
         dim);
}
} // namespace pocketbeats
