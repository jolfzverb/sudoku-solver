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
  Box result;
  for (int k = 0; k < BOX_SIZE; k++) {
    for (int l = 0; l < BOX_SIZE; l++) {
      result.coords.emplace_back(std::make_pair(i + k, j + l));
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

std::optional<std::pair<int, int>> Box::Begin() const {
  if (coords.empty()) return std::nullopt;
  return coords.front();
}

}  // namespace box
