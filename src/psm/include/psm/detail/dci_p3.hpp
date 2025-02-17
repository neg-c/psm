#pragma once

#include <span>

namespace psm {
namespace detail {

class DciP3 {
 public:
  DciP3() = delete;

  template <typename T>
  static void fromSRGB(const std::span<const T>& src, std::span<T> dst);
  template <typename T>
  static void toSRGB(const std::span<const T>& src, std::span<T> dst);
};
}  // namespace detail

struct DciP3 {
  using type = detail::DciP3;
};

}  // namespace psm
