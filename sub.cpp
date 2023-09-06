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
      used.set(id);
      ans.push_back({ v[veg_idx].idx + 1, i, j, 1 });
      placed[v[veg_idx].idx] = true;
      veg_idx++;
      for (int next : Graph[id]) {
        if (!used.test(next)) q.push(next);
      }
    }
  }

  for (int now_turn = 2; now_turn <= turn; now_turn++) {
    // Plant Phase
    

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
    printf("%d %d %d %d\n", o.veg_idx, o.plant_i, o.plant_j, o.plant_turn);
  }
  return 0;
}
