#pragma once

#include <span>

#include "color_space_traits.hpp"
#include "percent.hpp"

namespace psm {

struct sRGB {};

class Srgb {
 public:
  Srgb() = delete;

  template <typename T>
  static void fromSRGB(const std::span<T>& src, std::span<T> dst) {
    dst = src;  // passthrough cuz we are already in sRGB
  }

  template <typename T>
  static void toSRGB(const std::span<T>& src, std::span<T> dst) {
    dst = src;  // passthrough cuz we are already in sRGB
  }
};

template <>
struct detail::ColorSpace<sRGB> {
  using Type = Srgb;
};

}  // namespace psm
