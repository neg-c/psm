#pragma once

#include <ranges>
#include <span>

#if __has_include("detail/adobe_rgb.hpp")
#include "detail/adobe_rgb.hpp"
#endif
#if __has_include("detail/orgb.hpp")
#include "detail/orgb.hpp"
#endif
#if __has_include("detail/display_p3.hpp")
#include "detail/display_p3.hpp"
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

/**
 * @brief Converts color values from source format to destination format
 *
 * @tparam SrcFormat Source color space format
 * @tparam DstFormat Destination color space format
 * @tparam SrcRange Input range type (must be contiguous)
 *
 * @param src_bgr Source range containing color values
 * @return Converted color values in the same container type as input
 *
 * @throws std::invalid_argument if input buffer size is not a multiple of 3
 */
template <typename SrcFormat, typename DstFormat,
          std::ranges::contiguous_range SrcRange>
auto Convert(const SrcRange& src_bgr) {
  if (std::ranges::size(src_bgr) % 3 != 0) {
    throw std::invalid_argument("Input buffer size must be a multiple of 3");
  }

  // Create output container of the same type as input
  std::remove_cvref_t<SrcRange> dst;
  dst.resize(std::ranges::size(src_bgr));

  detail::ConvertImpl<SrcFormat, DstFormat>(
      std::span<const std::ranges::range_value_t<SrcRange>>{src_bgr.data(),
                                                            src_bgr.size()},
      std::span<std::ranges::range_value_t<SrcRange>>{dst.data(), dst.size()});

  return dst;
}
}  // namespace psm
