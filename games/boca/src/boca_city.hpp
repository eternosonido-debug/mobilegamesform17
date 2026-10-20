#pragma once
namespace fmg {
static const int cityCount = 180;
static const float cityWidth = 3200, cityHeight = 2000;
inline float islandWave(float angle) {
  return 1 + .022f * std::sin(angle * 5) + .012f * std::sin(angle * 9 + .6f);
}
inline float islandMetric(float x, float y, float margin = 0) {
  float nx = (x - 1600) / (1520 - margin), ny = (y - 1000) / (920 - margin);
  return std::pow(nx * nx * nx * nx + ny * ny * ny * ny, .25f) / islandWave(std::atan2(ny, nx));
}
inline void islandClamp(float &x, float &y, float margin = 12) {
  float metric = islandMetric(x, y, margin);
  if (metric > 1) {
    x = 1600 + (x - 1600) / metric;
    y = 1000 + (y - 1000) / metric;
  }
}
inline void ensureIsland(State &s) {
  if (s.a[998] == 1601)
    return;
  islandClamp(s.f[0], s.f[1]);
  for (int i = 0; i < cityCount; ++i)
    islandClamp(s.f[8 + i * 4], s.f[9 + i * 4], s.f[10 + i * 4] + 18);
  for (int i = 0; i < 3; ++i) {
    islandClamp(s.f[800 + i * 8], s.f[801 + i * 8]);
    if (s.f[806 + i * 8] > 0)
      islandClamp(s.f[806 + i * 8], s.f[807 + i * 8], 60);
  }
  s.a[998] = 1601;
}
inline bool isCity(const State &s) {
  return s.game == 6 && s.a[255] == 1401;
}
inline float cityGrowth(float radius, float size) {
  return std::sqrt(radius * radius + size * size * .4f + 4);
}
// FMG 2 has spare slots: timers 500..679, owners skip rival flags 800..802.
inline int cityOwnerSlot(int object) {
  return 680 + object + (object >= 120 ? 3 : 0);
}
inline unsigned cityRandom(State &s) {
  unsigned n = unsigned(s.a[999]);
  if (!n)
    n = 2463534242u;
  n ^= n << 13;
  n ^= n >> 17;
  n ^= n << 5;
  s.a[999] = int(n & 0x7fffffffu);
  return n;
}
static const float cityRoadWidth = 112, citySidewalk = 124;
inline float cityModulo(float value) {
  return value - std::floor(value / 400) * 400;
}
inline bool cityVertical(const State &s, int object) {
  return object >= 100 && object < 150 && s.a[180 + object - 100] != 0;
}
inline bool citySpriteFlip(const State &s, int object) {
  if (object >= 100 && object < 150)
    return cityVertical(s, object);
  if (object >= 150) {
    float mx = cityModulo(s.f[8 + object * 4]), my = cityModulo(s.f[9 + object * 4]);
    return std::min(mx - citySidewalk, 400 - mx) < std::min(my - citySidewalk, 400 - my);
  }
  return false;
}
inline bool cityZoneFits(const State &s, int object, float x, float y) {
  int kind = s.a[300 + object];
  float r = s.f[10 + object * 4];
  if (islandMetric(x, y, r + ((kind == 2 || kind == 3) ? 46.f : 18.f)) > 1)
    return false;
  float mx = cityModulo(x), my = cityModulo(y);
  if (kind >= 4)
    return mx >= citySidewalk + r + 12 && my >= citySidewalk + r + 12 && mx <= 400 - r - 12 &&
           my <= 400 - r - 12;
  if (kind == 2 || kind == 3) {
    float cross = cityVertical(s, object) ? mx : my;
    return cross >= r + 3 && cross <= cityRoadWidth - r - 3;
  }
  return true;
}
inline void cityPlace(State &s, int object, bool avoidActors) {
  int b = 8 + object * 4, kind = s.a[300 + object];
  float r = s.f[b + 2], best = 1e30, bx = 1600, by = 1000;
  bool bestVertical = false;
  for (int attempt = 0; attempt < 1200; ++attempt) {
    float x = r + 20 + cityRandom(s) % unsigned(cityWidth - 2 * r - 40),
          y = r + 20 + cityRandom(s) % unsigned(cityHeight - 2 * r - 40);
    if (kind >= 4) {
      float low = citySidewalk + r + 12, span = 400 - r - 12 - low;
      x = (cityRandom(s) % 8) * 400 + low + (cityRandom(s) % 1000) * .001f * std::max(0.f, span);
      y = (cityRandom(s) % 5) * 400 + low + (cityRandom(s) % 1000) * .001f * std::max(0.f, span);
    } else if (kind == 2 || kind == 3) {
      bool vertical = cityRandom(s) % 2;
      s.a[180 + object - 100] = vertical;
      float lane = r >= 30 ? 56.f : (cityRandom(s) % 2 ? 32.f : 80.f);
      if (vertical)
        x = (cityRandom(s) % 8) * 400 + lane;
      else
        y = (cityRandom(s) % 5) * 400 + lane;
    } else if (kind == 0 && object < 16 && attempt < 120) {
      float angle = (cityRandom(s) % 6283) * .001f, reach = 45 + cityRandom(s) % 200;
      x = 1656 + std::cos(angle) * reach;
      y = 1056 + std::sin(angle) * reach;
    }
    if (!cityZoneFits(s, object, x, y))
      continue;
    float penalty = 0;
    for (int j = 0; j < cityCount; ++j)
      if (j != object && s.a[j]) {
        int q = 8 + j * 4;
        float dx = x - s.f[q], dy = y - s.f[q + 1], gap = r + s.f[q + 2] + 8;
        penalty += std::max(0.f, gap * gap - dx * dx - dy * dy);
      }
    if (avoidActors)
      for (int actor = -1; actor < 3; ++actor) {
        if (actor >= 0 && !s.a[800 + actor])
          continue;
        int q = 800 + actor * 8;
        float dx = x - (actor < 0 ? s.f[0] : s.f[q]), dy = y - (actor < 0 ? s.f[1] : s.f[q + 1]),
              gap = r + (actor < 0 ? s.f[2] : s.f[q + 2]) + 40;
        penalty += std::max(0.f, gap * gap - dx * dx - dy * dy);
      }
    if (penalty < best) {
      best = penalty;
      bx = x;
      by = y;
      bestVertical = cityVertical(s, object);
    }
    if (penalty == 0)
      break;
  }
  if (best >= 1e29f) {
    bool found = false;
    for (int row = 0; row < 5 && !found; ++row)
      for (int col = 0; col < 8 && !found; ++col) {
        float x = col * 400.f + 256, y = row * 400.f + 256;
        if (kind == 2 || kind == 3) {
          s.a[180 + object - 100] = 1;
          x = col * 400.f + 56;
          y = row * 400.f + 200;
        }
        if (cityZoneFits(s, object, x, y)) {
          bx = x;
          by = y;
          bestVertical = cityVertical(s, object);
          found = true;
        }
      }
  }
  // Preserve the orientation of the selected candidate, not the last attempted one.
  if (kind == 2 || kind == 3)
    s.a[180 + object - 100] = bestVertical ? 1 : 0;
  s.f[b] = bx;
  s.f[b + 1] = by;
}
inline void ensureCityLayout(State &s) {
  if (s.a[997] == 1801)
    return;
  std::array<int, cityCount> alive;
  std::vector<int> order;
  for (int i = 0; i < cityCount; ++i) {
    alive[i] = s.a[i];
    s.a[i] = 0;
    order.push_back(i);
  }
  std::stable_sort(order.begin(), order.end(),
                   [&](int a, int b) { return s.f[10 + a * 4] > s.f[10 + b * 4]; });
  for (int i : order) {
    cityPlace(s, i, false);
    s.a[i] = alive[i];
    s.f[11 + i * 4] = 0;
  }
  s.a[997] = 1801;
}
inline void cityConsume(State &s, int object, int actor) {
  s.a[object] = 0;
  s.f[11 + object * 4] = .28f;
  s.a[500 + object] = 20000 + 14000 * s.a[300 + object];
  s.a[cityOwnerSlot(object)] = actor + 1;
}
inline void cityRespawn(State &s, int object) {
  int b = 8 + object * 4;
  cityPlace(s, object, true);
  s.f[b + 3] = 0;
  s.a[object] = 1;
  s.a[500 + object] = 0;
  s.a[cityOwnerSlot(object)] = 0;
}
inline void cityRivalHit(State &s, int victim, int attacker) {
  int v = 800 + victim * 8, a = 800 + attacker * 8;
  if (s.f[v + 5] > 0)
    return;
  float old = s.f[v + 2];
  s.f[v + 2] = std::max(6.f, old * .8f);
  s.f[v + 5] = 5;
  s.f[a + 2] = std::sqrt(s.f[a + 2] * s.f[a + 2] + (old * old - s.f[v + 2] * s.f[v + 2]) * .2f);
  float dx = s.f[v] - s.f[a], dy = s.f[v + 1] - s.f[a + 1], d = std::sqrt(dx * dx + dy * dy);
  if (d < .01f) {
    dx = victim % 2 ? 1.f : -1.f;
    dy = .5f;
    d = std::sqrt(1.25f);
  }
  s.f[v] =
      std::max(12.f, std::min(cityWidth - 12, s.f[a] + dx / d * (s.f[a + 2] + s.f[v + 2] + 35)));
  s.f[v + 1] = std::max(
      12.f, std::min(cityHeight - 12, s.f[a + 1] + dy / d * (s.f[a + 2] + s.f[v + 2] + 35)));
  islandClamp(s.f[v], s.f[v + 1]);
}
inline void initCity(State &s, std::mt19937 &rng) {
  s.a[255] = 1401;
  s.a[999] = int(rng() & 0x7fffffffu);
  s.f[0] = 1656;
  s.f[1] = 1056;
  s.f[2] = 9;
  s.f[3] = 600;
  const int counts[] = {60, 40, 30, 20, 18, 12};
  const float sizes[] = {3, 9, 22, 40, 70, 110};
  float starterAngle = float(rng() % 6283) / 1000;
  int index = 0;
  for (int tier = 0; tier < 6; ++tier)
    for (int n = 0; n < counts[tier]; ++n, ++index) {
      int b = 8 + index * 4;
      float size = sizes[tier];
      s.a[index] = 1;
      s.a[300 + index] = tier;
      s.f[b + 2] = size;
      bool placed = false;
      for (int attempt = 0; attempt < 300 && !placed; ++attempt) {
        float x = size + 20 + float(rng() % unsigned(cityWidth - 2 * size - 40));
        float y = size + 20 + float(rng() % unsigned(cityHeight - 2 * size - 40));
        // A few starter fruit clusters around the spawn, with the rest spread city-wide.
        if (tier == 0 && n < 16) {
          float angle = starterAngle + n * 2.399963f + float(rng() % 100) * .001f;
          float radius = 55 + float(n % 4) * 48 + float(rng() % 15);
          x = 1600 + std::cos(angle) * radius;
          y = 1000 + std::sin(angle) * radius;
        }
        placed = islandMetric(x, y, size + 18) <= 1;
        for (int j = 0; j < index; ++j) {
          int q = 8 + j * 4;
          float dx = x - s.f[q], dy = y - s.f[q + 1];
          float gap = size + s.f[q + 2] + 8;
          if (dx * dx + dy * dy < gap * gap) {
            placed = false;
            break;
          }
        }
        if (placed) {
          s.f[b] = x;
          s.f[b + 1] = y;
        }
      }
      // Bounded fallback keeps every object in the world even in an unusually dense seed.
      if (!placed) {
        s.f[b] = size + 20 + float(rng() % unsigned(cityWidth - 2 * size - 40));
        s.f[b + 1] = size + 20 + float(rng() % unsigned(cityHeight - 2 * size - 40));
      }
    }
  for (int i = 0; i < 3; ++i) {
    int b = 800 + i * 8;
    s.a[800 + i] = 1;
    s.f[b] = 400 + i * 1100 + float(rng() % 161) - 80;
    s.f[b + 1] = (i % 2 ? 1650.f : 350.f) + float(rng() % 161) - 80;
    s.f[b + 2] = 12 + i * 14;
  }
  ensureIsland(s);
  ensureCityLayout(s);
}
inline void tickCity(State &s, float dt, float dx, float dy) {
  dt = std::max(0.f, std::min(.05f, dt));
  if (dt == 0 || s.phase)
    return;
  ensureIsland(s);
  ensureCityLayout(s);
  s.f[900] += dt;
  s.f[6] = std::max(0.f, s.f[6] - dt);
  s.f[7] = std::max(0.f, s.f[7] - dt);
  s.f[901] = std::max(0.f, s.f[901] - dt);
  float norm = 1;
  float speed = (s.f[6] > 0 ? 330.f : 175.f) + std::min(100.f, s.f[2] * .6f);
  float blend = 1 - std::exp(-14 * dt);
  s.f[4] += (dx * speed * norm - s.f[4]) * blend;
  s.f[5] += (dy * speed * norm - s.f[5]) * blend;
  s.f[0] = std::max(12.f, std::min(cityWidth - 12, s.f[0] + s.f[4] * dt));
  s.f[1] = std::max(12.f, std::min(cityHeight - 12, s.f[1] + s.f[5] * dt));
  islandClamp(s.f[0], s.f[1]);
  s.f[3] = std::max(0.f, s.f[3] - dt);
  s.f[902] += dt * 1000;
  int milliseconds = int(s.f[902]);
  s.f[902] -= milliseconds;
  for (int i = 0; i < cityCount; ++i) {
    int b = 8 + i * 4;
    if (!s.a[i]) {
      if (s.a[500 + i] <= 0)
        s.a[500 + i] = 20000 + 14000 * s.a[300 + i]; // migrate earlier city saves
      s.a[500 + i] = std::max(0, s.a[500 + i] - milliseconds);
      if (s.a[500 + i] == 0)
        cityRespawn(s, i);
    }
    s.f[b + 3] = std::max(0.f, s.f[b + 3] - dt);
    if (!s.a[i] || s.f[2] <= s.f[b + 2] + 2)
      continue;
    float x = s.f[0] - s.f[b], y = s.f[1] - s.f[b + 1], d = std::sqrt(x * x + y * y),
          reach = s.f[2] + s.f[b + 2] + 18;
    if (s.a[300 + i] < 2 && d < reach && d > .01f) {
      float pull = std::min(d, (50 + 140 * (1 - d / reach)) * dt);
      s.f[b] += x / d * pull;
      s.f[b + 1] += y / d * pull;
      d -= pull;
    }
    if (d < s.f[2] - s.f[b + 2] * .4f) {
      cityConsume(s, i, 0);
      ++s.score;
      s.f[2] = cityGrowth(s.f[2], s.f[b + 2]);
      if (s.a[300 + i] == 5)
        ++s.a[254];
    }
  }
  for (int enemy = 0; enemy < 3; ++enemy) {
    int b = 800 + enemy * 8;
    if (!s.a[800 + enemy]) {
      if (s.f[b + 4] <= 0)
        s.f[b + 4] = 12;
      s.f[b + 4] = std::max(0.f, s.f[b + 4] - dt);
      if (s.f[b + 4] > 0)
        continue;
      s.a[800 + enemy] = 1;
      s.f[b + 2] = 12 + enemy * 14;
      s.f[b + 5] = 5;
      for (int attempt = 0; attempt < 32; ++attempt) {
        s.f[b] = 80 + cityRandom(s) % 3040;
        s.f[b + 1] = 80 + cityRandom(s) % 1840;
        float dx = s.f[b] - s.f[0], dy = s.f[b + 1] - s.f[1];
        if (dx * dx + dy * dy > 500 * 500 && islandMetric(s.f[b], s.f[b + 1], 24) <= 1)
          break;
      }
    }
    islandClamp(s.f[b], s.f[b + 1]);
    float &x = s.f[b], &y = s.f[b + 1], &r = s.f[b + 2];
    float oldX = x, oldY = y;
    s.f[b + 3] = std::max(0.f, s.f[b + 3] - dt);
    s.f[b + 5] = std::max(0.f, s.f[b + 5] - dt);
    int target = -1;
    float nearest = 1e12, tx = x, ty = y;
    for (int i = 0; i < cityCount; ++i)
      if (s.a[i] && r > s.f[10 + i * 4] + 2) {
        float ox = s.f[8 + i * 4] - x, oy = s.f[9 + i * 4] - y, d = ox * ox + oy * oy;
        if (d < nearest) {
          nearest = d;
          target = i;
          tx = s.f[8 + i * 4];
          ty = s.f[9 + i * 4];
        }
      }
    if (target < 0) {
      if (s.f[b + 6] <= 0 || std::hypot(x - s.f[b + 6], y - s.f[b + 7]) < 30) {
        s.f[b + 6] = 100 + cityRandom(s) % 3000;
        s.f[b + 7] = 100 + cityRandom(s) % 1800;
        islandClamp(s.f[b + 6], s.f[b + 7], 60);
      }
      tx = s.f[b + 6];
      ty = s.f[b + 7];
    }
    float danger = 1e12, awayX = 0, awayY = 0, prey = 220 * 220;
    for (int other = -1; other < 3; ++other) {
      if (other == enemy || (other >= 0 && !s.a[800 + other]))
        continue;
      int q = 800 + other * 8;
      float ox = (other < 0 ? s.f[0] : s.f[q]) - x, oy = (other < 0 ? s.f[1] : s.f[q + 1]) - y,
            rr = other < 0 ? s.f[2] : s.f[q + 2], d = ox * ox + oy * oy;
      float protection = other < 0 ? s.f[901] : s.f[q + 5];
      if (rr > r * 1.25f && d < 300 * 300 && d < danger) {
        danger = d;
        awayX = -ox;
        awayY = -oy;
      }
      if (r > rr * 1.25f && protection <= 0 && d < prey) {
        prey = d;
        tx = x + ox;
        ty = y + oy;
        target = -1;
      }
    }
    if (danger < 300.f * 300.f) {
      float length = std::hypot(awayX, awayY);
      if (length < 1) {
        awayX = enemy % 2 ? 1.f : -1.f;
        awayY = 1;
        length = std::sqrt(2.f);
      }
      awayX /= length;
      awayY /= length;
      // Slide along a boundary instead of repeatedly steering out of the world.
      if ((x < 35 && awayX < 0) || (x > cityWidth - 35 && awayX > 0)) {
        awayX = 0;
        awayY =
            std::abs(awayY) > .01f ? std::copysign(1.f, awayY) : (y < cityHeight / 2 ? 1.f : -1.f);
      }
      if ((y < 35 && awayY < 0) || (y > cityHeight - 35 && awayY > 0)) {
        awayY = 0;
        awayX =
            std::abs(awayX) > .01f ? std::copysign(1.f, awayX) : (x < cityWidth / 2 ? 1.f : -1.f);
      }
      tx = x + awayX * 300;
      ty = y + awayY * 300;
      target = -1;
    }
    int stuck = 920 + enemy * 2;
    if (s.f[stuck + 1] > 0) {
      s.f[stuck + 1] = std::max(0.f, s.f[stuck + 1] - dt);
      tx = s.f[b + 6];
      ty = s.f[b + 7];
      target = -1;
    }
    float vx = tx - x, vy = ty - y, d = std::hypot(vx, vy);
    if (d > 1) {
      float step = std::min(d, (70 + std::min(30.f, r * .2f)) * dt);
      x += vx / d * step;
      y += vy / d * step;
    }
    x = std::max(12.f, std::min(cityWidth - 12, x));
    y = std::max(12.f, std::min(cityHeight - 12, y));
    islandClamp(x, y);
    if (d > 1 && std::hypot(x - oldX, y - oldY) < dt * 20) {
      float nx = (oldX - 1600) / 1520, ny = (oldY - 1000) / 920;
      float gx = nx * nx * nx / 1520, gy = ny * ny * ny / 920;
      float tangentX = -gy, tangentY = gx, length = std::hypot(tangentX, tangentY);
      if (length > .000001f) {
        float sign = tangentX * vx + tangentY * vy >= 0 ? 1.f : -1.f;
        x = oldX + sign * tangentX / length * 75 * dt;
        y = oldY + sign * tangentY / length * 75 * dt;
        islandClamp(x, y);
      }
    }
    s.f[stuck] = std::hypot(x - oldX, y - oldY) < dt * 4 ? s.f[stuck] + dt : 0;
    if (s.f[stuck] > .8f) {
      s.f[b + 6] = 200 + cityRandom(s) % 2800;
      s.f[b + 7] = 200 + cityRandom(s) % 1600;
      islandClamp(s.f[b + 6], s.f[b + 7], 60);
      s.f[stuck + 1] = 2;
      s.f[stuck] = 0;
    }
    if (target >= 0 && s.f[b + 3] <= 0) {
      int q = 8 + target * 4;
      float ox = s.f[q] - x, oy = s.f[q + 1] - y;
      if (ox * ox + oy * oy < (r - s.f[q + 2] * .4f) * (r - s.f[q + 2] * .4f)) {
        cityConsume(s, target, enemy + 1);
        r = std::sqrt(r * r + s.f[q + 2] * s.f[q + 2] * .2f + 2);
        s.f[b + 3] = 2;
      }
    }
    float px = s.f[0] - x, py = s.f[1] - y, playerDist = std::hypot(px, py);
    if (s.f[2] > r * 1.25f && playerDist < s.f[2] - r * .4f && s.f[b + 5] <= 0) {
      s.a[800 + enemy] = 0;
      s.f[b + 4] = 12;
      s.f[2] = cityGrowth(s.f[2], r);
    } else if (r > s.f[2] * 1.25f && playerDist < r && s.f[901] <= 0) {
      s.f[2] = std::max(6.f, s.f[2] * .8f);
      s.f[901] = 5;
      if (playerDist < .01f) {
        px = 1;
        py = 0;
        playerDist = 1;
      }
      float away = std::max(1.f, playerDist);
      s.f[0] = std::max(12.f, std::min(cityWidth - 12, x + px / away * (r + 30)));
      s.f[1] = std::max(12.f, std::min(cityHeight - 12, y + py / away * (r + 30)));
      islandClamp(s.f[0], s.f[1]);
    }
  }
  for (int i = 0; i < 3; ++i)
    for (int j = i + 1; j < 3; ++j)
      if (s.a[800 + i] && s.a[800 + j]) {
        int a = 800 + i * 8, b = 800 + j * 8;
        float dx = s.f[a] - s.f[b], dy = s.f[a + 1] - s.f[b + 1], d = std::hypot(dx, dy);
        if (s.f[a + 2] > s.f[b + 2] * 1.25f && d < s.f[a + 2])
          cityRivalHit(s, j, i);
        else if (s.f[b + 2] > s.f[a + 2] * 1.25f && d < s.f[b + 2])
          cityRivalHit(s, i, j);
      }
  if (s.a[254] >= 3)
    s.phase = 1;
  else if (s.f[3] <= 0)
    s.phase = 2;
}
} // namespace fmg
