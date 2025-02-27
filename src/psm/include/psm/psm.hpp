#pragma once

#include <ranges>
#include <span>
#include <stdexcept>

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
  if (src.size() % 3 != 0) {
    throw std::invalid_argument("Input buffer size must be a multiple of 3");
  }

  if (dst.size() != src.size()) {
    throw std::invalid_argument(
        "Output buffer must be the same size as input buffer");
  }

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
 * @tparam DstRange Output range type (must be contiguous)
 *
 * @param src Source range containing color values
 * @param dst Destination range where converted values will be stored
 *
 * @throws std::invalid_argument if input buffer size is not a multiple of 3
 * @throws std::invalid_argument if output buffer size doesn't match input buffer size
 */
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
