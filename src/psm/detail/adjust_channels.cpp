#include "psm/detail/adjust_channels.hpp"

#include <Eigen/Dense>

namespace psm::detail {

template <typename T>
void adjustChannels(std::span<T> buffer, const Percent& adjust_percentage) {
  Eigen::Map<Eigen::RowVectorX<T>> map_src(buffer.data(), buffer.size());

  Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 3, Eigen::RowMajor>> split_src(
      map_src.data(), buffer.size() / 3, 3);

  Eigen::Array<float, 1, 3> adjustments;
  adjustments << adjust_percentage.channel0_ / 100.0f,
      adjust_percentage.channel1_ / 100.0f,
      adjust_percentage.channel2_ / 100.0f;

  split_src = (split_src.template cast<float>().array() *
               (1.0f + adjustments.replicate(split_src.rows(), 1)))
                  .cwiseMin(255.0f)
                  .cwiseMax(0.0f)
                  .template cast<T>();
}

template void adjustChannels<unsigned char>(std::span<unsigned char>,
                                            const Percent&);

}  // namespace psm::detail
