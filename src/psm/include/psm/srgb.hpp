#pragma once
#include <span>

namespace psm {

namespace detail {

class Srgb {
 public:
  Srgb() = delete;

  template <typename T>
  static void fromSRGB(const std::span<T>& src, std::span<T> dst) {
    dst = src;  // passthrough because we are already in sRGB
  }

  template <typename T>
  static void toSRGB(const std::span<T>& src, std::span<T> dst) {
    dst = src;  // passthrough because we are already in sRGB
  }
};

}  // namespace detail

struct sRGB {
  using type = detail::Srgb;
};

}  // namespace psm
