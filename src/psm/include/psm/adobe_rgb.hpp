#pragma once

#include <span>

#include "color_space_traits.hpp"
#include "percent.hpp"

namespace psm {

struct AdobeRGB {};

class AdobeRgb {
 public:
  AdobeRgb() = delete;

  template <typename T>
  static void fromSRGB(const std::span<T>& src, std::span<T> dst);
  template <typename T>
  static void toSRGB(const std::span<T>& src, std::span<T> dst);
  template <typename T>
  static void adjustChannels(std::span<T> buffer,
                             const Percent& adjust_percentage);
};

template <>
struct detail::ColorSpace<AdobeRGB> {
  using Type = AdobeRgb;
};

}  // namespace psm
