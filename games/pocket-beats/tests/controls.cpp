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
  int index = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_UNKNOWN, 4, 12, 0);
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
      check(input.direction().x == 1, "both sticks");
      app.action(Right);
      SDL_JoystickSetVirtualAxis(j, axis, 0);
      SDL_JoystickUpdate();
    }
    check(app.state.cursor == 2, "navigate");
    app.state.track = 1;
    app.state.current().notes[1][2] = 0;
    SDL_Keycode keys[] = {SDLK_q, SDLK_w, SDLK_e, SDLK_r,
                          SDLK_c, SDLK_v, SDLK_x};
    for (int i = 0; i < 7; ++i) {
      e = {};
      e.type = SDL_KEYDOWN;
      e.key.keysym.sym = keys[i];
      app.action(input.event(e));
      check(app.state.current().notes[1][2] == 0 &&
                app.state.current().chosen[1][2] == 9 + i &&
                app.previewRequested,
            "note key activates step or wrong note");
    }
    app.action(A);
    check(app.state.current().notes[1][2] == 15, "A enables chosen SI");
    app.action(ToneDo);
    check(app.state.current().notes[1][2] == 9, "active note replaced");
    app.action(A);
    app.action(A);
    check(app.state.current().notes[1][2] == 9, "A preserves note");
    input.beginMapping();
    int buttons[] = {4, 5, 8, 9};
    for (int i = 0; i < 4; ++i) {
      e = {};
      e.type = SDL_JOYBUTTONDOWN;
      e.jbutton.which = SDL_JoystickInstanceID(j);
      e.jbutton.button = buttons[i];
      input.event(e);
      if (i < 3)
        check(input.learning == i + 2, "mapping sequence");
    }
    check(!input.learning && input.mappingChanged, "mapping complete");
    check(input.event(e) == ToneFa, "R2 maps FA");
    std::string path = "/tmp/pocket-map-" + std::to_string(getpid());
    check(input.saveMapping(path), "save mapping");
    Input other;
    check(other.loadMapping(path) && other.event(e) == ToneFa, "load mapping");
    std::remove(path.c_str());
    input.beginMapping();
    e = {};
    e.type = SDL_KEYDOWN;
    e.key.keysym.sym = SDLK_x;
    input.event(e);
    check(!input.learning && input.pitch[3].button == 9,
          "cancel preserves mapping");
    app.action(Start);
    check(app.state.playing, "play");
    app.action(B);
    check(app.state.playing && app.state.current().notes[1][2] == 15,
          "B is SI while playing");
    app.action(Back);
    check(!app.state.playing && app.state.scene == Scene::Paused,
          "Select menu");
    app.state.pause = 1;
    app.action(A);
    app.action(Right);
    check(app.state.current().bpm == 105, "tempo");
    app.action(A);
    app.state.pause = 7;
    app.action(A);
    check(app.quit, "save exit");
  }
  SDL_JoystickClose(j);
  SDL_JoystickDetachVirtual(index);
  SDL_Quit();
  std::cout << "PASS seven direct note keys, A-only activation, both sticks, "
               "four shoulder mappings and simple menu\n";
}
