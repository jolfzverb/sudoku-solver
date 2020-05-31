#include "box_iterator.hpp"

namespace box {

int Box::GRID_SIZE = 9;
int Box::BOX_SIZE = 3;

Box::Box() {}

Box Box::GetRow(int i) {
  Box result;
  for (int j = 0; j < GRID_SIZE; j++) {
    result.coords.emplace_back(std::make_pair(i, j));
  }
  return result;
}

Box Box::GetColumn(int j) {
  Box result;
  for (int i = 0; i < GRID_SIZE; i++) {
    result.coords.emplace_back(std::make_pair(i, j));
  }
  return result;
}

Box Box::GetBox(int i, int j) {
  int start_i = i - i % BOX_SIZE;
  int start_j = j - j % BOX_SIZE;
  Box result;
  for (int k = 0; k < BOX_SIZE; k++) {
    for (int l = 0; l < BOX_SIZE; l++) {
      result.coords.emplace_back(std::make_pair(start_i + k, start_j + l));
    }
  }
  return result;
}

Box Box::GetGrid() {
  Box result;
  for (int k = 0; k < GRID_SIZE; k++) {
    for (int l = 0; l < GRID_SIZE; l++) {
      result.coords.emplace_back(std::make_pair(k, l));
    }
  }
  return result;
}

std::vector<Box> Box::GetGridAsRows() {
  std::vector<Box> result;
  for (int i = 0; i < GRID_SIZE; i++) {
    result.emplace_back(GetRow(i));
  }
  return result;
}

std::vector<Box> Box::GetGridAsColumns() {
  std::vector<Box> result;
  for (int j = 0; j < GRID_SIZE; j++) {
    result.emplace_back(GetColumn(j));
  }
  return result;
}

std::vector<Box> Box::GetGridAsBoxes() {
  std::vector<Box> result;
  for (int i = 0; i < GRID_SIZE; i += BOX_SIZE) {
    for (int j = 0; j < GRID_SIZE; j += BOX_SIZE) {
      result.emplace_back(GetBox(i, j));
    }
  }
  return result;
}

std::optional<std::pair<int, int>> Box::GetNext(int i, int j) const {
  return GetNext(std::make_pair(i, j));
}

std::optional<std::pair<int, int>> Box::GetNext(
    const std::pair<int, int>& p) const {
  auto it = std::find(coords.begin(), coords.end(), p);
  if (it == coords.end()) {
    return std::nullopt;
  }

  it++;

  if (it == coords.end()) {
    return std::nullopt;
  }
  return *it;
}

bool Box::Contains(int i, int j) const {
  return Contains(std::make_pair(i, j));
}

bool Box::Contains(const std::pair<int, int>& p) const {
  auto it = std::find(coords.begin(), coords.end(), p);
  if (it == coords.end()) {
    return false;
  }
  return true;
}

std::optional<std::pair<int, int>> Box::Begin() const {
  if (coords.empty()) return std::nullopt;
  return coords.front();
}

}  // namespace box
