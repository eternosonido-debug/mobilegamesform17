#include "game_logic.hpp"
#include "solutions.hpp"
#include <cassert>
#include <iostream>
int main() {
  using namespace pop;
  State s = initial(0);
  for (int y = 0; y < H; ++y)
    for (int x = 0; x < W; ++x)
      s.board[y * W + x] = (x + y) % 3;
  auto unchanged = s;
  assert(!act(s, 0, 0, false));
  assert(s.board == unchanged.board && s.random == unchanged.random &&
         s.moves == unchanged.moves);
  assert(!act(s, 0, -1, false));
  assert(!act(s, 0, N, false));
  s.board[35] = s.board[36] = 0;
  auto old = s;
  Effect e;
  assert(act(s, 0, 35, false, &e));
  assert(e.count == 2 && s.moves == old.moves - 1);
  assert(e.from[35] == 28 && s.board[35] == old.board[28] && e.from[0] == -1);
  assert(hasMove(s));
  s = initial(0);
  s.charge = 11;
  auto seed = s.random;
  assert(!act(s, 0, 10, true));
  assert(s.random == seed);
  s.charge = 12;
  assert(act(s, 0, 10, true, &e));
  assert(e.count == W + H - 1 && s.charge == 0);
  s.moves = 0;
  assert(!act(s, 0, 0, false));
  assert(!won(s, 0));
  bool shuffled = false;
  for (uint32_t seed = 1; seed < 200 && !shuffled; ++seed) {
    auto board = initial(0);
    for (int y = 0; y < H; ++y)
      for (int x = 0; x < W; ++x)
        board.board[y * W + x] = (x + y) % 3;
    board.board[35] = board.board[36] = 0;
    board.random = seed;
    Effect fx;
    assert(act(board, 0, 35, false, &fx));
    if (fx.shuffled) {
      assert(hasMove(board) && board.shuffles == 1);
      shuffled = true;
    }
  }
  assert(shuffled);
  auto complete = initial(0);
  complete.board.fill(0);
  complete.collected[1] = levels()[0].quota;
  complete.moves = 1;
  assert(act(complete, 0, 0, false));
  assert(complete.moves == 0 && won(complete, 0));
  Game g;
  auto start = g.state;
  assert(g.action());
  g.undo();
  assert(g.state.board == start.board && g.state.random == start.random &&
         g.state.moves == start.moves);
  assert(g.action());
  assert(g.save("pop-test.save"));
  Game loaded;
  assert(loaded.load("pop-test.save"));
  assert(loaded.state.board == g.state.board &&
         loaded.state.random == g.state.random);
  std::ofstream("pop-test.save") << "POP1 99";
  assert(!loaded.load("pop-test.save"));
  std::remove("pop-test.save");
  assert(poptest::solutions().size() == LevelCount);
  Game campaign;
  for (int i = 0; i < LevelCount; ++i) {
    campaign.begin(i);
    auto state = initial(i);
    int count = 0;
    for (int action : poptest::solutions()[i]) {
      assert(act(state, i, action % N, action >= N));
      campaign.selected = action % N;
      campaign.pulse = action >= N;
      assert(campaign.action());
      ++count;
    }
    assert(won(state, i) && won(campaign.state, i));
    assert(campaign.unlocked == std::min(LevelCount - 1, i + 1));
    assert(state.moves >= 0);
    std::cout << "Fase " << i + 1 << ": metas completas em " << count
              << " jogadas\n";
  }
  assert(campaign.save("pop-test.save"));
  assert(loaded.load("pop-test.save") && loaded.level == 19 &&
         loaded.unlocked == 19);
  std::remove("pop-test.save");
}
