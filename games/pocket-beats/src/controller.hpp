#pragma once
#include "game_logic.hpp"
#include "input.hpp"
namespace pocketbeats {
struct Controller {
  Session state;
  bool quit = false, restart = false, dirty = false, settings = false,
       requestMapping = false;
  bool previewRequested = false;
  int previewNote = 0, lastNoteKey = -1;
  unsigned variation = 0;
  std::string message;
  std::vector<Session> history;
  void remember() {
    if (history.size() == 32)
      history.erase(history.begin());
    history.push_back(state);
  }
  void undo() {
    if (history.empty()) {
      message = "NADA PARA DESFAZER";
      return;
    }
    auto scene = state.scene;
    bool playing = state.playing;
    state = history.back();
    history.pop_back();
    state.scene = scene;
    state.playing = playing;
    state.songMode = false;
    dirty = true;
    message = "DESFEITO";
  }
  void audition() {
    previewNote = state.current().chosen[state.track][state.cursor];
    previewRequested = true;
  }
  void vary() {
    remember();
    ++variation;
    auto &p = state.current();
    unsigned rng = 1957 + variation * 7919 + state.slot * 101;
    auto random = [&]() {
      rng = rng * 1664525u + 1013904223u;
      return rng;
    };
    const int chords[] = {9, 12, 13, 14};
    for (int i = 0; i < p.length; ++i) {
      int k = i % 16;
      int drum = k % 8 == 0 ? 1 : k % 8 == 4 ? 2 : 0;
      if (k % 2 == 0)
        drum |= 4;
      if (k >= 14 && random() % 3 == 0)
        drum = 1 << (3 + random() % 3);
      p.notes[0][i] = drum;
      int root = chords[(i / 4 + variation) % 4];
      p.notes[1][i] =
          k % 4 == 0 ? root : (k % 4 == 3 && random() % 3 == 0 ? root : 0);
      p.notes[2][i] = k % 4 == 2 ? root : 0;
      p.notes[3][i] = (k == 7 || k == 15) ? 9 + random() % 7 : 0;
      for (int t = 0; t < tracks; ++t)
        if (p.notes[t][i])
          p.chosen[t][i] = p.notes[t][i];
    }
    dirty = true;
    message = "NOVO BEAT - PODE DESFAZER";
  }
  void action(Control c) {
    auto &s = state;
    restart = false;
    previewRequested = false;
    requestMapping = false;
    if (s.scene == Scene::Menu) {
      if (c == Back) {
        quit = true;
        return;
      }
      if (c == Up)
        s.menu = (s.menu + 5) % 6;
      if (c == Down)
        s.menu = (s.menu + 1) % 6;
      if (c == A) {
        if (s.menu == 0)
          s.scene = Scene::Playing;
        else if (s.menu <= 3) {
          s.chooseSlot(s.menu - 1);
          s.songMode = false;
          s.scene = Scene::Playing;
          s.playing = true;
          restart = true;
        } else if (s.menu == 4)
          s.scene = Scene::Tutorial;
        else
          quit = true;
      }
      return;
    }
    if (s.scene == Scene::Tutorial) {
      if (c == A || c == B || c == Back)
        s.scene = Scene::Menu;
      return;
    }
    if (s.scene == Scene::Paused) {
      if (settings) {
        if (c == A || c == B || c == Back) {
          settings = false;
          return;
        }
        if (c == Left || c == Right) {
          remember();
          s.adjust(c == Right ? 1 : -1);
          dirty = true;
        }
        return;
      }
      if (c == B) {
        s.scene = Scene::Playing;
        return;
      }
      if (c == Back) {
        s.scene = Scene::Playing;
        return;
      }
      if (c == Up)
        s.pause = (s.pause + 7) % 8;
      if (c == Down)
        s.pause = (s.pause + 1) % 8;
      if (c == A) {
        switch (s.pause) {
        case 0:
          s.scene = Scene::Playing;
          break;
        case 1:
        case 2:
          s.setting = s.pause - 1;
          settings = true;
          break;
        case 3:
          s.setting = 6;
          settings = true;
          break;
        case 4:
          vary();
          s.scene = Scene::Playing;
          break;
        case 5:
          undo();
          s.scene = Scene::Playing;
          break;
        case 6:
          requestMapping = true;
          break;
        case 7:
          quit = true;
          break;
        }
      }
      return;
    }
    if (c == Back) {
      s.playing = false;
      s.scene = Scene::Paused;
      s.pause = 0;
      return;
    }
    if (c == Start) {
      s.songMode = false;
      s.playing = !s.playing;
      restart = true;
      return;
    }
    int key = -1;
    switch (c) {
    case ToneDo:
      key = 0;
      break;
    case ToneRe:
      key = 1;
      break;
    case ToneMi:
      key = 2;
      break;
    case ToneFa:
      key = 3;
      break;
    case X:
      key = 4;
      break;
    case Y:
      key = 5;
      break;
    case B:
      key = 6;
      break;
    default:
      break;
    }
    if (key >= 0) {
      int value = s.track == 0 ? (1 << key) : 9 + key;
      auto &p = s.current();
      int &chosen = p.chosen[s.track][s.cursor];
      if (chosen != value) {
        remember();
        chosen = value;
        if (p.notes[s.track][s.cursor])
          p.notes[s.track][s.cursor] = value;
        dirty = true;
      }
      lastNoteKey = key;
      audition();
      return;
    }
    if (c == A) {
      remember();
      s.current().toggle(s.track, s.cursor);
      dirty = true;
      if (s.current().notes[s.track][s.cursor])
        audition();
      return;
    }
    if (c == Left || c == Right)
      s.move(c == Right ? 1 : -1, 0);
    if (c == Up || c == Down)
      s.move(0, c == Down ? 1 : -1);
  }
};
} // namespace pocketbeats
