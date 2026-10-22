#pragma once
#include "direction_input.hpp"
#include <set>
#include <vector>
namespace chef {
enum Control { Left, Right, Up, Down, A, B, X, Y, Start, Back, None };
inline Control keyboard(SDL_Keycode c) {
  const char *vars[] = {"FMG_KEY_LEFT",  "FMG_KEY_RIGHT", "FMG_KEY_UP",
                        "FMG_KEY_DOWN",  "FMG_KEY_A",     "FMG_KEY_B",
                        "FMG_KEY_X",     "FMG_KEY_Y",     "FMG_KEY_START",
                        "FMG_KEY_SELECT"};
  for (int i = 0; i < 10; ++i) {
    const char *v = std::getenv(vars[i]);
    if (v && SDL_GetKeyFromName(v) == c)
      return Control(i);
  }
  const char *right[] = {"FMG_KEY_RIGHT_LEFT", "FMG_KEY_RIGHT_RIGHT",
                         "FMG_KEY_RIGHT_UP", "FMG_KEY_RIGHT_DOWN"};
  for (int i = 0; i < 4; ++i) {
    const char *v = std::getenv(right[i]);
    if (v && SDL_GetKeyFromName(v) == c)
      return Control(i);
  }
  switch (c) {
  case SDLK_LEFT:
    return Left;
  case SDLK_RIGHT:
    return Right;
  case SDLK_UP:
    return Up;
  case SDLK_DOWN:
    return Down;
  case SDLK_z:
  case SDLK_SPACE:
    return A;
  case SDLK_x:
    return B;
  case SDLK_c:
    return X;
  case SDLK_v:
    return Y;
  case SDLK_RETURN:
    return Start;
  case SDLK_ESCAPE:
  case SDLK_RSHIFT:
    return Back;
  default:
    return None;
  }
}
struct Input {
  struct Device {
    SDL_Joystick *j;
    SDL_GameController *c;
  };
  std::vector<Device> devices;
  std::set<SDL_Keycode> held;
  void open(int index) {
    auto id = SDL_JoystickGetDeviceInstanceID(index);
    for (auto &d : devices)
      if (SDL_JoystickInstanceID(d.j) == id)
        return;
    auto *c =
        SDL_IsGameController(index) ? SDL_GameControllerOpen(index) : nullptr;
    auto *j = c ? SDL_GameControllerGetJoystick(c) : SDL_JoystickOpen(index);
    if (j)
      devices.push_back({j, c});
  }
  Input() {
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
      open(i);
  }
  ~Input() {
    for (auto &d : devices)
      if (d.c)
        SDL_GameControllerClose(d.c);
      else
        SDL_JoystickClose(d.j);
  }
  Control event(const SDL_Event &e) {
    if (e.type == SDL_JOYDEVICEADDED)
      open(e.jdevice.which);
    if (e.type == SDL_WINDOWEVENT &&
        e.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
      held.clear();
    if (e.type == SDL_KEYUP)
      held.erase(e.key.keysym.sym);
    if (e.type == SDL_KEYDOWN && !e.key.repeat) {
      auto key = keyboard(e.key.keysym.sym);
      if (key < 4) {
        held.insert(e.key.keysym.sym);
        return None;
      }
      return key;
    }
    if (e.type == SDL_CONTROLLERBUTTONDOWN) {
      switch (e.cbutton.button) {
      case SDL_CONTROLLER_BUTTON_A:
        return A;
      case SDL_CONTROLLER_BUTTON_B:
        return B;
      case SDL_CONTROLLER_BUTTON_X:
        return X;
      case SDL_CONTROLLER_BUTTON_Y:
        return Y;
      case SDL_CONTROLLER_BUTTON_START:
        return Start;
      case SDL_CONTROLLER_BUTTON_BACK:
        return Back;
      default:
        return None;
      }
    }
    if (e.type == SDL_JOYBUTTONDOWN &&
        !SDL_GameControllerFromInstanceID(e.jbutton.which)) {
      const char *vars[] = {"FMG_BUTTON_A",     "FMG_BUTTON_B",
                            "FMG_BUTTON_X",     "FMG_BUTTON_Y",
                            "FMG_BUTTON_START", "FMG_BUTTON_SELECT"};
      const int defaults[] = {0, 1, 2, 3, 7, 6};
      for (int i = 0; i < 6; ++i)
        if (e.jbutton.button == fmg::inputSetting(vars[i], defaults[i]))
          return Control(A + i);
    }
    return None;
  }
  fmg::Direction direction() {
    fmg::Direction result;
    for (auto key : held) {
      auto k = keyboard(key);
      result.add((k == Right) - (k == Left), (k == Down) - (k == Up));
    }
    for (auto &d : devices) {
      auto v = fmg::joystickDirection(d.j, d.c);
      result.add(v.x, v.y);
    }
    result.clamp();
    return result;
  }
};
} // namespace chef
