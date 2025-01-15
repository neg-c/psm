#pragma once
#include <span>

namespace psm {

struct oRGB {
  using type = class Orgb;
};

class Orgb {
 public:
  Orgb() = delete;

  template <typename T>
  static void fromSRGB(const std::span<T>& src, std::span<T> dst);
  template <typename T>
  static void toSRGB(const std::span<T>& src, std::span<T> dst);
};

}  // namespace psm
