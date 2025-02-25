#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <vector>

#include "psm/psm.hpp"

namespace psm_test::display_p3 {

MATCHER_P(IsNearVector, expected, "") {
  const auto& actual = arg;
  if (actual.size() != expected.size()) {
    *result_listener << "Vector sizes differ. Expected: " << expected.size()
                     << " Actual: " << actual.size();
    return false;
  }

  static constexpr int Tolerance = 1;
  for (size_t i = 0; i < actual.size(); ++i) {
    if (std::abs(static_cast<int>(actual[i]) - static_cast<int>(expected[i])) >
        Tolerance) {
      *result_listener << "Vectors differ at index " << i
                       << ". Expected: " << static_cast<int>(expected[i])
                       << " Actual: " << static_cast<int>(actual[i])
                       << " (tolerance: " << Tolerance << ")";
      return false;
    }
  }
  return true;
}

class DisplayP3Test : public ::testing::Test {
 protected:
  static constexpr int Tolerance = 1;

  // Common test vectors
  std::vector<unsigned char> red{255, 0, 0};
  std::vector<unsigned char> green{0, 255, 0};
  std::vector<unsigned char> blue{0, 0, 255};
  std::vector<unsigned char> black{0, 0, 0};
  std::vector<unsigned char> white{255, 255, 255};

  // Known Display P3 values
  std::vector<unsigned char> p3_red{233, 40, 54};
  std::vector<unsigned char> p3_green{115, 77, 251};
  std::vector<unsigned char> p3_blue{0, 0, 244};
  std::vector<unsigned char> p3_black{0, 0, 0};

  void SetUp() override {}
};

// Primary Colors Group
TEST_F(DisplayP3Test, HandlesPrimaryColors) {
  auto result = psm::Convert<psm::sRGB, psm::DisplayP3>(red);
  EXPECT_THAT(result, IsNearVector(p3_red));

  result = psm::Convert<psm::sRGB, psm::DisplayP3>(green);
  EXPECT_THAT(result, IsNearVector(p3_green));

  result = psm::Convert<psm::sRGB, psm::DisplayP3>(blue);
  EXPECT_THAT(result, IsNearVector(p3_blue));
}

// Black and White Group
TEST_F(DisplayP3Test, HandlesBlackAndWhite) {
  // Test pure black
  auto result = psm::Convert<psm::sRGB, psm::DisplayP3>(black);
  EXPECT_EQ(result, p3_black);

  // Test pure white
  result = psm::Convert<psm::sRGB, psm::DisplayP3>(white);
  EXPECT_THAT(result, IsNearVector(white));

  // Test near black
  const std::vector<unsigned char> near_black{1, 1, 1};
  result = psm::Convert<psm::sRGB, psm::DisplayP3>(near_black);
  EXPECT_GT(result[0], 0);
  EXPECT_GT(result[1], 0);
  EXPECT_GT(result[2], 0);

  // Test near white
  const std::vector<unsigned char> near_white{254, 254, 254};
  result = psm::Convert<psm::sRGB, psm::DisplayP3>(near_white);
  EXPECT_LT(result[0], 255);
  EXPECT_LT(result[1], 255);
  EXPECT_LT(result[2], 255);
}

// Gray Values Group
TEST_F(DisplayP3Test, HandlesGrayValues) {
  const std::vector<unsigned char> mid_gray{128, 128, 128};
  auto result = psm::Convert<psm::sRGB, psm::DisplayP3>(mid_gray);
  EXPECT_THAT(result, IsNearVector(std::vector<unsigned char>{128, 128, 128}));

  const std::vector<unsigned char> quarter_gray{64, 64, 64};
  result = psm::Convert<psm::sRGB, psm::DisplayP3>(quarter_gray);
  EXPECT_THAT(result, IsNearVector(std::vector<unsigned char>{64, 64, 64}));

  const std::vector<unsigned char> three_quarter_gray{192, 192, 192};
  result = psm::Convert<psm::sRGB, psm::DisplayP3>(three_quarter_gray);
  EXPECT_THAT(result, IsNearVector(std::vector<unsigned char>{192, 192, 192}));
}

// Round-trip fidelity
TEST_F(DisplayP3Test, RoundTripConversion) {
  const std::vector<unsigned char> original = {210, 92, 180};

  auto intermediate = psm::Convert<psm::sRGB, psm::DisplayP3>(original);
  auto result = psm::Convert<psm::DisplayP3, psm::sRGB>(intermediate);

  EXPECT_THAT(result, IsNearVector(original));
}

TEST_F(DisplayP3Test, ValidatesInputSize) {
  const std::vector<unsigned char> invalid_size = {255,
                                                   255};  // Only 2 components

  EXPECT_THROW((psm::Convert<psm::sRGB, psm::DisplayP3>(invalid_size)),
               std::invalid_argument);
}

TEST_F(DisplayP3Test, BulkConversionPerformance) {
  constexpr size_t num_pixels = 1000000;
  const std::vector<unsigned char> large_input(num_pixels * 3, 128);

  auto start = std::chrono::high_resolution_clock::now();
  auto large_output = psm::Convert<psm::sRGB, psm::DisplayP3>(large_input);
  auto end = std::chrono::high_resolution_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  EXPECT_LT(duration.count(), 1000);  // Should complete within 1 second
}

// Wide Color Gamut Tests
TEST_F(DisplayP3Test, HandlesWideGamutColors) {
  // Test colors that are outside sRGB gamut but within Display P3
  const std::vector<unsigned char> saturated_red{255, 0,
                                                 0};  // Pure red in sRGB
  auto result = psm::Convert<psm::sRGB, psm::DisplayP3>(saturated_red);
  // Display P3 should represent this red as more saturated
  EXPECT_GT(result[0], saturated_red[0] - Tolerance);
}

}  // namespace psm_test::display_p3