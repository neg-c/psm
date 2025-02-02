#pragma once

#include <span>

namespace psm {

namespace detail {

class AdobeRgb {
 public:
  AdobeRgb() = delete;

  template <typename T>
  static void fromSRGB(const std::span<const T>& src, std::span<T> dst);
  template <typename T>
  static void toSRGB(const std::span<const T>& src, std::span<T> dst);
};

}  // namespace detail

struct AdobeRGB {
  using type = detail::AdobeRgb;
};

}  // namespace psm
