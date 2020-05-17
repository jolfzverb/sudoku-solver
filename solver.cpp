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
  return r;
}

void FullPrint(const State& state) {
  for (int i = 0; i < state.size; i++) {
    const auto& a = state.f[i];
    const auto& p = state.p[i];
    for (int j = 0; j < state.size; j++) {
      int val = a[j];
      const auto& pm = p[j];
      if (val) {
        std::cout << "|    " << val << "    ";
      } else {
        std::cout << "|";
        for (int k : pm) {
          std::cout << k;
        }
        for (int k = pm.size(); k < state.size; k++) {
          std::cout << " ";
        }
      }
    }
    std::cout << "|" << std::endl;
  }
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

std::vector<Pair> GetPairsForRow(const State& state, int i) {
  int count = 0;
  for (const auto& pm : state.p[i]) {
    if (!pm.empty()) {
      count++;
    }
  }
  if (count < 3) return {};

  std::vector<Pair> result;
  // pairs of two
  for (int j = 0; j < state.size - 1; j++) {
    if (state.p[i][j].empty()) continue;
    for (int k = j + 1; k < state.size; k++) {
      if (state.p[i][k].empty()) continue;
      std::set<int> possible_marks = state.p[i][j];
      possible_marks.insert(state.p[i][k].begin(), state.p[i][k].end());
      if (possible_marks.size() == 2) {
        Pair r;
        r.numbers = possible_marks;
        r.coords.insert({i, j});
        r.coords.insert({i, k});
        result.emplace_back(r);
      }
    }
  }
  if (count < 4) return result;
  // pairs of tree
  for (int j = 0; j < state.size - 2; j++) {
    if (state.p[i][j].empty()) continue;
    for (int k = j + 1; k < state.size - 1; k++) {
      if (state.p[i][k].empty()) continue;

      for (int l = k + 1; l < state.size; l++) {
        if (state.p[i][l].empty()) continue;

        std::set<int> possible_marks = state.p[i][j];
        possible_marks.insert(state.p[i][k].begin(), state.p[i][k].end());
        possible_marks.insert(state.p[i][l].begin(), state.p[i][l].end());

        if (possible_marks.size() == 3) {
          Pair r;
          r.numbers = possible_marks;
          r.coords.insert({i, j});
          r.coords.insert({i, k});
          r.coords.insert({i, l});
          result.emplace_back(r);
        }
      }
    }
  }
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
  if (count < 3) return {};

  std::vector<Pair> result;
  // pairs of two
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
          std::cout << "found pair " << i << " " << j << " " << i1 << " " << j1
                    << " " << std::endl;
          Pair r;
          r.numbers = possible_marks;
          r.coords.insert({i, j});
          r.coords.insert({i1, j1});
          result.emplace_back(r);
        }
      }
    }
  }
  if (count < 4) return result;
  // pairs of tree
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
            std::cout << "found pair " << i << " " << j << " " << i1 << " "
                      << j1 << " " << i2 << " " << j2 << " " << std::endl;
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
  if (count < 3) return {};

  std::vector<Pair> result;
  // pairs of two
  for (int i = 0; i < state.size - 1; i++) {
    if (state.p[i][j].empty()) continue;
    for (int k = i + 1; k < state.size; k++) {
      if (state.p[k][j].empty()) continue;
      std::set<int> possible_marks = state.p[i][j];
      possible_marks.insert(state.p[k][j].begin(), state.p[k][j].end());
      if (possible_marks.size() == 2) {
        Pair r;
        r.numbers = possible_marks;
        r.coords.insert({i, j});
        r.coords.insert({k, j});
        result.emplace_back(r);
      }
    }
  }
  if (count < 4) return result;
  // pairs of tree
  for (int i = 0; i < state.size - 2; i++) {
    if (state.p[i][j].empty()) {
      continue;
    }
    for (int k = i + 1; k < state.size - 1; k++) {
      if (state.p[k][j].empty()) {
        continue;
      }
      for (int l = k + 1; l < state.size; l++) {
        if (state.p[l][j].empty()) {
          continue;
        }
        std::set<int> possible_marks = state.p[i][j];
        possible_marks.insert(state.p[k][j].begin(), state.p[k][j].end());
        possible_marks.insert(state.p[l][j].begin(), state.p[l][j].end());
        if (possible_marks.size() == 3) {
          Pair r;
          r.numbers = possible_marks;
          r.coords.insert({i, j});
          r.coords.insert({k, j});
          r.coords.insert({l, j});
          result.emplace_back(r);
        }
      }
    }
  }
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
  return;
}

void ReducePairsInColumn(State& state, int j) {
  const auto& pairs = GetPairsForColumn(state, j);
  if (pairs.empty()) return;
  for (const auto& p : pairs) {
    ApplyPairRestrictionsForColumn(state, j, p);
  }
  return;
}

void ReducePairsInBox(State& state, const Box& bb) {
  const auto& pairs = GetPairsForBox(state, bb);
  if (pairs.empty()) return;
  std::cout << "found pairs in box " << pairs.size() << std::endl;
  for (const auto& p : pairs) {
    ApplyPairRestrictionsForBox(state, bb, p);
  }
  return;
}

void ReducePairs(State& state) {
  for (int i = 0; i < state.size; i++) {
    ReducePairsInRow(state, i);
  }
  for (int j = 0; j < state.size; j++) {
    std::cout << "trying to reduce column " << j << std::endl;
    ReducePairsInColumn(state, j);
  }
  for (int i = 0; i < state.size; i += 3) {
    for (int j = 0; j < state.size; j += 3) {
      std::cout << "trying to reduce box " << i << " " << j << std::endl;
      Box bb;
      bb.start_i = i;
      bb.end_i = i + 3;
      bb.start_j = j;
      bb.end_j = j + 3;
      ReducePairsInBox(state, bb);
    }
  }
  return;
}

void FillValue(State& state, int i, int j, int val) { state.f[i][j] = val; }

int TryFillSingleValue(State& state, int i, int j) {
  const auto& pm = state.p[i][j];
  if (pm.size() == 1) {
    std::cout << "inserted from single " << i + 1 << " " << j + 1 << std::endl;

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
      std::cout << "inserted from row " << i + 1 << " " << j + 1 << std::endl;
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
      std::cout << "inserted from column " << i + 1 << " " << j + 1
                << std::endl;
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
      std::cout << "inserted from box " << i + 1 << " " << j + 1 << std::endl;
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
    std::cout << "Solved!" << std::endl;
    PrintState(state);
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
        std::cout << "Exiting";
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
