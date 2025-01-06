#pragma once

#include <cstddef>
#include <optional>
#include <span>

#include "color_space_traits.hpp"

namespace psm {

struct oRGB {};

class Orgb {
 public:
  Orgb() = delete;

  template <typename T>
  static void fromSRGB(const std::span<T>& src, std::span<T> dst);
  template <typename T>
  static void toSRGB(const std::span<T>& src, std::span<T> dst);
  template <typename T>
  static void adjustChannels(std::span<T> buffer,
                             std::optional<T> b = std::nullopt,
                             std::optional<T> g = std::nullopt,
                             std::optional<T> r = std::nullopt);
};

template <>
struct detail::ColorSpace<oRGB> {
  using Type = Orgb;
};

}  // namespace psm
