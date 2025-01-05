#ifndef ORGB_HPP
#define ORGB_HPP

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
};

template <>
struct detail::ColorSpace<oRGB> {
  using Type = Orgb;
};

}  // namespace psm
#endif  // ORGB_HPP
