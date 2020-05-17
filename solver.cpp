#include <iostream>
#include <set>
#include <vector>

enum class Command {
  kReadState,
  kPrintState,
  kFullPrint,
  kFill,
  kInsert,
  kIter,
  kReduce,
  kReducePairs,
  kExit,
  kUnknown,
};

Command GetNextCommand() {
  std::string command;
  std::getline(std::cin, command);

  if (std::cin.eof()) {
    return Command::kExit;
  }
  if (command == "r") {
    return Command::kReadState;
  }
  if (command == "p") {
    return Command::kPrintState;
  }
  if (command == "pp") {
    return Command::kFullPrint;
  }
  if (command == "q") {
    return Command::kExit;
  }
  if (command == "i" || command.empty()) {
    return Command::kIter;
  }
  if (command == "pmfill") {
    return Command::kFill;
  }
  if (command == "pminsert") {
    return Command::kInsert;
  }
  if (command == "pmreduce") {
    return Command::kReduce;
  }
  if (command == "pmreducepairs") {
    return Command::kReducePairs;
  }
  return Command::kUnknown;
}

struct State {
  std::vector<std::vector<int>> f;
  std::vector<std::vector<std::set<int>>> p;
  int current_iter;
  int changed_on_iter;
  int size;
};

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

class ValueReference {
 public:
  ValueReference(const State& state, int i, int j)
      : i(i), j(j), val(state.f[i][j]), pm(state.p[i][j]) {}

  int i;
  int j;
  const int& val;
  const std::set<int>& pm;
};

template <int N, bool Direction>
class RowIterator {
 public:
  static RowIterator Begin(const State& state, int i) {
    return RowIterator(state, i);
  }

  static RowIterator End(const State& state, int i) {
    RowIterator r(state, i);
    r.is_end = true;
    return r;
  }

  RowIterator(const State& state, int i) : state(state), i(i), is_end(false) {
    if (N > state.size) {
      is_end = true;
      return;
    }
    for (int k = 0; k < N; k++) {
      indexes.emplace_back(k);
    }
  }

  RowIterator& operator++() {
    if (indexes.back() + 1 < state.size) {
      indexes.back()++;
      return *this;
    }
    for (int k = N - 2; k >= 0; k--) {
      if (indexes[k] + 1 < indexes[k + 1]) {
        indexes[k]++;
        return *this;
      }
    }
    is_end = true;
    return *this;
  }

  RowIterator operator++(int) {
    RowIterator tmp(state, i);
    operator++();
    return tmp;
  }

  bool operator!=(const RowIterator& other) const {
    if (other.is_end != is_end) return true;

    if (other.is_end && is_end) return false;

    for (int k = 0; k < N; k++) {
      if (indexes[k] != other.indexes[k]) return true;
    }
    return false;
  }

  std::vector<ValueReference> operator*() const {
    if (is_end) return {};
    std::vector<ValueReference> r;
    for (int k = 0; k < N; k++) {
      if (Direction) {
        r.emplace_back(state, i, indexes[k]);
      } else {
        r.emplace_back(state, indexes[k], i);
      }
    }
    return r;
  }

 private:
  const State& state;
  int i;
  std::vector<int> indexes;
  bool is_end;
};

template <int N, bool Direction>
class RowIterable {
 public:
  RowIterable(const State& state, int i) : state(state), i(i) {}
  RowIterator<N, Direction> begin() {
    return RowIterator<N, Direction>::Begin(state, i);
  }
  RowIterator<N, Direction> end() {
    return RowIterator<N, Direction>::End(state, i);
  }

 private:
  const State& state;
  int i;
};

