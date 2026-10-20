#pragma once
#include <algorithm>
#include <array>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
namespace pocketbeats {
constexpr int tracks = 4, maxSteps = 32, slots = 3;
static const char *const names[] = {"DRUMS", "BASS", "GUITAR", "SYNTH"};
static const char *const presets[] = {"NIGHT DRIVE", "SUNNY POP",
                                      "BROKEN BEAT"};
inline std::string noteName(int track, int note) {
  if (note == 0)
    return "OFF";
  if (track == 0) {
    static const char *drums[] = {"OFF", "BUM", "CX",  "B+C",
                                  "CH",  "B+H", "C+H", "ALL"};
    return drums[note];
  }
  static const char *notes[] = {"OFF", "LA", "DO", "RE", "MI",
                                "SOL", "LA", "DO", "MI"};
  return notes[note];
}
inline std::string fullNoteName(int track, int note) {
  if (!note || track == 0)
    return noteName(track, note);
  int octave = track == 1 ? 1 : track == 2 ? 3 : 4;
  if (note >= 2)
    ++octave;
  if (note >= 7)
    ++octave;
  return noteName(track, note) + std::to_string(octave);
}
struct Pattern {
  int length = 16, bpm = 110;
  std::array<std::array<int, maxSteps>, tracks> notes{};
  std::array<int, tracks> volume{{80, 75, 65, 60}};
  std::array<bool, tracks> mute{{false, false, false, false}};
  int solo = -1;
  std::array<int, tracks> timbre{{0, 0, 0, 0}};
  bool audible(int t) const {
    return t >= 0 && t < tracks && !mute[t] && (solo < 0 || solo == t);
  }
  bool toggle(int t, int s) {
    if (t < 0 || t >= tracks || s < 0 || s >= length)
      return false;
    notes[t][s] = notes[t][s] ? 0 : 1;
    return true;
  }
  bool setNote(int t, int s, int n) {
    if (t < 0 || t >= tracks || s < 0 || s >= length || n < 0 ||
        n > (t == 0 ? 7 : 8))
      return false;
    notes[t][s] = n;
    return true;
  }
  bool resize(int n) {
    if (n != 4 && n != 8 && n != 16 && n != 32)
      return false;
    length = n;
    return true;
  }
};
inline Pattern preset(int id) {
  Pattern p;
  p.bpm = id == 0 ? 104 : id == 1 ? 120 : 96;
  for (int s = 0; s < 32; ++s) {
    int k = s % 16;
    // Drum values are instrument triggers: kick, snare, hi-hat, kick+hat,
    // snare+hat.
    p.notes[0][s] = k % 4 == 0   ? (k == 4 || k == 12 ? 5 : 4)
                    : k % 2 == 0 ? 3
                                 : 0;
    p.notes[1][s] = k % 4 == 0 ? 1 + (k / 4 + (id == 2 ? 1 : 0)) % 4 : 0;
    p.notes[2][s] =
        (k == 2 || k == 6 || k == 10 || k == 14) ? 1 + (k / 4 + id) % 4 : 0;
    p.notes[3][s] = (k == (id == 0 ? 7 : 3) || k == 11) ? 5 + id : 0;
    if (id == 1 && k % 2 == 1)
      p.notes[3][s] = 1 + (k / 2) % 8;
    if (id == 2) {
      if (k == 3 || k == 10)
        p.notes[0][s] = 1;
      if (k == 8)
        p.notes[0][s] = 3;
      if (k == 7 || k == 14)
        p.notes[1][s] = 4;
    }
  }
  static const int migrate[] = {0, 1, 2, 4, 5, 6, 4, 4, 4};
  for (auto &n : p.notes[0])
    n = migrate[n];
  return p;
}
enum class Scene { Menu, Tutorial, Playing, Paused };
struct Session {
  std::array<Pattern, slots> patterns;
  Scene scene = Scene::Menu;
  int slot = 0, track = 0, cursor = 0, menu = 0, setting = 0, pause = 0;
  bool playing = false, preview = true, songMode = false;
  int drumLane = 0, orderLength = 4;
  std::array<int, 8> order{{0, 1, 2, 1, 0, 0, 0, 0}};
  Session() {
    for (int i = 0; i < slots; ++i)
      patterns[i] = preset(i);
  }
  Pattern &current() { return patterns[slot]; }
  const Pattern &current() const { return patterns[slot]; }
  int page() const { return cursor / 8; }
  void move(int dx, int dy) {
    int row = track == 0 ? drumLane : track + 2;
    row = (row + dy + 6) % 6;
    if (row < 3) {
      track = 0;
      drumLane = row;
    } else
      track = row - 2;
    cursor = (cursor + dx + current().length) % current().length;
  }
  void chooseSlot(int n) {
    slot = (n + slots) % slots;
    cursor = std::min(cursor, current().length - 1);
  }
  void adjust(int delta) {
    auto &p = current();
    switch (setting) {
    case 0:
      p.bpm = std::max(40, std::min(240, p.bpm + delta));
      break;
    case 1: {
      int sizes[] = {4, 8, 16, 32}, i = 0;
      while (sizes[i] != p.length)
        ++i;
      p.resize(sizes[std::max(0, std::min(3, i + delta))]);
      cursor = std::min(cursor, p.length - 1);
      break;
    }
    case 2:
      p.volume[track] = std::max(0, std::min(100, p.volume[track] + delta * 5));
      break;
    case 3:
      p.mute[track] = !p.mute[track];
      break;
    case 4:
      p.solo = p.solo == track ? -1 : track;
      break;
    case 5: {
      int n = p.notes[track][cursor];
      p.setNote(track, cursor, std::max(0, std::min(8, n + delta)));
      break;
    }
    case 6:
      chooseSlot(slot + delta);
      break;
    }
  }
  bool save(const std::string &path) const {
    const auto tmp = path + ".tmp";
    std::ofstream f(tmp);
    if (!f)
      return false;
    f << "POCKETBEATS2\n" << slot << ' ' << track << ' ' << cursor << '\n';
    f << drumLane << ' ' << preview << ' ' << songMode << ' ' << orderLength;
    for (int n : order)
      f << ' ' << n;
    f << '\n';
    for (const auto &p : patterns) {
      f << p.length << ' ' << p.bpm << ' ' << p.solo << '\n';
      for (int style : p.timbre)
        f << style << ' ';
      f << '\n';
      for (int t = 0; t < tracks; ++t) {
        f << p.volume[t] << ' ' << p.mute[t];
        for (int n : p.notes[t])
          f << ' ' << n;
        f << '\n';
      }
    }
    f.close();
    if (!f) {
      std::remove(tmp.c_str());
      return false;
    }
    return std::rename(tmp.c_str(), path.c_str()) == 0;
  }
  bool load(const std::string &path) {
    std::ifstream f(path);
    Session candidate;
    std::string magic;
    if (!(f >> magic) || (magic != "POCKETBEATS1" && magic != "POCKETBEATS2") ||
        !(f >> candidate.slot >> candidate.track >> candidate.cursor))
      return false;
    if (candidate.slot < 0 || candidate.slot >= slots || candidate.track < 0 ||
        candidate.track >= tracks)
      return false;
    bool modern = magic == "POCKETBEATS2";
    if (modern) {
      int prev, song;
      if (!(f >> candidate.drumLane >> prev >> song >> candidate.orderLength) ||
          candidate.drumLane < 0 || candidate.drumLane > 2 || prev < 0 ||
          prev > 1 || song < 0 || song > 1 || candidate.orderLength < 1 ||
          candidate.orderLength > 8)
        return false;
      candidate.preview = prev;
      candidate.songMode = song;
      for (int &v : candidate.order)
        if (!(f >> v) || v < 0 || v >= slots)
          return false;
    }
    for (auto &p : candidate.patterns) {
      int len;
      if (!(f >> len >> p.bpm >> p.solo) || !p.resize(len) || p.bpm < 40 ||
          p.bpm > 240 || p.solo < -1 || p.solo >= tracks)
        return false;
      if (modern)
        for (int &style : p.timbre)
          if (!(f >> style) || style < 0 || style > 2)
            return false;
      for (int t = 0; t < tracks; ++t) {
        int mute;
        if (!(f >> p.volume[t] >> mute) || p.volume[t] < 0 ||
            p.volume[t] > 100 || (mute != 0 && mute != 1))
          return false;
        p.mute[t] = mute;
        for (int &n : p.notes[t])
          if (!(f >> n) || n < 0 || n > (modern && t == 0 ? 7 : 8))
            return false;
      }
    }
    if (!modern) {
      static const int migrate[] = {0, 1, 2, 4, 5, 6, 4, 4, 4};
      for (auto &p : candidate.patterns)
        for (int &n : p.notes[0])
          n = migrate[n];
    }
    if (candidate.cursor < 0 || candidate.cursor >= candidate.current().length)
      return false;
    *this = candidate;
    return true;
  }
};
} // namespace pocketbeats
