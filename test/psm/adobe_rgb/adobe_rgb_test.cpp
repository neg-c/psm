#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <vector>

#include "psm/psm.hpp"

namespace psm_test::adobe_rgb {

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

class AdobeRgbTest : public ::testing::Test {
 protected:
  static constexpr int Tolerance = 1;

  // Common test vectors
  std::vector<unsigned char> red{0, 0, 255};
  std::vector<unsigned char> green{0, 255, 0};
  std::vector<unsigned char> blue{255, 0, 0};
  std::vector<unsigned char> black{0, 0, 0};
  std::vector<unsigned char> white{255, 255, 255};

  // Known Adobe RGB values
  std::vector<unsigned char> adobe_red{0, 0, 218};
  std::vector<unsigned char> adobe_green{60, 255, 144};
  std::vector<unsigned char> adobe_blue{250, 0, 0};
  std::vector<unsigned char> adobe_black{0, 0, 0};

  void SetUp() override {}
};

// Primary Colors Group
TEST_F(AdobeRgbTest, HandlesPrimaryColors) {
  auto result = psm::Convert<psm::sRGB, psm::AdobeRGB>(red);
  EXPECT_THAT(result, IsNearVector(adobe_red));

  result = psm::Convert<psm::sRGB, psm::AdobeRGB>(green);
  EXPECT_THAT(result, IsNearVector(adobe_green));

  result = psm::Convert<psm::sRGB, psm::AdobeRGB>(blue);
  EXPECT_THAT(result, IsNearVector(adobe_blue));
}

// Black and White Group
TEST_F(AdobeRgbTest, HandlesBlackAndWhite) {
  // Test pure black
  auto result = psm::Convert<psm::sRGB, psm::AdobeRGB>(black);
  EXPECT_EQ(result, adobe_black);

  // Test near black
  const std::vector<unsigned char> near_black{1, 1, 1};
  result = psm::Convert<psm::sRGB, psm::AdobeRGB>(near_black);
  EXPECT_GT(result[0], 0);
  EXPECT_GT(result[1], 0);
  EXPECT_GT(result[2], 0);

  // Test near white
  const std::vector<unsigned char> near_white{254, 254, 254};
  result = psm::Convert<psm::sRGB, psm::AdobeRGB>(near_white);
  EXPECT_LT(result[0], 255);
  EXPECT_LT(result[1], 255);
  EXPECT_LT(result[2], 255);
}

// Gray Values Group
TEST_F(AdobeRgbTest, HandlesGrayValues) {
  const std::vector<unsigned char> mid_gray{128, 128, 128};
  auto result = psm::Convert<psm::sRGB, psm::AdobeRGB>(mid_gray);
  EXPECT_THAT(result, IsNearVector(std::vector<unsigned char>{127, 127, 127}));

  const std::vector<unsigned char> quarter_gray{64, 64, 64};
  result = psm::Convert<psm::sRGB, psm::AdobeRGB>(quarter_gray);
  EXPECT_THAT(result, IsNearVector(std::vector<unsigned char>{64, 64, 64}));

  const std::vector<unsigned char> three_quarter_gray{192, 192, 192};
  result = psm::Convert<psm::sRGB, psm::AdobeRGB>(three_quarter_gray);
  EXPECT_THAT(result, IsNearVector(std::vector<unsigned char>{191, 191, 191}));
}

// Round-trip fidelity
TEST_F(AdobeRgbTest, RoundTripConversion) {
  const std::vector<unsigned char> original = {180, 92, 210};

  auto intermediate = psm::Convert<psm::sRGB, psm::AdobeRGB>(original);
  auto result = psm::Convert<psm::AdobeRGB, psm::sRGB>(intermediate);

  EXPECT_THAT(result, IsNearVector(original));
}

TEST_F(AdobeRgbTest, ValidatesInputSize) {
  const std::vector<unsigned char> invalid_size = {255,
                                                   255};  // Only 2 components

  EXPECT_THROW((psm::Convert<psm::sRGB, psm::AdobeRGB>(invalid_size)),
               std::invalid_argument);
}

TEST_F(AdobeRgbTest, BulkConversionPerformance) {
  constexpr size_t num_pixels = 1000000;
  const std::vector<unsigned char> large_input(num_pixels * 3, 128);

  auto start = std::chrono::high_resolution_clock::now();
  auto large_output = psm::Convert<psm::sRGB, psm::AdobeRGB>(large_input);
  auto end = std::chrono::high_resolution_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  EXPECT_LT(duration.count(), 1000);  // Should complete within 1 second
}

}  // namespace psm_test::adobe_rgb