template <int N, bool Direction>
std::vector<Pair> GetPairsForRowN(const State& state, int i) {
  // std::cout << "GetPairsFor " << N << " " << i << std::endl;
  std::vector<Pair> result;
  for (const auto& pair_list : RowIterable<N, Direction>(state, i)) {
    // std::cout << "pair_list " << pair_list.size() << std::endl;
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

std::vector<Pair> GetPairsForRow(const State& state, int i) {
  int count = 0;
  for (const auto& pm : state.p[i]) {
    if (!pm.empty()) {
      count++;
    }
  }
  std::vector<Pair> result;
  // pairs of two
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

std::vector<Pair> GetPairsForBox(const State& state, const Box& bb) {
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
  // pairs of two
  if (count < 3) return result;
  for (int i = bb.start_i; i < bb.end_i; i++) {
    for (int j = bb.start_j; j < bb.end_j; j++) {
      if (state.p[i][j].empty()) continue;
      int i1 = i;
      int j1 = j;
      for (int k = 0; k < state.size - 1; k++) {
        const auto& next = GetNextInBox(i1, j1);
        i1 = next.first;
        j1 = next.second;
        if (state.p[i1][j1].empty()) continue;
        std::set<int> possible_marks = state.p[i][j];
        possible_marks.insert(state.p[i1][j1].begin(), state.p[i1][j1].end());
        if (possible_marks.size() == 2) {
          Pair r;
          r.numbers = possible_marks;
          r.coords.insert({i, j});
          r.coords.insert({i1, j1});
          result.emplace_back(r);
        }
      }
    }
  }

  // pairs of tree
  if (count < 4) return result;
  for (int i = bb.start_i; i < bb.end_i; i++) {
    for (int j = bb.start_j; j < bb.end_j; j++) {
      if (state.p[i][j].empty()) {
        continue;
      }
      int i1 = i;
      int j1 = j;
      for (int k = 0; k < state.size - 1; k++) {
        const auto& next = GetNextInBox(i1, j1);
        i1 = next.first;
        j1 = next.second;
        if (state.p[i1][j1].empty()) continue;

        int i2 = i1;
        int j2 = j1;
        for (int l = k + 1; l < state.size - 1; l++) {
          const auto& next2 = GetNextInBox(i2, j2);
          i2 = next2.first;
          j2 = next2.second;

          if (state.p[i2][j2].empty()) continue;

          std::set<int> possible_marks = state.p[i][j];
          possible_marks.insert(state.p[i1][j1].begin(), state.p[i1][j1].end());
          possible_marks.insert(state.p[i2][j2].begin(), state.p[i2][j2].end());

          if (possible_marks.size() == 3) {
            Pair r;
            r.numbers = possible_marks;
            r.coords.insert({i, j});
            r.coords.insert({i1, j1});
            r.coords.insert({i2, j2});
            result.emplace_back(r);
          }
        }
      }
    }
  }
  return result;
}

std::vector<Pair> GetPairsForColumn(const State& state, int j) {
  int count = 0;
  for (int i = 0; i < state.size; i++) {
    const auto& pm = state.p[i][j];
    if (!pm.empty()) {
      count++;
    }
  }

  std::vector<Pair> result;
  // pairs of two
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

void CheckIfSolved(State& state) {
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

int main() {
  Command command = Command::kReadState;
  State state;
  state.current_iter = 0;

  while (true) {
    state.current_iter++;
    switch (command) {
      case Command::kReadState:
        state = ReadState();
        FillPencilMarks(state);
        break;
      case Command::kPrintState:
        PrintState(state);
        break;
      case Command::kFullPrint:
        FullPrint(state);
        break;
      case Command::kExit:
        std::cout << "Exiting" << std::endl;
        return 0;
      case Command::kFill:
        FillPencilMarks(state);
        break;
      case Command::kInsert:
        InsertValuesFromPencilMarks(state);
        break;
      case Command::kReduce:
        ReducePencilMarks(state);
        break;
      case Command::kReducePairs:
        ReducePairs(state);
        break;
      case Command::kIter:
        ReducePencilMarks(state);
        if (state.changed_on_iter != state.current_iter) {
          ReducePairs(state);
        }
        if (state.changed_on_iter != state.current_iter) {
          std::cout << std::endl;
          FullPrint(state);
          std::cout << "Cannot reduce further, try using brute force"
                    << std::endl;
        }
        InsertValuesFromPencilMarks(state);
        CheckIfSolved(state);
        break;
      default:
        std::cout << "Unknown command, for exit type 'q'" << std::endl;
        break;
    }
    command = GetNextCommand();
  }
  return 0;
}
