#pragma once

#include <ranges>
#include <span>

#include "color_space_traits.hpp"
#include "detail/adjust_channels.hpp"
#include "orgb.hpp"
#include "percent.hpp"
#include "srgb.hpp"

namespace psm {

namespace detail {
template <typename SrcTag, typename DstTag, typename T>
void ConvertImpl(std::span<T> src, std::span<T> dst) {
  const std::span<T> intermediate{src.data(), src.size()};

  static_assert(ColorSpaceType<SrcTag>,
                "Source color space tag must satisfy ColorSpaceType concept");
  static_assert(
      ColorSpaceType<DstTag>,
      "Destination color space tag must satisfy ColorSpaceType concept");

  using SrcColorSpace = detail::ColorSpaceImpl<SrcTag>;
  using DstColorSpace = detail::ColorSpaceImpl<DstTag>;

  SrcColorSpace::toSRGB(src, intermediate);
  DstColorSpace::fromSRGB(intermediate, dst);
}

template <typename T>
void AdjustChannelsImpl(std::span<T> buffer, const Percent& adjust_percentage) {
  detail::adjustChannels(buffer, adjust_percentage);
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

template <std::ranges::contiguous_range Range>
void AdjustChannels(Range& buffer, const Percent& adjust_percentage) {
  detail::AdjustChannelsImpl(std::span{buffer}, adjust_percentage);
}

}  // namespace psm
