#include "psm/detail/display_p3.hpp"

#include <Eigen/Dense>
#include <cmath>

#include "psm/detail/types.hpp"

namespace {

template <typename T>
psm::detail::RowXf linearize(
    const Eigen::Map<const Eigen::RowVectorX<T>>& src) {
  const psm::detail::RowXf normalized = src.template cast<float>() / 255.0f;
  return normalized.unaryExpr([](float value) {
    return (value <= 0.04045f) ? (value / 12.92f)
                               : std::pow((value + 0.055f) / 1.055f, 2.4f);
  });
}

template <typename T>
psm::detail::RowXf delinearize(
    const Eigen::Map<const Eigen::RowVectorX<T>>& src) {
  const psm::detail::RowXf normalized = src.template cast<float>();
  return normalized.unaryExpr([](float value) {
    return (value <= 0.0031308f)
               ? (12.92f * value)
               : (1.055f * std::pow(value, 1.0f / 2.4f) - 0.055f);
  });
}

psm::detail::Mat3f rgb2xyz(const psm::detail::Mat3f& src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << 0.4124564f, 0.3575761f, 0.1804375f,
                   0.2126729f, 0.7151522f, 0.0721750f,
                   0.0193339f, 0.1191920f, 0.9503041f;
  // clang-format on
  return src * transform_mat.transpose();
}

psm::detail::Mat3f xyz2rgb(const psm::detail::Mat3f& src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << 3.2404542f, -1.5371385f, -0.4985314f,
                  -0.9692660f,  1.8760108f,  0.0415560f,
                   0.0556434f, -0.2040259f,  1.0572252f;
  // clang-format on
  return src * transform_mat.transpose();
}

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
void DisplayP3::fromSRGB(const std::span<const T>& src, std::span<T> dst) {
  const Eigen::Map<const Eigen::RowVectorX<T>> map_src(src.data(), src.size());
  psm::detail::RowXf norm_src = linearize(map_src);

  // Assuming RGB/BGR as input
  const psm::detail::Mat3fView norm_rgb(norm_src.data(), norm_src.cols() / 3,
                                        3);
  const psm::detail::Mat3f xyz = rgb2xyz(norm_rgb);
  const psm::detail::Mat3f display_p3 = xyz2display_p3(xyz);

  const Eigen::Map<const psm::detail::RowXf> display_p3_row(
      display_p3.data(), display_p3.rows() * display_p3.cols());

  psm::detail::RowXf encoded_display_p3 = delinearize(display_p3_row);

  const psm::detail::Mat3fView result(encoded_display_p3.data(),
                                      encoded_display_p3.size() / 3, 3);

  Eigen::Map<Eigen::RowVectorX<T>> dst_map(dst.data(), dst.size());
  dst_map = (result.reshaped<Eigen::RowMajor>() * 255.0f)
                .cwiseMin(255.0f)
                .cwiseMax(0.0f)
                .template cast<T>();
}

template <typename T>
void DisplayP3::toSRGB(const std::span<const T>& src, std::span<T> dst) {
  const Eigen::Map<const Eigen::RowVectorX<T>> map_src(src.data(), src.size());
  psm::detail::RowXf norm_src = linearize(map_src);

  // Assuming RGB/BGR as input
  const psm::detail::Mat3fView display_p3(norm_src.data(), norm_src.cols() / 3,
                                          3);
  const psm::detail::Mat3f xyz = display_p3_2xyz(display_p3);
  const psm::detail::Mat3f srgb = xyz2rgb(xyz);

  const Eigen::Map<const psm::detail::RowXf> srgb_row(
      srgb.data(), srgb.rows() * srgb.cols());
  psm::detail::RowXf encoded_srgb = delinearize(srgb_row);

  const psm::detail::Mat3fView result(encoded_srgb.data(),
                                      encoded_srgb.size() / 3, 3);

  Eigen::Map<Eigen::RowVectorX<T>> dst_map(dst.data(), dst.size());
  dst_map = (result.reshaped<Eigen::RowMajor>() * 255.0f)
                .cwiseMin(255.0f)
                .cwiseMax(0.0f)
                .template cast<T>();
}

template void DisplayP3::fromSRGB<unsigned char>(
    const std::span<const unsigned char>&, std::span<unsigned char>);
template void DisplayP3::toSRGB<unsigned char>(
    const std::span<const unsigned char>&, std::span<unsigned char>);
}  // namespace psm::detail
