#include <bits/stdc++.h>
using namespace std;
#ifdef LOCAL
  #include "settings/debug.cpp"
  #define _GLIBCXX_DEBUG
#else
  #define Debug(...) void(0)
#endif
using ll = long long;
using ull = unsigned long long;
#define rep(i, n) for (int i = 0; i < (n); ++i)

constexpr int turn = 100, h = 20, w = 20;
int in;

struct vegeta_t {
  int plant_before;
  int crop_turn;
  int idx;

  bool operator<(const vegeta_t& other) const {
    if (crop_turn == other.crop_turn) return plant_before < other.plant_before;
    return crop_turn < other.crop_turn;
  }
};

struct output_t {
  int veg_idx;
  int plant_i;
  int plant_j;
  int plant_turn;
};

int getId(int i, int j) { return i * w + j; }
pair<int, int> getPos(int id) { return { id / w, id % w }; }

inline bool canPlant(const int id, vector<vector<int>> crop_turn, const int veg_crop, bitset<h * w> used, const vector<vector<int>>& Graph) {
  queue<int> q;
  q.push(getId(in, 0));
  {
    auto [i, j] = getPos(id);
    crop_turn[i][j] = -veg_crop;
  }
  bitset<h * w> visited;
  while (!q.empty()) {
    int now = q.front();
    q.pop();
    if (visited.test(now)) continue;
    auto [i, j] = getPos(now);
    visited.set(now);
    for (int next : Graph[now]) {
      auto [ni, nj] = getPos(next);
      bool can_visit = false;
      if (crop_turn[i][j] == 0) can_visit = true;
      if (crop_turn[i][j] > 0 && crop_turn[i][j] <= crop_turn[ni][nj]) can_visit = true;
      if (crop_turn[i][j] < 0 && -crop_turn[i][j] <= abs(crop_turn[ni][nj])) can_visit = true;
      if (!visited.test(next) && can_visit) q.push(next);
    }
  }
  if ((visited & used) == used && visited.test(id)) return true;
  return false;
}

int main() {
  cin.ignore(10), cin >> in;
  vector Graph(h * w, vector<int>(0));
  vector<vegeta_t> v;
  vector crop_turn(h, vector<int>(w, 0));

  {  // Input
    rep(i, h - 1) {
      string s;
      cin >> s;
      rep(j, w) {
        if (s[j] == '0') {
          Graph[getId(i, j)].push_back(getId(i + 1, j));
          Graph[getId(i + 1, j)].push_back(getId(i, j));
        }
      }
    }
    rep(i, h) {
      string s;
      cin >> s;
      rep(j, w - 1) {
        if (s[j] == '0') {
          Graph[getId(i, j)].push_back(getId(i, j + 1));
          Graph[getId(i, j + 1)].push_back(getId(i, j));
        }
      }
    }
    int k;
    cin >> k;
    v.resize(k);
    rep(i, k) cin >> v[i].plant_before >> v[i].crop_turn;
    rep(i, k) v[i].idx = i;
  }
  sort(v.begin(), v.end());

  vector<output_t> ans(0);
  vector<bool> placed(v.size(), false);
  bitset<h * w> used;
  int veg_idx = 0;

  {  // まずスコアの大きいものを 100 個外側にうめちゃう
    constexpr int outer = 100;
    deque<int> can_plant;
    {  // 埋められる場所の列挙、外側から
      queue<int> q;
      q.push(getId(in, 0));
      bitset<h * w> visited;
      while (!q.empty()) {
        int now = q.front();
        q.pop();
        if (visited.test(now)) continue;
        auto [i, j] = getPos(now);
        visited.set(now);
        if (crop_turn[i][j] == 0) can_plant.push_back(now);
        for (int next : Graph[now]) {
          auto [ni, nj] = getPos(next);
          if (!visited.test(next) && !used.test(next)) q.push(next);
        }
      }
    }
    sort(v.begin(), v.end(), [](const vegeta_t& a, const vegeta_t& b) {
      int score_a = a.crop_turn - a.plant_before;
      int score_b = b.crop_turn - b.plant_before;
      if (score_a == score_b) return a.crop_turn > b.crop_turn;
      return score_a > score_b;
    });
    vector<vegeta_t> tobeused;
    rep(i, outer) tobeused.push_back(v[i]);
    sort(tobeused.rbegin(), tobeused.rend());
    sort(v.begin(), v.end());
    rep(idx, tobeused.size()) {
      while (!can_plant.empty()) {
        int id = can_plant.back();
        can_plant.pop_back();
        auto [i, j] = getPos(id);
        if (placed[tobeused[idx].idx]) continue;
        if (canPlant(id, crop_turn, tobeused[idx].crop_turn, used, Graph)) {
          ans.push_back({ tobeused[idx].idx, i, j, 1 });
          placed[tobeused[idx].idx] = true;
          crop_turn[i][j] = -tobeused[idx].crop_turn;
          break;
        }
      }
    }
  }

  for (int now_turn = 1; now_turn <= turn; now_turn++) {
    // Plant Phase
    // まず埋められるところをチェック
    while (true) {
      bool updated = false;
      queue<int> can_plant;
      {
        queue<int> q;
        q.push(getId(in, 0));
        bitset<h * w> visited;
        while (!q.empty()) {
          int now = q.front();
          q.pop();
          if (visited.test(now)) continue;
          auto [i, j] = getPos(now);
          visited.set(now);
          if (crop_turn[i][j] == 0) can_plant.push(now);
          for (int next : Graph[now]) {
            auto [ni, nj] = getPos(next);
            if (!visited.test(next) && !used.test(next)) q.push(next);
          }
        }
      }
      queue<int> planted;
      while (!can_plant.empty()) {
        if (veg_idx >= v.size()) break;
        if (v[veg_idx].plant_before < now_turn || placed[v[veg_idx].idx]) {
          veg_idx++;
          continue;
        }
        // Simulation
        int id = can_plant.front();
        can_plant.pop();
        if (used.test(id)) continue;
        // おける
        if (canPlant(id, crop_turn, v[veg_idx].crop_turn, used, Graph)) {
          auto [i, j] = getPos(id);
          ans.push_back({ v[veg_idx].idx, i, j, now_turn });
          crop_turn[i][j] = -v[veg_idx].crop_turn;
          placed[v[veg_idx].idx] = true;
          veg_idx++;
          planted.push(id);
        }
        else {
          auto [i, j] = getPos(id);
          crop_turn[i][j] = 0;
        }
      }
      while (!planted.empty()) {
        auto [i, j] = getPos(planted.front());
        planted.pop();
        used.set(getId(i, j));
        if (crop_turn[i][j] < 0) crop_turn[i][j] = -crop_turn[i][j];
        updated = true;
      }
      if (now_turn == 1) {
        rep(i, h) rep(j, w) {
          if (crop_turn[i][j] < 0) {
            used.set(getId(i, j));
            crop_turn[i][j] = -crop_turn[i][j];
          }
        }
      }
      if (!updated) break;
    }
    // Crop Phase
    for (output_t o : ans) {
      int i = o.plant_i;
      int j = o.plant_j;
      if (crop_turn[i][j] == now_turn) {
        crop_turn[i][j] = 0;
        used.reset(getId(i, j));
      }
    }
  }

  cout << ans.size() << '\n';
  for (output_t o : ans) {
    printf("%d %d %d %d\n", o.veg_idx + 1, o.plant_i, o.plant_j, o.plant_turn);
  }
  return 0;
}
