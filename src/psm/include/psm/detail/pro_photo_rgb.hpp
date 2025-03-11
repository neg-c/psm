#pragma once

#include <span>

namespace psm {

namespace detail {

class ProPhotoRgb {
 public:
  ProPhotoRgb() = delete;

  template <typename T>
  static void fromSRGB(std::span<const T> src, std::span<T> dst);
  template <typename T>
  static void toSRGB(std::span<const T> src, std::span<T> dst);
};

}  // namespace detail

struct ProPhotoRGB {
  using type = detail::ProPhotoRgb;
};

}  // namespace psm
