#pragma once
#include <span>

namespace psm {

class Orgb {
 public:
  Orgb() = delete;

  template <typename T>
  static void fromSRGB(const std::span<T>& src, std::span<T> dst);
  template <typename T>
  static void toSRGB(const std::span<T>& src, std::span<T> dst);
};

struct oRGB {
  using type = Orgb;
};

}  // namespace psm
