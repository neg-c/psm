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
  std::vector<unsigned char> red{255, 0, 0};
  std::vector<unsigned char> green{0, 255, 0};
  std::vector<unsigned char> blue{0, 0, 255};
  std::vector<unsigned char> black{0, 0, 0};
  std::vector<unsigned char> white{255, 255, 255};

  // Known Adobe RGB values
  std::vector<unsigned char> adobe_red{218, 0, 0};
  std::vector<unsigned char> adobe_green{144, 255, 60};
  std::vector<unsigned char> adobe_blue{0, 0, 250};
  std::vector<unsigned char> adobe_black{0, 0, 0};

  // Result buffer
  std::vector<unsigned char> result{0, 0, 0};

  void SetUp() override {}
};

// Primary Colors Group
TEST_F(AdobeRgbTest, HandlesPrimaryColors) {
  psm::Convert<psm::sRGB, psm::AdobeRGB>(red, result);
  EXPECT_THAT(result, IsNearVector(adobe_red));

  psm::Convert<psm::sRGB, psm::AdobeRGB>(green, result);
  EXPECT_THAT(result, IsNearVector(adobe_green));

  psm::Convert<psm::sRGB, psm::AdobeRGB>(blue, result);
  EXPECT_THAT(result, IsNearVector(adobe_blue));
}

// Black and White Group
TEST_F(AdobeRgbTest, HandlesBlackAndWhite) {
  // Test pure black
  psm::Convert<psm::sRGB, psm::AdobeRGB>(black, result);
  EXPECT_EQ(result, adobe_black);

  // Test near black
  const std::vector<unsigned char> near_black{1, 1, 1};
  psm::Convert<psm::sRGB, psm::AdobeRGB>(near_black, result);
  EXPECT_GT(result[0], 0);
  EXPECT_GT(result[1], 0);
  EXPECT_GT(result[2], 0);

  // Test near white
  const std::vector<unsigned char> near_white{254, 254, 254};
  psm::Convert<psm::sRGB, psm::AdobeRGB>(near_white, result);
  EXPECT_LT(result[0], 255);
  EXPECT_LT(result[1], 255);
  EXPECT_LT(result[2], 255);
}

// Gray Values Group
TEST_F(AdobeRgbTest, HandlesGrayValues) {
  const std::vector<unsigned char> mid_gray{128, 128, 128};
  psm::Convert<psm::sRGB, psm::AdobeRGB>(mid_gray, result);
  EXPECT_THAT(result, IsNearVector(std::vector<unsigned char>{127, 127, 127}));

  const std::vector<unsigned char> quarter_gray{64, 64, 64};
  psm::Convert<psm::sRGB, psm::AdobeRGB>(quarter_gray, result);
  EXPECT_THAT(result, IsNearVector(std::vector<unsigned char>{64, 64, 64}));

  const std::vector<unsigned char> three_quarter_gray{192, 192, 192};
  psm::Convert<psm::sRGB, psm::AdobeRGB>(three_quarter_gray, result);
  EXPECT_THAT(result, IsNearVector(std::vector<unsigned char>{191, 191, 191}));
}

// Round-trip fidelity
TEST_F(AdobeRgbTest, RoundTripConversion) {
  const std::vector<unsigned char> original = {210, 92, 180};
  std::vector<unsigned char> intermediate(3);
  std::vector<unsigned char> result(3);

  psm::Convert<psm::sRGB, psm::AdobeRGB>(original, intermediate);
  psm::Convert<psm::AdobeRGB, psm::sRGB>(intermediate, result);

  EXPECT_THAT(result, IsNearVector(original));
}

TEST_F(AdobeRgbTest, ValidatesInputSize) {
  const std::vector<unsigned char> invalid_size = {255, 255};  // Only 2 components
  std::vector<unsigned char> result(3);

  EXPECT_THROW((psm::Convert<psm::sRGB, psm::AdobeRGB>(invalid_size, result)),
               std::invalid_argument);
}

TEST_F(AdobeRgbTest, ValidatesOutputBuffer) {
  const std::vector<unsigned char> input = {255, 255, 255};
  std::vector<unsigned char> small_output(2);

  EXPECT_THROW((psm::Convert<psm::sRGB, psm::AdobeRGB>(input, small_output)),
               std::invalid_argument);
}

TEST_F(AdobeRgbTest, BulkConversionPerformance) {
  constexpr size_t num_pixels = 1000000;
  const std::vector<unsigned char> large_input(num_pixels * 3, 128);
  std::vector<unsigned char> large_output(num_pixels * 3);

  auto start = std::chrono::high_resolution_clock::now();
  psm::Convert<psm::sRGB, psm::AdobeRGB>(large_input, large_output);
  auto end = std::chrono::high_resolution_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  EXPECT_LT(duration.count(), 1000);  // Should complete within 1 second
}

}  // namespace psm_test::adobe_rgb
