#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

namespace psm_test {

MATCHER_P2(IsNearVector, expected, tolerance, "") {
  const auto& actual = arg;
  if (actual.size() != expected.size()) {
    *result_listener << "Vector sizes differ. Expected: " << expected.size()
                     << " Actual: " << actual.size();
    return false;
  }

  for (size_t i = 0; i < actual.size(); ++i) {
    if (std::abs(static_cast<int>(actual[i]) - static_cast<int>(expected[i])) >
        tolerance) {
      *result_listener << "Vectors differ at index " << i
                       << ". Expected: " << static_cast<int>(expected[i])
                       << " Actual: " << static_cast<int>(actual[i])
                       << " (tolerance: " << tolerance << ")";
      return false;
    }
  }
  return true;
}

}  // namespace psm_test
