#pragma once

#include "state.hpp"

namespace solver {
void FillPencilMarks(state::State& state);
void ReducePencilMarks(state::State& state);
void ReducePairs(state::State& state);
void RemovePencilMark(state::State& state);
void FindSwordfish(state::State& state);
void InsertValuesFromPencilMarks(state::State& state);
}  // namespace solver
