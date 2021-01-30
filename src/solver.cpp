#include "solver.hpp"

#include <map>

#include "box_iterator.hpp"
#include <iostream>

namespace solver {

std::set<int> kFullSetPencilMarks = {1, 2, 3, 4, 5, 6, 7, 8, 9};

void FillPencilMarks(state::State& state) {
  box::Box b = box::Box::GetGrid();
  for (const auto& pair_list : box::GetIterable<1>(state, b)) {
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

std::set<int> CollectVisible(const state::State& state, int i, int j) {
  std::set<int> result;

  box::Box b = box::Box::GetColumn(j);
  for (const auto& pair_list : box::GetIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.val) result.insert(p.val);
  }

  b = box::Box::GetRow(i);
  for (const auto& pair_list : box::GetIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.val) result.insert(p.val);
  }

  b = box::Box::GetBox(i, j);
  for (const auto& pair_list : box::GetIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.val) result.insert(p.val);
  }

  return result;
}

void ReducePencilMarks(state::State& state) {
  box::Box b = box::Box::GetGrid();
  for (const auto& pair_list : box::GetIterable<1>(state, b)) {
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
std::vector<Pair> GetPairsForRowN(const state::State& state,
                                  const box::Box& b) {
  std::vector<Pair> result;
  for (const auto& pair_list : box::GetIterable<N>(state, b)) {
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
std::vector<Pair> GetPairsForBoxN(const state::State& state,
                                  const box::Box& b) {
  std::vector<Pair> result;

  for (const auto& pair_list : box::GetIterable<N>(state, b)) {
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

std::vector<Pair> GetPairsForRow(const state::State& state, const box::Box& b) {
  int count = 0;
  for (const auto& pair_list : box::GetIterable<1>(state, b)) {
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

std::vector<Pair> GetPairsForBox(const state::State& state, const box::Box& b) {
  int count = 0;
  for (const auto& pair_list : box::GetIterable<1>(state, b)) {
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

std::vector<Pair> GetPairsForColumn(const state::State& state,
                                    const box::Box& b) {
  int count = 0;
  for (const auto& pair_list : box::GetIterable<1>(state, b)) {
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
  for (const auto& pair_list : box::GetIterable<1>(state, b)) {
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
  for (const auto& pair_list : box::GetIterable<1>(state, b)) {
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
  for (const auto& pair_list : box::GetIterable<1>(state, b)) {
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

int CountPencilMarksInRow(const state::State& state, int i, int val) {
  int c = 0;

  box::Box b = box::Box::GetRow(i);
  for (const auto& pair_list : box::GetIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.pm.count(val)) c++;
  }
  return c;
}

int CountPencilMarksInColumn(const state::State& state, int j, int val) {
  int c = 0;

  box::Box b = box::Box::GetColumn(j);
  for (const auto& pair_list : box::GetIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.pm.count(val)) c++;
  }
  return c;
}

int CountPencilMarksInBox(const state::State& state, int i, int j, int val) {
  int c = 0;
  box::Box b = box::Box::GetBox(i, j);
  for (const auto& pair_list : box::GetIterable<1>(state, b)) {
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
  for (const auto& pair_list : box::GetIterable<1>(state, b)) {
    const auto& p = pair_list.front();
    if (p.val) {
      if (p.pm.size()) {
        p.pm.clear();
      }
    }
  }
}

std::vector<std::map<int, std::set<int>>> SeparateRowsForSwordfish(
    state::State& state, const std::vector<box::Box>& g) {

  std::vector<std::map<int, std::set<int>>> result;
  for (const auto& r : g) {
    std::map<int, std::set<int>> marks_distribution;
    int i = 0;
    for (const auto& it : box::GetIterable<1>(state, r)) {
      const auto& ref = it.front();
      if (ref.val) {
        i++;
        continue;
      }
      for (int val : ref.pm) {
        marks_distribution[val].insert(i);
      }
      i++;
    }
    result.emplace_back(marks_distribution);
  }
  return result;
}

struct DetectedXwing {
  int val;
  std::set<int> positions;
};

std::vector<DetectedXwing> DetectXwing(state::State& state, const std::vector<std::vector<std::map<int, std::set<int>>>::const_iterator>& rows){
  std::set<int> values;
  for (const auto& r : rows){
    for (const auto& p : (*r)){
      values.insert(p.first);
    }
  }

  std::vector<DetectedXwing> result;

  for (int val: values){
    bool xwing_possible = true;
    std::set<int> positions;
    for (const auto& r : rows){
      if(r->count(val)){
        const auto& ps = r->at(val);
        positions.insert(ps.begin(), ps.end());
      } else {
        xwing_possible = false;
        break;
      }
    }
    if(!xwing_possible) continue;

    if(positions.size() <= rows.size()){
      DetectedXwing x;
      x.val = val;
      x.positions = positions;
      result.push_back(x);
    }
  }
  return result;
}
struct XwingWithRows{
  int val;
  std::set<int> positions_in_row;
  std::set<int> rows;
};

std::vector<XwingWithRows> GetSwordfish(state::State& state, const std::vector<box::Box>& g) {
  const auto& s = SeparateRowsForSwordfish(state, g);
  using Iterator = decltype(s.begin());
  std::vector<XwingWithRows> r;
  {
  auto iterable = box::PairIterable<Iterator, 2>(s.begin(), s.end());
  for (auto it = iterable.begin(); it!= iterable.end(); ++it){
    const auto& xwings = DetectXwing(state, *it);
    if(xwings.empty()){
      continue;
    }
    std::set<int> rows;
    const auto& indexes = it.GetIndexes();
    rows.insert(indexes.begin(), indexes.end());
    for (const auto& x: xwings){
      XwingWithRows xr;
      xr.val = x.val;
      xr.positions_in_row = x.positions;
      xr.rows = rows;
      r.push_back(xr);
    }
  }
  }

{
  auto iterable = box::PairIterable<Iterator, 3>(s.begin(), s.end());
  std::vector<XwingWithRows> r;
  for (auto it = iterable.begin(); it!= iterable.end(); ++it){
    const auto& xwings = DetectXwing(state, *it);
    if(xwings.empty()){
      continue;
    }
    std::set<int> rows;
    const auto& indexes = it.GetIndexes();
    rows.insert(indexes.begin(), indexes.end());
    for (const auto& x: xwings){
      XwingWithRows xr;
      xr.val = x.val;
      xr.positions_in_row = x.positions;
      xr.rows = rows;
      r.push_back(xr);
    }
  }
  }
{
  auto iterable = box::PairIterable<Iterator, 4>(s.begin(), s.end());
  std::vector<XwingWithRows> r;
  for (auto it = iterable.begin(); it!= iterable.end(); ++it){
    const auto& xwings = DetectXwing(state, *it);
    if(xwings.empty()){
      continue;
    }
    std::set<int> rows;
    const auto& indexes = it.GetIndexes();
    rows.insert(indexes.begin(), indexes.end());
    for (const auto& x: xwings){
      XwingWithRows xr;
      xr.val = x.val;
      xr.positions_in_row = x.positions;
      xr.rows = rows;
      r.push_back(xr);
    }
  }
  }
  return r;
}

void ApplyXwingRestrictionToColumns(state::State& state, const box::Box& b, int val, std::set<int> ignored_rows){
  for (auto it : box::GetIterable<1>(state, b)) {
    auto& ref = it.front();
    if(ignored_rows.count(ref.i)){
      continue;
    }
    if(ref.pm.count(val)){
      ref.pm.erase(val);
      state.changed_on_iter = state.current_iter;
    }
  }
  
}

void ApplyXwingRestrictionToRows(state::State& state, const box::Box& b, int val, std::set<int> ignored_cols){
  for (auto it : box::GetIterable<1>(state, b)) {
    auto& ref = it.front();
    if(ignored_cols.count(ref.j)){
      continue;
    }
    if(ref.pm.count(val)){
      ref.pm.erase(val);
      state.changed_on_iter = state.current_iter;
    }
  }
  
}

void FindSwordfish(state::State& state){
{
  const auto& g = box::Box::GetGridAsRows();
  const auto& xwings = GetSwordfish(state, g);
  for (const auto& x : xwings){
    int val = x.val;
    std::set<int> column_indexes = x.positions_in_row;
    std::set<int> rows_to_ignore = x.rows;
    std::vector<box::Box> cols = box::Box::GetGridAsColumns();
    for (int i : column_indexes){
      ApplyXwingRestrictionToColumns(state, cols[i], val, rows_to_ignore);
    }
  }
  }
{
  const auto& g = box::Box::GetGridAsColumns();
  const auto& xwings = GetSwordfish(state, g);
  for (const auto& x : xwings){
    int val = x.val;
    std::set<int> column_indexes = x.positions_in_row;
    std::set<int> rows_to_ignore = x.rows;
    std::vector<box::Box> cols = box::Box::GetGridAsRows();
    for (int i : column_indexes){
      ApplyXwingRestrictionToRows(state, cols[i], val, rows_to_ignore);
    }
  }
  }
}

void InsertValuesFromPencilMarks(state::State& state) {
  box::Box b = box::Box::GetGrid();
  for (auto pair_list : box::GetIterable<1>(state, b)) {
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
