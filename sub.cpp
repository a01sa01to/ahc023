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
  int place_before;
  int cut_turn;
  int idx;

  bool operator<(const vegeta_t& other) const {
    if (cut_turn == other.cut_turn) return place_before < other.place_before;
    return cut_turn < other.cut_turn;
  }
};

struct output_t {
  int veg;
  int place_i;
  int place_j;
  int place_turn;
};

int getId(int i, int j) { return i * w + j; }
pair<int, int> getPos(int id) { return { id / w, id % w }; }

int main() {
  int in;
  cin.ignore(10), cin >> in;
  vector Graph(h * w, vector<int>(0));
  vector<vegeta_t> v;

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
    rep(i, k) cin >> v[i].place_before >> v[i].cut_turn;
    rep(i, k) v[i].idx = i;
  }
  sort(v.begin(), v.end());

  vector<output_t> ans(0);
  vector<bool> placed(v.size(), false);
  int veg_idx = 0;

  {  // Initial Ans
    queue<int> q;
    bitset<h * w> used;
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

  cout << ans.size() << '\n';
  for (output_t o : ans) {
    printf("%d %d %d %d\n", o.veg, o.place_i, o.place_j, o.place_turn);
  }
  return 0;
}
