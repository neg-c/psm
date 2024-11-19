#ifndef PSM_HPP
#define PSM_HPP

#include <span>
#include <unordered_map>
#include <utility>
#include <stdexcept>

#include "orgb.hpp"
#include "psm/orgb.hpp"
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

template <typename T>
void Color(std::span<const T> src, std::span<T> dst, Format src_format,
           Format dst_format) {
  auto cvt_format_it =
      conversion_table.find(std::make_pair(src_format, dst_format));

  switch (cvt_format_it->second) {
    case Conversion::ksRGB2oRGB: {
      Orgb orgb;
      orgb.convert(src, dst);
      break;
    }
    default:
      throw std::invalid_argument("Unsupported format");
  }
}

}  // namespace psm

#endif  // PSM_HPP
