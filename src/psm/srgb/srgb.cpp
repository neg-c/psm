#include "psm/detail/srgb.hpp"

namespace psm::detail {

template void Srgb::fromSRGB<unsigned char>(const std::span<unsigned char>&,
                                            std::span<unsigned char>);
template void Srgb::toSRGB<unsigned char>(const std::span<unsigned char>&,
                                          std::span<unsigned char>);
}  // namespace psm::detail
