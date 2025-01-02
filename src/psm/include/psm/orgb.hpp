#ifndef ORGB_HPP
#define ORGB_HPP

#include <span>

class OrgbImpl;

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

}  // namespace psm
#endif  // ORGB_HPP
