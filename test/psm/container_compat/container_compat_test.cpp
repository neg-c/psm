#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <deque>
#include <span>
#include <string>
#include <vector>

#include "psm/psm.hpp"

namespace psm_test::container_compat {

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

class ContainerCompatTest : public ::testing::Test {
 protected:
  // Common test data - using identity conversion (sRGB -> sRGB)
  std::vector<unsigned char> test_data = {255, 0, 0, 0, 255, 0, 0, 0, 255};
  std::vector<unsigned char> result = {0, 0, 0, 0, 0, 0, 0, 0, 0};
};

// Test with std::vector
TEST_F(ContainerCompatTest, VectorContainer) {
  psm::Convert<psm::sRGB, psm::sRGB>(test_data, result);
  EXPECT_THAT(result, IsNearVector(test_data));
}

TEST_F(ContainerCompatTest, ArrayContainer) {
  std::array<unsigned char, 9> src = {255, 0, 0, 0, 255, 0, 0, 0, 255};
  std::array<unsigned char, 9> arr_result = {};
  psm::Convert<psm::sRGB, psm::sRGB>(src, arr_result);
  EXPECT_THAT(arr_result, IsNearVector(src));
}

TEST_F(ContainerCompatTest, SpanContainer) {
  std::span<unsigned char> src(test_data);
  std::span<unsigned char> span_result(result);
  psm::Convert<psm::sRGB, psm::sRGB>(src, span_result);
  EXPECT_THAT(result, IsNearVector(test_data));
}

TEST_F(ContainerCompatTest, InvalidContainerSize) {
  std::vector<unsigned char> invalid_src = {255, 0};
  EXPECT_THROW((psm::Convert<psm::sRGB, psm::sRGB>(invalid_src, result)),
               std::invalid_argument);
}

TEST_F(ContainerCompatTest, InvalidOutputSize) {
  std::vector<unsigned char> src = {255, 0, 0};
  std::vector<unsigned char> small_result = {0, 0};
  EXPECT_THROW((psm::Convert<psm::sRGB, psm::sRGB>(src, small_result)),
               std::invalid_argument);
}

// Test with custom allocator
TEST_F(ContainerCompatTest, CustomAllocatorVector) {
  std::vector<unsigned char, std::allocator<unsigned char>> src = {255, 0, 0};
  std::vector<unsigned char, std::allocator<unsigned char>> alloc_result = {
      0, 0, 0};
  psm::Convert<psm::sRGB, psm::sRGB>(src, alloc_result);
  EXPECT_THAT(alloc_result, IsNearVector(src));
}

// Test with const container
TEST_F(ContainerCompatTest, ConstContainer) {
  const std::vector<unsigned char> src = {255, 0, 0};
  std::vector<unsigned char> const_result = {0, 0, 0};
  psm::Convert<psm::sRGB, psm::sRGB>(src, const_result);
  EXPECT_THAT(const_result, IsNearVector(src));
}

}  // namespace psm_test::container_compat
