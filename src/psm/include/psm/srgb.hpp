#ifndef SRGB_HPP
#define SRGB_HPP

#include <memory>
#include <span>

class SrgbImpl;

namespace psm {

struct sRGB {};

class SrgbImpl;

class Srgb {
 public:
  Srgb();
  ~Srgb();

  Srgb(const Srgb&) = delete;
  Srgb& operator=(const Srgb&) = delete;

  Srgb(Srgb&&) noexcept;
  Srgb& operator=(Srgb&&) noexcept;

  template <typename T>
  void toXYZ(std::span<const T> src, std::span<float> dst);
  template <typename T>
  void fromXYZ(std::span<float> src, std::span<T> dst);

 private:
  std::unique_ptr<SrgbImpl> impl_;
};

}  // namespace psm
#endif  // SRGB_HPP
