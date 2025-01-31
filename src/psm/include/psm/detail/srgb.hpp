#pragma once
#include <span>

namespace psm {

namespace detail {

class Srgb {
 public:
  Srgb() = delete;

  template <typename T>
  static void fromSRGB(const std::span<T>& src, std::span<T> dst) {
    // passthrough because we are already in sRGB
    std::copy(src.begin(), src.end(), dst.begin());
  }

  template <typename T>
  static void toSRGB(const std::span<T>& src, std::span<T> dst) {
    // passthrough because we are already in sRGB
    std::copy(src.begin(), src.end(), dst.begin());
  }
};

}  // namespace detail

struct sRGB {
  using type = detail::Srgb;
};

}  // namespace psm
