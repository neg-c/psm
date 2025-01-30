#include <gtest/gtest.h>

#include <chrono>
#include <vector>

#include "psm/psm.hpp"

namespace psm_test::adobe_rgb {

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

  // Result buffer
  std::vector<unsigned char> result{0, 0, 0};

  void SetUp() override {}
};

// Primary Colors Group
TEST_F(AdobeRgbTest, HandlesPrimaryColors) {
  psm::Convert<psm::sRGB, psm::AdobeRGB>(red, result);
  EXPECT_NEAR(result[0], adobe_red[0], Tolerance);
  EXPECT_NEAR(result[1], adobe_red[1], Tolerance);
  EXPECT_NEAR(result[2], adobe_red[2], Tolerance);

  psm::Convert<psm::sRGB, psm::AdobeRGB>(green, result);
  EXPECT_NEAR(result[0], adobe_green[0], Tolerance);
  EXPECT_NEAR(result[1], adobe_green[1], Tolerance);
  EXPECT_NEAR(result[2], adobe_green[2], Tolerance);

  psm::Convert<psm::sRGB, psm::AdobeRGB>(blue, result);
  EXPECT_NEAR(result[0], adobe_blue[0], Tolerance);
  EXPECT_NEAR(result[1], adobe_blue[1], Tolerance);
  EXPECT_NEAR(result[2], adobe_blue[2], Tolerance);
}

// Black and White Group
TEST_F(AdobeRgbTest, HandlesBlackAndWhite) {
  // Test pure black
  psm::Convert<psm::sRGB, psm::AdobeRGB>(black, result);
  EXPECT_EQ(result, adobe_black);

  // Test near black
  std::vector<unsigned char> near_black{1, 1, 1};
  psm::Convert<psm::sRGB, psm::AdobeRGB>(near_black, result);
  EXPECT_GT(result[0], 0);
  EXPECT_GT(result[1], 0);
  EXPECT_GT(result[2], 0);

  // Test near white
  std::vector<unsigned char> near_white{254, 254, 254};
  psm::Convert<psm::sRGB, psm::AdobeRGB>(near_white, result);
  EXPECT_LT(result[0], 255);
  EXPECT_LT(result[1], 255);
  EXPECT_LT(result[2], 255);
}

// Gray Values Group
TEST_F(AdobeRgbTest, HandlesGrayValues) {
  // Test mid-gray
  std::vector<unsigned char> mid_gray{128, 128, 128};
  psm::Convert<psm::sRGB, psm::AdobeRGB>(mid_gray, result);
  const int expected_mid =
      127;  // Expected linearized value (Adobe RGB gamma 2.2)
  EXPECT_NEAR(result[0], expected_mid, Tolerance);
  EXPECT_NEAR(result[1], expected_mid, Tolerance);
  EXPECT_NEAR(result[2], expected_mid, Tolerance);

  // Test quarter gray
  std::vector<unsigned char> quarter_gray{64, 64, 64};
  psm::Convert<psm::sRGB, psm::AdobeRGB>(quarter_gray, result);
  const int expected_quarter = 64;
  EXPECT_NEAR(result[0], expected_quarter, Tolerance);
  EXPECT_NEAR(result[1], expected_quarter, Tolerance);
  EXPECT_NEAR(result[2], expected_quarter, Tolerance);

  // Test three-quarter gray
  std::vector<unsigned char> three_quarter_gray{192, 192, 192};
  psm::Convert<psm::sRGB, psm::AdobeRGB>(three_quarter_gray, result);
  const int expected_three_quarter = 191;
  EXPECT_NEAR(result[0], expected_three_quarter, Tolerance);
  EXPECT_NEAR(result[1], expected_three_quarter, Tolerance);
  EXPECT_NEAR(result[2], expected_three_quarter, Tolerance);
}

