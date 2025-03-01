#include "psm/detail/adobe_rgb.hpp"

#include <Eigen/Dense>
#include <cmath>

#include "psm/detail/colorspace.hpp"
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

psm::detail::Mat3f xyz2adobe_rgb(const psm::detail::Mat3f& src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << 2.0413690f, -0.5649464f, -0.3446944f,
                   -0.9692660f, 1.8760108f, 0.0415560f,
                   0.0134474f, -0.1183897f, 1.0154096f;
  // clang-format on
  return src * transform_mat.transpose();
}

psm::detail::Mat3f adobe_rgb2xyz(const psm::detail::Mat3f& src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << 0.5767309f,  0.1855540f,  0.1881852f,
                   0.2973769f,  0.6273491f,  0.0752741f,
                   0.0270343f,  0.0706872f,  0.9911085f;
  // clang-format on
  return src * transform_mat.transpose();
}
}  // namespace

namespace psm::detail {

template <typename T>
void AdobeRgb::fromSRGB(const std::span<const T>& src, std::span<T> dst) {
  const Eigen::Map<const Eigen::RowVectorX<T>> map_src(src.data(), src.size());
  psm::detail::RowXf norm_src = linearize(map_src);

  // Assuming RGB/BGR as input
  const psm::detail::Mat3fView norm_rgb(norm_src.data(), norm_src.cols() / 3,
                                        3);
  const psm::detail::Mat3f xyz = psm::detail::srgbToXyz(norm_rgb);
  const psm::detail::Mat3f adobe_rgb = xyz2adobe_rgb(xyz);

  const Eigen::Map<const psm::detail::RowXf> adobe_rgb_row(
      adobe_rgb.data(), adobe_rgb.rows() * adobe_rgb.cols());
  psm::detail::RowXf encoded_adobe_rgb = delinearize(adobe_rgb_row);

  const psm::detail::Mat3fView result(encoded_adobe_rgb.data(),
                                      encoded_adobe_rgb.size() / 3, 3);

  Eigen::Map<Eigen::RowVectorX<T>> dst_map(dst.data(), dst.size());
  dst_map = (result.reshaped<Eigen::RowMajor>() * 255.0f)
                .cwiseMin(255.0f)
                .cwiseMax(0.0f)
                .template cast<T>();
}

template <typename T>
void AdobeRgb::toSRGB(const std::span<const T>& src, std::span<T> dst) {
  const Eigen::Map<const Eigen::RowVectorX<T>> map_src(src.data(), src.size());
  psm::detail::RowXf norm_src = linearize(map_src);

  // Assuming RGB/BGR as input
  const psm::detail::Mat3fView adobe_rgb(norm_src.data(), norm_src.cols() / 3,
                                         3);
  const psm::detail::Mat3f xyz = adobe_rgb2xyz(adobe_rgb);
  const psm::detail::Mat3f srgb = psm::detail::xyzToSrgb(xyz);

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

template void AdobeRgb::fromSRGB<unsigned char>(
    const std::span<const unsigned char>&, std::span<unsigned char>);
template void AdobeRgb::toSRGB<unsigned char>(
    const std::span<const unsigned char>&, std::span<unsigned char>);
}  // namespace psm::detail
