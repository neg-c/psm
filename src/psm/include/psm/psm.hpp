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
template <typename SrcFormat, typename DstFormat, typename SrcT, typename DstT>
void ConvertImpl(std::span<const SrcT> src, std::span<DstT> dst) {
  if constexpr (std::is_same_v<SrcFormat, sRGB> &&
                std::is_same_v<DstFormat, oRGB>) {
    Orgb orgb;
    orgb.fromSRGB(src, dst);
  } else if constexpr (std::is_same_v<SrcFormat, oRGB> &&
                       std::is_same_v<DstFormat, sRGB>) {
    Orgb orgb;
    orgb.toSRGB(src, dst);
  } else {
    throw std::invalid_argument("Unsupported format");
  }
}
}  // namespace detail

template <typename SrcFormat, typename DstFormat,
          std::ranges::contiguous_range Src_Range,
          std::ranges::contiguous_range Dst_Range>
void Convert(const Src_Range& src, Dst_Range& dst) {
  using SrcT = std::ranges::range_value_t<Src_Range>;
  using DstT = std::ranges::range_value_t<Dst_Range>;
  detail::ConvertImpl<SrcFormat, DstFormat, SrcT, DstT>(std::span{src},
                                                        std::span{dst});
}

}  // namespace psm

#endif  // PSM_HPP
