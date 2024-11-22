#ifndef ORGB_HPP
#define ORGB_HPP

#include <memory>
#include <span>

class OrgbImpl;

namespace psm {

class OrgbImpl;  // Forward declaration

class Orgb {
 public:
  Orgb();
  ~Orgb();

  Orgb(const Orgb&) = delete;
  Orgb& operator=(const Orgb&) = delete;

  Orgb(Orgb&&) noexcept;
  Orgb& operator=(Orgb&&) noexcept;

  template <typename T>
  void convert(std::span<const T> src, std::span<T> dst);

 private:
  std::unique_ptr<OrgbImpl> impl_;  // Pimpl
};

}  // namespace psm
#endif  // ORGB_HPP
