#pragma once
#include "direction_input.hpp"
#include <fstream>
#include <set>
#include <string>
#include <vector>
namespace pocketbeats {
enum Control {
  Left,
  Right,
  Up,
  Down,
  A,
  B,
  X,
  Y,
  Start,
  Back,
  ToneDo,
  ToneRe,
  ToneMi,
  ToneFa,
  None
};
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
  const char *shoulders[] = {"FMG_KEY_L1", "FMG_KEY_R1", "FMG_KEY_L2",
                             "FMG_KEY_R2"};
  for (int i = 0; i < 4; ++i) {
    const char *v = std::getenv(shoulders[i]);
    if (v && SDL_GetKeyFromName(v) == c)
      return Control(ToneDo + i);
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
  case SDLK_q:
    return ToneDo;
  case SDLK_w:
    return ToneRe;
  case SDLK_e:
    return ToneMi;
  case SDLK_r:
    return ToneFa;
  default:
    return None;
  }
}
struct Input {
  struct Binding {
    int key = -1, button = -1;
    std::string guid = "-";
  };
  Binding pitch[4], pending[4];
  struct Device {
    SDL_Joystick *j;
    SDL_GameController *c;
  };
  std::vector<Device> devices;
  std::set<SDL_Keycode> held;
  std::set<int> triggerHeld;
  int learning = 0;
  bool mappingChanged = false;
  std::string guid(SDL_JoystickID id) {
    char text[33] = {};
    auto *j = SDL_JoystickFromInstanceID(id);
    if (j)
      SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(j), text, sizeof(text));
    return text;
  }
  bool loadMapping(const std::string &path) {
    std::ifstream f(path);
    std::string magic;
    Binding temp[4];
    if (!(f >> magic) || (magic != "PBNOTES2" && magic != "PBPITCH1"))
      return false;
    int first = magic == "PBPITCH1" ? 2 : 0;
    for (int i = first; i < 4; ++i)
      if (!(f >> temp[i].key >> temp[i].button >> temp[i].guid) ||
          temp[i].button < -1 || temp[i].button > 255 || temp[i].key < -1)
        return false;
    for (int i = 0; i < 4; ++i)
      pitch[i] = temp[i];
    return true;
  }
  bool saveMapping(const std::string &path) {
    std::ofstream f(path + ".tmp");
    f << "PBNOTES2\n";
    for (auto &b : pitch)
      f << b.key << ' ' << b.button << ' ' << b.guid << '\n';
    f.close();
    return bool(f) && std::rename((path + ".tmp").c_str(), path.c_str()) == 0;
  }
  void beginMapping() {
    for (int i = 0; i < 4; ++i)
      pending[i] = pitch[i];
    learning = 1;
  }
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
    if (learning) {
      if ((e.type == SDL_KEYDOWN && keyboard(e.key.keysym.sym) == B) ||
          (e.type == SDL_CONTROLLERBUTTONDOWN &&
           e.cbutton.button == SDL_CONTROLLER_BUTTON_B)) {
        learning = 0;
        return None;
      }
      Binding binding;
      bool got = false;
      if (e.type == SDL_KEYDOWN && !e.key.repeat) {
        auto key = keyboard(e.key.keysym.sym);
        if (key >= Left && key <= Back)
          return None;
        binding.key = e.key.keysym.sym;
        got = true;
      }
      if (e.type == SDL_JOYBUTTONDOWN) {
        auto *c = SDL_GameControllerFromInstanceID(e.jbutton.which);
        bool reserved = false, cancel = false;
        if (c) {
          SDL_GameControllerButton buttons[] = {
              SDL_CONTROLLER_BUTTON_A,     SDL_CONTROLLER_BUTTON_B,
              SDL_CONTROLLER_BUTTON_X,     SDL_CONTROLLER_BUTTON_Y,
              SDL_CONTROLLER_BUTTON_START, SDL_CONTROLLER_BUTTON_BACK};
          for (int i = 0; i < 6; ++i) {
            auto b = SDL_GameControllerGetBindForButton(c, buttons[i]);
            if (b.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON &&
                b.value.button == e.jbutton.button) {
              reserved = true;
              cancel = i == 1;
            }
          }
        } else {
          const char *vars[] = {"FMG_BUTTON_A",     "FMG_BUTTON_B",
                                "FMG_BUTTON_X",     "FMG_BUTTON_Y",
                                "FMG_BUTTON_START", "FMG_BUTTON_SELECT"};
          int defaults[] = {0, 1, 2, 3, 7, 6};
          for (int i = 0; i < 6; ++i)
            if (fmg::inputSetting(vars[i], defaults[i]) == e.jbutton.button) {
              reserved = true;
              cancel = i == 1;
            }
        }
        if (reserved) {
          if (cancel)
            learning = 0;
          return None;
        }
        binding.button = e.jbutton.button;
        binding.guid = guid(e.jbutton.which);
        got = true;
      }
      if (got) {
        for (int i = 0; i < learning - 1; ++i)
          if (binding.key == pending[i].key &&
              binding.button == pending[i].button &&
              binding.guid == pending[i].guid)
            return None;
        pending[learning - 1] = binding;
        if (++learning == 5) {
          for (int i = 0; i < 4; ++i)
            pitch[i] = pending[i];
          learning = 0;
          mappingChanged = true;
        }
      }
      return None;
    }
    if (e.type == SDL_KEYDOWN && !e.key.repeat)
      for (int i = 0; i < 4; ++i)
        if (pitch[i].key >= 0 && pitch[i].key == e.key.keysym.sym)
          return Control(ToneDo + i);
    if (e.type == SDL_JOYBUTTONDOWN) {
      const char *vars[] = {"FMG_BUTTON_L1", "FMG_BUTTON_R1", "FMG_BUTTON_L2",
                            "FMG_BUTTON_R2"};
      for (int i = 0; i < 4; ++i) {
        int configured = fmg::inputSetting(vars[i], -1);
        if ((pitch[i].button == e.jbutton.button &&
             pitch[i].guid == guid(e.jbutton.which)) ||
            (configured >= 0 && configured == e.jbutton.button))
          return Control(ToneDo + i);
      }
    }
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
    if (e.type == SDL_CONTROLLERAXISMOTION &&
        (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT ||
         e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) {
      int id = int(e.caxis.which) * 2 +
               (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
      bool was = triggerHeld.count(id) > 0;
      if (e.caxis.value > 16000) {
        triggerHeld.insert(id);
        if (!was)
          return e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT ? ToneFa
                                                                  : ToneMi;
      } else if (e.caxis.value < 8000)
        triggerHeld.erase(id);
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
      case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        return ToneDo;
      case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        return ToneRe;
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
} // namespace pocketbeats
