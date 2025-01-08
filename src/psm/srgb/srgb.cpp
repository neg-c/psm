#include "psm/srgb.hpp"
#include "psm/detail/adjust_channels.hpp"

namespace psm {

template <typename T>
void Srgb::adjustChannels(std::span<T> buffer,
                         const Percent& adjust_percentage) {
  detail::adjustChannels(buffer, adjust_percentage);
}

template void Srgb::fromSRGB<unsigned char>(const std::span<unsigned char>&,
                                          std::span<unsigned char>);
template void Srgb::toSRGB<unsigned char>(const std::span<unsigned char>&,
                                        std::span<unsigned char>);
template void Srgb::adjustChannels<unsigned char>(std::span<unsigned char>,
                                               const Percent&);

}  // namespace psm
