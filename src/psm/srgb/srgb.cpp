#include "psm/srgb.hpp"

#include <Eigen/Dense>

namespace psm {

template void Srgb::fromSRGB<unsigned char>(const std::span<unsigned char>&,
                                            std::span<unsigned char>);
template void Srgb::toSRGB<unsigned char>(const std::span<unsigned char>&,
                                          std::span<unsigned char>);
template <typename T>
void Srgb::adjustChannels(std::span<T> buffer, std::optional<T> b,
                          std::optional<T> g, std::optional<T> r) {
  Eigen::Map<Eigen::RowVectorX<T>> map_src(buffer.data(), buffer.size());

  Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 3, Eigen::RowMajor>> split_src(
      map_src.data(), buffer.size() / 3, 3);

  Eigen::Array<float, 1, 3> adjustments;
  adjustments << b.value_or(0), g.value_or(0), r.value_or(0);

  split_src = (split_src.template cast<float>().array().rowwise() + adjustments)
                  .cwiseMin(255.0f)
                  .cwiseMax(0.0f)
                  .template cast<T>();
}

template void Srgb::adjustChannels<unsigned char>(std::span<unsigned char>,
                                                  std::optional<unsigned char>,
                                                  std::optional<unsigned char>,
                                                  std::optional<unsigned char>);

}  // namespace psm
