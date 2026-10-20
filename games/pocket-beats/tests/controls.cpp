#include "../src/controller.hpp"
#include <iostream>
#include <stdexcept>
#include <unistd.h>
void check(bool x, const char *m) {
  if (!x)
    throw std::runtime_error(m);
}
int main() {
  using namespace pocketbeats;
  check(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_JOYSTICK |
                 SDL_INIT_GAMECONTROLLER) == 0,
        "SDL init");
  int index = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_UNKNOWN, 4, 8, 0);
  check(index >= 0, "virtual joystick");
  auto *j = SDL_JoystickOpen(index);
  {
    Input input;
    Controller app;
    SDL_Event e{};
    e.type = SDL_KEYDOWN;
    e.key.keysym.sym = SDLK_z;
    app.action(input.event(e));
    check(app.state.scene == Scene::Playing, "A enter");
    for (int axis : {0, 2}) {
      SDL_JoystickSetVirtualAxis(j, axis, 30000);
      SDL_JoystickUpdate();
      auto d = input.direction();
      check(d.x == 1, "both sticks");
      app.action(Right);
      SDL_JoystickSetVirtualAxis(j, axis, 0);
      SDL_JoystickUpdate();
    }
    check(app.state.cursor == 2, "sticks navigation");
    int n = app.state.current().notes[0][2];
    app.action(A);
    check(app.state.current().notes[0][2] == (n ^ 1), "A toggle");
    // Actual SDL events for pitch edits, learned digital triggers and rollback.
    app.state.track = 1;
    app.state.current().setNote(1, 2, 2);
    e.key.keysym.sym = SDLK_e;
    app.action(input.event(e));
    check(app.state.current().notes[1][2] == 3 && app.previewRequested,
          "R2 previews changed note");
    e.key.keysym.sym = SDLK_q;
    app.action(input.event(e));
    check(app.state.current().notes[1][2] == 2, "L2 keyboard note down");
    app.state.current().setNote(1, 2, 0);
    app.action(NoteDown);
    check(app.state.current().notes[1][2] == 0, "empty step remains off");
    app.state.current().setNote(1, 2, 1);
    app.action(NoteDown);
    check(app.state.current().notes[1][2] == 1, "lowest note remains on");
    input.beginMapping();
    e = {};
    e.type = SDL_JOYBUTTONDOWN;
    e.jbutton.which = SDL_JoystickInstanceID(j);
    e.jbutton.button = 4;
    input.event(e);
    check(input.learning == 2, "learn L2");
    input.event(e);
    check(input.learning == 2, "duplicate trigger rejected");
    e.jbutton.button = 5;
    input.event(e);
    check(!input.learning && input.mappingChanged, "learn R2");
    check(input.event(e) == NoteUp, "learned R2 event");
    std::string map = "/tmp/pocket-beats-controls-" + std::to_string(getpid());
    check(input.saveMapping(map), "save trigger mapping");
    Input loaded;
    check(loaded.loadMapping(map) && loaded.event(e) == NoteUp,
          "reload trigger mapping");
    std::remove(map.c_str());
    input.beginMapping();
    e.jbutton.button = 5;
    input.event(e);
    e = {};
    e.type = SDL_KEYDOWN;
    e.key.keysym.sym = SDLK_x;
    input.event(e);
    check(!input.learning && input.pitch[0].button == 4,
          "cancel mapping preserves old pair");
    check(noteName(1, 1) == "LA" && fullNoteName(1, 2) == "DO2" &&
              fullNoteName(2, 1) == "LA3",
          "note names");
    app.action(Start);
    check(app.state.playing, "transport");
    app.action(Y);
    check(app.tools, "Y opens six action cards");
    app.tool = 5;
    app.action(A);
    app.action(Right);
    check(app.state.current().bpm == 105, "Y tempo");
    app.action(Down);
    app.action(Right);
    check(app.state.current().length == 32, "global loop");
    app.action(B);
    check(!app.settings && app.state.playing,
          "B closes settings while playing");
    app.action(B);
    check(!app.tools && app.state.playing, "B closes action cards");
    app.action(B);
    check(!app.state.playing && app.state.scene == Scene::Paused, "B pause");
    app.action(A);
    check(app.state.scene == Scene::Playing, "resume");
    app.action(Back);
    check(app.quit && !app.state.playing, "save exit");
  }
  SDL_JoystickClose(j);
  SDL_JoystickDetachVirtual(index);
  SDL_Quit();
  std::cout << "PASS SDL A, both virtual sticks, transport, global settings, B "
               "pause and exit\n";
}
