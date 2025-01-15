#pragma once

#include <span>

namespace psm {

class AdobeRgb {
 public:
  AdobeRgb() = delete;

  template <typename T>
  static void fromSRGB(const std::span<T>& src, std::span<T> dst);
  template <typename T>
  static void toSRGB(const std::span<T>& src, std::span<T> dst);
};

struct AdobeRGB {
  using type = AdobeRgb;
};

}  // namespace psm
