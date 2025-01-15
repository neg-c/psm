#pragma once
#include <span>

namespace psm {

class Srgb {
 public:
  Srgb() = delete;

  template <typename T>
  static void fromSRGB(const std::span<T>& src, std::span<T> dst) {
    dst = src;
  }

  template <typename T>
  static void toSRGB(const std::span<T>& src, std::span<T> dst) {
    dst = src;
  }
};

struct sRGB {
  using type = Srgb;
};

}  // namespace psm
