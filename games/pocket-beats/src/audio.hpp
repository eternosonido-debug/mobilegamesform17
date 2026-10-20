#pragma once
#include "synth.hpp"
#include <SDL.h>
namespace pocketbeats {
struct Audio {
  SDL_AudioDeviceID device = 0;
  Synth synth;
  SDL_atomic_t visibleStep{}, callbackCount{}, renderedSamples{}, visibleSlot{},
      visibleOrder{};
  static void callback(void *userdata, Uint8 *stream, int bytes) {
    auto *self = static_cast<Audio *>(userdata);
    self->synth.render(reinterpret_cast<int16_t *>(stream), bytes / 2);
    SDL_AtomicSet(&self->visibleStep,
                  self->synth.playing ? self->synth.step : -1);
    SDL_AtomicSet(&self->visibleSlot, self->synth.activeSlot);
    SDL_AtomicSet(&self->visibleOrder, self->synth.orderIndex);
    SDL_AtomicAdd(&self->callbackCount, 1);
    SDL_AtomicAdd(&self->renderedSamples, bytes / 2);
  }
  Audio() {
    Synth::bank(); // Build original instrument samples BEFORE opening real-time
                   // audio.
    SDL_AtomicSet(&visibleStep, -1);
    SDL_AudioSpec spec{};
    spec.freq = Synth::rate;
    spec.format = AUDIO_S16SYS;
    spec.channels = 1;
    spec.samples = 1024;
    spec.callback = callback;
    spec.userdata = this;
    device = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, 0);
    if (device)
      SDL_PauseAudioDevice(device, 0);
  }
  ~Audio() {
    if (device)
      SDL_CloseAudioDevice(device);
  }
  void sync(const Pattern &p, bool run, bool restart = false) {
    if (device)
      SDL_LockAudioDevice(device);
    synth.sync(p, run, restart);
    if (device)
      SDL_UnlockAudioDevice(device);
  }
  void syncSession(const Session &s, bool restart = false) {
    if (device)
      SDL_LockAudioDevice(device);
    synth.syncSession(s, restart);
    if (device)
      SDL_UnlockAudioDevice(device);
  }
  void audition(int t, int note, int style, int volume) {
    if (device)
      SDL_LockAudioDevice(device);
    synth.audition(t, note, style, volume);
    if (device)
      SDL_UnlockAudioDevice(device);
  }
  int audibleSlot() { return SDL_AtomicGet(&visibleSlot); }
  int audibleOrder() { return SDL_AtomicGet(&visibleOrder); }
  int audibleStep() { return SDL_AtomicGet(&visibleStep); }
};
} // namespace pocketbeats
