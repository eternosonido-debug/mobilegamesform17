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
  Controller c;
  c.action(A);
  auto original = c.state.current().notes;
  c.action(A);
  check(c.state.current().notes[0][0] == (original[0][0] ^ 1),
        "toggle only kick");
  c.undo();
  check(c.state.current().notes == original, "undo drums");
  c.state.drumLane = 1;
  c.action(A);
  check(c.previewRequested && c.previewNote == 2, "snare preview");
  c.copy();
  c.state.cursor = 8;
  c.paste();
  for (int t = 0; t < 4; ++t)
    for (int i = 0; i < 8; ++i)
      check(c.state.current().notes[t][i] == c.state.current().notes[t][i + 8],
            "copy eight steps across tracks");
  c.undo();
  check(c.state.cursor == 8, "undo keeps edit location");
  c.state.current().length = 4;
  c.state.cursor = 0;
  c.copy();
  check(c.copied == 4, "four-step copy bound");
  c.state.chooseSlot(1);
  c.state.cursor = 8;
  c.paste();
  check(c.state.current().notes[0][8] == c.clipboard[0][0],
        "paste another pattern");
  c.action(Y);
  c.tool = 0;
  int style = c.state.current().timbre[c.state.track];
  c.action(A);
  check(c.state.current().timbre[c.state.track] == (style + 1) % 3 &&
            c.previewRequested,
        "timbre instant preview");
  c.tool = 4;
  c.action(A);
  check(c.song, "song screen");
  c.songCursor = c.state.orderLength;
  c.action(A);
  check(c.state.orderLength == 5, "append song beat");
  c.action(Y);
  check(c.state.orderLength == 4, "remove song beat");
  c.action(Start);
  check(c.state.songMode && c.state.playing, "song transport");
  std::string path = "/tmp/pocket-beats-workflow-" + std::to_string(getpid());
  c.state.drumLane = 2;
  check(c.state.save(path), "save2");
  Session loaded;
  check(loaded.load(path) &&
            loaded.current().notes == c.state.current().notes &&
            loaded.order == c.state.order && loaded.orderLength == 4 &&
            loaded.drumLane == 2 && loaded.songMode && !loaded.playing &&
            loaded.current().timbre == c.state.current().timbre,
        "new save roundtrip");
  // Old saves retain their musical meaning when converted to independent drum
  // masks.
  Session old;
  std::ofstream f(path);
  f << "POCKETBEATS1\n0 0 0\n";
  for (auto &p : old.patterns) {
    f << p.length << ' ' << p.bpm << ' ' << p.solo << '\n';
    for (int t = 0; t < 4; ++t) {
      f << p.volume[t] << ' ' << p.mute[t];
      for (int i = 0; i < 32; ++i)
        f << ' ' << (t == 0 ? i % 9 : p.notes[t][i]);
      f << '\n';
    }
  }
  f.close();
  Session migrated;
  check(migrated.load(path), "legacy save load");
  int masks[] = {0, 1, 2, 4, 5, 6, 4, 4, 4};
  for (int i = 0; i < 32; ++i)
    check(migrated.current().notes[0][i] == masks[i % 9],
          "legacy drum conversion");
  std::remove(path.c_str());
  std::cout << "PASS direct edit/preview, independent drums, undo, copy/paste, "
               "timbres, song editing and legacy saves\n";
}
