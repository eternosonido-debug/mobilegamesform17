#define main fmg_application_main
#include "../src/main.cpp"
#undef main
#include <thread>
#include <unistd.h>
void require(bool b, const char *m) {
  if (!b)
    throw std::runtime_error(m);
}
int main() {
  const char *ids[] = {"juices-sort", "new-blocks", "color-mix", "sudoku",
                       "traffic-jam", "mahjong",    "boca",      "color-loop"};
  for (int game = 0; game < 8; ++game) {
    char folder[] = "/tmp/fmg-dual-XXXXXX";
    require(mkdtemp(folder), "temp folder");
    setenv("FMG_SAVE_DIR", folder, 1);
    Engine initial(17);
    initial.reset(game);
    std::string path = std::string(folder) + "/game-" + std::to_string(game) + ".sav";
    require(save(path, initial.s), "initial save");
    SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_EVENTS);
    int index = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_GAMECONTROLLER, 4, 6, 1);
    require(index >= 0, "virtual joystick");
    auto *j = SDL_JoystickOpen(index);
    char guid[64];
    SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(j), guid, sizeof(guid));
    // Exercise standard mappings and M17-style button-only mappings in alternating games.
    std::string mapping =
        std::string(guid) + ",FMG Dual Test,a:b0,b:b1,x:b2,y:b3,back:b4,start:b5,";
    if (game % 2)
      mapping += "leftx:a0,lefty:a1,rightx:a2,righty:a3,";
    require(SDL_GameControllerAddMapping(mapping.c_str()) >= 0, "controller map");
    std::thread input([&]() {
      SDL_Delay(350);
      for (int axis : {0, 2, 1, 3}) {
        SDL_JoystickSetVirtualAxis(j, axis, 30000);
        SDL_Delay(85);
        SDL_JoystickSetVirtualAxis(j, axis, 0);
        SDL_Delay(90);
      }
      // X has no attack in these games and must not trigger a hint or menu.
      SDL_Event event{};
      event.type = SDL_KEYDOWN;
      event.key.keysym.sym = SDLK_c;
      SDL_PushEvent(&event);
      SDL_Delay(40);
      SDL_JoystickClose(j);
      event.type = SDL_KEYDOWN;
      event.key.keysym.sym = SDLK_ESCAPE;
      SDL_PushEvent(&event);
    });
    char program[] = "dual-sticks", flag[] = "--game";
    char *args[] = {program, flag, const_cast<char *>(ids[game])};
    int result = fmg_application_main(3, args);
    input.join();
    State actual;
    require(result == 0 && load(path, actual) && actual.game == game, "save after sticks");
    if (game == 6)
      require(actual.f[0] + actual.f[1] > initial.s.f[0] + initial.s.f[1] + 8 &&
                  actual.f[0] - actual.f[1] > initial.s.f[0] - initial.s.f[1] + 8,
              "both sticks move Boca");
    else
      require(actual.cursor == (game == 0 || game == 7 ? 2
                                : game == 2            ? 8
                                : game == 3            ? 20
                                : game == 4            ? 14
                                                       : 18),
              "left and right stick each move one cell in every puzzle");
    require((game == 6 ? actual.f[7] <= 0 : actual.score == 0) && actual.selected == -1,
            "stick motion and X trigger no action");
    std::remove(path.c_str());
    std::remove((std::string(folder) + "/juices-progress.sav").c_str());
    rmdir(folder);
  }
  std::cout << "PASS both virtual sticks across all eight games, mapped and unmapped axes\n";
}
