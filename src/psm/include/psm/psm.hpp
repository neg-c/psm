#pragma once

#include <ranges>
#include <span>

#include "color_space_traits.hpp"
#include "orgb.hpp"
#include "percent.hpp"
#include "srgb.hpp"

namespace psm {

namespace detail {
template <typename SrcFormat, typename DstFormat, typename T>
void ConvertImpl(std::span<T> src, std::span<T> dst) {
  const std::span<T> intermediate{src.data(), src.size()};

  ColorSpace_t<SrcFormat>::toSRGB(src, intermediate);
  ColorSpace_t<DstFormat>::fromSRGB(intermediate, dst);
}

template <typename Format, typename T>
void AdjustChannelsImpl(std::span<T> buffer, const Percent& adjust_percentage) {
  ColorSpace_t<Format>::adjustChannels(buffer, adjust_percentage);
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

template <typename Format, std::ranges::contiguous_range Range>
void AdjustChannels(Range& buffer, const Percent& adjust_percentage) {
  detail::AdjustChannelsImpl<Format>(std::span{buffer}, adjust_percentage);
}

}  // namespace psm