// Round-trip fidelity
TEST_F(AdobeRgbTest, RoundTripConversion) {
  std::vector<unsigned char> original = {180, 92, 210};
  std::vector<unsigned char> intermediate(3);
  std::vector<unsigned char> result(3);

  psm::Convert<psm::sRGB, psm::AdobeRGB>(original, intermediate);
  psm::Convert<psm::AdobeRGB, psm::sRGB>(intermediate, result);

  for (size_t i = 0; i < original.size(); ++i) {
    EXPECT_NEAR(original[i], result[i], Tolerance);
  }
}

// Edge case tests
TEST_F(AdobeRgbTest, HandlesValueOverflow) {
  // Max valid value for unsigned char
  std::vector<unsigned char> input{0, 0, 255};
  std::vector<unsigned char> result(3);

  EXPECT_THROW((psm::Convert<psm::sRGB, psm::AdobeRGB>(input, result)),
               std::invalid_argument);
}

TEST_F(AdobeRgbTest, HandlesAllPrimaryColors) {
  std::vector<unsigned char> red{0, 0, 255};
  std::vector<unsigned char> green{0, 255, 0};
  std::vector<unsigned char> blue{255, 0, 0};
  std::vector<unsigned char> black{0, 0, 0};
  std::vector<unsigned char> result(3);

  // Known Adobe RGB values
  std::vector<unsigned char> adobe_red{0, 0, 218};
  std::vector<unsigned char> adobe_green{0, 235, 0};
  std::vector<unsigned char> adobe_blue{237, 0, 0};
  std::vector<unsigned char> adobe_black{0, 0, 0};

  // Test black
  psm::Convert<psm::sRGB, psm::AdobeRGB>(black, result);
  EXPECT_EQ(result, adobe_black);

  // Test primary colors
  psm::Convert<psm::sRGB, psm::AdobeRGB>(red, result);
  EXPECT_NEAR(result[0], adobe_red[0], Tolerance);
  EXPECT_NEAR(result[1], adobe_red[1], Tolerance);
  EXPECT_NEAR(result[2], adobe_red[2], Tolerance);

  psm::Convert<psm::sRGB, psm::AdobeRGB>(green, result);
  EXPECT_NEAR(result[0], adobe_green[0], Tolerance);
  EXPECT_NEAR(result[1], adobe_green[1], Tolerance);
  EXPECT_NEAR(result[2], adobe_green[2], Tolerance);

  psm::Convert<psm::sRGB, psm::AdobeRGB>(blue, result);
  EXPECT_NEAR(result[0], adobe_blue[0], Tolerance);
  EXPECT_NEAR(result[1], adobe_blue[1], Tolerance);
  EXPECT_NEAR(result[2], adobe_blue[2], Tolerance);
}

TEST_F(AdobeRgbTest, ValidatesInputSize) {
  std::vector<unsigned char> invalid_size = {255, 255};  // Only 2 components
  std::vector<unsigned char> result(3);

  EXPECT_THROW((psm::Convert<psm::sRGB, psm::AdobeRGB>(invalid_size, result)),
               std::invalid_argument);
}

TEST_F(AdobeRgbTest, ValidatesOutputBuffer) {
  std::vector<unsigned char> input = {255, 255, 255};
  std::vector<unsigned char> small_output(2);

  EXPECT_THROW((psm::Convert<psm::sRGB, psm::AdobeRGB>(input, small_output)),
               std::invalid_argument);
}

TEST_F(AdobeRgbTest, BulkConversionPerformance) {
  constexpr size_t num_pixels = 1000000;
  std::vector<unsigned char> large_input(num_pixels * 3, 128);
  std::vector<unsigned char> large_output(num_pixels * 3);

  auto start = std::chrono::high_resolution_clock::now();
  psm::Convert<psm::sRGB, psm::AdobeRGB>(large_input, large_output);
  auto end = std::chrono::high_resolution_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  EXPECT_LT(duration.count(), 1000);  // Should complete within 1 second
}

}  // namespace psm_test::adobe_rgb
