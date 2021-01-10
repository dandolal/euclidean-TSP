struct Point {
  double x, y;
};

double distance(const Point &a, const Point &b) {
  return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

struct Edge {
  Edge() = default;
  Edge(int a, int b, double weight) : a(a), b(b), weight(weight) {}
  int a, b;
  double weight;

  bool operator<(const Edge &another) const {
    if (weight < another.weight)
      return true;
    if (weight == another.weight) {
      if (a < another.a)
        return true;
      if (a == another.a)
        return b < another.b;
    }
    return false;
  }
};

class Graph {
 public:
  explicit Graph(const vector<Point> &points) {
    size = int(points.size());
    points_ = points;

    neighbours.resize((size_t) size);

//    Заполняем список соседей
    for (int i = 0; i < size; ++i)
      for (int j = 0; j < size; ++j)
        if (i != j)
          neighbours[i].insert(j);
  }

  std::vector<double> Solve(){
    build_mst();
    add_perfect_matching();
    find_hamilton_cycle();
    return return_result();
  }


 private:
  void build_mst() {
    set<Edge> current; //ребра, которы можно добавить на текущем шаге
    set<int> used; //использованные вершины
    used.insert(0);
    for (int v: neighbours[0])
      current.insert({0, v, distance(points_[0], points_[v])});
    for (int i = 0 ; i < size - 1; ++i) {
      while(true) {
        auto current_edge = *current.begin();
        current.erase(current_edge);
        int a = current_edge.a, b = current_edge.b;
        if (used.count(a) == 0 || used.count(b) == 0) {
          MST.insert({a , b, distance(points_[a], points_[b])});
          if (used.count(a) == 0) {
            used.insert(a);
            for (int v: neighbours[a])
              current.insert({a, v, distance(points_[a], points_[v])});
          } else {
            used.insert(b);
            for (int v: neighbours[b])
              current.insert({b, v, distance(points_[b], points_[v])});
          }
          break;
        }
      }
    }
  }

  void add_perfect_matching() {
    auto odds = find_odds();
    set<Edge> odd_edges;
    for (int i = 0; i < odds.size(); ++i) {
      for (int j = i + 1; j < odds.size(); ++j) {
        int a = odds[i], b = odds[j];
        odd_edges.insert({a, b, distance(points_[a], points_[b])});
      }
    }
    set<int> used;
    while(!odd_edges.empty()) {
      auto cur = *odd_edges.begin();
      odd_edges.erase(cur);
      if (used.count(cur.a) == 0 && used.count(cur.b) == 0) {
        MST.insert({cur.a , cur.b, distance(points_[cur.a], points_[cur.b])});
        used.insert(cur.a);
        used.insert(cur.b);
      }
    }
  }

  vector<int> find_odds() {
    vector<int> count(size, 0);
    for (auto& edge: MST) {
      ++count[edge.a];
      ++count[edge.b];
    }
    vector<int> odds;
    for (int i = 0; i < size; ++i) {
      if (count[i] % 2 == 1)
        odds.push_back(i);
    }
    return odds;
  }

  void find_hamilton_cycle() {
    auto euler_cycle = find_euler_cycle();
    set<int> used;
    vector<int> hamilton_cycle;
    for (int i: euler_cycle) {
      if (used.count(i) == 0) {
        used.insert(i);
        hamilton_cycle.push_back(i);
      }
    }
    for (int i = 0; i < size; ++i) {
      int a = hamilton_cycle[i], b = hamilton_cycle[(i + 1) % size];
      result.insert({a ,b, distance(points_[a], points_[b])});
    }
  }

  vector<int> find_euler_cycle() {
    vector<int> euler_cycle;
    vector<map<int, int>> g((size_t)size);
    for (auto e: MST) {
      if (g[e.a].count(e.b) == 0)
        g[e.a][e.b] = 1;
      else ++ g[e.a][e.b];
      if (g[e.b].count(e.a) == 0)
        g[e.b][e.a] = 1;
      else ++ g[e.b][e.a];
    }
    int cur_vertex = 0;
    stack<int> st;
    st.push(0);
    while (!st.empty()) {
      int v = st.top();
      if (!g[v].empty()) {
        int u = g[v].begin()->first;
        if (g[v][u] == 1)
          g[v].erase(u);
        else --g[v][u];
        if (g[u][v] == 1)
          g[u].erase(v);
        else --g[u][v];
        st.push(u);
      } else {
        euler_cycle.push_back(v);
        st.pop();
      }
    }
    return euler_cycle;
  }




  void print_result() {
    double ans = 0;
    for (Edge e: result)
      ans += e.weight;
    vector<vector<int>> g((size_t) size);
    cout << "Длина пути: ";
    cout << ans << '\n';

    for (auto &e: result) {
      g[e.a].push_back(e.b);
      g[e.b].push_back(e.a);
    }
    cout << "Порядок точек: ";
    cout << "1 ";
    int prev = 0;
    int cur = g[prev][0];
    for (int i = 0; i < size - 1; ++i) {
      cout << cur + 1 << ' ';
      if (g[cur][0] != prev) {
        prev = cur;
        cur = g[cur][0];
      } else {
        prev = cur;
        cur = g[cur][1];
      }
    }
  }

  std::vector<double> return_result() {
    vector<vector<int>> g((size_t) size);
    vector<double> ans;
    ans.push_back(0);

    for (auto &e: result) {
      g[e.a].push_back(e.b);
      g[e.b].push_back(e.a);
    }
    int prev = 0;
    int cur = g[prev][0];
    for (int i = 0; i < size - 1; ++i) {
      ans.push_back(cur);
      if (g[cur][0] != prev) {
        prev = cur;
        cur = g[cur][0];
      } else {
        prev = cur;
        cur = g[cur][1];
      }
    }
    return ans;
  }

  set<Edge> result;
  multiset<Edge> MST;
  vector<Point> points_;
  vector<set<int>> neighbours;
  int size;

};
