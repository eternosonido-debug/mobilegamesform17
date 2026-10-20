#pragma once
#include <SDL.h>
#include <algorithm>
#include <cstdlib>
namespace fmg {
struct Direction {
  int x = 0, y = 0;
  void add(int xx, int yy) {
    x += xx;
    y += yy;
  }
  void clamp() {
    x = std::max(-1, std::min(1, x));
    y = std::max(-1, std::min(1, y));
  }
};
inline int inputSetting(const char *name, int fallback) {
  const char *v = std::getenv(name);
  return v ? std::atoi(v) : fallback;
}
inline int axisDirection(int value) {
  int deadzone = std::max(4000, std::min(30000, inputSetting("FMG_DEADZONE", 16000)));
  return (value > deadzone) - (value < -deadzone);
}
inline int rawAxis(SDL_Joystick *j, int index) {
  return index >= 0 && index < SDL_JoystickNumAxes(j) ? axisDirection(SDL_JoystickGetAxis(j, index))
                                                      : 0;
}
inline Direction joystickDirection(SDL_Joystick *j, SDL_GameController *c = nullptr) {
  Direction d;
  if (!j || !SDL_JoystickGetAttached(j))
    return d;
  const SDL_GameControllerAxis axes[] = {SDL_CONTROLLER_AXIS_LEFTX, SDL_CONTROLLER_AXIS_LEFTY,
                                         SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTY};
  const char *vars[] = {"FMG_AXIS_X", "FMG_AXIS_Y", "FMG_AXIS_RX", "FMG_AXIS_RY"};
  int values[4] = {};
  for (int i = 0; i < 4; ++i) {
    // A button-only controller mapping must not hide the physical analog axes.
    bool mapped =
        c && SDL_GameControllerGetBindForAxis(c, axes[i]).bindType != SDL_CONTROLLER_BINDTYPE_NONE;
    values[i] = mapped && !std::getenv(vars[i])
                    ? axisDirection(SDL_GameControllerGetAxis(c, axes[i]))
                    : rawAxis(j, inputSetting(vars[i], i));
  }
  d.add(values[0] + values[2], values[1] + values[3]);
  if (c)
    d.add(SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) -
              SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_DPAD_LEFT),
          SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_DPAD_DOWN) -
              SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_DPAD_UP));
  for (int i = 0; i < SDL_JoystickNumHats(j); ++i) {
    Uint8 h = SDL_JoystickGetHat(j, i);
    d.add(bool(h & SDL_HAT_RIGHT) - bool(h & SDL_HAT_LEFT),
          bool(h & SDL_HAT_DOWN) - bool(h & SDL_HAT_UP));
  }
  d.clamp();
  return d;
}
} // namespace fmg
