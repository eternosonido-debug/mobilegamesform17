#pragma once
#include "game_logic.hpp"
#include "input.hpp"
namespace pocketbeats {
struct Controller {
  Session state;
  bool quit = false, restart = false, dirty = false, settings = false;
  std::string message;
  void action(Control c) {
    auto &s = state;
    restart = false;
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
        else if (s.pause == 1) {
          s.current() = preset(s.slot);
          s.cursor = 0;
          restart = true;
          dirty = true;
          s.scene = Scene::Playing;
        } else if (s.pause == 2) {
          s.current().notes = {};
          restart = true;
          dirty = true;
          s.scene = Scene::Playing;
        } else if (s.pause == 3) {
          s.scene = Scene::Menu;
        } else
          quit = true;
      }
      return;
    }
    if (!settings && (c == NoteDown || c == NoteUp)) {
      int &note = s.current().notes[s.track][s.cursor];
      if (!note) {
        message = "A ADICIONA A NOTA";
        return;
      }
      int maximum = s.track == 0 ? 5 : 8;
      note = std::max(1, std::min(maximum, note + (c == NoteUp ? 1 : -1)));
      dirty = true;
      return;
    }
    if (c == Start) {
      s.playing = !s.playing;
      restart = true;
      return;
    }
    if (c == Y) {
      settings = !settings;
      return;
    }
    if (settings) {
      if (c == B) {
        settings = false;
        return;
      }
      if (c == Up)
        s.setting = (s.setting + 7) % 8;
      if (c == Down)
        s.setting = (s.setting + 1) % 8;
      if (c == Left || c == Right) {
        s.adjust(c == Right ? 1 : -1);
        dirty = true;
      }
      return;
    }
    if (c == A) {
      s.current().toggle(s.track, s.cursor);
      dirty = true;
    } else if (c == B) {
      s.playing = false;
      s.scene = Scene::Paused;
    } else if (c == Left || c == Right)
      s.move(c == Right ? 1 : -1, 0);
    else if (c == Up || c == Down)
      s.move(0, c == Down ? 1 : -1);
  }
};
} // namespace pocketbeats
