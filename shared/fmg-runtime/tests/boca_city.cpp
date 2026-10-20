#include "game.hpp"
#include <iostream>
#include <stdexcept>
using namespace fmg;
void check(bool x, const char *message) {
  if (!x)
    throw std::runtime_error(message);
}
int main() {
  try {
    for (unsigned seed = 1; seed <= 8; ++seed) {
      Engine e(seed);
      e.reset(6);
      check(isCity(e.s) && validState(e.s), "valid seeded city");
      for (int i = 0; i < cityCount; ++i)
        check(islandMetric(e.s.f[8 + i * 4], e.s.f[9 + i * 4], e.s.f[10 + i * 4] + 17) < 1.002f,
              "all generated objects fit island");
      int orientations = 0;
      for (int object = 100; object < cityCount; ++object)
        check(cityZoneFits(e.s, object, e.s.f[8 + object * 4], e.s.f[9 + object * 4]),
              "vehicles on asphalt and buildings inside lots");
      for (int object = 100; object < 150; ++object)
        orientations += cityVertical(e.s, object);
      check(orientations > 0 && orientations < 50, "vehicles use both street directions");
      int types[6] = {};
      for (int i = 0; i < cityCount; ++i)
        ++types[e.s.a[300 + i]];
      check(types[0] == 60 && types[1] == 40 && types[2] == 30 && types[3] == 20 &&
                types[4] == 18 && types[5] == 12,
            "complete six-stage object population");
      check(save("city-test.sav", e.s), "city save");
      State restored;
      check(load("city-test.sav", restored) && restored.a == e.s.a && restored.f == e.s.f,
            "extended city save roundtrip");
      for (int frame = 0; frame < 12001 && !e.s.phase; ++frame) {
        int target = -1;
        float best = 1e20;
        for (int i = 0; i < cityCount; ++i)
          if (e.s.a[i] && e.s.f[2] > e.s.f[10 + i * 4] + 2) {
            float x = e.s.f[8 + i * 4] - e.s.f[0], y = e.s.f[9 + i * 4] - e.s.f[1];
            float priority = (x * x + y * y) / std::pow(e.s.f[10 + i * 4] + 4, 1.2f);
            if (priority < best) {
              best = priority;
              target = i;
            }
          }
        if (target < 0)
          break;
        float x = e.s.f[8 + target * 4] - e.s.f[0], y = e.s.f[9 + target * 4] - e.s.f[1];
        if (e.s.f[7] <= 0)
          e.input(A);
        tickCity(e.s, .05f, ((x > 2) - (x < -2)) * .70710678f, ((y > 2) - (y < -2)) * .70710678f);
        if (frame % 100 == 0)
          check(validState(e.s), "city simulation remains valid");
      }
      std::cout << "seed " << seed << ": buildings=" << e.s.a[254] << " score=" << e.s.score
                << " radius=" << e.s.f[2] << " time=" << e.s.f[3] << "\n";
      check(e.s.phase == 1 && e.s.a[254] >= 3, "full growth route wins with active enemies");
      check(save("city-test.sav", e.s) && load("city-test.sav", restored) && restored.a == e.s.a &&
                restored.f == e.s.f,
            "expanded late-game city roundtrip");
    }
    Engine e(17);
    e.reset(6);
    e.s.a.fill(0);
    e.s.a[255] = 1401;
    // A larger rival damages the player once, then grants a grace period.
    e.s.a[800] = 1;
    e.s.f[800] = e.s.f[0];
    e.s.f[801] = e.s.f[1];
    e.s.f[802] = 30;
    e.s.f[2] = 20;
    e.tick(.01f, 0, 0);
    check(e.s.f[2] == 16 && e.s.f[901] == 5, "rival damage and five-second protection");
    float radius = e.s.f[2];
    e.s.f[0] = e.s.f[800];
    e.s.f[1] = e.s.f[801];
    e.tick(.01f, 0, 0);
    check(e.s.f[2] == radius, "protection prevents repeated contact damage");
    for (int frame = 0; frame < 90; ++frame) {
      e.s.f[0] = e.s.f[800];
      e.s.f[1] = e.s.f[801];
      e.tick(.05f, 0, 0);
    }
    check(e.s.f[2] == radius && e.s.f[901] > 0, "contact protection lasts beyond four seconds");
    for (int frame = 0; frame < 20; ++frame) {
      e.s.f[0] = e.s.f[800];
      e.s.f[1] = e.s.f[801];
      e.tick(.05f, 0, 0);
    }
    check(e.s.f[2] < radius && e.s.f[901] > 4, "contact damage resumes after five seconds");
    e.s.f[2] = 60;
    e.s.f[0] = e.s.f[800];
    e.s.f[1] = e.s.f[801];
    e.tick(.01f, 0, 0);
    check(!e.s.a[800] && e.s.f[2] > 60, "player absorbs smaller rival");

    e.reset(6);
    for (int rival = 0; rival < 3; ++rival)
      e.s.a[800 + rival] = 0;
    e.s.f[0] = e.s.f[8];
    e.s.f[1] = e.s.f[9];
    e.tick(.01f, 0, 0);
    check(!e.s.a[0] && e.s.a[500] > 0, "player consumption starts respawn timer");
    e.s.f[0] = 3100;
    e.s.f[1] = 1900;
    for (int frame = 0; frame < 410; ++frame)
      e.tick(.05f, 0, 0);
    check(e.s.a[0] && e.s.a[500] == 0 && validState(e.s),
          "food reappears after delay in valid world location");
    cityConsume(e.s, 20, 2);
    check(e.s.a[cityOwnerSlot(20)] == 3, "rival consumption tracks animation owner");
    e.s.a[520] = 1000;
    check(save("city-test.sav", e.s), "respawn timer save");
    State returning;
    check(load("city-test.sav", returning) && returning.a[520] == 1000,
          "respawn timer resumes after load");
    // Isolate steering: no food and an outward escape direction at the west edge.
    e.reset(6);
    for (int j = 0; j < cityCount; ++j) {
      e.s.a[j] = 0;
      e.s.a[500 + j] = 90000;
    }
    e.s.a[801] = e.s.a[802] = 0;
    e.s.a[800] = 1;
    e.s.f[800] = 12;
    e.s.f[801] = 900;
    e.s.f[802] = 12;
    islandClamp(e.s.f[800], e.s.f[801]);
    e.s.f[0] = e.s.f[800] + 120;
    e.s.f[1] = e.s.f[801];
    e.s.f[2] = 90;
    float startY = e.s.f[801];
    for (int frame = 0; frame < 60; ++frame)
      e.tick(.05f, 0, 0);
    check(e.s.a[800] && std::abs(e.s.f[801] - startY) > 30,
          "rival slides along boundary while escaping");
    e.s.f[0] = 3000;
    e.s.f[1] = 1900;
    e.s.f[2] = 9;
    float startX = e.s.f[800];
    startY = e.s.f[801];
    for (int frame = 0; frame < 60; ++frame)
      e.tick(.05f, 0, 0);
    check(std::hypot(e.s.f[800] - startX, e.s.f[801] - startY) > 80,
          "rival explores when no edible target exists");
    e.reset(6);
    for (int j = 0; j < cityCount; ++j) {
      e.s.a[j] = 0;
      e.s.a[500 + j] = 90000;
    }
    e.s.a[802] = 0;
    e.s.f[0] = 3000;
    e.s.f[1] = 1900;
    e.s.f[800] = 1000;
    e.s.f[801] = 1000;
    e.s.f[802] = 40;
    e.s.f[808] = 1001;
    e.s.f[809] = 1000;
    e.s.f[810] = 16;
    e.tick(.01f, 0, 0);
    check(e.s.f[810] < 16 && e.s.f[813] == 5 && e.s.f[802] > 40,
          "rivals attack each other and victim receives protection");
    float victimRadius = e.s.f[810];
    e.s.f[808] = e.s.f[800];
    e.s.f[809] = e.s.f[801];
    e.tick(.01f, 0, 0);
    check(e.s.f[810] == victimRadius, "rival protection blocks repeated attacks");
    e.s.a[800] = 0;
    e.s.f[804] = .1f;
    e.tick(.05f, 0, 0);
    e.tick(.05f, 0, 0);
    check(e.s.a[800] && e.s.f[805] > 0, "absorbed rival returns with grace period");

    e.reset(6);
    e.s.a[998] = 0;
    e.s.f[0] = 0;
    e.s.f[1] = 0;
    e.s.f[8] = 3199;
    e.s.f[9] = 1;
    ensureIsland(e.s);
    check(islandMetric(e.s.f[0], e.s.f[1], 11) <= 1.001f &&
              islandMetric(e.s.f[8], e.s.f[9], e.s.f[10] + 17) <= 1.001f,
          "old city save projects onto land");
    for (int frame = 0; frame < 200; ++frame)
      e.tick(.05f, -1, -1);
    check(islandMetric(e.s.f[0], e.s.f[1], 11) <= 1.001f, "player cannot cross coast");

    e.reset(6);
    for (int object = 100; object < cityCount; ++object) {
      cityConsume(e.s, object, 0);
      cityRespawn(e.s, object);
      check(cityZoneFits(e.s, object, e.s.f[8 + object * 4], e.s.f[9 + object * 4]),
            "respawn preserves asphalt and lot zoning");
    }
    e.s.a[997] = 0;
    e.s.f[608] = 1600;
    e.s.f[609] = 800; // house deliberately placed on a crossroads
    e.s.score = 7;
    e.s.f[2] = 42;
    ensureCityLayout(e.s);
    check(e.s.score == 7 && e.s.f[2] == 42, "layout migration preserves player progress");
    for (int object = 100; object < cityCount; ++object)
      check(cityZoneFits(e.s, object, e.s.f[8 + object * 4], e.s.f[9 + object * 4]),
            "old save relocates vehicles and buildings into correct zones");
    std::remove("city-test.sav");
    std::cout << "PASS city generation, full growth, rivals and saves\n";
    return 0;
  } catch (const std::exception &error) {
    std::cerr << "FAIL " << error.what() << "\n";
    return 1;
  }
}
