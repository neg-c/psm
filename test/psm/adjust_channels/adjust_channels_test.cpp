#include "psm/adjust_channels.hpp"

#include <gtest/gtest.h>

#include <vector>

namespace {

class AdjustChannelsTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  static void ValidatePixel(const std::vector<unsigned char>& buffer,
                            size_t pixel_index, unsigned char expected_r,
                            unsigned char expected_g,
                            unsigned char expected_b) {
    ASSERT_LT((pixel_index * 3) + 2, buffer.size());
    EXPECT_EQ(buffer[pixel_index * 3], expected_r);
    EXPECT_EQ(buffer[(pixel_index * 3) + 1], expected_g);
    EXPECT_EQ(buffer[(pixel_index * 3) + 2], expected_b);
  }
};

TEST_F(AdjustChannelsTest, MixedPercentages) {
  std::vector<unsigned char> buffer = {100, 150, 200};
  const psm::Percent adjust{50, -25, 10};

  psm::AdjustChannels(buffer, adjust);
  // -25% of 150 = 112.5, rounds down to 112
  ValidatePixel(buffer, 0, 150, 112, 220);
}

TEST_F(AdjustChannelsTest, MultiplePixels) {
  std::vector<unsigned char> buffer = {100, 150, 200, 50, 100, 150};
  const psm::Percent adjust{50, -25, 10};

  psm::AdjustChannels(buffer, adjust);

  ValidatePixel(buffer, 0, 150, 112, 220);
  ValidatePixel(buffer, 1, 75, 75, 165);
}

TEST_F(AdjustChannelsTest, NoAdjustment) {
  std::vector<unsigned char> buffer = {100, 150, 200};
  const psm::Percent adjust{0, 0, 0};

  psm::AdjustChannels(buffer, adjust);

  ValidatePixel(buffer, 0, 100, 150, 200);
}

TEST_F(AdjustChannelsTest, ClampingMaxValues) {
  std::vector<unsigned char> buffer = {200, 220, 240};
  const psm::Percent adjust{50, 50, 50};

  psm::AdjustChannels(buffer, adjust);

  ValidatePixel(buffer, 0, 255, 255, 255);
}

TEST_F(AdjustChannelsTest, ClampingMinValues) {
  std::vector<unsigned char> buffer = {20, 30, 40};
  const psm::Percent adjust{-150, -150, -150};

  psm::AdjustChannels(buffer, adjust);

  ValidatePixel(buffer, 0, 0, 0, 0);
}

TEST_F(AdjustChannelsTest, PartialAdjustments) {
  std::vector<unsigned char> buffer = {100, 150, 200};
  const psm::Percent adjust{50, 0, -25};

  psm::AdjustChannels(buffer, adjust);

  ValidatePixel(buffer, 0, 150, 150, 150);
}

TEST_F(AdjustChannelsTest, EmptyBuffer) {
  std::vector<unsigned char> buffer;
  const psm::Percent adjust{50, -25, 10};

  // Should not crash
  psm::AdjustChannels(buffer, adjust);

  EXPECT_TRUE(buffer.empty());
}

TEST_F(AdjustChannelsTest, LargeBuffer) {
  const size_t num_pixels = 1'000'000;
  std::vector<unsigned char> buffer(num_pixels * 3, 100);
  const psm::Percent adjust{25, 25, 25};

  psm::AdjustChannels(buffer, adjust);

  const size_t first_pixel = 0;
  const size_t last_pixel = num_pixels - 1;
  ValidatePixel(buffer, first_pixel, 125, 125, 125);
  ValidatePixel(buffer, last_pixel, 125, 125, 125);
}

TEST_F(AdjustChannelsTest, ExtremeAdjustmentValues) {
  std::vector<unsigned char> buffer = {100, 150, 200};
  const psm::Percent adjust{1000, -1000, 500};

  psm::AdjustChannels(buffer, adjust);

  // Values should be clamped between 0 and 255
  ValidatePixel(buffer, 0, 255, 0, 255);
}

