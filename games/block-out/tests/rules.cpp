#include "game_logic.hpp"
#include <iostream>
#include <stdexcept>
using namespace blockout;
void check(bool b, const char *m) {
  if (!b)
    throw std::runtime_error(m);
}
void repeat(Game &g, int block, int x, int y, int count) {
  g.selected = block;
  for (int i = 0; i < count; ++i)
    check(g.move(x, y), "solution move");
}
int main() {
  Game g;
  g.scene = Scene::Playing;
  repeat(g, 0, 1, 0, 5);
  repeat(g, 1, 0, 1, 2);
  check(won(g.board) && g.scene == Scene::Won, "tutorial solved");
  check(g.exiting && g.animation == exitDuration, "exit breakup duration");
  g.tick(.05f);
  check(g.animation < exitDuration && g.animation > 0,
        "exit animation progresses");
  g.reset(1);
  repeat(g, 1, 0, 1, 4);
  repeat(g, 0, 1, 0, 4);
  repeat(g, 2, -1, 0, 1);
  check(won(g.board), "level two solved");
  g.reset(2);
  repeat(g, 2, -1, 0, 3);
  repeat(g, 1, 0, 1, 3);
  repeat(g, 0, 1, 0, 5);
  repeat(g, 3, 0, -1, 1);
  check(won(g.board) && g.unlocked == 3, "level three solved");
  for (int id = 3; id < levelCount; ++id) {
    g.reset(id);
    auto stage = level(id);
    check(valid(g.board, id), "generated board valid");
    for (auto &step : stage.solution) {
      g.selected = step.block;
      check(g.move(step.dx, step.dy), "generated solution valid");
    }
    check(won(g.board), "all extended stages solved");
  }
  auto l = level(0);
  Board b;
  b.blocks = l.blocks;
  auto before = b;
  check(!slide(b, l, 0, -1, 0) && b.moves == before.moves &&
            b.blocks[0].x == before.blocks[0].x,
        "wrong gate does not mutate board");
  b.blocks[0].y = 3;
  check(!slide(b, l, 0, 1, 0) && b.blocks[0].x == 0,
        "wall blocks full footprint");
  l = level(1);
  b.blocks = l.blocks;
  check(!slide(b, l, 0, 1, 0), "blocks cannot overlap");
  g.reset(0);
  g.selected = 0;
  check(g.move(1, 0), "valid action");
  g.undo();
  check(g.board.moves == 0 && g.board.blocks[0].x == 0 && !g.canUndo,
        "one-step undo");
  g.selected = 0;
  g.move(1, 0);
  g.muted = true;
  std::string path = "/tmp/block-out-rule-test.sav";
  check(g.save(path), "save");
  Game loaded;
  check(loaded.load(path) && loaded.current == 0 && loaded.board.moves == 1 &&
            loaded.canUndo && loaded.muted,
        "resume board and undo");
  loaded.undo();
  check(loaded.board.blocks[0].x == 0, "undo after resume");
  std::ofstream bad(path);
  bad << "BLOCKOUT 1\n99 0 0 -1 0 0 0\n";
  bad.close();
  check(!loaded.load(path), "reject invalid level");
  std::remove(path.c_str());
  g.reset(0);
  g.scene = Scene::Paused;
  auto x = g.board.blocks[0].x;
  check(!g.move(1, 0) && g.board.blocks[0].x == x, "pause freezes gameplay");
  std::cout << "PASS three solved stages, wrong gates, walls, collision, undo, "
               "save and pause\n";
}
