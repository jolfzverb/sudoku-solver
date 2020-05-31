#include <gtest/gtest.h>

#include "box_iterator.hpp"
#include "state.hpp"

TEST(Box, Row) {
  box::Box b = box::Box::GetRow(3);

  auto n = b.Begin();

  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 3);
  ASSERT_EQ(n->second, 0);

  n = b.GetNext(*n);

  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 3);
  ASSERT_EQ(n->second, 1);

  n = b.GetNext(3, 1);

  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 3);
  ASSERT_EQ(n->second, 2);

  n = b.GetNext(3, 8);
  ASSERT_FALSE(n);

  n = b.GetNext(0, 0);
  ASSERT_FALSE(n);

  ASSERT_TRUE(b.Contains(std::make_pair(3, 0)));
  ASSERT_TRUE(b.Contains(3, 0));
  ASSERT_TRUE(b.Contains(3, 1));
  ASSERT_TRUE(b.Contains(3, 2));
  ASSERT_TRUE(b.Contains(3, 3));
  ASSERT_TRUE(b.Contains(3, 4));
  ASSERT_TRUE(b.Contains(3, 5));
  ASSERT_TRUE(b.Contains(3, 6));
  ASSERT_TRUE(b.Contains(3, 7));
  ASSERT_TRUE(b.Contains(3, 8));

  ASSERT_FALSE(b.Contains(3, 9));
  ASSERT_FALSE(b.Contains(std::make_pair(3, 9)));
  ASSERT_FALSE(b.Contains(2, 2));
  ASSERT_FALSE(b.Contains(std::make_pair(2, 2)));
}

TEST(Box, Column) {
  box::Box b = box::Box::GetColumn(3);

  auto n = b.Begin();

  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 0);
  ASSERT_EQ(n->second, 3);

  n = b.GetNext(*n);

  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 1);
  ASSERT_EQ(n->second, 3);

  n = b.GetNext(1, 3);

  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 2);
  ASSERT_EQ(n->second, 3);

  n = b.GetNext(8, 3);
  ASSERT_FALSE(n);

  n = b.GetNext(0, 0);
  ASSERT_FALSE(n);

  ASSERT_TRUE(b.Contains(std::make_pair(0, 3)));
  ASSERT_TRUE(b.Contains(0, 3));
  ASSERT_TRUE(b.Contains(1, 3));
  ASSERT_TRUE(b.Contains(2, 3));
  ASSERT_TRUE(b.Contains(3, 3));
  ASSERT_TRUE(b.Contains(4, 3));
  ASSERT_TRUE(b.Contains(5, 3));
  ASSERT_TRUE(b.Contains(6, 3));
  ASSERT_TRUE(b.Contains(7, 3));
  ASSERT_TRUE(b.Contains(8, 3));

  ASSERT_FALSE(b.Contains(9, 3));
  ASSERT_FALSE(b.Contains(std::make_pair(9, 3)));
  ASSERT_FALSE(b.Contains(2, 2));
  ASSERT_FALSE(b.Contains(std::make_pair(2, 2)));
}

TEST(Box, Box) {
  box::Box b = box::Box::GetBox(4, 4);

  auto n = b.Begin();

  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 3);
  ASSERT_EQ(n->second, 3);

  n = b.GetNext(*n);

  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 3);
  ASSERT_EQ(n->second, 4);

  n = b.GetNext(3, 4);

  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 3);
  ASSERT_EQ(n->second, 5);

  n = b.GetNext(3, 5);

  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 4);
  ASSERT_EQ(n->second, 3);

  n = b.GetNext(5, 5);
  ASSERT_FALSE(n);

  n = b.GetNext(0, 0);
  ASSERT_FALSE(n);

  ASSERT_TRUE(b.Contains(3, 3));
  ASSERT_TRUE(b.Contains(3, 4));
  ASSERT_TRUE(b.Contains(3, 5));
  ASSERT_TRUE(b.Contains(4, 3));
  ASSERT_TRUE(b.Contains(4, 4));
  ASSERT_TRUE(b.Contains(4, 5));
  ASSERT_TRUE(b.Contains(5, 3));
  ASSERT_TRUE(b.Contains(5, 4));
  ASSERT_TRUE(b.Contains(5, 5));

  ASSERT_FALSE(b.Contains(2, 2));
  ASSERT_FALSE(b.Contains(6, 6));
}

TEST(Box, Grid) {
  box::Box b = box::Box::GetGrid();

  auto n = b.Begin();

  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 0);
  ASSERT_EQ(n->second, 0);

  n = b.GetNext(*n);

  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 0);
  ASSERT_EQ(n->second, 1);

  n = b.GetNext(8, 8);
  ASSERT_FALSE(n);

  ASSERT_TRUE(b.Contains(0, 0));
  ASSERT_TRUE(b.Contains(3, 3));
  ASSERT_TRUE(b.Contains(8, 8));

  ASSERT_FALSE(b.Contains(9, 2));
}

