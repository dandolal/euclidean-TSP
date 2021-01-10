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
    if (weight > another.weight)
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

struct State {
  int action; // 0 - удаление, 1 - фиксация
  Edge edge;
};

class Graph {
 public:
  explicit Graph(const vector<Point> &points) {
//    points = points;
    size = int(points.size());

//    Заполняем ребра
    for (int i = 0; i < size; ++i) {
      for (int j = i + 1; j < size; ++j) {
        edges.insert({i, j, distance(points[i], points[j])});
      }
    }
    neighbours.resize((size_t) size);

//    Заполняем список соседей
    for (int i = 0; i < size; ++i)
      for (int j = 0; j < size; ++j)
        if (i != j)
          neighbours[i].insert(j);

  }

  void Solve() {
    while (result.size() < size) {
//      Пытаемся удалить самое тяжелое ребро
      Edge cur;
      for (auto edge: edges)
        if (result.count(edge) == 0) {
          cur = edge;
          edges.erase(edge);
          break;
        }

      neighbours[cur.a].erase(cur.b);
      neighbours[cur.b].erase(cur.a);
      if (check_erase()) {
        states.push({0, cur});
        continue;
      } else {
        edges.insert(cur);
        neighbours[cur.a].insert(cur.b);
        neighbours[cur.b].insert(cur.a);
      }

//      Пытаемся зафиксировать текущее ребро
      result.insert(cur);
      if (check_fix()) {
        states.push({1, cur});
        continue;
      } else {
//        Все плохо, надо откатываться
        result.erase(cur);
        while (true) {
          auto top = states.top();
          states.pop();
          if (top.action == 1) {
            result.erase(top.edge);
          } else {
            edges.insert(top.edge);
            neighbours[top.edge.a].insert(top.edge.b);
            neighbours[top.edge.b].insert(top.edge.a);
            result.insert(top.edge);
            if (check_fix()) {
              states.push({1, top.edge});
              break;
            } else
              result.erase(top.edge);
          }
        }
      }
    }
    print_result();
  }

 private:
  bool check_erase() {
    for (auto &s: neighbours)
      if (s.size() < 2)
        return false;

//    Проверим на связность
    vector<vector<int>> g((size_t) size);
    for (auto &e: edges) {
      g[e.a].push_back(e.b);
      g[e.b].push_back(e.a);
    }
    vector<bool> used((size_t) size, false);
    queue<int> q;

    used[0] = true;
    q.push(0);
    while (!q.empty()) {
      int cur = q.front();
      q.pop();
      for (int u: g[cur]) {
        if (!used[u]) {
          used[u] = true;
          q.push(u);
        }
      }
    }
    for (bool us: used)
      if(!us)
        return false;

    return true;
  }

  bool check_fix() {
    vector<vector<int>> g((size_t) size);
    for (auto &e: result) {
      g[e.a].push_back(e.b);
      g[e.b].push_back(e.a);
    }
    // Проверяем, что нет вершины степени 3
    for (auto &v: g)
      if (v.size() >= 3)
        return false;

    // Проверяем, что нет коротких циклов
    vector<bool> used((size_t) size, false);
    vector<int> parents((size_t) size);
    queue<int> q;
    vector<int> distance((size_t) size);
    for (int i = 0; i < size; ++i) {
      if (used[i])
        continue;
      used[i] = true;
      distance[i] = 0;
      q.push(i);
      while (!q.empty()) {
        int cur = q.front();
        q.pop();
        for (int u: g[cur]) {
          if (used[u] && u != parents[cur] && distance[u] + distance[cur] != size - 1)
            return false;
          if (!used[u]) {
            parents[u] = cur;
            used[u] = true;
            distance[u] = distance[cur] + 1;
            q.push(u);
          }
        }
      }
    }
    return true;
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

  stack<State> states;
  set<Edge> result;
  vector<set<int>> neighbours;
  set<Edge> edges;
  int size;
};
