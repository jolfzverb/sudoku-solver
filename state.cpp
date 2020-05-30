#include "state.hpp"

#include <iostream>

#include "box_iterator.hpp"

namespace state {
State ReadState() {
  State r;
  r.size = 9;
  for (int i = 0; i < r.size; i++) {
    std::vector<int> rf;
    std::vector<std::set<int>> rp;
    for (int j = 0; j < r.size; j++) {
      int val;
      std::cin >> val;
      rf.emplace_back(val);
      rp.emplace_back(std::set<int>());
    }
    r.f.emplace_back(rf);
    r.p.emplace_back(rp);
  }
  std::string tmp;
  std::getline(std::cin, tmp);
  return r;
}

int CalculateBoxSize(const State& state) {
  int box_size = 3;
  for (const auto& p : state.p) {
    for (const auto& pm : p) {
      int val = pm.size();
      box_size = box_size > val ? box_size : val;
    }
  }
  return box_size;
}

void FullPrint(const State& state) {
  int box_size = CalculateBoxSize(state);

  for (int i = 0; i < state.size; i++) {
    const auto& a = state.f[i];
    const auto& p = state.p[i];
    for (int k = 0; k < state.size * (box_size + 1); k++) {
      std::cout << "-";
    }
    std::cout << "-" << std::endl;
    for (int j = 0; j < state.size; j++) {
      int val = a[j];
      const auto& pm = p[j];
      if (val) {
        std::cout << "| " << val;
        for (int k = 2; k < box_size; k++) {
          std::cout << " ";
        }
      } else {
        std::cout << "|";
        for (int k : pm) {
          std::cout << k;
        }
        for (int k = pm.size(); k < box_size; k++) {
          std::cout << " ";
        }
      }
    }
    std::cout << "|" << std::endl;
  }
  for (int k = 0; k < state.size * (box_size + 1); k++) {
    std::cout << "-";
  }
  std::cout << "-" << std::endl;
}

void PrintState(const State& state) {
  if (state.f.size() != state.p.size()) {
    std::cout << "SIZES DO NOT MATCH!!" << std::endl;
  }
  for (int i = 0; i < state.size; i++) {
    const auto& a = state.f[i];
    const auto& p = state.p[i];
    if (a.size() != p.size()) {
      std::cout << "SIZES DO NOT MATCH!" << std::endl;
    }
    std::cout << "-------------------------------------" << std::endl;
    for (int j = 0; j < state.size; j++) {
      int val = a[j];
      const auto& pm = p[j];
      if (val) {
        std::cout << "| " << val << " ";
      } else {
        std::cout << "|(" << pm.size() << ")";
      }
    }
    std::cout << "|" << std::endl;
  }
  std::cout << "-------------------------------------" << std::endl;
}

std::set<int> kFullSetPencilMarks = {1, 2, 3, 4, 5, 6, 7, 8, 9};

void FillPencilMarks(State& state) {
  for (int i = 0; i < state.size; i++) {
    const auto& a = state.f[i];
    auto& p = state.p[i];
    for (int j = 0; j < state.size; j++) {
      int val = a[j];
      if (val) continue;

      p[j] = kFullSetPencilMarks;
    }
  }
  state.changed_on_iter = state.current_iter;
  return;
}

struct Box {
  int start_i;
  int start_j;
  int end_i;
  int end_j;
};

Box GetBox(int i, int j) {
  Box result;
  result.start_i = i - i % 3;
  result.start_j = j - j % 3;
  i += 3;
  j += 3;
  result.end_i = i - i % 3;
  result.end_j = j - j % 3;
  return result;
}

std::set<int> CollectVisible(const State& state, int i, int j) {
  std::set<int> result;
  // column
  for (int k = 0; k < state.size; k++) {
    int val = state.f[k][j];
    if (!val) continue;
    result.insert(val);
  }

  // row
  for (int k = 0; k < state.size; k++) {
    int val = state.f[i][k];
    if (!val) continue;
    result.insert(val);
  }

  // box
  Box bb = GetBox(i, j);
  for (int k = bb.start_i; k < bb.end_i; k++) {
    for (int l = bb.start_j; l < bb.end_j; l++) {
      int val = state.f[k][l];
      if (!val) continue;
      result.insert(val);
    }
  }
  return result;
}

void ReducePencilMarks(State& state) {
  for (int i = 0; i < state.size; i++) {
    for (int j = 0; j < state.size; j++) {
      int val = state.f[i][j];
      if (val) continue;
      const auto& visible_values = CollectVisible(state, i, j);
      for (auto k : visible_values) {
        int count = state.p[i][j].erase(k);
        if (count) state.changed_on_iter = state.current_iter;
      }
    }
  }
  return;
}

struct Pair {
  std::set<int> numbers;
  std::set<std::pair<int, int>> coords;
};

template <int N, bool Direction>
std::vector<Pair> GetPairsForRowN(State& state, int i) {
  std::vector<Pair> result;
  box::Box b;
  if (Direction) {
    b = box::Box::GetRow(i);
  } else {
    b = box::Box::GetColumn(i);
  }
  for (const auto& pair_list : box::BoxIterable<N>(state, b)) {
    std::set<int> possible_marks;
    bool impossible = false;
    for (const auto& p : pair_list) {
      if (p.pm.empty()) {
        impossible = true;
        break;
      }
      possible_marks.insert(p.pm.begin(), p.pm.end());
    }
    if (impossible) continue;

    if (possible_marks.size() == N) {
      Pair r;
      r.numbers = possible_marks;
      for (const auto& p : pair_list) {
        r.coords.insert({p.i, p.j});
      }
      result.emplace_back(r);
    }
  }
  return result;
}

template <int N>
std::vector<Pair> GetPairsForBoxN(State& state, int i, int j) {
  std::vector<Pair> result;

  box::Box b = box::Box::GetBox(i, j);

  for (const auto& pair_list : box::BoxIterable<N>(state, b)) {
    std::set<int> possible_marks;
    bool impossible = false;
    for (const auto& p : pair_list) {
      if (p.pm.empty()) {
        impossible = true;
        break;
      }
      possible_marks.insert(p.pm.begin(), p.pm.end());
    }
    if (impossible) continue;

    if (possible_marks.size() == N) {
      Pair r;
      r.numbers = possible_marks;
      for (const auto& p : pair_list) {
        r.coords.insert({p.i, p.j});
      }
      result.emplace_back(r);
    }
  }
  return result;
}

std::vector<Pair> GetPairsForRow(State& state, int i) {
  int count = 0;
  for (const auto& pm : state.p[i]) {
    if (!pm.empty()) {
      count++;
    }
  }
  std::vector<Pair> result;
  if (count < 3) return result;
  const auto& result2 = GetPairsForRowN<2, true>(state, i);
  result.insert(result.end(), result2.begin(), result2.end());

  if (count < 4) return result;
  const auto& result3 = GetPairsForRowN<3, true>(state, i);
  result.insert(result.end(), result3.begin(), result3.end());

  if (count < 5) return result;
  const auto& result4 = GetPairsForRowN<4, true>(state, i);
  result.insert(result.end(), result4.begin(), result4.end());

  return result;
}

std::pair<int, int> GetNextInBox(int i, int j) {
  int base_i = i - i % 3;
  int base_j = j - j % 3;

  if ((j + 1) % 3 == 0) {
    if ((i + 1) % 3 == 0) {
      return {base_i, base_j};
    } else {
      return {i + 1, base_j};
    }
  } else {
    return {i, j + 1};
  }
}

std::vector<Pair> GetPairsForBox(State& state, const Box& bb) {
  int count = 0;
  for (int i = bb.start_i; i < bb.end_i; i++) {
    for (int j = bb.start_j; j < bb.end_j; j++) {
      const auto& pm = state.p[i][j];
      if (!pm.empty()) {
        count++;
      }
    }
  }
  std::vector<Pair> result;
  if (count < 3) return result;
  const auto& result2 = GetPairsForBoxN<2>(state, bb.start_i, bb.start_j);
  result.insert(result.end(), result2.begin(), result2.end());

  if (count < 4) return result;
  const auto& result3 = GetPairsForBoxN<3>(state, bb.start_i, bb.start_j);
  result.insert(result.end(), result3.begin(), result3.end());

  if (count < 5) return result;
  const auto& result4 = GetPairsForBoxN<4>(state, bb.start_i, bb.start_j);
  result.insert(result.end(), result4.begin(), result4.end());

  return result;
}

std::vector<Pair> GetPairsForColumn(State& state, int j) {
  int count = 0;
  for (int i = 0; i < state.size; i++) {
    const auto& pm = state.p[i][j];
    if (!pm.empty()) {
      count++;
    }
  }

  std::vector<Pair> result;
  if (count < 3) return result;
  const auto& result2 = GetPairsForRowN<2, false>(state, j);
  result.insert(result.end(), result2.begin(), result2.end());

  if (count < 4) return result;
  const auto& result3 = GetPairsForRowN<3, false>(state, j);
  result.insert(result.end(), result3.begin(), result3.end());

  if (count < 5) return result;
  const auto& result4 = GetPairsForRowN<4, false>(state, j);
  result.insert(result.end(), result4.begin(), result4.end());

  return result;
}

void ApplyPairRestrictionsForRow(State& state, int i, const Pair& p) {
  for (int j = 0; j < state.size; j++) {
    auto& pm = state.p[i][j];
    if (pm.empty()) continue;
    if (p.coords.count({i, j})) continue;
    for (int val : p.numbers) {
      int count = pm.erase(val);
      if (count) state.changed_on_iter = state.current_iter;
    }
  }
}

void ApplyPairRestrictionsForColumn(State& state, int j, const Pair& p) {
  for (int i = 0; i < state.size; i++) {
    auto& pm = state.p[i][j];
    if (pm.empty()) continue;
    if (p.coords.count({i, j})) continue;
    for (int val : p.numbers) {
      int count = pm.erase(val);
      if (count) state.changed_on_iter = state.current_iter;
    }
  }
}

void ApplyPairRestrictionsForBox(State& state, const Box& bb, const Pair& p) {
  for (int i = bb.start_i; i < bb.end_i; i++) {
    for (int j = bb.start_j; j < bb.end_j; j++) {
      auto& pm = state.p[i][j];
      if (pm.empty()) continue;
      if (p.coords.count({i, j})) continue;
      for (int val : p.numbers) {
        int count = pm.erase(val);
        if (count) state.changed_on_iter = state.current_iter;
      }
    }
  }
}

void ReducePairsInRow(State& state, int i) {
  const auto& pairs = GetPairsForRow(state, i);
  if (pairs.empty()) return;
  for (const auto& p : pairs) {
    ApplyPairRestrictionsForRow(state, i, p);
  }
}

void ReducePairsInColumn(State& state, int j) {
  const auto& pairs = GetPairsForColumn(state, j);
  if (pairs.empty()) return;
  for (const auto& p : pairs) {
    ApplyPairRestrictionsForColumn(state, j, p);
  }
}

void ReducePairsInBox(State& state, const Box& bb) {
  const auto& pairs = GetPairsForBox(state, bb);
  if (pairs.empty()) return;
  for (const auto& p : pairs) {
    ApplyPairRestrictionsForBox(state, bb, p);
  }
}

void ReducePairs(State& state) {
  for (int i = 0; i < state.size; i++) {
    ReducePairsInRow(state, i);
  }
  for (int j = 0; j < state.size; j++) {
    ReducePairsInColumn(state, j);
  }
  for (int i = 0; i < state.size; i += 3) {
    for (int j = 0; j < state.size; j += 3) {
      Box bb;
      bb.start_i = i;
      bb.end_i = i + 3;
      bb.start_j = j;
      bb.end_j = j + 3;
      ReducePairsInBox(state, bb);
    }
  }
}

void FillValue(State& state, int i, int j, int val) { state.f[i][j] = val; }

int TryFillSingleValue(State& state, int i, int j) {
  const auto& pm = state.p[i][j];
  if (pm.size() == 1) {
    int val = *pm.begin();
    FillValue(state, i, j, *pm.begin());
    return val;
  }
  return 0;
}

int CountPencilMarksInRow(const State& state, int i, int val) {
  int c = 0;
  for (const auto& pm : state.p[i]) {
    if (pm.count(val)) c++;
  }
  return c;
}

int CountPencilMarksInColumn(const State& state, int j, int val) {
  int c = 0;
  for (const auto& pm : state.p) {
    if (pm[j].count(val)) c++;
  }
  return c;
}

int CountPencilMarksInBox(const State& state, int i, int j, int val) {
  int c = 0;
  Box bb = GetBox(i, j);
  for (int k = bb.start_i; k < bb.end_i; k++) {
    for (int l = bb.start_j; l < bb.end_j; l++) {
      const auto& pm = state.p[k][l];
      if (pm.count(val)) c++;
    }
  }
  return c;
}

int TryFillForRow(State& state, int i, int j) {
  const auto& pm = state.p[i][j];
  for (int val : pm) {
    int count = CountPencilMarksInRow(state, i, val);
    if (count == 1) {
      FillValue(state, i, j, val);
      return val;
    }
  }
  return 0;
}

int TryFillForColumn(State& state, int i, int j) {
  const auto& pm = state.p[i][j];
  for (int val : pm) {
    int count = CountPencilMarksInColumn(state, j, val);
    if (count == 1) {
      FillValue(state, i, j, val);
      return val;
    }
  }
  return 0;
}

int TryFillForBox(State& state, int i, int j) {
  const auto& pm = state.p[i][j];
  for (int val : pm) {
    int count = CountPencilMarksInBox(state, i, j, val);
    if (count == 1) {
      FillValue(state, i, j, val);
      return val;
    }
  }
  return 0;
}

void ClearPencilMarks(State& state) {
  for (int i = 0; i < state.size; i++) {
    for (int j = 0; j < state.size; j++) {
      int val = state.f[i][j];
      if (val) {
        state.p[i][j] = std::set<int>();
      }
    }
  }
}

void InsertValuesFromPencilMarks(State& state) {
  for (int i = 0; i < state.size; i++) {
    for (int j = 0; j < state.size; j++) {
      int val = state.f[i][j];
      int r = 0;
      if (val) continue;
      r = TryFillSingleValue(state, i, j);
      if (r) continue;
      r = TryFillForRow(state, i, j);
      if (r) continue;
      r = TryFillForColumn(state, i, j);
      if (r) continue;
      r = TryFillForBox(state, i, j);
      if (r) continue;
    }
  }
  ClearPencilMarks(state);
  return;
}

void CheckIfSolved(const State& state) {
  bool solved = true;
  for (int i = 0; i < state.size; i++) {
    for (int j = 0; j < state.size; j++) {
      int val = state.f[i][j];
      if (!val) {
        solved = false;
        break;
      }
    }
    if (!solved) break;
  }
  if (solved) {
    std::cout << std::endl;
    PrintState(state);
    std::cout << "Solved! current iteration is " << state.current_iter
              << std::endl;
  }
}

void RemovePencilMark(State& state) {
  int i, j, val;
  std::cin >> i >> j >> val;

  state.p[i][j].erase(val);
  std::string tmp;
  std::getline(std::cin, tmp);
}
}  // namespace state
