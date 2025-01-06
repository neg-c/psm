#pragma once

#include <cstddef>
#include <optional>
#include <ranges>
#include <span>
#include <type_traits>

#include "color_space_traits.hpp"
#include "orgb.hpp"
#include "srgb.hpp"

namespace psm {

namespace detail {
template <typename SrcFormat, typename DstFormat, typename T>
void ConvertImpl(std::span<T> src, std::span<T> dst) {
  std::span<T> intermediate{src.data(), src.size()};

  ColorSpace_t<SrcFormat>::toSRGB(src, intermediate);
  ColorSpace_t<DstFormat>::fromSRGB(intermediate, dst);
}

template <typename Format, typename T>
void AdjustChannelsImpl(std::span<T> buffer,
                        std::optional<T> channel0 = std::nullopt,
                        std::optional<T> channel1 = std::nullopt,
                        std::optional<T> channel2 = std::nullopt) {
  ColorSpace_t<Format>::adjustChannels(buffer, channel0, channel1, channel2);
}
}  // namespace detail

template <typename SrcFormat, typename DstFormat,
          std::ranges::contiguous_range Src_Range,
          std::ranges::contiguous_range Dst_Range>
void Convert(Src_Range& src, Dst_Range& dst) {
  detail::ConvertImpl<SrcFormat, DstFormat>(
      std::span<std::ranges::range_value_t<Src_Range>>{src.data(), src.size()},
      std::span<std::ranges::range_value_t<Dst_Range>>{dst.data(), dst.size()});
}

template <typename Format, std::ranges::contiguous_range Range>
void AdjustChannels(
    Range& buffer,
    std::optional<std::ranges::range_value_t<Range>> channel0 = std::nullopt,
    std::optional<std::ranges::range_value_t<Range>> channel1 = std::nullopt,
    std::optional<std::ranges::range_value_t<Range>> channel2 = std::nullopt) {
  detail::AdjustChannelsImpl<Format>(std::span{buffer}, channel0, channel1,
                                     channel2);
}

}  // namespace psm
