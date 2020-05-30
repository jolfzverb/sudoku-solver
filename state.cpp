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
  box::Box b = box::Box::GetGrid();
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& ref = pair_list.front();
    if (ref.val) continue;
    ref.pm = kFullSetPencilMarks;
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

std::set<int> CollectVisible(State& state, int i, int j) {
  std::set<int> result;

  box::Box b = box::Box::GetColumn(j);
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.val) result.insert(p.val);
  }

  b = box::Box::GetRow(i);
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.val) result.insert(p.val);
  }

  b = box::Box::GetBox(i, j);
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.val) result.insert(p.val);
  }

  return result;
}

void ReducePencilMarks(State& state) {
  box::Box b = box::Box::GetGrid();
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& ref = pair_list.front();

    if (ref.val) continue;

    const auto& visible_values = CollectVisible(state, ref.i, ref.j);
    for (auto k : visible_values) {
      int count = state.p[ref.i][ref.j].erase(k);

      if (count) state.changed_on_iter = state.current_iter;
    }
  }

  return;
}

struct Pair {
  std::set<int> numbers;
  std::set<std::pair<int, int>> coords;
};

template <int N, bool Direction>
std::vector<Pair> GetPairsForRowN(State& state, const box::Box& b) {
  std::vector<Pair> result;
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
std::vector<Pair> GetPairsForBoxN(State& state, const box::Box& b) {
  std::vector<Pair> result;

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

std::vector<Pair> GetPairsForRow(State& state, const box::Box& b) {
  int count = 0;
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& ref = pair_list.front();
    if (!ref.pm.empty()) {
      count++;
    }
  }

  std::vector<Pair> result;
  if (count < 3) return result;
  const auto& result2 = GetPairsForRowN<2, true>(state, b);
  result.insert(result.end(), result2.begin(), result2.end());

  if (count < 4) return result;
  const auto& result3 = GetPairsForRowN<3, true>(state, b);
  result.insert(result.end(), result3.begin(), result3.end());

  if (count < 5) return result;
  const auto& result4 = GetPairsForRowN<4, true>(state, b);
  result.insert(result.end(), result4.begin(), result4.end());

  return result;
}

std::vector<Pair> GetPairsForBox(State& state, const box::Box& b) {
  int count = 0;
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& ref = pair_list.front();
    if (!ref.pm.empty()) {
      count++;
    }
  }

  std::vector<Pair> result;
  if (count < 3) return result;
  const auto& result2 = GetPairsForBoxN<2>(state, b);
  result.insert(result.end(), result2.begin(), result2.end());

  if (count < 4) return result;
  const auto& result3 = GetPairsForBoxN<3>(state, b);
  result.insert(result.end(), result3.begin(), result3.end());

  if (count < 5) return result;
  const auto& result4 = GetPairsForBoxN<4>(state, b);
  result.insert(result.end(), result4.begin(), result4.end());

  return result;
}

std::vector<Pair> GetPairsForColumn(State& state, const box::Box& b) {
  int count = 0;
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& ref = pair_list.front();
    if (!ref.pm.empty()) {
      count++;
    }
  }

  std::vector<Pair> result;
  if (count < 3) return result;
  const auto& result2 = GetPairsForRowN<2, false>(state, b);
  result.insert(result.end(), result2.begin(), result2.end());

  if (count < 4) return result;
  const auto& result3 = GetPairsForRowN<3, false>(state, b);
  result.insert(result.end(), result3.begin(), result3.end());

  if (count < 5) return result;
  const auto& result4 = GetPairsForRowN<4, false>(state, b);
  result.insert(result.end(), result4.begin(), result4.end());

  return result;
}

void ApplyPairRestrictionsForRow(State& state, const box::Box& b,
                                 const Pair& p) {
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& ref = pair_list.front();
    if (ref.pm.empty()) continue;
    if (p.coords.count({ref.i, ref.j})) continue;
    for (int val : p.numbers) {
      int count = ref.pm.erase(val);
      if (count) state.changed_on_iter = state.current_iter;
    }
  }
}

void ApplyPairRestrictionsForColumn(State& state, const box::Box& b,
                                    const Pair& p) {
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& ref = pair_list.front();
    if (ref.pm.empty()) continue;
    if (p.coords.count({ref.i, ref.j})) continue;
    for (int val : p.numbers) {
      int count = ref.pm.erase(val);
      if (count) state.changed_on_iter = state.current_iter;
    }
  }
}

