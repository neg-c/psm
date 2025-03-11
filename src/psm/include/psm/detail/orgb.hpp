#pragma once
#include <span>

namespace psm {

namespace detail {

class Orgb {
 public:
  Orgb() = delete;

  template <typename T>
  static void fromSRGB(std::span<const T> src, std::span<T> dst);
  template <typename T>
  static void toSRGB(std::span<const T> src, std::span<T> dst);
};

}  // namespace detail

struct oRGB {
  using type = detail::Orgb;
};

}  // namespace psm
