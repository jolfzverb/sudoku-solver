#pragma once

#include <set>
#include <vector>

namespace state {
struct State {
  std::vector<std::vector<int>> f;
  std::vector<std::vector<std::set<int>>> p;
  int current_iter;
  int changed_on_iter;
  int size;
};

State ReadState();
void FullPrint(const State& state);
void PrintState(const State& state);
void FillPencilMarks(State& state);
void ReducePencilMarks(State& state);
void ReducePairs(State& state);
void RemovePencilMark(State& state);
void InsertValuesFromPencilMarks(State& state);
void CheckIfSolved(const State& state);

}  // namespace state
