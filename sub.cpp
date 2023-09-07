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

struct LowLink {
  const vector<vector<int>>& Graph;
  const bitset<h * w>& cant_visit;
  bitset<h * w> used;
  bitset<h * w> articulation;
  vector<int> ord, low;

  LowLink(const vector<vector<int>>& Graph, const bitset<h * w>& cant_visit)
    : Graph(Graph), cant_visit(cant_visit) {
    ord.resize(h * w, 0);
    low.resize(h * w, 0);
    articulation.reset();
    articulation.flip();
    for (int i = 0; i < h * w; i++) {
      if (used.test(i) || cant_visit.test(i)) continue;
      dfs(i, 0, -1);
    }
  }

  int dfs(int v, int k, int par) {
    used.set(v);
    ord[v] = k++;
    low[v] = ord[v];
    bool is_articulation = false;
    int cnt = 0;
    for (int next_v : Graph[v]) {
      if (!used.test(next_v)) {
        cnt++;
        k = dfs(next_v, k, v);
        low[v] = min(low[v], low[next_v]);
        if (par != -1 && ord[v] <= low[next_v]) is_articulation = true;
      }
      else if (next_v != par) {
        low[v] = min(low[v], ord[next_v]);
      }
    }
    if (par == -1 && cnt > 1) is_articulation = true;
    if (is_articulation) articulation.reset(v);
    return k;
  }
};

int getId(int i, int j) { return i * w + j; }
pair<int, int> getPos(int id) { return { id / w, id % w }; }

int main() {
  int in;
  cin.ignore(10), cin >> in;
  vector Graph(h * w, vector<int>(0));
  vector<vegeta_t> v;
  vector<int> crop_turn;

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
    crop_turn.resize(k);
    rep(i, k) cin >> v[i].plant_before >> v[i].crop_turn;
    rep(i, k) v[i].idx = i;
    rep(i, k) crop_turn[i] = v[i].crop_turn;
  }
  sort(v.begin(), v.end());

  vector<output_t> ans(0);
  vector<bool> placed(v.size(), false);
  vector<int> bfs_order(0);
  bitset<h * w> used;
  int veg_idx = 0;
  int ans_idx = 0;

  {  // Initial Ans
    queue<int> q;
    q.push(getId(in, 0));
    while (!q.empty() && veg_idx < v.size()) {
      int id = q.front();
      q.pop();
      auto [i, j] = getPos(id);
      if (used.test(id)) continue;
      bfs_order.push_back(id);
      used.set(id);
      ans.push_back({ v[veg_idx].idx, i, j, 1 });
      placed[v[veg_idx].idx] = true;
      veg_idx++;
      for (int next : Graph[id]) {
        if (!used.test(next)) q.push(next);
      }
    }
  }
  assert(bfs_order.size() == h * w);

  for (int now_turn = 2; now_turn <= turn; now_turn++) {
    // Plant Phase
    queue<int> can_plant;
    rep(i, h * w) if (!used.test(bfs_order[i])) can_plant.push(bfs_order[i]);
    Debug(can_plant);
    while (!can_plant.empty()) {
      if (veg_idx >= v.size()) break;
      if (v[veg_idx].plant_before < now_turn) {
        veg_idx++;
        continue;
      }
      int id = can_plant.front();
      can_plant.pop();
      if (used.test(id)) continue;
      queue<int> q;
      q.push(getId(in, 0));
      bitset<h * w> visited;
      while (!q.empty()) {
        int now = q.front();
        q.pop();
        if (visited.test(now)) continue;
        visited.set(now);
        if (now == id) continue;
        for (int next : Graph[now]) {
          if (!visited.test(next)) q.push(next);
        }
      }
      if ((visited & used) == used) {
        auto [i, j] = getPos(id);
        ans.push_back({ v[veg_idx].idx, i, j, now_turn });
        placed[v[veg_idx].idx] = true;
        veg_idx++;
        used.set(id);
      }
    }

    // Crop Phase
    while (ans_idx < ans.size()) {
      if (crop_turn[ans[ans_idx].veg_idx] == now_turn) {
        int i = ans[ans_idx].plant_i;
        int j = ans[ans_idx].plant_j;
        used.reset(getId(i, j));
        ans_idx++;
      }
      else
        break;
    }
  }

  cout << ans.size() << '\n';
  for (output_t o : ans) {
    printf("%d %d %d %d\n", o.veg_idx + 1, o.plant_i, o.plant_j, o.plant_turn);
  }
  return 0;
}
