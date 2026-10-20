#pragma once
#include "game_logic.hpp"
#include "input.hpp"
namespace pocketbeats {
struct Controller {
  Session state;
  bool quit = false, restart = false, dirty = false;
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
        s.menu = (s.menu + 3) % 4;
      if (c == Down)
        s.menu = (s.menu + 1) % 4;
      if (c == A) {
        if (s.menu == 0)
          s.scene = Scene::Playing;
        else if (s.menu == 1)
          s.scene = Scene::Tutorial;
        else if (s.menu == 2) {
          s.scene = Scene::Playing;
          s.playing = true;
          restart = true;
        } else
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
    if (c == Start) {
      s.playing = !s.playing;
      restart = true;
    } else if (c == A) {
      s.current().toggle(s.track, s.cursor);
      dirty = true;
    } else if (c == B) {
      s.playing = false;
      s.scene = Scene::Paused;
    } else if (c == Y) {
      s.setting = (s.setting + 1) % 8;
    } else if (c == Left || c == Right) {
      int delta = c == Right ? 1 : -1;
      if (s.setting == 0)
        s.move(delta, 0);
      else {
        s.setting--;
        s.adjust(delta);
        s.setting++;
        dirty = true;
      }
    } else if (c == Up || c == Down) {
      int delta = c == Down ? 1 : -1;
      s.move(0, delta);
    }
  }
};
} // namespace pocketbeats