TEST_F(AdjustChannelsTest, BoundaryValues) {
  std::vector<unsigned char> near_zero = {1, 1, 1};
  const psm::Percent small_adjust{-10, -10, -10};
  psm::AdjustChannels(near_zero, small_adjust);
  ValidatePixel(near_zero, 0, 0, 0, 0);

  std::vector<unsigned char> near_max = {254, 254, 254};
  const psm::Percent small_positive{10, 10, 10};
  psm::AdjustChannels(near_max, small_positive);
  ValidatePixel(near_max, 0, 255, 255, 255);
}

TEST_F(AdjustChannelsTest, NearMax) {
  std::vector<unsigned char> buffer = {254, 254, 254};
  const psm::Percent extreme{1000000, -1000000, 1000000};
  psm::AdjustChannels(buffer, extreme);
  ValidatePixel(buffer, 0, 255, 0, 255);
}

TEST_F(AdjustChannelsTest, MultipleSuccessiveAdjustments) {
  std::vector<unsigned char> buffer = {100, 100, 100};

  psm::AdjustChannels(buffer, psm::Percent{50, 50, 50});
  ValidatePixel(buffer, 0, 150, 150, 150);

  psm::AdjustChannels(buffer, psm::Percent{-33, -33, -33});
  ValidatePixel(buffer, 0, 100, 100, 100);
}

TEST_F(AdjustChannelsTest, SixteenBit_MixedPercentages) {
  std::vector<std::uint16_t> buffer = {10000, 30000, 50000};
  const psm::Percent adjust{50, -25, 10};

  psm::AdjustChannels(buffer, adjust);

  // channel0: 10000 * 1.5 = 15000
  // channel1: 30000 * 0.75 = 22500
  // channel2: 50000 * 1.1 = 55000
  EXPECT_EQ(buffer[0], static_cast<std::uint16_t>(15000));
  EXPECT_EQ(buffer[1], static_cast<std::uint16_t>(22500));
  EXPECT_EQ(buffer[2], static_cast<std::uint16_t>(55000));
}

TEST_F(AdjustChannelsTest, SixteenBit_Clamping) {
  std::vector<std::uint16_t> buffer = {65000, 10, 32000};
  const psm::Percent adjust{10, -200, 0};

  psm::AdjustChannels(buffer, adjust);

  // clamp above 65535 and below 0
  EXPECT_EQ(buffer[0], static_cast<std::uint16_t>(65535));
  EXPECT_EQ(buffer[1], static_cast<std::uint16_t>(0));
  EXPECT_EQ(buffer[2], static_cast<std::uint16_t>(32000));
}

TEST_F(AdjustChannelsTest, OverflowProtection) {
  std::vector<unsigned char> buffer = {200, 100, 100};
  psm::AdjustChannels(buffer, psm::Percent{100, 0, 0});
  ValidatePixel(buffer, 0, 255, 100, 100);

  buffer = {100, 200, 100};
  psm::AdjustChannels(buffer, psm::Percent{0, 100, 0});
  ValidatePixel(buffer, 0, 100, 255, 100);

  buffer = {100, 100, 200};
  psm::AdjustChannels(buffer, psm::Percent{0, 0, 100});
  ValidatePixel(buffer, 0, 100, 100, 255);
}

TEST_F(AdjustChannelsTest, GradualLimitApproach) {
  std::vector<unsigned char> buffer = {250, 250, 250};

  psm::AdjustChannels(buffer, psm::Percent{1, 1, 1});
  ValidatePixel(buffer, 0, 252, 252, 252);

  psm::AdjustChannels(buffer, psm::Percent{1, 1, 1});
  ValidatePixel(buffer, 0, 254, 254, 254);

  psm::AdjustChannels(buffer, psm::Percent{1, 1, 1});
  ValidatePixel(buffer, 0, 255, 255, 255);  // Should cap at 255
}

}  // namespace