void ApplyPairRestrictionsForBox(State& state, const box::Box& b,
                                 const Pair& p) {
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& ref = pair_list.front();
    if (ref.pm.empty()) continue;
    if (p.coords.count({ref.i, ref.j})) continue;
    for (int val : p.numbers) {
      int count = ref.pm.erase(val);
      if (count) state.changed_on_iter = state.current_iter;
    }
  }
}

void ReducePairsInRow(State& state, const box::Box& b) {
  const auto& pairs = GetPairsForRow(state, b);
  if (pairs.empty()) return;
  for (const auto& p : pairs) {
    ApplyPairRestrictionsForRow(state, b, p);
  }
}

void ReducePairsInColumn(State& state, const box::Box& b) {
  const auto& pairs = GetPairsForColumn(state, b);
  if (pairs.empty()) return;
  for (const auto& p : pairs) {
    ApplyPairRestrictionsForColumn(state, b, p);
  }
}

void ReducePairsInBox(State& state, const box::Box& b) {
  const auto& pairs = GetPairsForBox(state, b);
  if (pairs.empty()) return;
  for (const auto& p : pairs) {
    ApplyPairRestrictionsForBox(state, b, p);
  }
}

void ReducePairs(State& state) {
  const auto& rows = box::Box::GetGridAsRows();
  for (const auto& row : rows) {
    ReducePairsInRow(state, row);
  }
  const auto& cols = box::Box::GetGridAsColumns();
  for (const auto& col : cols) {
    ReducePairsInColumn(state, col);
  }
  const auto& boxes = box::Box::GetGridAsBoxes();
  for (const auto& box : boxes) {
    ReducePairsInBox(state, box);
  }
}

int TryFillSingleValue(box::ValueReference& ref) {
  if (ref.pm.size() == 1) {
    ref.val = *ref.pm.begin();
    return ref.val;
  }
  return 0;
}

int CountPencilMarksInRow(State& state, int i, int val) {
  int c = 0;

  box::Box b = box::Box::GetRow(i);
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.pm.count(val)) c++;
  }
  return c;
}

int CountPencilMarksInColumn(State& state, int j, int val) {
  int c = 0;

  box::Box b = box::Box::GetColumn(j);
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.pm.count(val)) c++;
  }
  return c;
}

int CountPencilMarksInBox(State& state, int i, int j, int val) {
  int c = 0;
  box::Box b = box::Box::GetBox(i, j);
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.pm.count(val)) c++;
  }
  return c;
}

int TryFillForRow(State& state, box::ValueReference& ref) {
  for (int val : ref.pm) {
    int count = CountPencilMarksInRow(state, ref.i, val);
    if (count == 1) {
      ref.val = val;
      return val;
    }
  }
  return 0;
}

int TryFillForColumn(State& state, box::ValueReference& ref) {
  for (int val : ref.pm) {
    int count = CountPencilMarksInColumn(state, ref.j, val);
    if (count == 1) {
      ref.val = val;
      return val;
    }
  }
  return 0;
}

int TryFillForBox(State& state, box::ValueReference& ref) {
  for (int val : ref.pm) {
    int count = CountPencilMarksInBox(state, ref.i, ref.j, val);
    if (count == 1) {
      ref.val = val;
      return val;
    }
  }
  return 0;
}

void ClearPencilMarks(State& state) {
  box::Box b = box::Box::GetGrid();
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.val) {
      p.pm.clear();
    }
  }
}

void InsertValuesFromPencilMarks(State& state) {
  box::Box b = box::Box::GetGrid();
  for (auto pair_list : box::BoxIterable<1>(state, b)) {
    auto& p = pair_list.front();

    if (p.val) continue;

    int r = 0;
    r = TryFillSingleValue(p);
    if (r) continue;
    r = TryFillForRow(state, p);
    if (r) continue;
    r = TryFillForColumn(state, p);
    if (r) continue;
    r = TryFillForBox(state, p);
    if (r) continue;
  }
  ClearPencilMarks(state);
  return;
}

void CheckIfSolved(State& state) {
  box::Box b = box::Box::GetGrid();
  for (auto pair_list : box::BoxIterable<1>(state, b)) {
    auto& p = pair_list.front();
    if (!p.val) {
      return;
    }
  }

  std::cout << std::endl;
  PrintState(state);
  std::cout << "Solved! current iteration is " << state.current_iter
            << std::endl;
}

void RemovePencilMark(State& state) {
  int i, j, val;
  std::cin >> i >> j >> val;

  state.p[i][j].erase(val);

  std::string tmp;
  std::getline(std::cin, tmp);
}
}  // namespace state
