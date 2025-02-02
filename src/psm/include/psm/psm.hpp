#pragma once

#include <ranges>
#include <span>

#if __has_include("detail/adobe_rgb.hpp")
#include "detail/adobe_rgb.hpp"
#endif
#if __has_include("detail/orgb.hpp")
#include "detail/orgb.hpp"
#endif

#include "detail/color_space_concept.hpp"
#include "detail/srgb.hpp"

namespace psm {

namespace detail {
template <ColorSpaceType SrcTag, ColorSpaceType DstTag, typename T>
void ConvertImpl(std::span<const T> src, std::span<T> dst) {
  using SrcColorSpace = detail::ColorSpaceImpl<SrcTag>;
  using DstColorSpace = detail::ColorSpaceImpl<DstTag>;

  SrcColorSpace::toSRGB(src, dst);
  std::span<const T> dst_view{dst.data(), dst.size()};
  DstColorSpace::fromSRGB(dst_view, dst);
}
}  // namespace detail

template <typename SrcFormat, typename DstFormat,
          std::ranges::contiguous_range SrcRange,
          std::ranges::contiguous_range DstRange>
void Convert(const SrcRange& src, DstRange& dst) {
  detail::ConvertImpl<SrcFormat, DstFormat>(
      std::span<const std::ranges::range_value_t<SrcRange>>{src.data(),
                                                            src.size()},
      std::span<std::ranges::range_value_t<DstRange>>{dst.data(), dst.size()});
}
}  // namespace psm
