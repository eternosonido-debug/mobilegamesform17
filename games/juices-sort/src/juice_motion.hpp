#pragma once
#include "juice_campaign.hpp"
namespace fmg {
// Presentation only: the rules commit a whole pour once; saves never contain half a move.
struct JuiceMotion {
  State before;
  JuiceProgress progress;
  int panel = 0, option = 0, browse = 0;
  int from = -1, to = -1, amount = 0, shakeTube = -1;
  float elapsed = 0, duration = .82f, clock = 0, messageTime = 0, shakeTime = 0;
  bool active = false;
  std::string message;
  static float smooth(float t) {
    t = std::max(0.f, std::min(1.f, t));
    return t * t * (3 - 2 * t);
  }
  bool start(const State &oldState, const State &newState) {
    if (oldState.game != 0 || newState.game != 0 || oldState.selected < 0 ||
        oldState.selected == oldState.cursor || newState.score != oldState.score + 1)
      return false;
    int moved = oldState.a[40 + oldState.selected] - newState.a[40 + oldState.selected];
    if (moved <= 0 || newState.a[40 + oldState.cursor] - oldState.a[40 + oldState.cursor] != moved)
      return false;
    before = oldState;
    from = oldState.selected;
    to = oldState.cursor;
    amount = moved;
    elapsed = 0;
    duration = .95f + .06f * moved;
    active = true;
    message.clear();
    messageTime = 0;
    return true;
  }
  float fraction() const {
    return std::min(1.f, elapsed / duration);
  }
  float poured() const {
    return smooth((fraction() - .23f) / .53f);
  }
  float lift() const {
    float t = fraction();
    return t < .23f ? smooth(t / .23f) : (t > .76f ? 1 - smooth((t - .76f) / .24f) : 1);
  }
  void feedback(const std::string &text, int tube = -1) {
    message = text;
    messageTime = 2.4f;
    shakeTube = tube;
    shakeTime = tube < 0 ? 0 : .32f;
  }
  void update(float dt) {
    dt = std::max(0.f, std::min(.1f, dt));
    clock += dt;
    messageTime = std::max(0.f, messageTime - dt);
    shakeTime = std::max(0.f, shakeTime - dt);
    if (active) {
      elapsed += dt;
      if (elapsed >= duration)
        active = false;
    }
  }
  void finish() {
    active = false;
  }
};
} // namespace fmg
