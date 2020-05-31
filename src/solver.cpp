#include "solver.hpp"

#include "box_iterator.hpp"

namespace solver {

std::set<int> kFullSetPencilMarks = {1, 2, 3, 4, 5, 6, 7, 8, 9};

void FillPencilMarks(state::State& state) {
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

std::set<int> CollectVisible(state::State& state, int i, int j) {
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

void ReducePencilMarks(state::State& state) {
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
std::vector<Pair> GetPairsForRowN(state::State& state, const box::Box& b) {
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
std::vector<Pair> GetPairsForBoxN(state::State& state, const box::Box& b) {
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

std::vector<Pair> GetPairsForRow(state::State& state, const box::Box& b) {
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

std::vector<Pair> GetPairsForBox(state::State& state, const box::Box& b) {
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

std::vector<Pair> GetPairsForColumn(state::State& state, const box::Box& b) {
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

void ApplyPairRestrictionsForRow(state::State& state, const box::Box& b,
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

void ApplyPairRestrictionsForColumn(state::State& state, const box::Box& b,
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

void ApplyPairRestrictionsForBox(state::State& state, const box::Box& b,
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

std::optional<box::Box> GetRowForPair(const Pair& p) {
  const auto& c = *p.coords.begin();
  int i = c.first;
  for (const auto& coord : p.coords) {
    if (coord.first != i) {
      return std::nullopt;
    }
  }
  return box::Box::GetRow(i);
}

std::optional<box::Box> GetColumnForPair(const Pair& p) {
  const auto& c = *p.coords.begin();
  int i = c.second;
  for (const auto& coord : p.coords) {
    if (coord.second != i) {
      return std::nullopt;
    }
  }
  return box::Box::GetColumn(i);
}

std::optional<box::Box> GetBoxForPair(const Pair& p) {
  const auto& c = *p.coords.begin();

  box::Box b = box::Box::GetBox(c.first, c.second);

  for (const auto& coord : p.coords) {
    if (!b.Contains(coord)) {
      return std::nullopt;
    }
  }
  return b;
}

void ReducePairsInRow(state::State& state, const box::Box& b) {
  const auto& pairs = GetPairsForRow(state, b);
  if (pairs.empty()) return;
  for (const auto& p : pairs) {
    ApplyPairRestrictionsForRow(state, b, p);
    const auto& box = GetBoxForPair(p);
    if (box) {
      ApplyPairRestrictionsForBox(state, *box, p);
    }
  }
}

void ReducePairsInColumn(state::State& state, const box::Box& b) {
  const auto& pairs = GetPairsForColumn(state, b);
  if (pairs.empty()) return;
  for (const auto& p : pairs) {
    ApplyPairRestrictionsForColumn(state, b, p);
    const auto& box = GetBoxForPair(p);
    if (box) {
      ApplyPairRestrictionsForBox(state, *box, p);
    }
  }
}

void ReducePairsInBox(state::State& state, const box::Box& b) {
  const auto& pairs = GetPairsForBox(state, b);
  if (pairs.empty()) return;
  for (const auto& p : pairs) {
    ApplyPairRestrictionsForBox(state, b, p);
    const auto& col = GetColumnForPair(p);
    if (col) {
      ApplyPairRestrictionsForColumn(state, *col, p);
    }
    const auto& row = GetRowForPair(p);
    if (row) {
      ApplyPairRestrictionsForRow(state, *row, p);
    }
  }
}

void ReducePairs(state::State& state) {
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

int CountPencilMarksInRow(state::State& state, int i, int val) {
  int c = 0;

  box::Box b = box::Box::GetRow(i);
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.pm.count(val)) c++;
  }
  return c;
}

int CountPencilMarksInColumn(state::State& state, int j, int val) {
  int c = 0;

  box::Box b = box::Box::GetColumn(j);
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.pm.count(val)) c++;
  }
  return c;
}

int CountPencilMarksInBox(state::State& state, int i, int j, int val) {
  int c = 0;
  box::Box b = box::Box::GetBox(i, j);
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.pm.count(val)) c++;
  }
  return c;
}

int TryFillForRow(state::State& state, box::ValueReference& ref) {
  for (int val : ref.pm) {
    int count = CountPencilMarksInRow(state, ref.i, val);
    if (count == 1) {
      ref.val = val;
      return val;
    }
  }
  return 0;
}

int TryFillForColumn(state::State& state, box::ValueReference& ref) {
  for (int val : ref.pm) {
    int count = CountPencilMarksInColumn(state, ref.j, val);
    if (count == 1) {
      ref.val = val;
      return val;
    }
  }
  return 0;
}

int TryFillForBox(state::State& state, box::ValueReference& ref) {
  for (int val : ref.pm) {
    int count = CountPencilMarksInBox(state, ref.i, ref.j, val);
    if (count == 1) {
      ref.val = val;
      return val;
    }
  }
  return 0;
}

void ClearPencilMarks(state::State& state) {
  box::Box b = box::Box::GetGrid();
  for (const auto& pair_list : box::BoxIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.val) {
      p.pm.clear();
    }
  }
}

void InsertValuesFromPencilMarks(state::State& state) {
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

}  // namespace solver
