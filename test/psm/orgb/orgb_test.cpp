#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <vector>

#include "psm/psm.hpp"
#include "test_utils.hpp"

namespace psm_test::orgb {

class OrgbTest : public ::testing::Test {
 protected:
  static constexpr int Tolerance = 1;

  // Common test vectors
  std::vector<unsigned char> red{255, 0, 0};
  std::vector<unsigned char> green{0, 255, 0};
  std::vector<unsigned char> blue{0, 0, 255};
  std::vector<unsigned char> black{0, 0, 0};
  std::vector<unsigned char> white{255, 255, 255};
  std::vector<unsigned char> gray{128, 128, 128};

  // Result buffer
  std::vector<unsigned char> result{0, 0, 0};
  std::vector<unsigned char> round_trip{0, 0, 0};

  void SetUp() override {}

  // Helper to check if opponent channels are near zero
  bool hasNeutralOpponentChannels(const std::vector<unsigned char>& orgb_color,
                                  int tolerance = 5) {
    // In oRGB, channels 2 and 3 are opponent channels and should be near
    // neutral (128) for gray values
    return std::abs(static_cast<int>(orgb_color[1]) - 128) <= tolerance &&
           std::abs(static_cast<int>(orgb_color[2]) - 128) <= tolerance;
  }
};

// Neutral Colors Tests
TEST_F(OrgbTest, HandlesNeutralColors) {
  // Test white
  psm::Convert<psm::sRGB, psm::oRGB>(white, result);
  EXPECT_TRUE(hasNeutralOpponentChannels(result));

  // Test black
  psm::Convert<psm::sRGB, psm::oRGB>(black, result);
  EXPECT_TRUE(hasNeutralOpponentChannels(result));

  // Test mid-gray
  psm::Convert<psm::sRGB, psm::oRGB>(gray, result);
  EXPECT_TRUE(hasNeutralOpponentChannels(result));
}

// Primary Colors Tests
TEST_F(OrgbTest, HandlesPrimaryColors) {
  // Test red - should have strong red-green opponent signal
  psm::Convert<psm::sRGB, psm::oRGB>(red, result);
  EXPECT_GT(result[1], 128);  // Expect positive red-green signal

  // Test green - should have opposite red-green opponent signal
  psm::Convert<psm::sRGB, psm::oRGB>(green, result);
  EXPECT_LT(result[1], 128);  // Expect negative red-green signal

  // Test blue - should affect yellow-blue channel
  psm::Convert<psm::sRGB, psm::oRGB>(blue, result);
  EXPECT_NE(result[2], 128);  // Expect non-neutral yellow-blue signal
}

// Round-trip Tests
TEST_F(OrgbTest, RoundTripConversion) {
  // Test with primary colors
  for (const auto& color : {red, green, blue}) {
    psm::Convert<psm::sRGB, psm::oRGB>(color, result);
    psm::Convert<psm::oRGB, psm::sRGB>(result, round_trip);
    EXPECT_THAT(round_trip, IsNearVector(color, Tolerance));
  }

  // Test with neutral colors
  for (const auto& color : {white, black, gray}) {
    psm::Convert<psm::sRGB, psm::oRGB>(color, result);
    EXPECT_THAT(round_trip, IsNearVector(color, Tolerance));
  }
}

// Gradient preservation test
TEST_F(OrgbTest, PreservesGradients) {
  std::vector<unsigned char> light_gray{192, 192, 192};
  std::vector<unsigned char> dark_gray{64, 64, 64};

  psm::Convert<psm::sRGB, psm::oRGB>(light_gray, result);
  float light_luminance = result[0];

  psm::Convert<psm::sRGB, psm::oRGB>(dark_gray, result);
  float dark_luminance = result[0];

  EXPECT_GT(light_luminance, dark_luminance);
}

// Input validation tests
TEST_F(OrgbTest, ValidatesInputSize) {
  std::vector<unsigned char> invalid_size = {255, 255};  // Only 2 components
  EXPECT_THROW((psm::Convert<psm::sRGB, psm::oRGB>(invalid_size, result)),
               std::invalid_argument);
}

TEST_F(OrgbTest, ValidatesOutputBuffer) {
  std::vector<unsigned char> small_output(2);
  EXPECT_THROW((psm::Convert<psm::sRGB, psm::oRGB>(red, small_output)),
               std::invalid_argument);
}

// Performance test
TEST_F(OrgbTest, BulkConversionPerformance) {
  constexpr size_t num_pixels = 1000000;
  std::vector<unsigned char> large_input(num_pixels * 3, 128);
  std::vector<unsigned char> large_output(num_pixels * 3);

  auto start = std::chrono::high_resolution_clock::now();
  psm::Convert<psm::sRGB, psm::oRGB>(large_input, large_output);
  auto end = std::chrono::high_resolution_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  EXPECT_LT(duration.count(), 1000);  // Should complete within 1 second
}

// Edge case tests
TEST_F(OrgbTest, HandlesExtremeValues) {
  std::vector<unsigned char> max_color{255, 255, 255};
  std::vector<unsigned char> min_color{0, 0, 0};

  // Test maximum values
  psm::Convert<psm::sRGB, psm::oRGB>(max_color, result);
  psm::Convert<psm::oRGB, psm::sRGB>(result, round_trip);
  EXPECT_THAT(round_trip, IsNearVector(max_color, Tolerance));

  // Test minimum values
  psm::Convert<psm::sRGB, psm::oRGB>(min_color, result);
  psm::Convert<psm::oRGB, psm::sRGB>(result, round_trip);
  EXPECT_THAT(round_trip, IsNearVector(min_color, Tolerance));
}

}  // namespace psm_test::orgb
