#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <vector>

#include "psm/psm.hpp"
#include "test_utils.hpp"

namespace psm_test::display_p3 {

class DisplayP3Test : public ::testing::Test {
 protected:
  static constexpr int Tolerance = 1;

  std::vector<unsigned char> red{255, 0, 0};
  std::vector<unsigned char> green{0, 255, 0};
  std::vector<unsigned char> blue{0, 0, 255};
  std::vector<unsigned char> black{0, 0, 0};
  std::vector<unsigned char> white{255, 255, 255};

  std::vector<unsigned char> p3_red{233, 50, 35};
  std::vector<unsigned char> p3_green{115, 251, 77};
  std::vector<unsigned char> p3_blue{0, 0, 244};
  std::vector<unsigned char> p3_black{0, 0, 0};

  // Result buffer
  std::vector<unsigned char> result{0, 0, 0};
};

TEST_F(DisplayP3Test, HandlesPrimaryColors) {
  psm::Convert<psm::sRGB, psm::DisplayP3>(red, result);
  EXPECT_THAT(result, IsNearVector(p3_red, Tolerance));

  psm::Convert<psm::sRGB, psm::DisplayP3>(green, result);
  EXPECT_THAT(result, IsNearVector(p3_green, Tolerance));

  psm::Convert<psm::sRGB, psm::DisplayP3>(blue, result);
  EXPECT_THAT(result, IsNearVector(p3_blue, Tolerance));
}

TEST_F(DisplayP3Test, HandlesBlackAndWhite) {
  psm::Convert<psm::sRGB, psm::DisplayP3>(black, result);
  EXPECT_EQ(result, p3_black);

  psm::Convert<psm::sRGB, psm::DisplayP3>(white, result);
  EXPECT_THAT(result, IsNearVector(white, Tolerance));

  const std::vector<unsigned char> near_black{1, 1, 1};
  psm::Convert<psm::sRGB, psm::DisplayP3>(near_black, result);
  EXPECT_LT(result[0], 2);
  EXPECT_LT(result[1], 2);
  EXPECT_LT(result[2], 2);

  const std::vector<unsigned char> near_white{254, 254, 254};
  psm::Convert<psm::sRGB, psm::DisplayP3>(near_white, result);
  EXPECT_LT(result[0], 255);
  EXPECT_LT(result[1], 255);
  EXPECT_LT(result[2], 255);
}

TEST_F(DisplayP3Test, HandlesGrayValues) {
  const std::vector<unsigned char> mid_gray{128, 128, 128};
  psm::Convert<psm::sRGB, psm::DisplayP3>(mid_gray, result);
  EXPECT_THAT(result, IsNearVector(std::vector<unsigned char>{128, 128, 128},
                                   Tolerance));

  const std::vector<unsigned char> quarter_gray{64, 64, 64};
  psm::Convert<psm::sRGB, psm::DisplayP3>(quarter_gray, result);
  EXPECT_THAT(result,
              IsNearVector(std::vector<unsigned char>{64, 64, 64}, Tolerance));

  const std::vector<unsigned char> three_quarter_gray{192, 192, 192};
  psm::Convert<psm::sRGB, psm::DisplayP3>(three_quarter_gray, result);
  EXPECT_THAT(result, IsNearVector(std::vector<unsigned char>{192, 192, 192},
                                   Tolerance));
}

TEST_F(DisplayP3Test, RoundTripConversion) {
  const std::vector<unsigned char> original = {210, 92, 180};
  std::vector<unsigned char> intermediate(3);
  std::vector<unsigned char> result(3);

  psm::Convert<psm::sRGB, psm::DisplayP3>(original, intermediate);
  psm::Convert<psm::DisplayP3, psm::sRGB>(intermediate, result);

  EXPECT_THAT(result, IsNearVector(original, Tolerance));
}

TEST_F(DisplayP3Test, ValidatesInputSize) {
  const std::vector<unsigned char> invalid_size = {255,
                                                   255};  // Only 2 components
  std::vector<unsigned char> result(3);

  EXPECT_THROW((psm::Convert<psm::sRGB, psm::DisplayP3>(invalid_size, result)),
               std::invalid_argument);
}

TEST_F(DisplayP3Test, ValidatesOutputBuffer) {
  const std::vector<unsigned char> input = {255, 255, 255};
  std::vector<unsigned char> small_output(2);

  EXPECT_THROW((psm::Convert<psm::sRGB, psm::DisplayP3>(input, small_output)),
               std::invalid_argument);
}

TEST_F(DisplayP3Test, BulkConversionPerformance) {
  constexpr size_t num_pixels = 1000000;
  const std::vector<unsigned char> large_input(num_pixels * 3, 128);
  std::vector<unsigned char> large_output(num_pixels * 3);

  auto start = std::chrono::high_resolution_clock::now();
  psm::Convert<psm::sRGB, psm::DisplayP3>(large_input, large_output);
  auto end = std::chrono::high_resolution_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  EXPECT_LT(duration.count(), 1000);  // Should complete within 1 second
}

TEST_F(DisplayP3Test, HandlesWideGamutColors) {
  const std::vector<unsigned char> saturated_blue{0, 0, 255};
  psm::Convert<psm::sRGB, psm::DisplayP3>(saturated_blue, result);
  // Display P3 should represent this blue as more saturated
  EXPECT_GT(result[0], saturated_blue[0] - Tolerance);
}

}  // namespace psm_test::display_p3
