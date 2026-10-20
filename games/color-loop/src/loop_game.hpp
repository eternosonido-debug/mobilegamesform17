#pragma once
// Included after State is defined. All loop state fits the FMG1 save format.
namespace fmg {
static const int loopLevels = 24, loopCapacity = 6, loopLanes = 4, loopQueueDepth = 4;
inline int loopActive(const State &s) {
  int n = 0;
  for (int i = 0; i < 4; ++i)
    n += s.a[130 + i] > 0;
  return n;
}
inline int loopWaiting(const State &s) {
  int n = 0;
  for (int i = 0; i < 4; ++i)
    n += s.a[150 + i] > 0;
  return n;
}
inline void initLoop(State &s, int level, int unlocked) {
  s = State{};
  s.game = 7;
  s.choice = 0;
  s.a[200] = std::max(0, std::min(loopLevels - 1, level));
  s.a[201] = std::max(s.a[200], std::min(loopLevels - 1, unlocked));
  s.a[202] = 11201;
  int colors = std::min(4, 2 + s.a[200] / 4), collectors = 6 + 2 * (s.a[200] / 8);
  // Each color has a whole number of collectors. The item bag and queue agree exactly.
  int counts[4] = {};
  for (int i = 0; i < collectors; ++i)
    ++counts[i % colors];
  int size = collectors * loopCapacity;
  s.a[204] = size;
  // A six-column board gains depth in later chapters (36 / 48 / 60 items).
  s.a[205] = size / 6;
  std::vector<int> bag;
  for (int c = 0; c < colors; ++c)
    for (int i = 0; i < counts[c] * loopCapacity; ++i)
      bag.push_back(c + 1);
  std::mt19937 random(3701 + s.a[200] * 937);
  if (s.a[200] == 1)
    std::rotate(bag.begin(), bag.begin() + 6, bag.end());
  else if (s.a[200] > 1)
    std::shuffle(bag.begin(), bag.end(), random);
  for (int i = 0; i < size; ++i)
    s.a[i] = bag[size_t(i)];
  std::vector<int> queue;
  for (int round = 0; round < collectors; ++round)
    queue.push_back(round % colors + 1);
  if (s.a[200] > 3)
    std::shuffle(queue.begin(), queue.end(), random);
  // Queue is column-major: only the front collector of each lane can enter.
  for (int i = 0; i < collectors; ++i)
    s.a[70 + (i % 4) * 4 + i / 4] = queue[size_t(i)];
}
inline int loopItemCount(const State &s) {
  int n = 0;
  for (int i = 0; i < 6 * s.a[205]; ++i)
    n += s.a[i] > 0;
  return n;
}
inline void loopFeedback(State &s, int code) {
  s.a[206] = code;
  s.f[20] = 1.5f;
}
inline bool launchLoop(State &s, int choice, bool allowPending = true) {
  if (s.phase || choice < 0 || choice > 7)
    return false;
  if (s.a[220] && allowPending) {
    loopFeedback(s, 7);
    return false;
  }
  int color = choice < 4 ? s.a[70 + choice * 4] : s.a[150 + choice - 4];
  if (!color) {
    loopFeedback(s, 1);
    return false;
  }
  int free = -1;
  for (int i = 0; i < 4; ++i)
    if (!s.a[130 + i]) {
      free = i;
      break;
    }
  if (free < 0) {
    if (allowPending) {
      s.a[220] = choice + 1;
      loopFeedback(s, 7);
      return true;
    }
    return false;
  }
  // Keep the entry clear so collectors never overlap at launch.
  for (int i = 0; i < 4; ++i)
    if (s.a[130 + i] && s.f[i] < 1.4f) {
      if (allowPending) {
        s.a[220] = choice + 1;
        loopFeedback(s, 7);
        return true;
      }
      return false;
    }
  int amount = loopCapacity;
  if (choice < 4) {
    int b = 70 + choice * 4;
    for (int i = 0; i < 3; ++i)
      s.a[b + i] = s.a[b + i + 1];
    s.a[b + 3] = 0;
  } else {
    int i = choice - 4;
    amount = s.a[154 + i];
    s.a[150 + i] = s.a[154 + i] = 0;
  }
  s.a[130 + free] = color;
  s.a[134 + free] = amount;
  s.f[free] = 0;
  s.a[224 + free] = choice;
  s.f[44 + free] = .32f;
  s.f[21] = .2f;
  return true;
}
inline int loopExposed(const State &s, int segment) {
  int h = s.a[205], perimeter = 12 + 2 * h, x = 0, y = 0, dx = 0, dy = 0;
  segment %= perimeter;
  if (segment < 6) {
    x = segment;
    y = 0;
    dy = 1;
  } else if (segment < 6 + h) {
    x = 5;
    y = segment - 6;
    dx = -1;
  } else if (segment < 12 + h) {
    x = 5 - (segment - 6 - h);
    y = h - 1;
    dy = -1;
  } else {
    x = 0;
    y = h - 1 - (segment - 12 - h);
    dx = 1;
  }
  for (int i = 0; i < std::max(6, h); ++i, x += dx, y += dy) {
    if (x < 0 || x >= 6 || y < 0 || y >= h)
      break;
    if (s.a[y * 6 + x])
      return y * 6 + x;
  }
  return -1;
}
inline void tickLoop(State &s, float dt) {
  dt = std::max(0.f, std::min(dt, .05f));
  s.f[20] = std::max(0.f, s.f[20] - dt);
  s.f[21] = std::max(0.f, s.f[21] - dt);
  for (int i = 0; i < 4; ++i) {
    s.f[30 + i] = std::max(0.f, s.f[30 + i] - dt);
    s.f[44 + i] = std::max(0.f, s.f[44 + i] - dt);
    s.f[60 + i] = std::max(0.f, s.f[60 + i] - dt);
  }
  if (s.phase)
    return;
  s.f[40] = std::fmod(s.f[40] + dt, 1000.f);
  int perimeter = 12 + 2 * s.a[205];
  for (int i = 0; i < 4; ++i) {
    if (!s.a[130 + i])
      continue;
    float old = s.f[i];
    s.f[i] += dt * 6.f;
    int boundary = int(std::floor(s.f[i]));
    if (boundary > int(std::floor(old)) && boundary <= perimeter) {
      int cell = loopExposed(s, boundary - 1);
      if (cell >= 0 && s.a[cell] == s.a[130 + i]) {
        s.a[cell] = 0;
        --s.a[134 + i];
        ++s.a[203];
        s.score += 10;
        s.a[210 + i] = cell;
        s.a[214 + i] = s.a[130 + i];
        s.f[30 + i] = .3f;
      }
    }
    if (s.a[134 + i] == 0) {
      s.a[234 + i] = s.a[130 + i];
      s.a[238 + i] = 0;
      s.a[242 + i] = 0;
      s.f[64 + i] = s.f[i];
      s.f[60 + i] = .4f;
      s.a[130 + i] = s.a[134 + i] = 0;
      s.score += 50;
      loopFeedback(s, 4);
    } else if (s.f[i] >= perimeter) {
      int slot = -1;
      for (int k = 0; k < 4; ++k)
        if (!s.a[150 + k]) {
          slot = k;
          break;
        }
      if (slot < 0) {
        s.phase = 2;
        loopFeedback(s, 5);
        return;
      }
      s.a[234 + i] = s.a[130 + i];
      s.a[238 + i] = s.a[134 + i];
      s.a[242 + i] = slot + 1;
      s.f[64 + i] = s.f[i];
      s.f[60 + i] = .4f;
      s.a[150 + slot] = s.a[130 + i];
      s.a[154 + slot] = s.a[134 + i];
      s.a[130 + i] = s.a[134 + i] = 0;
      loopFeedback(s, 6);
    }
  }
  if (s.a[220]) {
    int choice = s.a[220] - 1;
    bool room = loopActive(s) < 4;
    for (int i = 0; i < 4; ++i)
      if (s.a[130 + i] && s.f[i] < 1.4f)
        room = false;
    if (room) {
      s.a[220] = 0;
      launchLoop(s, choice, false);
    }
  }
  if (loopItemCount(s) == 0) {
    s.a[220] = 0;
    s.phase = 1;
    s.a[201] = std::min(loopLevels - 1, std::max(s.a[201], s.a[200] + 1));
  }
}
inline bool validLoop(const State &s) {
  if (s.a[202] != 11201 || s.cursor > 7 || s.a[200] < 0 || s.a[200] >= loopLevels ||
      s.a[201] < s.a[200] || s.a[201] >= loopLevels)
    return false;
  if (s.a[220] < 0 || s.a[220] > 8)
    return false;
  if (s.f[40] < 0 || s.f[40] > 1000)
    return false;
  for (int i = 0; i < 4; ++i) {
    if (s.a[214 + i] < 0 || s.a[214 + i] > 4 || s.a[224 + i] < 0 || s.a[224 + i] > 7 ||
        s.a[234 + i] < 0 || s.a[234 + i] > 4 || s.a[238 + i] < 0 || s.a[238 + i] > 6 ||
        s.a[242 + i] < 0 || s.a[242 + i] > 4)
      return false;
    if (s.f[30 + i] < 0 || s.f[30 + i] > .301f || s.f[44 + i] < 0 || s.f[44 + i] > .321f ||
        s.f[60 + i] < 0 || s.f[60 + i] > .401f)
      return false;
  }
  int h = s.a[205];
  if (h != 6 && h != 8 && h != 10)
    return false;
  if (s.a[204] != 6 * h || s.a[203] < 0 || s.a[203] > 6 * h)
    return false;
  int items[5] = {}, capacity[5] = {};
  for (int i = 0; i < 60; ++i) {
    if (s.a[i] < 0 || s.a[i] > 4 || (i >= 6 * h && s.a[i]))
      return false;
    if (s.a[i])
      ++items[s.a[i]];
  }
  for (int i = 70; i < 86; ++i) {
    if (s.a[i] < 0 || s.a[i] > 4)
      return false;
    if (s.a[i])
      capacity[s.a[i]] += 6;
  }
  for (int i = 0; i < 4; ++i) {
    for (int b : {130, 150}) {
      int color = s.a[b + i], remaining = s.a[b + 4 + i];
      if (color < 0 || color > 4 || remaining < 0 || remaining > 6 || (!color && remaining) ||
          (color && !remaining))
        return false;
      if (color)
        capacity[color] += remaining;
    }
    if (s.f[i] < 0 || s.f[i] > 12 + 2 * h + .3f)
      return false;
  }
  if (s.phase != 2)
    for (int c = 1; c <= 4; ++c)
      if (items[c] != capacity[c])
        return false;
  return loopItemCount(s) + s.a[203] == s.a[204];
}
} // namespace fmg
