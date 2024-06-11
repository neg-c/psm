#pragma once

#include <span>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace cvt {

enum class Format { kRGB, kRGBA, kGray };
enum class Conversion { kRGB2Gray, kRGBA2Gray };

constexpr float kRedWeight = 0.299;
constexpr float kGreenWeight = 0.587;
constexpr float kBlueWeight = 0.114;

struct PairHash {
  template <class T, class U>
  std::size_t operator()(const std::pair<T, U> &format_pair) const {
    auto h1 = std::hash<T>{}(format_pair.first);
    auto h2 = std::hash<U>{}(format_pair.second);
    return h1 ^ h2;
  }
};

static std::unordered_map<std::pair<Format, Format>, Conversion, PairHash>
    conversion_table{
        {std::pair(Format::kRGB, Format::kGray), Conversion::kRGB2Gray},
        {std::pair(Format::kRGBA, Format::kGray), Conversion::kRGBA2Gray},
    };

template <typename T> constexpr T toGray(T r, T g, T b) {
  return static_cast<T>(kRedWeight * r + kGreenWeight * g + kBlueWeight * b);
}

template <typename T>
void rgb2gray(std::span<const T> src, std::span<T> dst, std::size_t channel) {
  for (std::size_t i = 0; i < src.size() / channel; ++i) {
    T r = src[i * channel];
    T g = src[i * channel + 1];
    T b = src[i * channel + 2];

    dst[i] = toGray(r, g, b);
  }
}

template <typename T>
void Color(std::span<const T> src, std::span<T> dst, Format src_format,
           Format dst_format) {
  auto cvt_format_it =
      conversion_table.find(std::make_pair(src_format, dst_format));

  switch (cvt_format_it->second) {
  case Conversion::kRGB2Gray:
    rgb2gray(src, dst, 3);
    break;
  case Conversion::kRGBA2Gray:
    rgb2gray(src, dst, 4);
    break;
  default:
    throw std::invalid_argument("Unsupported format");
  }
}

} // namespace cvt