TEST(Box, GridAsRows) {
  std::vector<box::Box> b = box::Box::GetGridAsRows();

  ASSERT_EQ(b.size(), 9);

  auto n = b[0].Begin();
  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 0);
  ASSERT_EQ(n->second, 0);

  n = b[1].Begin();
  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 1);
  ASSERT_EQ(n->second, 0);

  n = b[8].Begin();
  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 8);
  ASSERT_EQ(n->second, 0);
}

TEST(Box, GridAsColumns) {
  std::vector<box::Box> b = box::Box::GetGridAsColumns();

  ASSERT_EQ(b.size(), 9);

  auto n = b[0].Begin();
  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 0);
  ASSERT_EQ(n->second, 0);

  n = b[1].Begin();
  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 0);
  ASSERT_EQ(n->second, 1);

  n = b[8].Begin();
  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 0);
  ASSERT_EQ(n->second, 8);
}

TEST(Box, GridAsBoxes) {
  std::vector<box::Box> b = box::Box::GetGridAsBoxes();

  ASSERT_EQ(b.size(), 9);

  auto n = b[0].Begin();
  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 0);
  ASSERT_EQ(n->second, 0);

  n = b[1].Begin();
  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 0);
  ASSERT_EQ(n->second, 3);

  n = b[8].Begin();
  ASSERT_TRUE(n);
  ASSERT_EQ(n->first, 6);
  ASSERT_EQ(n->second, 6);
}

state::State PrepareState() {
  state::State result;
  result.size = 9;
  result.current_iter = 1;
  result.changed_on_iter = 2;
  result.f =
      std::vector<std::vector<int>>(result.size, std::vector<int>(result.size));
  result.p = std::vector<std::vector<std::set<int>>>(
      result.size, std::vector<std::set<int>>(result.size));
  for (int i = 0; i < 9 * 9; i++) {
    result.f[i / 9][i % 9] = i;
    result.p[i / 9][i % 9].insert(i + 1);
  }
  return result;
}

TEST(Iterator, Row) {
  std::vector<std::pair<int, int>> expected = {
      {3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {3, 6}, {3, 7}, {3, 8}};
  state::State state = PrepareState();
  int i = 0;
  box::Box b = box::Box::GetRow(3);
  for (const auto& p_list : box::GetIterable<1>(state, b)) {
    const auto& p = p_list.front();
    ASSERT_EQ(p.i, expected[i].first);
    ASSERT_EQ(p.j, expected[i].second);
    ASSERT_EQ(p.val, p.i * 9 + p.j);
    ASSERT_EQ(p.pm.size(), 1);
    ASSERT_EQ(*p.pm.begin(), p.i * 9 + p.j + 1);
    i++;
  }
}

TEST(Iterator, Column) {
  std::vector<std::pair<int, int>> expected = {
      {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}};
  state::State state = PrepareState();
  int i = 0;
  box::Box b = box::Box::GetColumn(3);
  for (const auto& p_list : box::GetIterable<1>(state, b)) {
    const auto& p = p_list.front();
    ASSERT_EQ(p.i, expected[i].first);
    ASSERT_EQ(p.j, expected[i].second);
    ASSERT_EQ(p.val, p.i * 9 + p.j);
    ASSERT_EQ(p.pm.size(), 1);
    ASSERT_EQ(*p.pm.begin(), p.i * 9 + p.j + 1);
    i++;
  }
}

TEST(Iterator, BoxAndConst) {
  std::vector<std::pair<int, int>> expected = {
      {3, 3}, {3, 4}, {3, 5}, {4, 3}, {4, 4}, {4, 5}, {5, 3}, {5, 4}, {5, 5}};
  const state::State& state = PrepareState();
  int i = 0;
  box::Box b = box::Box::GetBox(4, 4);
  for (const auto& p_list : box::GetIterable<1>(state, b)) {
    const auto& p = p_list.front();
    ASSERT_EQ(p.i, expected[i].first);
    ASSERT_EQ(p.j, expected[i].second);
    ASSERT_EQ(p.val, p.i * 9 + p.j);
    ASSERT_EQ(p.pm.size(), 1);
    ASSERT_EQ(*p.pm.begin(), p.i * 9 + p.j + 1);
    i++;
  }
}

TEST(Iterator, Ref) {
  state::State state = PrepareState();
  int i = 0;
  box::Box b = box::Box::GetBox(4, 4);
  for (const auto& p_list : box::GetIterable<1>(state, b)) {
    if (i == 5) {
      const auto& p = p_list.front();
      p.val = 5;
      p.pm = std::set<int>({1, 2, 3, 4});
    }
    i++;
  }

  ASSERT_EQ(state.f[4][5], 5);
  ASSERT_EQ(state.p[4][5], std::set<int>({1, 2, 3, 4}));
}
