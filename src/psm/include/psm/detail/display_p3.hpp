#pragma once

#include <span>

namespace psm {
namespace detail {

class DisplayP3 {
 public:
  DisplayP3() = delete;

  template <typename T>
  static void fromSRGB(std::span<const T> src, std::span<T> dst);
  template <typename T>
  static void toSRGB(std::span<const T> src, std::span<T> dst);
};
}  // namespace detail

struct DisplayP3 {
  using type = detail::DisplayP3;
};

}  // namespace psm
