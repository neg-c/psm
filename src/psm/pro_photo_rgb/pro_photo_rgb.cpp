#include "psm/detail/pro_photo_rgb.hpp"

#include <Eigen/Dense>
#include <cmath>
#include <cstdint>

#include "psm/detail/colorspace.hpp"
#include "psm/detail/pixel_transformation.hpp"
#include "psm/detail/types.hpp"

namespace {

psm::detail::Mat3f xyz2pro_photo_rgb(const psm::detail::Mat3f& src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << 1.3460f, -0.2556f, -0.0511f,
                  -0.5446f,  1.5082f,  0.0205f,
                   0.0f,     0.0f,     1.2123f;
  // clang-format on
  return src * transform_mat.transpose();
}

psm::detail::Mat3f pro_photo_rgb2xyz(const psm::detail::Mat3f& src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << 0.7974f,   0.1352f,   0.03131f,
                   0.2880f,   0.7118f,   0.00008f,
                   0.0f,      0.0f,      0.8251f;
  // clang-format on
  return src * transform_mat.transpose();
}

}  // namespace

namespace psm::detail {

template <typename T>
void ProPhotoRgb::fromSRGB(std::span<const T> src, std::span<T> dst) {
  const Eigen::Map<const Eigen::RowVectorX<T>> map_src(src.data(), src.size());

  psm::detail::RowXf norm_src = transform::srgb::decode(map_src);

  // Assuming RGB/BGR as input
  const psm::detail::Mat3fView norm_rgb(norm_src.data(), norm_src.cols() / 3,
                                        3);
  const psm::detail::Mat3f xyz65 = psm::detail::srgbToXyz(norm_rgb);

  // Bradford chromatic adaptation matrix from D65 to D50
  Eigen::Matrix3f bradford;
  // clang-format off
  bradford << 1.0478f, 0.0229f, -0.0501f,
             0.0295f, 0.9904f, -0.0170f,
            -0.0092f, 0.0150f,  0.7521f;
  // clang-format on
  const psm::detail::Mat3f xyz50 = xyz65 * bradford.transpose();

  const psm::detail::Mat3f pro_photo_rgb = xyz2pro_photo_rgb(xyz50);

  const Eigen::Map<const psm::detail::RowXf> pro_photo_rgb_row(
      pro_photo_rgb.data(), pro_photo_rgb.rows() * pro_photo_rgb.cols());

  psm::detail::RowXf encoded_pro_photo_rgb =
      pro_photo_rgb_row.unaryExpr([](float value) {
        return (value < 1.0f / 512.0f) ? (16 * value)
                                       : (std::pow(value, 1.0f / 1.8f));
      });

  const psm::detail::Mat3fView result(encoded_pro_photo_rgb.data(),
                                      encoded_pro_photo_rgb.size() / 3, 3);

  Eigen::Map<Eigen::RowVectorX<T>> dst_map(dst.data(), dst.size());

  dst_map = psm::detail::denormalize_as<T>(result);
}

template <typename T>
void ProPhotoRgb::toSRGB(std::span<const T> src, std::span<T> dst) {
  const Eigen::Map<const Eigen::RowVectorX<T>> map_src(src.data(), src.size());

  psm::detail::RowXf norm_src = psm::detail::normalize_pixels(map_src);

  psm::detail::RowXf decoded_pro_photo = norm_src.unaryExpr([](float value) {
    return (value < 16.0f / 512.0f) ? (value / 16.0f) : (std::pow(value, 1.8f));
  });

  // Assuming RGB/BGR as input
  const psm::detail::Mat3fView pro_photo_rgb(decoded_pro_photo.data(),
                                             decoded_pro_photo.cols() / 3, 3);

  const psm::detail::Mat3f xyz50 = pro_photo_rgb2xyz(pro_photo_rgb);

  // Bradford chromatic adaptation matrix from D50 to D65 (inverse of D65->D50)
  Eigen::Matrix3f bradford_inverse;
  // clang-format off
  bradford_inverse << 0.9555766, -0.0230393, 0.0631636,
                     -0.0282895,  1.0099416, 0.0210077,
                      0.0122982, -0.0204830, 1.3299098;
  // clang-format on

  const psm::detail::Mat3f xyz65 = xyz50 * bradford_inverse.transpose();

  const psm::detail::Mat3f srgb = psm::detail::xyzToSrgb(xyz65);

  const Eigen::Map<const psm::detail::RowXf> srgb_row(
      srgb.data(), srgb.rows() * srgb.cols());
  psm::detail::RowXf encoded_srgb = transform::srgb::encode(srgb_row);

  const psm::detail::Mat3fView result(encoded_srgb.data(),
                                      encoded_srgb.size() / 3, 3);

  Eigen::Map<Eigen::RowVectorX<T>> dst_map(dst.data(), dst.size());

  dst_map = psm::detail::denormalize_as<T>(result);
}

template void ProPhotoRgb::fromSRGB<unsigned char>(
    std::span<const unsigned char>, std::span<unsigned char>);
template void ProPhotoRgb::toSRGB<unsigned char>(std::span<const unsigned char>,
                                                 std::span<unsigned char>);

// Add 16-bit support
template void ProPhotoRgb::fromSRGB<std::uint16_t>(
    std::span<const std::uint16_t>, std::span<std::uint16_t>);
template void ProPhotoRgb::toSRGB<std::uint16_t>(std::span<const std::uint16_t>,
                                                 std::span<std::uint16_t>);
}  // namespace psm::detail
