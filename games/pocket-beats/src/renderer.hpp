#pragma once
#include "controller.hpp"
#include "pixel.hpp"
namespace pocketbeats {
inline void render(SDL_Renderer *r, const Controller &app, int step, bool audio,
                   int learning = 0, int playingSlot = 0, int = 0) {
  using namespace ui;
  Canvas c(r);
  int w, h;
  SDL_RenderGetLogicalSize(r, &w, &h);
  if (w != 480 || h != 272)
    SDL_RenderSetLogicalSize(r, 480, 272);
  Paint bg(17, 23, 39), panel(30, 40, 60), white(231, 238, 245),
      dim(131, 149, 172), accent(101, 236, 199);
  Paint colors[] = {Paint(247, 139, 108), Paint(113, 192, 248),
                    Paint(242, 201, 94), Paint(192, 143, 244)};
  const auto &s = app.state;
  const auto &p = s.current();
  c.rect(0, 0, 480, 272, bg);
  c.text(12, 8, "POCKET BEATS", accent, 2);
  c.text(242, 8, std::to_string(p.bpm) + " BPM", white, 2);
  c.text(372, 8, s.playing ? "TOCANDO" : "PARADO", s.playing ? accent : dim, 2);
  if (learning) {
    const char *buttons[] = {"L1", "R1", "L2", "R2"};
    c.centered(240, 65, "CONFIGURAR BOTOES", accent, 2);
    c.centered(240, 110, std::string("APERTE ") + buttons[learning - 1], white,
               3);
    c.centered(240, 158, std::to_string(learning) + " DE 4", dim, 2);
    c.centered(240, 225, "B CANCELA", dim, 2);
    return;
  }
  if (s.scene == Scene::Menu) {
    c.centered(240, 55, "ESCOLHA UM BEAT E TOQUE", white, 2);
    const char *items[] = {"CONTINUAR",   "NIGHT DRIVE", "SUNNY POP",
                           "BROKEN BEAT", "COMO TOCAR",  "SALVAR E SAIR"};
    for (int i = 0; i < 6; ++i) {
      if (s.menu == i)
        c.round(45, 80 + i * 26, 390, 24, 4, panel);
      c.centered(240, 84 + i * 26, items[i], s.menu == i ? accent : dim, 2);
    }
    c.centered(240, 250, "A ESCOLHE", dim, 2);
    return;
  }
  if (s.scene == Scene::Tutorial) {
    const char *lines[] = {"DIRECIONAIS ESCOLHEM O ESPACO",
                           "L1 DO  R1 RE  L2 MI  R2 FA",
                           "X SOL  Y LA  B SI",
                           "A LIGA OU DESLIGA O ESPACO",
                           "ESCOLHER A NOTA JA TOCA O SOM",
                           "NA BATERIA CADA BOTAO E UM SOM",
                           "START TOCA OU PARA O BEAT",
                           "SELECT ABRE O MENU",
                           "A OU B VOLTAM"};
    for (int i = 0; i < 9; ++i)
      c.centered(240, 58 + i * 20, lines[i], i == 8 ? accent : white, 2);
    return;
  }
  if (s.scene == Scene::Paused) {
    if (app.settings) {
      c.centered(240, 62,
                 s.setting == 0   ? "TEMPO"
                 : s.setting == 1 ? "TAMANHO DO BEAT"
                                  : "ESCOLHER BEAT",
                 accent, 2);
      std::string value = s.setting == 0 ? std::to_string(p.bpm) + " BPM"
                          : s.setting == 1
                              ? std::to_string(p.length) + " PASSOS"
                              : std::string(presets[s.slot]);
      c.centered(240, 112, value, white, s.setting == 6 ? 3 : 4);
      c.centered(240, 210, "ESQ DIR MUDA  A CONFIRMA", dim, 2);
      c.centered(240, 240, "B VOLTA", dim, 2);
      return;
    }
    const char *items[] = {"CONTINUAR",      "TEMPO",         "TAMANHO",
                           "TROCAR BEAT",    "VARIAR O BEAT", "DESFAZER",
                           "CONFIGURAR L/R", "SALVAR E SAIR"};
    for (int i = 0; i < 8; ++i) {
      if (s.pause == i)
        c.round(42, 51 + i * 23, 396, 22, 4, panel);
      c.centered(240, 55 + i * 23, items[i], s.pause == i ? accent : dim, 2);
    }
    c.centered(240, 244,
               app.message.empty() ? "A ESCOLHE  B VOLTA" : app.message, accent,
               2);
    return;
  }
  c.text(12, 34,
         std::string(presets[s.slot]) + " " + std::to_string(s.slot + 1), dim,
         2);
  c.text(360, 34, std::to_string(p.length) + " PASSOS", dim, 2);
  c.centered(280, 54, "START TOCA/PARA    SELECT MENU", dim);
  const int left = 94, pitch = 384 / p.length, pad = pitch - 2;
  bool showPlayhead = s.playing && (!s.songMode || s.slot == playingSlot);
  for (int n = 0; n < p.length; ++n) {
    int x = left + n * pitch;
    if (n % 4 == 0)
      c.line(x - 2, 66, x - 2, 211, Paint(58, 72, 94));
    if (p.length < 32 || n % 4 == 0 || n == p.length - 1)
      c.centered(x + pad / 2, 66, std::to_string(n + 1),
                 showPlayhead && n == step ? accent : dim);
    if (showPlayhead && n == step)
      c.rect(x, 77, pad, 3, accent);
  }
  const char *labels[] = {"BATERIA", "BAIXO", "GUIT.", "SYNTH"};
  const char *letters[] = {"",  "A", "C", "D", "E", "G", "A", "C",
                           "E", "C", "D", "E", "F", "G", "A", "B"};
  const char *drumLetters[] = {"B", "C", "H", "T", "T", "T", "P"};
  for (int t = 0; t < 4; ++t) {
    int y = 84 + t * 32;
    c.text(5, y + 8, labels[t], t == s.track ? colors[t] : dim, 2);
    for (int n = 0; n < p.length; ++n) {
      int x = left + n * pitch;
      bool on = p.notes[t][n] != 0;
      bool selected = t == s.track && n == s.cursor;
      int value = on ? p.notes[t][n] : p.chosen[t][n];
      if (selected)
        c.round(x - 1, y - 1, pad + 2, 31, 3, white);
      c.round(x, y, pad, 29, 3, on ? colors[t] : panel);
      if (on || selected) {
        std::string name;
        if (t == 0) {
          int key = -1;
          for (int i = 0; i < 7; ++i)
            if (value == (1 << i))
              key = i;
          if (p.length == 32)
            name = key >= 0 ? drumLetters[key] : "+";
          else if (key < 0)
            name = "KIT";
          else {
            const char *abbr[] = {"BU", "CX", "CH", "TG", "TM", "TA", "PR"};
            name = abbr[key];
          }
        } else
          name = p.length == 32 ? letters[value] : noteName(t, value);
        if (p.length == 16 && name == "SOL")
          name = "SO";
        if (p.length == 16 && name == "KIT")
          name = "+";
        c.centered(x + pad / 2, y + 7, name, on ? bg : dim, 2);
      }
      if (showPlayhead && n == step)
        c.rect(x, y + 27, pad, 2, accent);
    }
  }
  int value = p.chosen[s.track][s.cursor];
  bool enabled = p.notes[s.track][s.cursor] != 0;
  c.text(8, 217,
         "PASSO " + std::to_string(s.cursor + 1) + "  " +
             fullNoteName(s.track, value) +
             (enabled ? "  LIGADO" : "  DESLIGADO"),
         accent, 2);
  if (!app.message.empty()) {
    c.centered(240, 237, app.message, white, 2);
  } else if (!audio)
    c.centered(240, 237, "SEM AUDIO", white, 2);
  else {
    const char *keys[] = {"L1", "R1", "L2", "R2", "X", "Y", "B"};
    const char *mel[] = {"DO", "RE", "MI", "FA", "SOL", "LA", "SI"};
    const char *drums[] = {"BUM", "CX", "CH", "T.G", "T.M", "T.A", "PRATO"};
    int selected = -1;
    for (int i = 0; i < 7; ++i)
      if (value == (s.track == 0 ? 1 << i : 9 + i))
        selected = i;
    for (int i = 0; i < 7; ++i) {
      int x = 8 + (i % 4) * 118, y = i < 4 ? 239 : 258;
      c.text(x, y,
             std::string(keys[i]) + " " + (s.track == 0 ? drums[i] : mel[i]),
             i == selected ? accent : dim, 2);
    }
  }
  c.text(362, 258, enabled ? "A DESL" : "A LIGA", white, 2);
}
} // namespace pocketbeats
