#include "psm/detail/display_p3.hpp"

#include <Eigen/Dense>
#include <cmath>
#include <cstdint>

#include "psm/detail/colorspace.hpp"
#include "psm/detail/pixel_transformation.hpp"
#include "psm/detail/types.hpp"

namespace {

psm::detail::Mat3f xyz2display_p3(const psm::detail::Mat3f& src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << 2.4934969, -0.9313836, -0.4027107,
                  -0.8294889,  1.7626640,  0.0236247,
                   0.0358458, -0.0761724,  0.9568845;
  // clang-format on
  return src * transform_mat.transpose();
}

psm::detail::Mat3f display_p3_2xyz(const psm::detail::Mat3f& src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << 0.4865709, 0.2656677, 0.1982173,
                   0.2289746, 0.6917385, 0.0792869,
                   0.0000000, 0.0451134, 1.0439444;
  // clang-format on
  return src * transform_mat.transpose();
}

}  // namespace

namespace psm::detail {

template <typename T>
void DisplayP3::fromSRGB(std::span<const T> src, std::span<T> dst) {
  const Eigen::Map<const Eigen::RowVectorX<T>> map_src(src.data(), src.size());
  psm::detail::RowXf norm_src = transform::srgb::decode(map_src);

  // Assuming RGB/BGR as input
  const psm::detail::Mat3fView norm_rgb(norm_src.data(), norm_src.cols() / 3,
                                        3);
  const psm::detail::Mat3f xyz = psm::detail::srgbToXyz(norm_rgb);
  const psm::detail::Mat3f display_p3 = xyz2display_p3(xyz);

  const Eigen::Map<const psm::detail::RowXf> display_p3_row(
      display_p3.data(), display_p3.rows() * display_p3.cols());

  psm::detail::RowXf encoded_display_p3 =
      transform::srgb::encode(display_p3_row);

  const psm::detail::Mat3fView result(encoded_display_p3.data(),
                                      encoded_display_p3.size() / 3, 3);

  Eigen::Map<Eigen::RowVectorX<T>> dst_map(dst.data(), dst.size());
  dst_map = psm::detail::denormalize_as<T>(result);
}

template <typename T>
void DisplayP3::toSRGB(std::span<const T> src, std::span<T> dst) {
  const Eigen::Map<const Eigen::RowVectorX<T>> map_src(src.data(), src.size());
  psm::detail::RowXf norm_src = transform::srgb::decode(map_src);

  // Assuming RGB/BGR as input
  const psm::detail::Mat3fView display_p3(norm_src.data(), norm_src.cols() / 3,
                                          3);
  const psm::detail::Mat3f xyz = display_p3_2xyz(display_p3);
  const psm::detail::Mat3f srgb = psm::detail::xyzToSrgb(xyz);

  const Eigen::Map<const psm::detail::RowXf> srgb_row(
      srgb.data(), srgb.rows() * srgb.cols());
  psm::detail::RowXf encoded_srgb = transform::srgb::encode(srgb_row);

  const psm::detail::Mat3fView result(encoded_srgb.data(),
                                      encoded_srgb.size() / 3, 3);

  Eigen::Map<Eigen::RowVectorX<T>> dst_map(dst.data(), dst.size());
  dst_map = psm::detail::denormalize_as<T>(result);
}

template void DisplayP3::fromSRGB<unsigned char>(std::span<const unsigned char>,
                                                 std::span<unsigned char>);
template void DisplayP3::toSRGB<unsigned char>(std::span<const unsigned char>,
                                               std::span<unsigned char>);

template void DisplayP3::fromSRGB<std::uint16_t>(std::span<const std::uint16_t>,
                                                 std::span<std::uint16_t>);
template void DisplayP3::toSRGB<std::uint16_t>(std::span<const std::uint16_t>,
                                               std::span<std::uint16_t>);
}  // namespace psm::detail
