#pragma once
#include "game_logic.hpp"
#include "input.hpp"
namespace pocketbeats {
struct Controller {
  Session state;
  bool quit = false, restart = false, dirty = false, settings = false,
       tools = false, song = false;
  bool previewRequested = false;
  int previewNote = 0, tool = 0, songCursor = 0;
  std::string message;
  std::vector<Session> history;
  std::array<std::array<int, 8>, tracks> clipboard{};
  int copied = 0;
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
    bool playing = state.playing;
    auto scene = state.scene;
    state = history.back();
    history.pop_back();
    state.playing = playing;
    state.scene = scene;
    dirty = true;
    message = "DESFEITO";
  }
  void audition() {
    if (!state.preview)
      return;
    previewNote = state.track == 0
                      ? (1 << state.drumLane)
                      : state.current().notes[state.track][state.cursor];
    previewRequested = previewNote != 0;
  }
  void copy() {
    int start = state.page() * 8;
    copied = std::min(8, state.current().length - start);
    for (int t = 0; t < tracks; ++t)
      for (int i = 0; i < copied; ++i)
        clipboard[t][i] = state.current().notes[t][start + i];
    message = "TRECHO COPIADO";
  }
  void paste() {
    if (!copied) {
      message = "COPIE UM TRECHO ANTES";
      return;
    }
    remember();
    int start = state.page() * 8;
    for (int t = 0; t < tracks; ++t)
      for (int i = 0; i < copied && start + i < state.current().length; ++i)
        state.current().notes[t][start + i] = clipboard[t][i];
    dirty = true;
    message = "TRECHO COLADO";
  }
  void action(Control c) {
    auto &s = state;
    restart = false;
    previewRequested = false;
    if (c == Back) {
      s.playing = false;
      quit = true;
      return;
    }
    if (s.scene == Scene::Menu) {
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
      if (c == A || c == B)
        s.scene = Scene::Menu;
      return;
    }
    if (s.scene == Scene::Paused) {
      if (c == B || c == Y) {
        s.scene = Scene::Playing;
        return;
      }
      if (c == Up)
        s.pause = (s.pause + 4) % 5;
      if (c == Down)
        s.pause = (s.pause + 1) % 5;
      if (c == A) {
        if (s.pause == 0)
          s.scene = Scene::Playing;
        else if (s.pause == 1 || s.pause == 2) {
          remember();
          if (s.pause == 1)
            s.current() = preset(s.slot);
          else
            s.current().notes = {};
          s.cursor = 0;
          dirty = true;
          restart = true;
          s.scene = Scene::Playing;
        } else if (s.pause == 3)
          s.scene = Scene::Menu;
        else
          quit = true;
      }
      return;
    }
    if (c == Start) {
      if (song && !s.songMode) {
        s.songMode = true;
        s.playing = true;
      } else
        s.playing = !s.playing;
      restart = true;
      return;
    }
    if (song) {
      if (c == B) {
        song = false;
        return;
      }
      if (c == Left || c == Right)
        songCursor = std::max(0, std::min(std::min(7, s.orderLength),
                                          songCursor + (c == Right ? 1 : -1)));
      if (c == A) {
        remember();
        if (songCursor == s.orderLength) {
          s.order[s.orderLength++] = s.slot;
        } else
          s.order[songCursor] = (s.order[songCursor] + 1) % slots;
        dirty = true;
      }
      if (c == Y && songCursor < s.orderLength && s.orderLength > 1) {
        remember();
        for (int i = songCursor; i < s.orderLength - 1; ++i)
          s.order[i] = s.order[i + 1];
        --s.orderLength;
        songCursor = std::min(songCursor, s.orderLength - 1);
        dirty = true;
      }
      if (c == Up || c == Down) {
        remember();
        s.songMode = !s.songMode;
        dirty = true;
        restart = true;
      }
      return;
    }
    if (settings) {
      if (c == B || c == Y) {
        settings = false;
        tools = true;
        return;
      }
      if (c == Up)
        s.setting = (s.setting + 7) % 8;
      if (c == Down)
        s.setting = (s.setting + 1) % 8;
      if (c == Left || c == Right) {
        remember();
        s.adjust(c == Right ? 1 : -1);
        dirty = true;
        if (s.setting == 5)
          audition();
      }
      return;
    }
    if (tools) {
      if (c == B || c == Y) {
        tools = false;
        return;
      }
      if (c == Left || c == Right)
        tool = (tool + (c == Right ? 1 : 5)) % 6;
      if (c == Up || c == Down)
        tool = (tool + (c == Down ? 2 : 4)) % 6;
      if (c == A) {
        switch (tool) {
        case 0:
          remember();
          s.current().timbre[s.track] = (s.current().timbre[s.track] + 1) % 3;
          dirty = true;
          audition();
          if (!previewRequested) {
            previewNote = s.track == 0 ? (1 << s.drumLane) : 1;
            previewRequested = true;
          }
          break;
        case 1:
          copy();
          break;
        case 2:
          paste();
          tools = false;
          break;
        case 3:
          undo();
          tools = false;
          break;
        case 4:
          tools = false;
          song = true;
          songCursor = 0;
          break;
        case 5:
          tools = false;
          settings = true;
          s.setting = 0;
          break;
        }
      }
      return;
    }
    if (c == Y) {
      tools = true;
      return;
    }
    if (c == NoteDown || c == NoteUp) {
      if (s.track == 0) {
        s.drumLane =
            std::max(0, std::min(2, s.drumLane + (c == NoteUp ? 1 : -1)));
        audition();
        return;
      }
      int &note = s.current().notes[s.track][s.cursor];
      if (!note) {
        message = "A ADICIONA A NOTA";
        return;
      }
      remember();
      note = std::max(1, std::min(8, note + (c == NoteUp ? 1 : -1)));
      dirty = true;
      audition();
      return;
    }
    if (c == A) {
      remember();
      int &note = s.current().notes[s.track][s.cursor];
      if (s.track == 0) {
        note ^= 1 << s.drumLane;
        if (note & (1 << s.drumLane))
          audition();
      } else {
        note = note ? 0 : 1;
        if (note)
          audition();
      }
      dirty = true;
    } else if (c == B) {
      s.playing = false;
      s.scene = Scene::Paused;
    } else if (c == Left || c == Right) {
      s.move(c == Right ? 1 : -1, 0);
      if (!s.playing)
        audition();
    } else if (c == Up || c == Down) {
      s.move(0, c == Down ? 1 : -1);
      if (!s.playing)
        audition();
    }
  }
};
} // namespace pocketbeats
