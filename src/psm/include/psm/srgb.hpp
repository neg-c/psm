#pragma once

#include <optional>
#include <span>

#include "color_space_traits.hpp"

namespace psm {

struct sRGB {};

class Srgb {
 public:
  Srgb() = delete;

  template <typename T>
  static void fromSRGB(const std::span<T>& src, std::span<T> dst) {
    dst = src;  // passthrough cuz we are already in sRGB
  }

  template <typename T>
  static void toSRGB(const std::span<T>& src, std::span<T> dst) {
    dst = src;  // passthrough cuz we are already in sRGB
  }

  template <typename T>
  static void adjustChannels(std::span<T> buffer, std::optional<T> b,
                             std::optional<T> g, std::optional<T> r);
};

template <>
struct detail::ColorSpace<sRGB> {
  using Type = Srgb;
};

}  // namespace psm
