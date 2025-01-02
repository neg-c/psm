#ifndef PSM_HPP
#define PSM_HPP

#include <ranges>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "orgb.hpp"
#include "psm/orgb.hpp"

namespace psm {

struct sRGB {};

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
void Convert(const Src_Range& src, Dst_Range& dst) {
  detail::ConvertImpl<SrcFormat, DstFormat>(std::span{src}, std::span{dst});
}

}  // namespace psm

#endif  // PSM_HPP
