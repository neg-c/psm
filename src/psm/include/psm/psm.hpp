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
  const std::span<const T> dst_view{dst.data(), dst.size()};
  DstColorSpace::fromSRGB(dst_view, dst);
}
}  // namespace detail

template <typename SrcFormat, typename DstFormat,
          std::ranges::contiguous_range SrcRange>
auto Convert(const SrcRange& src) {
  if (std::ranges::size(src) % 3 != 0) {
    throw std::invalid_argument("Input buffer size must be a multiple of 3");
  }

  // Create output container of the same type as input
  std::remove_cvref_t<SrcRange> dst;
  dst.resize(std::ranges::size(src));

  detail::ConvertImpl<SrcFormat, DstFormat>(
      std::span<const std::ranges::range_value_t<SrcRange>>{src.data(),
                                                            src.size()},
      std::span<std::ranges::range_value_t<SrcRange>>{dst.data(), dst.size()});

  return dst;
}
}  // namespace psm
