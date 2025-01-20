#pragma once

#include <ranges>
#include <span>

#include "detail/adobe_rgb.hpp"
#include "detail/color_space_concept.hpp"
#include "detail/orgb.hpp"
#include "detail/srgb.hpp"

namespace psm {

namespace detail {
template <ColorSpaceType SrcTag, ColorSpaceType DstTag, typename T>
void ConvertImpl(std::span<T> src, std::span<T> dst) {
  const std::span<T> intermediate{src.data(), src.size()};

  using SrcColorSpace = detail::ColorSpaceImpl<SrcTag>;
  using DstColorSpace = detail::ColorSpaceImpl<DstTag>;

  SrcColorSpace::toSRGB(src, intermediate);
  DstColorSpace::fromSRGB(intermediate, dst);
}
}  // namespace detail

template <typename SrcFormat, typename DstFormat,
          std::ranges::contiguous_range SrcRange,
          std::ranges::contiguous_range DstRange>
void Convert(SrcRange& src, DstRange& dst) {
  detail::ConvertImpl<SrcFormat, DstFormat>(
      std::span<std::ranges::range_value_t<SrcRange>>{src.data(), src.size()},
      std::span<std::ranges::range_value_t<DstRange>>{dst.data(), dst.size()});
}
}  // namespace psm
