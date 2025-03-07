#pragma once

#include <span>

namespace psm {

namespace detail {

class ProPhotoRgb {
 public:
  ProPhotoRgb() = delete;

  template <typename T>
  static void fromSRGB(const std::span<const T>& src, std::span<T> dst);
  template <typename T>
  static void toSRGB(const std::span<const T>& src, std::span<T> dst);
};

}  // namespace detail

struct ProPhotoRGB {
  using type = detail::ProPhotoRgb;
};

}  // namespace psm
