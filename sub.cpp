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

  int nowturn = 1;
  int idx = 0;
  while (nowturn <= turn && idx < v.size()) {
    queue<int> q;
    q.push(getId(in, 0));
    vector<bool> used(h * w, false);
    int lastturn = -1;
    while (!q.empty() && idx < v.size()) {
      if (nowturn > v[idx].place_before) {
        idx++;
        continue;
      }
      int id = q.front();
      q.pop();
      auto [i, j] = getPos(id);
      if (used[id]) continue;
      ans.push_back({ v[idx].idx + 1, i, j, nowturn });
      lastturn = max(lastturn, v[idx].cut_turn);
      used[getId(i, j)] = true;
      idx++;
      for (int next : Graph[id]) {
        if (!used[next]) q.push(next);
      }
    }
    nowturn = lastturn + 1;
  }

  cout << ans.size() << '\n';
  for (output_t o : ans) {
    printf("%d %d %d %d\n", o.veg, o.place_i, o.place_j, o.place_turn);
  }
  return 0;
}
