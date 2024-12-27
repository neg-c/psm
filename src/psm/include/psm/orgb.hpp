#ifndef ORGB_HPP
#define ORGB_HPP

#include <memory>
#include <span>

class OrgbImpl;

namespace psm {

struct oRGB {};

class OrgbImpl;

class Orgb {
 public:
  Orgb();
  ~Orgb();

  Orgb(const Orgb&) = delete;
  Orgb& operator=(const Orgb&) = delete;

  Orgb(Orgb&&) noexcept;
  Orgb& operator=(Orgb&&) noexcept;

  template <typename T>
  [[deprecated]] void convert(std::span<const T> src, std::span<T> dst);

  void toXYZ(std::span<const float> src, std::span<float> dst);
  void fromXYZ(std::span<float> src, std::span<float> dst);

 private:
  std::unique_ptr<OrgbImpl> impl_;
};

}  // namespace psm
#endif  // ORGB_HPP
