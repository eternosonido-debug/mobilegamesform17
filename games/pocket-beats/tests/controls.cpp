#include "../src/controller.hpp"
#include <iostream>
#include <stdexcept>
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
    check(app.state.current().notes[0][2] == (n ? 0 : 1), "A toggle");
    app.action(Start);
    check(app.state.playing, "transport");
    app.action(Y);
    app.action(Right);
    check(app.state.current().bpm == 105, "Y tempo");
    app.action(Down);
    app.action(Right);
    check(app.state.current().length == 32, "global loop");
    app.action(B);
    check(!app.settings && app.state.playing, "B closes settings while playing");
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
