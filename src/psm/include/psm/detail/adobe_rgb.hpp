#pragma once

#include <span>

namespace psm {

namespace detail {

class AdobeRgb {
 public:
  AdobeRgb() = delete;

  template <typename T>
  static void fromSRGB(std::span<const T> src, std::span<T> dst);
  template <typename T>
  static void toSRGB(std::span<const T> src, std::span<T> dst);
};

}  // namespace detail

struct AdobeRGB {
  using type = detail::AdobeRgb;
};

}  // namespace psm
