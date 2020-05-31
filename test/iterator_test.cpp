#include <gtest/gtest.h>

#include "box_iterator.hpp"

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
