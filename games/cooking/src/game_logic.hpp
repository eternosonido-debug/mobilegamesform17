#pragma once
#include <algorithm>
#include <array>
#include <cstdio>
#include <fstream>
#include <string>
namespace cooking {
constexpr int FlavorCount = 3, AlbumSize = 8;
enum Stage { Ingredients, Mixing, Filling, Baking, Decorating, Photo };
enum Animation { Idle = -1, FinishMix = 6, FillCup, PutIn, TakeOut };
struct Design {
  int icing = 0, sprinkles = 0, fruit = 0, face = 1, wrapper = 0;
  bool operator==(const Design &o) const {
    return icing == o.icing && sprinkles == o.sprinkles && fruit == o.fruit &&
           face == o.face && wrapper == o.wrapper;
  }
  bool operator!=(const Design &o) const { return !(*this == o); }
};
struct Creation {
  int flavor = 0, serial = 0;
  std::array<Design, 4> cakes;
};
struct State {
  int flavor = 0, stage = Ingredients, selected = 0;
  unsigned used = 0, filled = 0;
  int mixed = 0, lastDirection = -1, bake = 0, heat = 2, oven = 0;
  int pending = Idle, anim = 0, total = 0, target = 0, ticks = 0;
  int cake = 0, tool = 0, option = 1;
  std::array<Design, 4> cakes;
  bool undoValid = false;
  int undoCake = 0;
  Design undo;
};
inline const char *flavorName(int f) {
  const char *n[] = {"BAUNILHA SOLAR", "CHOCOLATE POP", "MORANGO MAGICO"};
  return n[std::max(0, std::min(2, f))];
}
inline const char *itemName(int i, int f) {
  const char *names[] = {"FARINHA", "ACUCAR", "MANTEIGA",
                         "OVOS",    "LEITE",  "SABOR"};
  return i == 5 ? (f == 0   ? "BAUNILHA"
                   : f == 1 ? "CACAU"
                            : "MORANGO")
                : names[std::max(0, std::min(5, i))];
}
inline int optionCount(int tool) {
  const int counts[] = {7, 4, 5, 4, 6, 1};
  return counts[std::max(0, std::min(5, tool))];
}
inline int designValue(const Design &d, int tool) {
  return tool == 0   ? d.icing
         : tool == 1 ? d.sprinkles
         : tool == 2 ? d.fruit
         : tool == 3 ? d.face
         : tool == 4 ? d.wrapper
                     : 0;
}
inline void setDesignValue(Design &d, int tool, int v) {
  if (tool == 0)
    d.icing = v;
  else if (tool == 1)
    d.sprinkles = v;
  else if (tool == 2)
    d.fruit = v;
  else if (tool == 3)
    d.face = v;
  else if (tool == 4)
    d.wrapper = v;
}
inline bool validDesign(const Design &d) {
  return d.icing >= 0 && d.icing < 7 && d.sprinkles >= 0 && d.sprinkles < 4 &&
         d.fruit >= 0 && d.fruit < 5 && d.face >= 0 && d.face < 4 &&
         d.wrapper >= 0 && d.wrapper < 6;
}
struct Game {
  State s;
  bool muted = false;
  std::array<Creation, AlbumSize> album;
  int albumCount = 0, albumNext = 0, serial = 0;
  std::array<int, FlavorCount> made{{0, 0, 0}};
  void begin(int flavor) {
    s = State();
    s.flavor = std::max(0, std::min(2, flavor));
    for (int i = 0; i < 4; ++i)
      s.cakes[i].wrapper = (s.flavor + i) % 6;
  }
  int mixGoal() const { return 12 + s.flavor * 4; }
  int bakeGoal() const {
    return 1800 + s.flavor * 360;
  } // half-ticks: 15/18/21 s at normal heat
  bool golden() const { return s.bake >= bakeGoal(); }
  void animate(int kind, int ticks, int target = 0) {
    s.pending = kind;
    s.anim = 0;
    s.total = ticks;
    s.target = target;
  }
  void next(int stage) {
    s.stage = stage;
    s.selected = 0;
  }
  void tick() {
    if (s.stage == Photo)
      return;
    ++s.ticks;
    if (s.pending != Idle) {
      if (++s.anim < s.total)
        return;
      int a = s.pending;
      s.pending = Idle;
      s.anim = s.total = 0;
      if (a >= 0 && a < 6) {
        s.used |= 1u << a;
        if (s.used == 63)
          next(Mixing);
      } else if (a == FinishMix)
        next(Filling);
      else if (a == FillCup) {
        s.filled |= 1u << s.target;
        if (s.filled == 15)
          next(Baking);
      } else if (a == PutIn)
        s.oven = 1;
      else if (a == TakeOut)
        next(Decorating);
      return;
    }
    if (s.stage == Baking && s.oven)
      s.bake = std::min(bakeGoal(), s.bake + s.heat);
  }
  void chooseTool(int tool) {
    s.tool = (tool + 6) % 6;
    s.option = designValue(s.cakes[s.cake], s.tool);
  }
  // Directions arranged clockwise: right=0, down=1, left=2, up=3.
  bool direction(int d) {
    if (d < 0 || d > 3 || s.pending != Idle)
      return false;
    if (s.stage == Ingredients) {
      s.selected = (s.selected + (d == 0 ? 1 : d == 2 ? 5 : 3)) % 6;
      return true;
    }
    if (s.stage == Mixing) {
      if (s.lastDirection >= 0) {
        int turn = (d - s.lastDirection + 4) % 4;
        if (turn != 1 && turn != 3)
          return false;
      }
      s.lastDirection = d;
      if (++s.mixed == mixGoal())
        animate(FinishMix, 36);
      return true;
    }
    if (s.stage == Filling) {
      int x = s.selected % 2, y = s.selected / 2;
      if (d % 2 == 0)
        x = 1 - x;
      else
        y = 1 - y;
      s.selected = y * 2 + x;
      return true;
    }
    if (s.stage == Baking && d % 2 == 0) {
      s.heat = std::max(1, std::min(3, s.heat + (d == 0 ? 1 : -1)));
      return true;
    }
    if (s.stage == Decorating) {
      if (d % 2 == 0)
        chooseTool(s.tool + (d == 0 ? 1 : 5));
      else
        s.option = (s.option + (d == 3 ? 1 : optionCount(s.tool) - 1)) %
                   optionCount(s.tool);
      return true;
    }
    return false;
  }
  bool action() {
    if (s.pending != Idle)
      return false;
    if (s.stage == Ingredients) {
      if (s.used & (1u << s.selected))
        return false;
      animate(s.selected, s.selected == 3 ? 90 : 66);
      return true;
    }
    if (s.stage == Filling) {
      if (s.filled & (1u << s.selected))
        return false;
      animate(FillCup, 48, s.selected);
      return true;
    }
    if (s.stage == Baking) {
      if (!s.oven) {
        animate(PutIn, 54);
        return true;
      }
      if (golden()) {
        animate(TakeOut, 54);
        return true;
      }
      return false;
    }
    if (s.stage == Decorating) {
      if (s.tool == 5) {
        ++serial;
        ++made[s.flavor];
        Creation c;
        c.flavor = s.flavor;
        c.serial = serial;
        c.cakes = s.cakes;
        album[albumNext] = c;
        albumNext = (albumNext + 1) % AlbumSize;
        albumCount = std::min(AlbumSize, albumCount + 1);
        s.stage = Photo;
        s.undoValid = false;
        return true;
      }
      if (designValue(s.cakes[s.cake], s.tool) == s.option)
        return false;
      s.undo = s.cakes[s.cake];
      s.undoCake = s.cake;
      s.undoValid = true;
      setDesignValue(s.cakes[s.cake], s.tool, s.option);
      return true;
    }
    return false;
  }
  bool undo() {
    if (s.stage != Decorating || !s.undoValid)
      return false;
    s.cake = s.undoCake;
    s.cakes[s.cake] = s.undo;
    s.undoValid = false;
    chooseTool(s.tool);
    return true;
  }
  void nextCake() {
    if (s.stage == Decorating) {
      s.cake = (s.cake + 1) % 4;
      chooseTool(s.tool);
    }
  }
  static void writeDesign(std::ostream &f, const Design &d) {
    f << ' ' << d.icing << ' ' << d.sprinkles << ' ' << d.fruit << ' ' << d.face
      << ' ' << d.wrapper;
  }
  static bool readDesign(std::istream &f, Design &d) {
    return bool(f >> d.icing >> d.sprinkles >> d.fruit >> d.face >>
                d.wrapper) &&
           validDesign(d);
  }
  bool save(const std::string &path) const {
    std::string temp = path + ".tmp";
    std::ofstream f(temp);
    f << "COOK1 " << muted << ' ' << albumCount << ' ' << albumNext << ' '
      << serial;
    for (int n : made)
      f << ' ' << n;
    for (const auto &c : album) {
      f << ' ' << c.flavor << ' ' << c.serial;
      for (const auto &d : c.cakes)
        writeDesign(f, d);
    }
    f << ' ' << s.flavor << ' ' << s.stage << ' ' << s.selected << ' ' << s.used
      << ' ' << s.filled << ' ' << s.mixed << ' ' << s.lastDirection << ' '
      << s.bake << ' ' << s.heat << ' ' << s.oven << ' ' << s.pending << ' '
      << s.anim << ' ' << s.total << ' ' << s.target << ' ' << s.ticks << ' '
      << s.cake << ' ' << s.tool << ' ' << s.option << ' ' << s.undoValid << ' '
      << s.undoCake;
    for (const auto &d : s.cakes)
      writeDesign(f, d);
    writeDesign(f, s.undo);
    f << '\n';
    f.close();
    return bool(f) && std::rename(temp.c_str(), path.c_str()) == 0;
  }
  bool valid() const {
    if (albumCount < 0 || albumCount > AlbumSize || albumNext < 0 ||
        albumNext >= AlbumSize || serial < 0 || serial > 1000000 ||
        albumCount != std::min(serial, AlbumSize) ||
        albumNext != serial % AlbumSize)
      return false;
    int sum = 0;
    for (int n : made) {
      if (n < 0 || n > 1000000)
        return false;
      sum += n;
    }
    if (sum != serial)
      return false;
    for (int i = 0; i < AlbumSize; ++i) {
      const auto &c = album[i];
      if (c.flavor < 0 || c.flavor >= FlavorCount || c.serial < 0 ||
          c.serial > serial)
        return false;
      for (const auto &d : c.cakes)
        if (!validDesign(d))
          return false;
      if (i < albumCount && c.serial == 0)
        return false;
    }
    if (s.flavor < 0 || s.flavor >= FlavorCount || s.stage < Ingredients ||
        s.stage > Photo || s.selected < 0 ||
        s.selected >= (s.stage == Ingredients ? 6
                       : s.stage == Filling   ? 4
                                              : 1) ||
        s.used > 63 || s.filled > 15 || s.mixed < 0 || s.mixed > mixGoal() ||
        s.lastDirection < -1 || s.lastDirection > 3 || s.bake < 0 ||
        s.bake > bakeGoal() || s.heat < 1 || s.heat > 3 || s.oven < 0 ||
        s.oven > 1 || s.pending < Idle || s.pending > TakeOut || s.anim < 0 ||
        s.total < 0 || s.total > 90 || s.target < 0 || s.target > 3 ||
        s.ticks < 0 || s.ticks > 100000000 || s.cake < 0 || s.cake > 3 ||
        s.tool < 0 || s.tool > 5 || s.option < 0 ||
        s.option >= optionCount(s.tool) || s.undoCake < 0 || s.undoCake > 3 ||
        !validDesign(s.undo))
      return false;
    for (const auto &d : s.cakes)
      if (!validDesign(d))
        return false;
    if (s.stage > Ingredients && s.used != 63)
      return false;
    if (s.stage > Mixing && s.mixed != mixGoal())
      return false;
    if (s.stage > Filling && s.filled != 15)
      return false;
    if (s.stage > Baking && (!s.oven || !golden()))
      return false;
    if (s.pending == Idle && (s.anim || s.total))
      return false;
    if (s.pending != Idle) {
      if (s.total <= 0 || s.anim >= s.total)
        return false;
      if (s.pending < 6 &&
          (s.stage != Ingredients || (s.used & (1u << s.pending)) ||
           s.total != (s.pending == 3 ? 90 : 66)))
        return false;
      if (s.pending == FinishMix &&
          (s.stage != Mixing || s.mixed != mixGoal() || s.total != 36))
        return false;
      if (s.pending == FillCup &&
          (s.stage != Filling || (s.filled & (1u << s.target)) ||
           s.total != 48))
        return false;
      if (s.pending == PutIn && (s.stage != Baking || s.oven || s.total != 54))
        return false;
      if (s.pending == TakeOut &&
          (s.stage != Baking || !s.oven || !golden() || s.total != 54))
        return false;
    }
    if ((s.stage == Ingredients && s.used == 63) ||
        (s.stage == Mixing && s.mixed == mixGoal() && s.pending != FinishMix) ||
        (s.stage == Filling && s.filled == 15))
      return false;
    return true;
  }
  bool load(const std::string &path) {
    std::ifstream f(path);
    std::string magic;
    Game g;
    if (!(f >> magic >> g.muted >> g.albumCount >> g.albumNext >> g.serial) ||
        magic != "COOK1")
      return false;
    for (int &n : g.made)
      if (!(f >> n))
        return false;
    for (auto &c : g.album) {
      if (!(f >> c.flavor >> c.serial))
        return false;
      for (auto &d : c.cakes)
        if (!readDesign(f, d))
          return false;
    }
    auto &v = g.s;
    if (!(f >> v.flavor >> v.stage >> v.selected >> v.used >> v.filled >>
          v.mixed >> v.lastDirection >> v.bake >> v.heat >> v.oven >>
          v.pending >> v.anim >> v.total >> v.target >> v.ticks >> v.cake >>
          v.tool >> v.option >> v.undoValid >> v.undoCake))
      return false;
    for (auto &d : v.cakes)
      if (!readDesign(f, d))
        return false;
    if (!readDesign(f, v.undo) || !g.valid())
      return false;
    *this = g;
    return true;
  }
};
} // namespace cooking
