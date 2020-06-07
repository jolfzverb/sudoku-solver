#include <gtest/gtest.h>

#include "box_iterator.hpp"

TEST(Pair, One) {
  const std::vector<int> input{1, 2, 3, 4, 5};
  box::PairIterable<std::vector<int>::const_iterator, 1> t(input.begin(),
                                                           input.end());
  int i = 1;
  for (const auto& it : t) {
    ASSERT_EQ(it.size(), 1);
    ASSERT_EQ(*it.front(), i);
    i++;
  }
}

TEST(Pair, Two) {
  const std::vector<int> input{1, 2, 3, 4, 5};
  box::PairIterable<std::vector<int>::const_iterator, 2> t(input.begin(),
                                                           input.end());
  std::vector<std::vector<int>> expected{{1, 2}, {1, 3}, {1, 4}, {1, 5},
                                         {2, 3}, {2, 4}, {2, 5}, {3, 4},
                                         {3, 5}, {4, 5}};
  auto it = t.begin();
  for (size_t i = 0; i < expected.size(); i++) {
    ASSERT_EQ(*((*it)[0]), expected[i][0]);
    ASSERT_EQ(*((*it)[1]), expected[i][1]);
    it++;
  }
  ASSERT_EQ(it, t.end());
}

TEST(Pair, Tree) {
  const std::vector<int> input{1, 2, 3, 4, 5};
  box::PairIterable<std::vector<int>::const_iterator, 3> t(input.begin(),
                                                           input.end());
  std::vector<std::vector<int>> expected{
      {1, 2, 3}, {1, 2, 4}, {1, 2, 5}, {1, 3, 4}, {1, 3, 5},
      {1, 4, 5}, {2, 3, 4}, {2, 3, 5}, {2, 4, 5}, {3, 4, 5}};
  auto it = t.begin();
  for (size_t i = 0; i < expected.size(); i++) {
    ASSERT_EQ(*((*it)[0]), expected[i][0]);
    ASSERT_EQ(*((*it)[1]), expected[i][1]);
    ASSERT_EQ(*((*it)[2]), expected[i][2]);
    it++;
  }
  ASSERT_EQ(it, t.end());
}

TEST(Pair, Four) {
  const std::vector<int> input{1, 2, 3, 4, 5};
  box::PairIterable<std::vector<int>::const_iterator, 4> t(input.begin(),
                                                           input.end());
  std::vector<std::vector<int>> expected{
      {1, 2, 3, 4}, {1, 2, 3, 5}, {1, 2, 4, 5}, {1, 3, 4, 5}, {2, 3, 4, 5},
  };
  auto it = t.begin();
  for (size_t i = 0; i < expected.size(); i++) {
    ASSERT_EQ(*((*it)[0]), expected[i][0]);
    ASSERT_EQ(*((*it)[1]), expected[i][1]);
    ASSERT_EQ(*((*it)[2]), expected[i][2]);
    ASSERT_EQ(*((*it)[3]), expected[i][3]);
    it++;
  }
  ASSERT_EQ(it, t.end());
}

TEST(Pair, Five) {
  const std::vector<int> input{1, 2, 3, 4, 5};
  box::PairIterable<std::vector<int>::const_iterator, 5> t(input.begin(),
                                                           input.end());
  std::vector<std::vector<int>> expected{
      {1, 2, 3, 4, 5},
  };
  auto it = t.begin();
  for (size_t i = 0; i < expected.size(); i++) {
    ASSERT_EQ(*((*it)[0]), expected[i][0]);
    ASSERT_EQ(*((*it)[1]), expected[i][1]);
    ASSERT_EQ(*((*it)[2]), expected[i][2]);
    ASSERT_EQ(*((*it)[3]), expected[i][3]);
    ASSERT_EQ(*((*it)[4]), expected[i][4]);
    it++;
  }
  ASSERT_EQ(it, t.end());
}
