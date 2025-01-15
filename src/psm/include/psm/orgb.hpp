#pragma once
#include <span>
#include "color_space_traits.hpp"
#include "percent.hpp"

namespace psm {

struct oRGB {};

class Orgb {
 public:
  Orgb() = delete;
  using tag_type = oRGB;

  template <typename T>
  static void fromSRGB(const std::span<T>& src, std::span<T> dst);
  template <typename T>
  static void toSRGB(const std::span<T>& src, std::span<T> dst);
};

// Specialization for this color space's tag
template<>
struct detail::ColorSpaceFromTag<oRGB> {
    using type = Orgb;
};

static_assert(psm::detail::ColorSpaceType<Orgb>, "Orgb must satisfy ColorSpaceType concept");

}  // namespace psm
