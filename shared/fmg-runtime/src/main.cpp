#include "blocks_view.hpp"
#include "boca_view.hpp"
#include "direction_input.hpp"
#include "game.hpp"
#include "juice_view.hpp"
#include "loop_view.hpp"
#include "mahjong_view.hpp"
#include <SDL.h>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sys/stat.h>
using namespace fmg;
namespace {
SDL_Renderer *renderer = nullptr;
const SDL_Color palette[] = {{17, 24, 39, 255},   {248, 113, 113, 255}, {56, 189, 248, 255},
                             {250, 204, 21, 255}, {167, 139, 250, 255}, {52, 211, 153, 255},
                             {251, 146, 60, 255}, {241, 245, 249, 255}};
void color(int c) {
  auto p = palette[c % 8];
  SDL_SetRenderDrawColor(renderer, p.r, p.g, p.b, 255);
}
void box(int x, int y, int w, int h, int c, bool fill = true) {
  SDL_Rect r{x, y, w, h};
  color(c);
  if (fill)
    SDL_RenderFillRect(renderer, &r);
  else
    SDL_RenderDrawRect(renderer, &r);
}
void circle(int x, int y, int r, int c) {
  color(c);
  for (int yy = -r; yy <= r; yy++) {
    int xx = int(std::sqrt(double(r * r - yy * yy)));
    SDL_RenderDrawLine(renderer, x - xx, y + yy, x + xx, y + yy);
  }
}
// Original 3x5 bitmap glyphs. No external font or asset dependency.
const char *glyph(char c) {
  switch (c) {
  case 'A':
    return "010101111101101";
  case 'B':
    return "110101110101110";
  case 'C':
    return "011100100100011";
  case 'D':
    return "110101101101110";
  case 'E':
    return "111100110100111";
  case 'F':
    return "111100110100100";
  case 'G':
    return "011100101101011";
  case 'H':
    return "101101111101101";
  case 'I':
    return "111010010010111";
  case 'J':
    return "001001001101010";
  case 'K':
    return "101101110101101";
  case 'L':
    return "100100100100111";
  case 'M':
    return "101111111101101";
  case 'N':
    return "101111111111101";
  case 'O':
    return "010101101101010";
  case 'P':
    return "110101110100100";
  case 'Q':
    return "010101101111011";
  case 'R':
    return "110101110101101";
  case 'S':
    return "011100010001110";
  case 'T':
    return "111010010010010";
  case 'U':
    return "101101101101111";
  case 'V':
    return "101101101101010";
  case 'W':
    return "101101111111101";
  case 'X':
    return "101101010101101";
  case 'Y':
    return "101101010010010";
  case 'Z':
    return "111001010100111";
  case '0':
    return "111101101101111";
  case '1':
    return "010110010010111";
  case '2':
    return "110001010100111";
  case '3':
    return "110001010001110";
  case '4':
    return "101101111001001";
  case '5':
    return "111100110001110";
  case '6':
    return "011100110101010";
  case '7':
    return "111001010010010";
  case '8':
    return "010101010101010";
  case '9':
    return "010101011001110";
  case ':':
    return "000010000010000";
  case '-':
    return "000000111000000";
  case '/':
    return "001001010100100";
  case '>':
    return "100010001010100";
  case '+':
    return "000010111010000";
  case '.':
    return "000000000000010";
  default:
    return "000000000000000";
  }
}
void text(int x, int y, const std::string &t, int c = 7, int scale = 2) {
  for (char ch : t) {
    const char *g = glyph(ch);
    for (int r = 0; r < 5; r++)
      for (int col = 0; col < 3; col++)
        if (g[r * 3 + col] == '1')
          box(x + col * scale, y + r * scale, scale, scale, c);
    x += 4 * scale;
  }
}
std::string number(int n) {
  return std::to_string(n);
}
void outline(int x, int y, int w, int h, bool selected) {
  box(x - 2, y - 2, w + 4, h + 4, selected ? 3 : 7, false);
  if (selected)
    box(x - 3, y - 3, w + 6, h + 6, 3, false);
}
void render(const Engine &e, bool menu, int menuCursor, bool help, const std::string &notice,
            const JuiceMotion &juice, bool extras = false, int extraChoice = 0) {
  if (extras) {
    juice_art::Canvas p(renderer);
    p.paint(juice_art::Paint(22, 48, 49));
    SDL_RenderClear(renderer);
    p.centered(240, 25, names[e.s.game], juice_art::Paint(250, 229, 182), 2);
    p.centered(240, 52, "EXTRAS", juice_art::Paint(180, 210, 187));
    const char *items[] = {e.s.game == 6 ? "IMPULSO" : "DESFAZER",
                           e.s.game == 5   ? "PEDIR DICA"
                           : e.s.game == 3 ? "APAGAR CELULA"
                                           : "COMO JOGAR",
                           "REINICIAR", "CONTINUAR"};
    for (int i = 0; i < 4; ++i) {
      if (i == extraChoice)
        p.round(65, 75 + i * 35, 350, 29, 7, juice_art::Paint(64, 110, 93));
      p.centered(240, 85 + i * 35, items[i], juice_art::Paint(248, 238, 207));
    }
    p.centered(240, 241, "DIRECIONAL ESCOLHER   A OK   B VOLTAR", juice_art::Paint(180, 210, 187));
    SDL_RenderPresent(renderer);
    return;
  }
  if (e.s.game == 0 && !menu) {
    juice_art::render(renderer, e, juice, help, notice);
    return;
  }
  if (e.s.game == 5 && !menu) {
    mahjong_art::render(renderer, e, help, notice);
    return;
  }
  if (e.s.game == 7 && !menu) {
    loop_art::render(renderer, e, help, notice);
    return;
  }
  if (e.s.game == 1 && !menu) {
    blocks_art::render(renderer, e, help, notice);
    return;
  }
  if (e.s.game == 6 && !menu) {
    boca_art::render(renderer, e, help, notice);
    return;
  }
  const State &s = e.s;
  color(0);
  SDL_RenderClear(renderer);
  box(0, 0, 480, 31, 2);
  text(12, 10, menu ? "FAKE MOBILE GAMES" : names[s.game], 0);
  text(12, 253, "START MENU   SELECT SAVE/EXIT", 7, 1);
  if (menu) {
    for (int i = 0; i < 8; i++) {
      int y = 36 + i * 26;
      box(14, y, 260, 21, i == menuCursor ? 5 : 0);
      text(24, y + 6, names[i], i == menuCursor ? 0 : 7);
    }
    text(296, 47, "OFFLINE", 5);
    text(296, 68, "8 GAMES", 2);
    text(296, 105, "A RESUME");
    text(296, 128, "X NEW GAME");
    text(296, 151, "B BACK");
    text(296, 174, "Y HELP");
    text(296, 215, "SD SAVES", 5);
  } else {
    text(340, 43, "SCORE " + number(s.score), 5, 1);
    if (s.game == 1) {
      for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++) {
          box(35 + x * 24, 39 + y * 24, 22, 22, s.a[y * 8 + x]);
          box(35 + x * 24, 39 + y * 24, 22, 22, 7, false);
        }
      int id = s.a[100 + s.choice];
      if (id >= 0)
        for (auto p : Engine::shape(id)) {
          int x = s.cursor % 8 + p.first, y = s.cursor / 8 + p.second;
          if (x < 8 && y < 8)
            box(35 + x * 24, 39 + y * 24, 22, 22, e.fits(id, s.cursor % 8, s.cursor / 8) ? 5 : 1,
                false);
        }
      outline(35 + s.cursor % 8 * 24, 39 + s.cursor / 8 * 24, 22, 22, true);
      for (int k = 0; k < 3; k++) {
        outline(270, 65 + k * 47, 75, 40, k == s.choice);
        int sh = s.a[100 + k];
        if (sh >= 0)
          for (auto p : Engine::shape(sh))
            box(278 + p.first * 12, 70 + k * 47 + p.second * 10, 10, 8, sh + 1);
      }
      text(365, 80, "A PLACE", 7, 1);
      text(365, 99, "B PIECE", 7, 1);
      text(365, 118, "Y EXTRAS", 7, 1);
      text(365, 154, "FILL ROWS", 5, 1);
      text(365, 172, "OR COLUMNS", 5, 1);
    }
    if (s.game == 2) {
      const int colors[] = {7, 1, 2};
      for (int y = 0; y < 3; y++)
        for (int x = 0; x < 3; x++) {
          int v = s.a[y * 3 + x];
          box(60 + x * 61, 44 + y * 61, 53, 53, colors[v]);
          text(81 + x * 61, 65 + y * 61, number(v), 0);
          if (s.cursor == y * 3 + x)
            outline(60 + x * 61, 44 + y * 61, 53, 53, true);
        }
      text(285, 75, "A ROW +1", 7, 1);
      text(285, 95, "B COLUMN +1", 7, 1);
      text(285, 115, "0 > 1 > 2 > 0", 7, 1);
      text(285, 145, "GOAL ALL WHITE", 5, 1);
      text(285, 170, "Y EXTRAS", 7, 1);
    }
    if (s.game == 3) {
      for (int y = 0; y < 9; y++)
        for (int x = 0; x < 9; x++) {
          int i = y * 9 + x, xx = 29 + x * 21, yy = 35 + y * 22;
          box(xx, yy, 20, 21, s.a[81 + i] ? 0 : 4);
          if (s.a[i])
            text(xx + 7, yy + 6, number(s.a[i]), s.a[81 + i] ? 7 : 0);
          if (s.cursor == i)
            outline(xx, yy, 19, 20, true);
        }
      for (int k = 0; k <= 9; k += 3) {
        box(28 + k * 21, 34, 1, 199, 2);
        box(28, 34 + k * 22, 190, 1, 2);
      }
      text(270, 72, "NUMBER " + number(s.choice), 3);
      text(270, 107, "B NEXT NUMBER", 7, 1);
      text(270, 128, "A WRITE   Y EXTRAS", 7, 1);
      text(270, 149, "Y EXTRAS", 7, 1);
      text(270, 180, "NO DUPLICATES", 5, 1);
    }
    if (s.game == 4) {
      box(50, 39, 193, 193, 7, false);
      for (int y = 0; y < 6; y++)
        for (int x = 0; x < 6; x++)
          box(52 + x * 32, 41 + y * 32, 30, 30, 7, false);
      box(242, 105, 9, 30, 5);
      for (int i = 0; i < s.a[100]; i++) {
        int p = i * 4, x = 53 + s.a[p] * 32, y = 42 + s.a[p + 1] * 32,
            w = s.a[p + 3] ? 27 : s.a[p + 2] * 32 - 5, h = s.a[p + 3] ? s.a[p + 2] * 32 - 5 : 27;
        box(x, y, w, h, i == 0 ? 1 : 2 + i % 5);
        text(x + 8, y + 8, number(i + 1), 0);
        if (s.selected == i)
          outline(x, y, w, h, true);
      }
      outline(52 + s.cursor % 6 * 32, 41 + s.cursor / 6 * 32, 29, 29, true);
      text(280, 77, "RED CAR TO EXIT", 5, 1);
      text(280, 105, "A SELECT/RELEASE", 7, 1);
      text(280, 126, "DPAD SLIDE", 7, 1);
      text(280, 147, "B RELEASE", 7, 1);
      text(280, 168, "Y EXTRAS", 7, 1);
    }
    if (s.game == 5) {
      for (int i = 0; i < 32; i++) {
        int x = 18 + i % 8 * 41, y = 48 + i / 8 * 45;
        if (s.a[i]) {
          box(x, y, 35, 38, e.tileFree(i) ? 7 : 4);
          text(x + 6, y + 13, number(s.a[i]), 0);
          if (s.selected == i)
            outline(x, y, 35, 38, true);
        }
        if (s.cursor == i)
          outline(x, y, 35, 38, true);
      }
      text(360, 80, "A PAIR", 7, 1);
      text(360, 101, "B CANCEL", 7, 1);
      text(360, 122, "Y EXTRAS", 7, 1);
      text(360, 143, "Y EXTRAS", 7, 1);
      if (e.hint().first < 0 && !s.phase)
        text(18, 238, "NO PAIRS: UNDO OR NEW GAME", 1, 1);
    }
    if (s.game == 6) {
      box(9, 35, 462, 203, 5, false);
      for (int i = 0; i < 60; i++)
        if (s.a[i]) {
          int r = int(s.f[10 + i * 4]), x = int(s.f[8 + i * 4]), y = int(s.f[9 + i * 4]);
          box(x - r, y - r, 2 * r, 2 * r, 1 + i % 6);
        }
      circle(int(s.f[0]), int(s.f[1]), int(s.f[2]), 7);
      circle(int(s.f[0]), int(s.f[1]), int(s.f[2]) - 2, 0);
      text(14, 39, "TIME " + number(int(s.f[3])), 3, 1);
      text(14, 239, "DPAD/ANALOG MOVE - EAT SMALLER OBJECTS", 7, 1);
    }
    if (s.phase) {
      box(78, 93, 323, 87, 0);
      outline(78, 93, 323, 87, true);
      text(100, 110, s.phase == 1 ? "COMPLETE" : "ROUND OVER", 5, 3);
      text(100, 146, s.game == 6 ? "B NEW ROUND" : "A NEW ROUND   Y EXTRAS", 7, 1);
    }
  }
  if (help) {
    box(12, 34, 456, 207, 0);
    outline(12, 34, 456, 207, true);
    text(25, 48, "CONTROLS", 5);
    text(25, 75, "DPAD / BOTH STICKS: MOVE");
    text(25, 99, "A: ACTION     B: SECOND ACTION");
    text(25, 123, "X: ATTACK     Y: EXTRAS");
    text(25, 147, "START: MENU   SELECT: SAVE/EXIT");
    text(25, 171, "KEYBOARD: ARROWS Z X C V");
    text(25, 195, "ENTER MENU / ESC EXIT");
    text(25, 220, "B CLOSE", 3, 1);
  }
  if (!notice.empty()) {
    box(0, 263, 480, 9, 0);
    text(8, 264, notice, 1, 1);
  }
  SDL_RenderPresent(renderer);
}
volatile std::sig_atomic_t stopped = 0;
void stop(int) {
  stopped = 1;
}
int envInt(const char *key, int fallback) {
  const char *p = std::getenv(key);
  if (!p)
    return fallback;
  char *end = nullptr;
  long n = std::strtol(p, &end, 10);
  return end != p && !*end && n >= 0 && n < 256 ? int(n) : fallback;
}
} // namespace
int main(int argc, char **argv) {
  bool smoke = false, inputTest = false;
  int initial = -1;
  std::string shot, juicePreview;
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--smoke")
      smoke = true;
    else if (arg == "--juice-preview" && i + 1 < argc)
      juicePreview = argv[++i];
    else if (arg == "--input-test")
      inputTest = true;
    else if (arg == "--game" && i + 1 < argc) {
      std::string n = argv[++i];
      const char *ids[] = {"juices-sort", "new-blocks", "color-mix", "sudoku",
                           "traffic-jam", "mahjong",    "boca",      "color-loop"};
      for (int k = 0; k < 8; k++)
        if (n == ids[k])
          initial = k;
      if (initial < 0) {
        std::cerr << "Unknown game\n";
        return 2;
      }
    } else if (arg == "--screenshot" && i + 1 < argc)
      shot = argv[++i];
    else {
      std::cerr << "Usage: fake-mobile-games [--game ID] [--smoke] [--screenshot FILE.bmp] "
                   "[--input-test]\n";
      return 2;
    }
  }
  if (!juicePreview.empty()) {
    const std::vector<std::string> modes = {"board",   "selection", "pour",   "success", "help",
                                            "invalid", "options",   "levels", "restart"};
    if (!smoke || std::find(modes.begin(), modes.end(), juicePreview) == modes.end()) {
      std::cerr << "--juice-preview requires --smoke and a valid preview mode\n";
      return 2;
    }
    initial = 0;
  }
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS) !=
      0) {
    std::cerr << SDL_GetError() << '\n';
    return 1;
  }
  std::signal(SIGTERM, stop);
  std::signal(SIGINT, stop);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  const bool fullscreen = std::getenv("FMG_FULLSCREEN") != nullptr;
  // Embedded video backends may reject a desktop-sized window before fullscreen is set.
  const int windowWidth = fullscreen ? 480 : 960;
  const int windowHeight = fullscreen ? 272 : 544;
  const Uint32 windowFlags = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_RESIZABLE;
  SDL_Window *win =
      SDL_CreateWindow("Fake Mobile Games", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       windowWidth, windowHeight, windowFlags);
  if (!win) {
    std::cerr << SDL_GetError() << '\n';
    SDL_Quit();
    return 1;
  }
  renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer)
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
  if (!renderer) {
    std::cerr << SDL_GetError() << '\n';
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }
  SDL_RenderSetLogicalSize(renderer, 480, 272);
  std::cerr << "FMG video: " << SDL_GetCurrentVideoDriver() << "; fullscreen: " << fullscreen
            << '\n';
  std::vector<SDL_GameController *> controllers;
  std::vector<SDL_Joystick *> sticks;
  auto open = [&](int i) {
    if (SDL_IsGameController(i)) {
      auto *c = SDL_GameControllerOpen(i);
      if (c) {
        controllers.push_back(c);
        auto *j = SDL_GameControllerGetJoystick(c);
        std::cerr << "INPUT " << SDL_JoystickName(j) << " axes=" << SDL_JoystickNumAxes(j)
                  << " hats=" << SDL_JoystickNumHats(j) << " buttons=" << SDL_JoystickNumButtons(j)
                  << " mapped=1\n";
      }
    } else {
      auto *j = SDL_JoystickOpen(i);
      if (j) {
        sticks.push_back(j);
        std::cerr << "INPUT " << SDL_JoystickName(j) << " axes=" << SDL_JoystickNumAxes(j)
                  << " hats=" << SDL_JoystickNumHats(j) << " buttons=" << SDL_JoystickNumButtons(j)
                  << " mapped=0\n";
      }
    }
  };
  for (int i = 0; i < SDL_NumJoysticks(); i++)
    open(i);
  char *pref = SDL_GetPrefPath("FakeMobileGames", "FakeMobileGames");
  const char *custom = std::getenv("FMG_SAVE_DIR");
  std::string dir = custom ? custom : (pref ? pref : ".");
  SDL_free(pref);
  if (!dir.empty() && dir.back() != '/')
    dir += '/';
  Engine e(unsigned(std::time(nullptr)));
  bool menu = initial < 0, help = false, running = true;
  int menuCursor = initial < 0 ? 0 : initial;
  e.reset(menuCursor);
  auto path = [&](int g) { return dir + "game-" + number(g) + ".sav"; };
  auto restore = [&](int g) {
    State saved;
    if (load(path(g), saved) && saved.game == g) {
      if (g == 6 && !isCity(saved))
        e.reset(6);
      else {
        e.s = saved;
        if (g == 6) {
          ensureIsland(e.s);
          ensureCityLayout(e.s);
        }
      }
    }
  };
  restore(menuCursor);
  JuiceMotion juice;
  juice.progress.read(dir + "juices-progress.sav");
  if (menuCursor == 0) {
    State existing;
    if (!load(path(0), existing))
      startJuiceLevel(e, 0);
  }
  if (!juicePreview.empty()) {
    e.s = State{};
    const int board[] = {1, 2, 3, 4, 4, 3, 2, 1, 1, 4, 2, 3, 3, 2, 4, 1};
    std::copy(board, board + 16, e.s.a.begin());
    for (int i = 0; i < 4; i++)
      e.s.a[40 + i] = 4;
    if (juicePreview == "options" || juicePreview == "levels" || juicePreview == "restart") {
      startJuiceLevel(e, 0);
      juice.panel = juicePreview == "options" ? 1 : juicePreview == "levels" ? 2 : 3;
    }
    if (juicePreview == "selection")
      e.s.selected = 1;
    if (juicePreview == "pour") {
      e.s.selected = 1;
      e.s.cursor = 4;
      State before = e.s;
      e.pour(1, 4);
      e.s.selected = -1;
      juice.start(before, e.s);
      juice.elapsed = juice.duration * .49f;
    }
    if (juicePreview == "success") {
      e.s.a.fill(0);
      for (int i = 0; i < 4; i++) {
        e.s.a[40 + i] = 4;
        for (int j = 0; j < 4; j++)
          e.s.a[i * 4 + j] = i + 1;
      }
      e.s.phase = 1;
      e.s.score = 18;
    }
    if (juicePreview == "help")
      help = true;
    if (juicePreview == "invalid")
      juice.feedback("ESSAS CORES NAO COMBINAM", 2);
  }
  bool extras = false;
  int extraChoice = 0;
  std::string notice;
  auto persist = [&]() {
    juice.progress.record(e.s);
    bool progressSaved = e.s.game != 0 || juice.progress.write(dir + "juices-progress.sav", e.s);
    if (!save(path(e.s.game), e.s) || !progressSaved) {
      notice = "SAVE FAILED - CHECK DIRECTORY";
      std::cerr << notice << ": " << dir << '\n';
      return false;
    }
    return true;
  };
  if (e.s.game == 6 && !menu) {
    juice_art::Canvas loading(renderer);
    loading.paint(juice_art::Paint(32, 61, 55));
    SDL_RenderClear(renderer);
    loading.centered(240, 130, "PREPARANDO A ILHA...", juice_art::Paint(245, 235, 203), 2);
    SDL_RenderPresent(renderer);
    boca_ground::initialize(renderer);
    boca_sprites::initialize(renderer);
  }
  if (inputTest)
    std::cout << "Input test: raw button/axis IDs printed; SELECT/ESC exits.\n";
  std::array<bool, 4> held{};
  std::set<SDL_Keycode> directionKeys;
  Uint32 last = SDL_GetTicks(), lastSave = last, nextRepeat = 0;
  int frames = 0;
  auto action = [&](Key key) {
    if (key == Back) {
      if (!smoke)
        persist();
      running = false;
      return;
    }
    if (key == Start) {
      if (!smoke)
        persist();
      juice.finish();
      juice.panel = 0;
      extras = false;
      menu = !menu;
      menuCursor = e.s.game;
      help = false;
      return;
    }
    if (extras) {
      if (key == Up)
        extraChoice = (extraChoice + 3) % 4;
      if (key == Down)
        extraChoice = (extraChoice + 1) % 4;
      if (key == B || key == Y)
        extras = false;
      if (key == A) {
        extras = false;
        if (extraChoice == 0)
          e.input(e.s.game == 6 ? A : Y);
        if (extraChoice == 1) {
          if (e.s.game == 5 || e.s.game == 3)
            e.input(X);
          else
            help = true;
        }
        if (extraChoice == 2)
          e.reset(e.s.game);
        if (!smoke)
          persist();
      }
      return;
    }
    if (!menu && !help && e.s.game != 0 && key == Y) {
      extras = true;
      extraChoice = 0;
      return;
    }
    if (!menu && key == X)
      return; // Reserved for attack; these games currently have none.
    if (help) {
      if (key == B || key == Y || (e.s.game == 0 && key == X))
        help = false;
      return;
    }
    if (menu) {
      if (key == Up)
        menuCursor = (menuCursor + 7) % 8;
      if (key == Down)
        menuCursor = (menuCursor + 1) % 8;
      if (key == Y) {
        help = true;
        return;
      }
      if (key == B) {
        menu = false;
        return;
      }
      if (key == A || key == X) {
        {
          JuiceProgress progress = juice.progress;
          juice = JuiceMotion{};
          juice.progress = progress;
        }
        if (!smoke)
          persist();
        if (key == X || menuCursor != e.s.game) {
          int currentLevel = juiceLevel(e.s);
          e.reset(menuCursor);
          if (menuCursor == 0)
            startJuiceLevel(e, key == X ? std::max(0, currentLevel) : juice.progress.unlocked());
          if (key == A)
            restore(menuCursor);
        }
        menu = false;
      }
    } else if (e.s.game == 0) {
      if (juice.active)
        return;
      auto beginLevel = [&](int level) {
        startJuiceLevel(e, level);
        juice.finish();
        juice.panel = 0;
        juice.messageTime = 0;
        if (!smoke)
          persist();
      };
      if (juice.panel == 2) {
        if (key == Left || key == Up)
          juice.browse = std::max(0, juice.browse - 1);
        if (key == Right || key == Down)
          juice.browse = std::min(juice.progress.unlocked(), juice.browse + 1);
        if (key == B || key == Y)
          juice.panel = 1;
        if (key == A)
          beginLevel(juice.browse);
        return;
      }
      if (juice.panel == 1) {
        if (key == Up)
          juice.option = (juice.option + 5) % 6;
        if (key == Down)
          juice.option = (juice.option + 1) % 6;
        if (key == B || key == Y)
          juice.panel = 0;
        if (key == A) {
          int option = juice.option;
          juice.panel = 0;
          if (option == 0) {
            auto hint = juiceHint(e.s);
            if (hint.first >= 0) {
              e.s.selected = hint.first;
              e.s.cursor = hint.second;
              juice.feedback("DICA: " + number(hint.first + 1) + " > " + number(hint.second + 1));
            } else
              juice.feedback("SEM DICA: DESFACA OU REINICIE");
          }
          if (option == 1) {
            juice.panel = 3;
            return;
          }
          if (option == 2) {
            juice.panel = 2;
            juice.browse = std::max(0, juiceLevel(e.s));
          }
          if (option == 3)
            help = true;
          if (option == 4) {
            e.input(Y);
            juice.feedback("JOGADA DESFEITA");
            if (!smoke)
              persist();
          }
        }
        return;
      }
      if (juice.panel == 3) {
        if (key == A) {
          int level = juiceLevel(e.s);
          if (level >= 0)
            beginLevel(level);
          else {
            e.reset(0);
            juice.panel = 0;
            if (!smoke)
              persist();
          }
        }
        if (key == B || key == Y)
          juice.panel = 1;
        return;
      }
      if (e.s.phase && (key == A || key == B)) {
        juice.progress.record(e.s);
        int level = juiceLevel(e.s);
        if (key == A && level == juiceLevelCount - 1) {
          juice.panel = 2;
          juice.browse = level;
          return;
        }
        beginLevel(key == B ? std::max(0, level) : std::min(juiceLevelCount - 1, level + 1));
        return;
      }
      if (key == Y) {
        juice.panel = 1;
        juice.option = 0;
        return;
      }
      State before = e.s;
      e.input(key);
      if (juice.start(before, e.s)) {
        juice.progress.record(e.s);
        if (!smoke)
          persist();
      } else if (key == A && !before.phase) {
        if (before.selected < 0 && before.a[40 + before.cursor] == 0)
          juice.feedback("ESTE TUBO ESTA VAZIO", before.cursor);
        else if (before.selected >= 0 && before.selected != before.cursor) {
          juice.feedback(before.a[40 + before.cursor] == 4 ? "ESSE TUBO JA ESTA CHEIO"
                                                           : "ESSAS CORES NAO COMBINAM",
                         before.cursor);
        }
      } else if (key == Y) {
        juice.feedback(before.a == e.s.a ? "NAO HA JOGADAS PARA DESFAZER" : "JOGADA DESFEITA");
        if (!smoke)
          persist();
      }
    } else if (e.s.game == 7 && e.s.phase && (key == A || key == B)) {
      int level = e.s.a[200], unlocked = e.s.a[201];
      if (key == A && e.s.phase == 1)
        level = std::min(loopLevels - 1, level + 1);
      initLoop(e.s, level, unlocked);
      e.undo.clear();
      if (!smoke)
        persist();
    } else if ((e.s.game == 1 || e.s.game == 5) && e.s.phase && key == A) {
      e.reset(e.s.game);
    } else if (key == B && e.s.phase) {
      e.reset(e.s.game);
    } else
      e.input(key);
  };
  auto keyboard = [&](SDL_Keycode c) -> int {
    const char *vars[] = {"FMG_KEY_LEFT",  "FMG_KEY_RIGHT", "FMG_KEY_UP", "FMG_KEY_DOWN",
                          "FMG_KEY_A",     "FMG_KEY_B",     "FMG_KEY_X",  "FMG_KEY_Y",
                          "FMG_KEY_START", "FMG_KEY_SELECT"};
    for (int i = 0; i < 10; i++) {
      const char *name = std::getenv(vars[i]);
      if (name && SDL_GetKeyFromName(name) == c)
        return i;
    }
    const char *rightVars[] = {"FMG_KEY_RIGHT_LEFT", "FMG_KEY_RIGHT_RIGHT", "FMG_KEY_RIGHT_UP",
                               "FMG_KEY_RIGHT_DOWN"};
    for (int i = 0; i < 4; ++i) {
      const char *name = std::getenv(rightVars[i]);
      if (name && SDL_GetKeyFromName(name) == c)
        return i;
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
    default:
      return -1;
    }
  };
  int buttons[] = {envInt("FMG_BUTTON_A", 0),     envInt("FMG_BUTTON_B", 1),
                   envInt("FMG_BUTTON_X", 2),     envInt("FMG_BUTTON_Y", 3),
                   envInt("FMG_BUTTON_START", 7), envInt("FMG_BUTTON_SELECT", 6)};
  while (running && !stopped) {
    Uint32 now = SDL_GetTicks();
    float dt = std::min(0.05f, float(now - last) / 1000);
    last = now;
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_QUIT)
        running = false;
      if (ev.type == SDL_JOYDEVICEADDED) {
        SDL_JoystickID id = SDL_JoystickGetDeviceInstanceID(ev.jdevice.which);
        bool exists = false;
        for (auto *j : sticks)
          if (SDL_JoystickInstanceID(j) == id)
            exists = true;
        for (auto *c : controllers)
          if (SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(c)) == id)
            exists = true;
        if (!exists)
          open(ev.jdevice.which);
      }
      if (ev.type == SDL_KEYDOWN && !ev.key.repeat) {
        if (inputTest)
          std::cout << "KEY " << SDL_GetKeyName(ev.key.keysym.sym) << std::endl;
        int key = keyboard(ev.key.keysym.sym);
        if (key >= 0) {
          if (key < 4) {
            directionKeys.insert(ev.key.keysym.sym);
            held[size_t(key)] = true;
          }
          action(Key(key));
          nextRepeat = now + 240;
        }
      }
      if (ev.type == SDL_KEYUP) {
        int key = keyboard(ev.key.keysym.sym);
        if (key >= 0 && key < 4)
          directionKeys.erase(ev.key.keysym.sym);
      }
      if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
        held.fill(false);
        directionKeys.clear();
      }
      if (ev.type == SDL_CONTROLLERBUTTONDOWN) {
        int key = -1;
        switch (ev.cbutton.button) {
        case SDL_CONTROLLER_BUTTON_A:
          key = A;
          break;
        case SDL_CONTROLLER_BUTTON_B:
          key = B;
          break;
        case SDL_CONTROLLER_BUTTON_X:
          key = X;
          break;
        case SDL_CONTROLLER_BUTTON_Y:
          key = Y;
          break;
        case SDL_CONTROLLER_BUTTON_START:
          key = Start;
          break;
        case SDL_CONTROLLER_BUTTON_BACK:
          key = Back;
          break;
        default:
          break;
        }
        if (key >= 0)
          action(Key(key));
      }
      if (inputTest && ev.type == SDL_JOYBUTTONDOWN)
        std::cout << "BUTTON " << ev.jbutton.which << " " << int(ev.jbutton.button) << std::endl;
      if (ev.type == SDL_JOYBUTTONDOWN && !SDL_GameControllerFromInstanceID(ev.jbutton.which)) {
        for (int k = 0; k < 6; k++)
          if (ev.jbutton.button == buttons[k])
            action(Key(A + k));
      }
      if (inputTest && ev.type == SDL_JOYHATMOTION)
        std::cout << "HAT " << ev.jhat.which << " " << int(ev.jhat.hat) << " " << int(ev.jhat.value)
                  << std::endl;
      if (inputTest && ev.type == SDL_JOYAXISMOTION)
        std::cout << "AXIS " << ev.jaxis.which << " " << int(ev.jaxis.axis) << " " << ev.jaxis.value
                  << std::endl;
    }
    held.fill(false);
    for (auto code : directionKeys) {
      int key = keyboard(code);
      if (key >= 0 && key < 4)
        held[size_t(key)] = true;
    }
    int dx = int(held[Right]) - int(held[Left]), dy = int(held[Down]) - int(held[Up]);
    for (auto *c : controllers) {
      auto d = joystickDirection(SDL_GameControllerGetJoystick(c), c);
      dx += d.x;
      dy += d.y;
    }
    for (auto *j : sticks) {
      auto d = joystickDirection(j);
      dx += d.x;
      dy += d.y;
    }
    dx = std::max(-1, std::min(1, dx));
    dy = std::max(-1, std::min(1, dy));
    if ((dx || dy) && now >= nextRepeat) {
      if (dx)
        action(dx > 0 ? Right : Left);
      if (dy)
        action(dy > 0 ? Down : Up);
      nextRepeat = now + 140;
    }
    if (!dx && !dy)
      nextRepeat = now;
    if (!menu && !help && !extras && !juice.panel) {
      e.tick(dt, dx, dy);
      if (juicePreview.empty())
        juice.update(dt);
    }
    if (!smoke && now - lastSave >= 5000) {
      persist();
      lastSave = now;
    }
    render(e, menu, menuCursor, help, notice, juice, extras, extraChoice);
    if (smoke && ++frames == 3) {
      if (!shot.empty()) {
        int w = 0, h = 0;
        SDL_GetRendererOutputSize(renderer, &w, &h);
        SDL_Surface *surface =
            SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ARGB8888);
        if (!surface ||
            SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_ARGB8888, surface->pixels,
                                 surface->pitch) != 0 ||
            SDL_SaveBMP(surface, shot.c_str()) != 0) {
          std::cerr << SDL_GetError() << '\n';
          SDL_FreeSurface(surface);
          return 1;
        }
        SDL_FreeSurface(surface);
      }
      running = false;
    }
    Uint32 elapsed = SDL_GetTicks() - now;
    if (elapsed < 16)
      SDL_Delay(16 - elapsed);
  }
  if (!smoke)
    persist();
  for (auto *c : controllers)
    SDL_GameControllerClose(c);
  for (auto *j : sticks)
    SDL_JoystickClose(j);
  boca_ground::release();
  boca_sprites::release();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}
