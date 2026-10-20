#pragma once
#include "controller.hpp"
#include "pixel.hpp"
namespace pocketbeats {
inline void render(SDL_Renderer *r, const Controller &app, int step, bool audio,
                   int learning = 0, int playingSlot = 0, int orderIndex = 0) {
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
  c.text(112, 5,
         std::to_string(s.playing && s.songMode ? s.patterns[playingSlot].bpm
                                                : p.bpm) +
             " BPM",
         white);
  c.text(174, 5, s.playing ? (s.songMode ? "MUSICA" : "TOCANDO") : "PARADO",
         s.playing ? accent : dim);
  if (learning) {
    c.centered(120, 37, "CONFIGURAR GATILHOS", accent);
    c.centered(120, 64,
               learning == 1 ? "APERTE L2 PARA DESCER" : "APERTE R2 PARA SUBIR",
               white);
    c.centered(120, 90, "B CANCELA", dim);
    return;
  }
  if (s.scene == Scene::Menu) {
    c.centered(120, 27, "ESCOLHA UM BEAT E TOQUE", white);
    const char *items[] = {"CONTINUAR EDITANDO", "NIGHT DRIVE",
                           "SUNNY POP",          "BROKEN BEAT",
                           "COMO TOCAR",         "SALVAR E SAIR"};
    for (int i = 0; i < 6; ++i) {
      if (s.menu == i)
        c.round(25, 39 + i * 13, 190, 12, 2, panel);
      c.centered(120, 42 + i * 13, items[i], s.menu == i ? accent : dim);
    }
    c.centered(120, 124, "A ESCOLHE", dim);
    return;
  }
  if (s.scene == Scene::Tutorial) {
    const char *lines[] = {"A MARCA E JA TOCA O SOM",
                           "DIRECIONAIS ESCOLHEM O PAD",
                           "L2 R2 DESCEM SOBEM A NOTA",
                           "BUMBO CAIXA CHIMBAL SEPARADOS",
                           "START TOCA OU PARA O BEAT",
                           "Y MOSTRA AS ACOES",
                           "B VOLTA  SELECT SALVA E SAI",
                           "MESMO TAMANHO NAS 4 PISTAS",
                           "A OU B VOLTAM"};
    for (int i = 0; i < 9; ++i)
      c.centered(120, 22 + i * 12, lines[i], i == 8 ? accent : white);
    return;
  }
  if (s.scene == Scene::Paused) {
    const char *items[] = {"CONTINUAR", "RESTAURAR BEAT INICIAL",
                           "LIMPAR ESTE PADRAO", "MENU INICIAL",
                           "SALVAR E SAIR"};
    c.centered(120, 24, "PAUSADO", accent);
    for (int i = 0; i < 5; ++i) {
      if (s.pause == i)
        c.round(24, 40 + i * 16, 192, 14, 3, panel);
      c.centered(120, 44 + i * 16, items[i], s.pause == i ? accent : dim);
    }
    return;
  }
  if (app.song) {
    c.centered(120, 22, s.songMode ? "MODO MUSICA" : "MODO PADRAO", accent);
    for (int i = 0; i < 8; ++i) {
      int x = 10 + (i % 4) * 57, y = 40 + (i / 4) * 30;
      bool selected = i == app.songCursor;
      if (selected)
        c.round(x - 2, y - 2, 55, 27, 3, white);
      c.round(x, y, 51, 23, 2,
              s.playing && s.songMode && orderIndex == i ? accent : panel);
      std::string text = i < s.orderLength
                             ? "BEAT " + std::to_string(s.order[i] + 1)
                         : i == s.orderLength ? "+"
                                              : "";
      c.centered(x + 25, y + 8, text,
                 s.playing && s.songMode && orderIndex == i ? bg : white);
    }
    c.centered(120, 104, "A TROCA  Y REMOVE  B VOLTA", white);
    c.centered(120, 116, "START TOCA  CIMA MUDA MODO", dim);
    c.centered(120, 128, app.message, accent);
    return;
  }
  if (app.tools) {
    c.centered(120, 22,
               std::string("ACOES - ") + (s.track == 0   ? "BATERIA"
                                          : s.track == 1 ? "BAIXO"
                                          : s.track == 2 ? "GUITARRA"
                                                         : "SYNTH"),
               accent);
    const char *labels[] = {"TIMBRE",   "COPIAR", "COLAR",
                            "DESFAZER", "MUSICA", "AJUSTES"};
    const char *styles[] = {"CLASSICO", "BRILHO", "SUAVE"};
    std::string values[] = {styles[p.timbre[s.track]],
                            "ESTES 8 PASSOS",
                            app.copied ? "NO TRECHO ATUAL" : "COPIE PRIMEIRO",
                            app.history.empty() ? "NADA AINDA" : "ULTIMA ACAO",
                            "JUNTAR BEATS",
                            "TEMPO E VOLUME"};
    for (int i = 0; i < 6; ++i) {
      int x = 9 + (i % 2) * 114, y = 35 + (i / 2) * 28;
      if (app.tool == i)
        c.round(x - 2, y - 2, 112, 27, 3, white);
      c.round(x, y, 108, 23, 2, panel);
      c.centered(x + 54, y + 3, labels[i], app.tool == i ? accent : white);
      c.centered(x + 54, y + 14, values[i], dim);
    }
    c.centered(120, 123, app.message.empty() ? "A USA   B VOLTA" : app.message,
               accent);
    return;
  }
  if (app.settings) {
    c.centered(120, 21, "AJUSTES", accent);
    std::string rows[] = {
        "TEMPO       " +
            std::to_string(s.playing && s.songMode ? s.patterns[playingSlot].bpm
                                                   : p.bpm) +
            " BPM",
        "TAMANHO     " + std::to_string(p.length) + " PASSOS",
        "VOLUME      " + std::to_string(p.volume[s.track]),
        std::string("SILENCIAR   ") + (p.mute[s.track] ? "SIM" : "NAO"),
        std::string("SO ESTA PISTA ") + (p.solo == s.track ? "SIM" : "NAO"),
        "NOTA        " + fullNoteName(s.track, p.notes[s.track][s.cursor]),
        "PADRAO      " + std::to_string(s.slot + 1),
        "MAPEAR L2 R2  A INICIA"};
    for (int i = 0; i < 8; ++i) {
      if (s.setting == i)
        c.round(14, 32 + i * 11, 212, 11, 2, panel);
      c.text(22, 34 + i * 11, rows[i], s.setting == i ? accent : dim);
    }
    c.centered(120, 124, "ESQ DIR MUDA  B VOLTA", white);
    return;
  }
  c.text(7, 19, std::string(presets[s.slot]) + " " + std::to_string(s.slot + 1),
         dim);
  c.text(160, 19,
         std::to_string(s.page() + 1) + "/" +
             std::to_string((p.length + 7) / 8) + " PAG",
         dim);
  bool showPlayhead = s.playing && (!s.songMode || s.slot == playingSlot);
  for (int j = 0; j < 8; ++j) {
    int n = s.page() * 8 + j;
    if (n >= p.length)
      break;
    int x = 61 + j * 22;
    c.centered(x + 9, 31, std::to_string(n + 1),
               showPlayhead && n == step ? accent : dim);
  }
  const char *labels[] = {"BUMBO", "CAIXA", "CHIMBAL",
                          "BAIXO", "GUIT.", "SYNTH"};
  for (int row = 0; row < 6; ++row) {
    int t = row < 3 ? 0 : row - 2;
    int y = 43 + row * 11;
    bool selectedRow = t == s.track && (t != 0 || row == s.drumLane);
    c.text(3, y + 2, labels[row], selectedRow ? colors[t] : dim);
    for (int j = 0; j < 8; ++j) {
      int n = s.page() * 8 + j;
      if (n >= p.length)
        break;
      int x = 61 + j * 22;
      bool on = t == 0 ? bool(p.notes[0][n] & (1 << row)) : p.notes[t][n] != 0;
      bool selected = selectedRow && n == s.cursor;
      if (selected)
        c.round(x - 2, y - 1, 22, 12, 2, white);
      c.round(x, y, 18, 10, 2, on ? colors[t] : panel);
      if (on) {
        if (t == 0) {
          c.disc(x + 9, y + 5, 2, bg);
        } else
          c.centered(x + 9, y + 2, noteName(t, p.notes[t][n]), bg);
      }
      if (showPlayhead && n == step)
        c.rect(x, y + 9, 18, 1, accent);
    }
  }
  std::string selected =
      s.track == 0 ? labels[s.drumLane]
                   : fullNoteName(s.track, p.notes[s.track][s.cursor]);
  c.text(5, 114, "L2 - " + selected + " + R2", accent);
  c.text(5, 127,
         app.message.empty() ? (audio ? "A MARCA  Y ACOES  START TOCA"
                                      : "SEM DISPOSITIVO DE AUDIO")
                             : app.message,
         dim);
}
} // namespace pocketbeats
