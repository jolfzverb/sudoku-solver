#include "state.hpp"

#include <iostream>

#include "box_iterator.hpp"

namespace state {

State ReadState() {
  State r;
  r.size = 9;
  r.current_iter = 0;
  r.changed_on_iter = 0;
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
