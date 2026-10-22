#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
namespace tiles {
struct Note {
  int ms, lane, sample, step;
  bool harmony;
};
inline int bpm(int song) {
  const int values[] = {96, 112, 128};
  return values[std::max(0, std::min(2, song))];
}
inline const char *title(int song) {
  const char *names[] = {"BRISA DIGITAL", "RUA NEON", "PULSO NOTURNO"};
  return names[std::max(0, std::min(2, song))];
}
inline int intro(int song) { return 240000 / bpm(song); }
inline int duration(int song) { return intro(song) + 180000 + 2000; }
inline std::vector<Note> melody(int song) {
  std::vector<Note> out;
  const int motif[] = {0, 1, 2, 1, 3, 2, 0, 2, 1, 3, 2, 0, 2, 1, 3, 0};
  const int major[] = {0, 4, 7, 11}, minor[] = {0, 3, 7, 10},
            roots[] = {0, 2, 9}, progression[] = {0, 5, 7, 0};
  for (int i = 0; i * 30000 / bpm(song) < 180000; ++i) {
    if (i % 16 == 15)
      continue;
    int lane = (motif[i % 16] + i / 32 + song) % 4,
        root = (roots[song] + progression[(i / 32) % 4]) % 12;
    int sample = 12 + (root + (song ? minor[lane] : major[lane])) % 12;
    out.push_back(
        {intro(song) + i * 30000 / bpm(song), lane, sample, i, false});
    if (i % 8 == 0) {
      int other = (lane + 2) % 4;
      out.push_back({intro(song) + i * 30000 / bpm(song), other,
                     12 + (root + (song ? minor[other] : major[other])) % 12, i,
                     true});
    }
  }
  return out;
}
inline std::vector<Note> chart(int song, int difficulty) {
  auto all = melody(song);
  std::vector<Note> out;
  for (auto n : all)
    if (difficulty == 2 ||
        (!n.harmony && n.step % (difficulty == 0 ? 4 : 2) == 0))
      out.push_back(n);
  return out;
}
struct State {
  std::vector<Note> notes;
  std::vector<int8_t> judged;
  int song = 0, difficulty = 0, life = 100, combo = 0, maxCombo = 0, points = 0,
      perfect = 0, misses = 0, position = 0;
  bool practice = false;
  size_t cursor = 0;
  State(int track = 0, int mode = 0, bool training = false)
      : notes(chart(track, mode)), judged(notes.size(), 0), song(track),
        difficulty(mode), practice(training) {}
  bool failed() const { return life == 0 && !practice; }
  bool won() const { return !failed() && position >= duration(song); }
  int score() const {
    return notes.empty()
               ? 0
               : int(int64_t(points) * 1000000 / (100 * notes.size()));
  }
  void advance(int ms, int audioMs = -1) {
    position = std::max(position, audioMs < 0 ? ms : audioMs);
    while (cursor < notes.size() && notes[cursor].ms < ms - 140) {
      if (!judged[cursor]) {
        judged[cursor] = 4;
        ++misses;
        combo = 0;
        life = std::max(0, life - 5);
      }
      ++cursor;
    }
  }
  int hit(int lane, int ms) {
    if (lane < 0 || lane > 3 || failed() || won())
      return 0;
    int nearest = -1, error = 141;
    for (size_t i = cursor; i < notes.size(); ++i) {
      if (notes[i].ms > ms + 140)
        break;
      if (!judged[i] && notes[i].lane == lane) {
        int e = std::abs(notes[i].ms - ms);
        if (e < error) {
          error = e;
          nearest = int(i);
        }
      }
    }
    if (nearest < 0) {
      life = std::max(0, life - 2);
      combo = 0;
      return 0;
    }
    int grade = error <= 55 ? 1 : error <= 105 ? 2 : 3;
    judged[nearest] = int8_t(grade);
    points += grade == 1 ? 100 : grade == 2 ? 75 : 40;
    if (grade == 1)
      ++perfect;
    ++combo;
    maxCombo = std::max(maxCombo, combo);
    life = std::min(100, life + 1);
    return grade;
  }
};
struct Settings {
  int song = 0, difficulty = 0, offset = 0;
  bool muted = false, practice = false;
  std::array<int, 9> best{{0, 0, 0, 0, 0, 0, 0, 0, 0}};
  bool save(const std::string &path) const {
    std::string tmp = path + ".tmp";
    std::ofstream f(tmp);
    f << "TILES1 " << song << ' ' << difficulty << ' ' << offset << ' ' << muted
      << ' ' << practice;
    for (int n : best)
      f << ' ' << n;
    f << '\n';
    f.close();
    return bool(f) && std::rename(tmp.c_str(), path.c_str()) == 0;
  }
  bool load(const std::string &path) {
    std::ifstream f(path);
    std::string magic;
    Settings g;
    if (!(f >> magic >> g.song >> g.difficulty >> g.offset >> g.muted >>
          g.practice) ||
        magic != "TILES1" || g.song < 0 || g.song > 2 || g.difficulty < 0 ||
        g.difficulty > 2 || g.offset < -150 || g.offset > 150)
      return false;
    for (int &n : g.best)
      if (!(f >> n) || n < 0 || n > 1000000)
        return false;
    *this = g;
    return true;
  }
  void finish(const State &s) {
    if (s.won() && !s.practice)
      best[s.song * 3 + s.difficulty] =
          std::max(best[s.song * 3 + s.difficulty], s.score());
  }
};
} // namespace tiles
