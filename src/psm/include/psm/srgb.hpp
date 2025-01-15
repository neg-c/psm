#pragma once
#include <span>
#include "color_space_traits.hpp"
#include "percent.hpp"

namespace psm {

struct sRGB {};

class Srgb {
 public:
  Srgb() = delete;
  using tag_type = sRGB;

  template <typename T>
  static void fromSRGB(const std::span<T>& src, std::span<T> dst) {
    dst = src;  // passthrough cuz we are already in sRGB
  }

  template <typename T>
  static void toSRGB(const std::span<T>& src, std::span<T> dst) {
    dst = src;  // passthrough cuz we are already in sRGB
  }
};

// Specialization for this color space's tag
template<>
struct psm::detail::ColorSpaceFromTag<sRGB> {
    using type = Srgb;
};

static_assert(psm::detail::ColorSpaceType<Srgb>, "Srgb must satisfy ColorSpaceType concept");

}  // namespace psm
