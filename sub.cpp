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

int main() {
  int in;
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

  for (int now_turn = 1; now_turn <= turn; now_turn++) {
    // Plant Phase
    // まず埋められるところをチェック
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
      if (v[veg_idx].plant_before < now_turn) {
        veg_idx++;
        continue;
      }
      // Simulation
      int id = can_plant.front();
      can_plant.pop();
      if (used.test(id)) continue;
      queue<int> q;
      bitset<h * w> visited;
      {
        auto [i, j] = getPos(id);
        q.push(getId(in, 0));
        crop_turn[i][j] = -v[veg_idx].crop_turn;
      }
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
      // おける
      if ((visited & used) == used && visited.test(id)) {
        auto [i, j] = getPos(id);
        ans.push_back({ v[veg_idx].idx, i, j, now_turn });
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
