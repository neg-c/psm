#pragma once

#include <cstddef>
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
}  // namespace detail

template <typename SrcFormat, typename DstFormat,
          std::ranges::contiguous_range Src_Range,
          std::ranges::contiguous_range Dst_Range>
void Convert(Src_Range& src, Dst_Range& dst) {
  detail::ConvertImpl<SrcFormat, DstFormat>(
      std::span<std::ranges::range_value_t<Src_Range>>{src.data(), src.size()},
      std::span<std::ranges::range_value_t<Dst_Range>>{dst.data(), dst.size()});
}

}  // namespace psm
