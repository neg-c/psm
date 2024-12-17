#ifndef PSM_HPP
#define PSM_HPP

#include <ranges>
#include <span>
#include <stdexcept>
#include <type_traits>

#include "orgb.hpp"
#include "srgb.hpp"

namespace psm {

enum class Format { ksRGB, koRGB };
enum class Conversion { ksRGB2oRGB };

struct PairHash {
  template <class T, class U>
  std::size_t operator()(const std::pair<T, U>& format_pair) const {
    auto h1 = std::hash<T>{}(format_pair.first);
    auto h2 = std::hash<U>{}(format_pair.second);
    return h1 ^ h2;
  }
};

static std::unordered_map<std::pair<Format, Format>, Conversion, PairHash>
    conversion_table{
        {std::pair(Format::ksRGB, Format::koRGB), Conversion::ksRGB2oRGB}};

namespace detail {
template <typename SrcFormat, typename DstFormat, typename T>
void ConvertImpl(std::span<const T> src, std::span<T> dst) {
  if constexpr (std::is_same_v<SrcFormat, sRGB> &&
                std::is_same_v<DstFormat, oRGB>) {
    Orgb orgb;
    Srgb srgb;
    std::vector<float> tmp(src.size());
    orgb.convert(src, dst);
    srgb.toXYZ(src, tmp);
    orgb.fromXYZ(tmp, dst);
  } else if constexpr (std::is_same_v<SrcFormat, oRGB> &&
                       std::is_same_v<DstFormat, sRGB>) {
    Orgb orgb;
    Srgb srgb;
    std::vector<float> tmp(src.size());
    orgb.toXYZ(src, tmp);
    srgb.fromXYZ(tmp, dst);
  } else {
    throw std::invalid_argument("Unsupported format");
  }
}
}  // namespace detail

template <std::ranges::contiguous_range Src_Range,
          std::ranges::contiguous_range Dst_Range>
void Convert(const Src_Range& src, Dst_Range& dst, Format src_format,
             Format dst_format) {
  detail::ConvertImpl(std::span{src}, std::span{dst}, src_format, dst_format);
}

}  // namespace psm

#endif  // PSM_HPP
