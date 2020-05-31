#pragma once

#include <set>
#include <utility>
#include <vector>

#include "state.hpp"

namespace box {

class Box {
 public:
  static int GRID_SIZE;
  static int BOX_SIZE;

  Box();

  static Box GetRow(int i);
  static Box GetColumn(int j);
  static Box GetBox(int i, int j);
  static Box GetGrid();
  static std::vector<Box> GetGridAsRows();
  static std::vector<Box> GetGridAsColumns();
  static std::vector<Box> GetGridAsBoxes();

  std::optional<std::pair<int, int>> GetNext(int i, int j) const;

  std::optional<std::pair<int, int>> GetNext(
      const std::pair<int, int>& p) const;

  std::optional<std::pair<int, int>> Begin() const;

  bool Contains(int i, int j) const;
  bool Contains(const std::pair<int, int>& p) const;

 private:
  std::vector<std::pair<int, int>> coords;
};

class ValueReference {
 public:
  ValueReference(state::State& state, int i, int j)
      : i(i), j(j), val(state.f[i][j]), pm(state.p[i][j]) {}

  int i;
  int j;
  int& val;
  std::set<int>& pm;
};

class ConstValueReference {
 public:
  ConstValueReference(const state::State& state, int i, int j)
      : i(i), j(j), val(state.f[i][j]), pm(state.p[i][j]) {}

  int i;
  int j;
  const int& val;
  const std::set<int>& pm;
};

template <int N>
class BoxIterator {
 public:
  static BoxIterator Begin(state::State& state, const Box& b) {
    return BoxIterator(state, b);
  }

  static BoxIterator End(state::State& state, const Box& b) {
    BoxIterator r(state, b);
    r.is_end = true;
    return r;
  }

  BoxIterator(state::State& state, const Box& b)
      : state(state), b(b), is_end(false) {
    if (N > state.size) {
      is_end = true;
      return;
    }
    auto n = b.Begin();
    for (int i = 0; i < N; i++) {
      if (n) {
        coordinates.push_back(*n);
      } else {
        is_end = true;
        return;
      }
      n = b.GetNext(*n);
    }
  }

  BoxIterator(const BoxIterator& other)
      : state(other.state),
        b(other.b),
        is_end(other.is_end),
        coordinates(other.coordinates) {}

  BoxIterator& operator++() {
    if (is_end) {
      return *this;
    }

    auto n = b.GetNext(coordinates.back());

    if (n) {
      coordinates.back() = *n;
      return *this;
    }

    for (int k = N - 2; k >= 0; k--) {
      auto n = b.GetNext(coordinates[k]);
      if (!n) {
        is_end = true;
        return *this;
      }
      if (*n != coordinates[k + 1]) {
        coordinates[k] = *n;
        return *this;
      }
    }
    is_end = true;
    return *this;
  }

  BoxIterator operator++(int) {
    BoxIterator tmp(*this);
    operator++();
    return tmp;
  }

  bool operator!=(const BoxIterator& other) const {
    if (other.is_end != is_end) return true;

    if (other.is_end && is_end) return false;

    for (int k = 0; k < N; k++) {
      if (coordinates[k] != other.coordinates[k]) return true;
    }
    return false;
  }

  std::vector<ValueReference> operator*() const {
    if (is_end) return {};
    std::vector<ValueReference> r;
    for (int k = 0; k < N; k++) {
      auto p = coordinates[k];
      int i_coord = p.first;
      int j_coord = p.second;
      r.emplace_back(state, i_coord, j_coord);
    }
    return r;
  }

 private:
  state::State& state;
  const Box& b;
  bool is_end;
  std::vector<std::pair<int, int>> coordinates;
};

template <int N>
class ConstBoxIterator {
 public:
  static ConstBoxIterator Begin(const state::State& state, const Box& b) {
    return ConstBoxIterator(state, b);
  }

  static ConstBoxIterator End(const state::State& state, const Box& b) {
    ConstBoxIterator r(state, b);
    r.is_end = true;
    return r;
  }

  ConstBoxIterator(const state::State& state, const Box& b)
      : state(state), b(b), is_end(false) {
    if (N > state.size) {
      is_end = true;
      return;
    }
    auto n = b.Begin();
    for (int i = 0; i < N; i++) {
      if (n) {
        coordinates.push_back(*n);
      } else {
        is_end = true;
        return;
      }
      n = b.GetNext(*n);
    }
  }

  ConstBoxIterator(const ConstBoxIterator& other)
      : state(other.state),
        b(other.b),
        is_end(other.is_end),
        coordinates(other.coordinates) {}

  ConstBoxIterator& operator++() {
    if (is_end) {
      return *this;
    }

    auto n = b.GetNext(coordinates.back());

    if (n) {
      coordinates.back() = *n;
      return *this;
    }

    for (int k = N - 2; k >= 0; k--) {
      auto n = b.GetNext(coordinates[k]);
      if (!n) {
        is_end = true;
        return *this;
      }
      if (*n != coordinates[k + 1]) {
        coordinates[k] = *n;
        return *this;
      }
    }
    is_end = true;
    return *this;
  }

  ConstBoxIterator operator++(int) {
    ConstBoxIterator tmp(*this);
    operator++();
    return tmp;
  }

  bool operator!=(const ConstBoxIterator& other) const {
    if (other.is_end != is_end) return true;

    if (other.is_end && is_end) return false;

    for (int k = 0; k < N; k++) {
      if (coordinates[k] != other.coordinates[k]) return true;
    }
    return false;
  }

  std::vector<ConstValueReference> operator*() const {
    if (is_end) return {};
    std::vector<ConstValueReference> r;
    for (int k = 0; k < N; k++) {
      auto p = coordinates[k];
      int i_coord = p.first;
      int j_coord = p.second;
      r.emplace_back(state, i_coord, j_coord);
    }
    return r;
  }

 private:
  const state::State& state;
  const Box& b;
  bool is_end;
  std::vector<std::pair<int, int>> coordinates;
};

template <int N>
class BoxIterable {
 public:
  BoxIterable(state::State& state, const Box& b) : state(state), b(b) {}
  BoxIterator<N> begin() { return BoxIterator<N>::Begin(state, b); }
  BoxIterator<N> end() { return BoxIterator<N>::End(state, b); }

 private:
  state::State& state;
  const Box& b;
};

template <int N>
class ConstBoxIterable {
 public:
  ConstBoxIterable(const state::State& state, const Box& b)
      : state(state), b(b) {}
  ConstBoxIterator<N> begin() { return ConstBoxIterator<N>::Begin(state, b); }
  ConstBoxIterator<N> end() { return ConstBoxIterator<N>::End(state, b); }

 private:
  const state::State& state;
  const Box& b;
};

template <int N>
BoxIterable<N> GetIterable(state::State& state, const Box& b) {
  return BoxIterable<N>(state, b);
}

template <int N>
ConstBoxIterable<N> GetIterable(const state::State& state, const Box& b) {
  return ConstBoxIterable<N>(state, b);
}

}  // namespace box
