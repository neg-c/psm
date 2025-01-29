#include "psm/percent.hpp"

#include <gtest/gtest.h>

TEST(PercentTest, ConstructionAndAccessors) {
  const psm::Percent p(10, 20, 30);
  EXPECT_EQ(p.channel(0), 10);
  EXPECT_EQ(p.channel(1), 20);
  EXPECT_EQ(p.channel(2), 30);
}

TEST(PercentTest, NeutralCreation) {
  const auto neutral = psm::Percent::neutral();
  EXPECT_TRUE(neutral.isNeutral());
  EXPECT_EQ(neutral.channel(0), 0);
  EXPECT_EQ(neutral.channel(1), 0);
  EXPECT_EQ(neutral.channel(2), 0);
}

TEST(PercentTest, UniformCreation) {
  const auto uniform = psm::Percent::uniform(15);
  EXPECT_TRUE(uniform.isUniform());
  EXPECT_EQ(uniform.channel(0), 15);
  EXPECT_EQ(uniform.channel(1), 15);
  EXPECT_EQ(uniform.channel(2), 15);
}

TEST(PercentTest, ComparisonOperators) {
  EXPECT_EQ(psm::Percent(10, 20, 30), psm::Percent(10, 20, 30));
  EXPECT_NE(psm::Percent(10, 20, 30), psm::Percent(10, 20, 31));
  EXPECT_NE(psm::Percent(0, 0, 0), psm::Percent(1, 1, 1));
}

TEST(PercentTest, ArithmeticOperations) {
  psm::Percent p(5, 10, 15);

  p += 5;
  EXPECT_EQ(p.channel(0), 10);
  EXPECT_EQ(p.channel(1), 15);
  EXPECT_EQ(p.channel(2), 20);

  p -= 3;
  EXPECT_EQ(p.channel(0), 7);
  EXPECT_EQ(p.channel(1), 12);
  EXPECT_EQ(p.channel(2), 17);

  auto p2 = p + 10;
  EXPECT_EQ(p2.channel(0), 17);
  EXPECT_EQ(p2.channel(1), 22);
  EXPECT_EQ(p2.channel(2), 27);

  auto p3 = p2 - 5;
  EXPECT_EQ(p3.channel(0), 12);
  EXPECT_EQ(p3.channel(1), 17);
  EXPECT_EQ(p3.channel(2), 22);
}

TEST(PercentTest, BoundaryChecks) {
  EXPECT_TRUE(psm::Percent(0, 0, 0).isNeutral());
  EXPECT_FALSE(psm::Percent(1, 0, 0).isNeutral());

  EXPECT_TRUE(psm::Percent(5, 5, 5).isUniform());
  EXPECT_FALSE(psm::Percent(5, 5, 6).isUniform());
}

TEST(PercentTest, OutOfBoundsAccess) {
  const psm::Percent p(10, 20, 30);
  EXPECT_THROW(p.channel(3), std::out_of_range);
  EXPECT_THROW(p.channel(100), std::out_of_range);
}

TEST(PercentTest, ExtremeValues) {
  constexpr int max = std::numeric_limits<int>::max();
  constexpr int min = std::numeric_limits<int>::min();

  const psm::Percent p1(max, max, max);
  EXPECT_EQ(p1 + 0, psm::Percent(max, max, max));

  const psm::Percent p2(min, min, min);
  EXPECT_EQ(p2 - 0, psm::Percent(min, min, min));
}

TEST(PercentTest, NegativeValues) {
  const psm::Percent p(-10, -20, -30);
  EXPECT_EQ(p.channel(0), -10);
  EXPECT_EQ(p.channel(1), -20);
  EXPECT_EQ(p.channel(2), -30);

  EXPECT_EQ(p + 15, psm::Percent(5, -5, -15));
  EXPECT_EQ(p - 10, psm::Percent(-20, -30, -40));
}

TEST(PercentTest, OperatorChaining) {
  auto result = (psm::Percent::uniform(10) + 5) - 3;
  EXPECT_TRUE(result.isUniform());
  EXPECT_EQ(result.channel(0), 12);
}
