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

  void Solve(){
//    Ищем кратчайшее ребро
    Edge shortest = {0 , 1 ,distance(points_[0], points_[1])};
    for (int i = 0; i < size; ++i) {
      for (int j = i + 1; j < size; ++j) {
        if (distance(points_[i], points_[j]) < shortest.weight)
          shortest = {i, j, distance(points_[i], points_[j])};
      }
    }
    result.insert(shortest);

//    Ищем кратчайшее ребро, инцендентное одной из крайних вершин
    int a = shortest.a, b = shortest.b;
    for (int i = 1; i < size - 1; ++i) {
      for (int j = 0; j < size; ++j) {
        neighbours[j].erase(a);
        neighbours[j].erase(b);
      }
      Edge shortest_a = {a , *neighbours[a].begin(), distance(points_[a], points_[*neighbours[a].begin()])};
      for (auto v: neighbours[a]) {
        if (distance(points_[a] , points_[v]) < shortest_a.weight)
          shortest_a = {a, v, distance(points_[a] , points_[v])};
      }
      Edge shortest_b = {b , *neighbours[b].begin(), distance(points_[b], points_[*neighbours[b].begin()])};
      for (auto v: neighbours[b]) {
        if (distance(points_[b] , points_[v]) < shortest_b.weight)
          shortest_b = {b, v, distance(points_[b] , points_[v])};
      }
      if (shortest_a.weight < shortest_b.weight) {
        result.insert(shortest_a);
        a = shortest_a.b;
      } else {
        result.insert(shortest_b);
        b = shortest_b.b;
      }
    }

//    Добавляем последнее ребро
    result.insert({a , b, distance(points_[a], points_[b])});
    print_result();
  }

 private:
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

  set<Edge> result;
  vector<Point> points_;
  vector<set<int>> neighbours;
  int size;

};
