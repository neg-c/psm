#include <gtest/gtest.h>

#include <chrono>
#include <vector>

#include "psm/psm.hpp"
#include "test_utils.hpp"

namespace psm_test::pro_photo_rgb {

class ProPhotoRgbTest : public ::testing::Test {
 protected:
  static constexpr int Tolerance = 2;

  std::vector<unsigned char> red{255, 0, 0};
  std::vector<unsigned char> green{0, 255, 0};
  std::vector<unsigned char> blue{0, 0, 255};
  std::vector<unsigned char> black{0, 0, 0};
  std::vector<unsigned char> white{255, 255, 255};

  std::vector<unsigned char> pro_photo_red{179, 70, 26};
  std::vector<unsigned char> pro_photo_green{137, 237, 77};
  std::vector<unsigned char> pro_photo_blue{86, 35, 235};
  std::vector<unsigned char> pro_photo_black{0, 0, 0};
  // Result buffer
  std::vector<unsigned char> result{0, 0, 0};

  void SetUp() override {}
};

TEST_F(ProPhotoRgbTest, HandlesPrimaryColors) {
  psm::Convert<psm::sRGB, psm::ProPhotoRGB>(red, result);
  EXPECT_THAT(result, IsNearVector(pro_photo_red, Tolerance));

  psm::Convert<psm::sRGB, psm::ProPhotoRGB>(green, result);
  EXPECT_THAT(result, IsNearVector(pro_photo_green, Tolerance));

  psm::Convert<psm::sRGB, psm::ProPhotoRGB>(blue, result);
  EXPECT_THAT(result, IsNearVector(pro_photo_blue, Tolerance));
}

TEST_F(ProPhotoRgbTest, HandlesBlackAndWhite) {
  psm::Convert<psm::sRGB, psm::ProPhotoRGB>(black, result);
  EXPECT_EQ(result, pro_photo_black);

  const std::vector<unsigned char> near_black{1, 1, 1};
  psm::Convert<psm::sRGB, psm::ProPhotoRGB>(near_black, result);
  EXPECT_LT(result[0], 2);
  EXPECT_LT(result[1], 2);
  EXPECT_LT(result[2], 2);

  const std::vector<unsigned char> near_white{254, 254, 254};
  psm::Convert<psm::sRGB, psm::ProPhotoRGB>(near_white, result);
  EXPECT_LT(result[0], 255);
  EXPECT_LT(result[1], 255);
  EXPECT_LT(result[2], 255);
}

TEST_F(ProPhotoRgbTest, HandlesGrayValues) {
  const std::vector<unsigned char> mid_gray{128, 128, 128};
  psm::Convert<psm::sRGB, psm::ProPhotoRGB>(mid_gray, result);
  EXPECT_THAT(result, IsNearVector(std::vector<unsigned char>{108, 108, 108},
                                   Tolerance));

  const std::vector<unsigned char> quarter_gray{64, 64, 64};
  psm::Convert<psm::sRGB, psm::ProPhotoRGB>(quarter_gray, result);
  EXPECT_THAT(result,
              IsNearVector(std::vector<unsigned char>{48, 48, 48}, Tolerance));

  const std::vector<unsigned char> three_quarter_gray{192, 192, 192};
  psm::Convert<psm::sRGB, psm::ProPhotoRGB>(three_quarter_gray, result);
  EXPECT_THAT(result, IsNearVector(std::vector<unsigned char>{177, 177, 177},
                                   Tolerance));
}

TEST_F(ProPhotoRgbTest, RoundTripConversion) {
  const std::vector<unsigned char> original = {210, 92, 180};
  std::vector<unsigned char> intermediate(3);
  std::vector<unsigned char> result(3);

  psm::Convert<psm::sRGB, psm::ProPhotoRGB>(original, intermediate);
  psm::Convert<psm::ProPhotoRGB, psm::sRGB>(intermediate, result);

  EXPECT_THAT(result, IsNearVector(original, Tolerance));
}

TEST_F(ProPhotoRgbTest, ValidatesInputSize) {
  const std::vector<unsigned char> invalid_size = {255,
                                                   255};  // Only 2 components
  std::vector<unsigned char> result(3);

  EXPECT_THROW(
      (psm::Convert<psm::sRGB, psm::ProPhotoRGB>(invalid_size, result)),
      std::invalid_argument);
}

TEST_F(ProPhotoRgbTest, ValidatesOutputBuffer) {
  const std::vector<unsigned char> input = {255, 255, 255};
  std::vector<unsigned char> small_output(2);

  EXPECT_THROW((psm::Convert<psm::sRGB, psm::ProPhotoRGB>(input, small_output)),
               std::invalid_argument);
}

TEST_F(ProPhotoRgbTest, BulkConversionPerformance) {
  constexpr size_t num_pixels = 1000000;
  const std::vector<unsigned char> large_input(num_pixels * 3, 128);
  std::vector<unsigned char> large_output(num_pixels * 3);

  auto start = std::chrono::high_resolution_clock::now();
  psm::Convert<psm::sRGB, psm::ProPhotoRGB>(large_input, large_output);
  auto end = std::chrono::high_resolution_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  EXPECT_LT(duration.count(), 1000);  // Should complete within 1 second
}

}  // namespace psm_test::pro_photo_rgb
