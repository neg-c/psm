#include "psm/detail/dci_p3.hpp"

#include <Eigen/Dense>
#include <cmath>

namespace {

using Mat3f = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;
using Mat4f = Eigen::Matrix<float, Eigen::Dynamic, 4, Eigen::RowMajor>;
using Mat3fView = Eigen::Map<Mat3f>;
using Mat4fView = Eigen::Map<Mat4f>;
using RowXf = Eigen::RowVectorXf;
using RowXfView = Eigen::Map<RowXf>;

Mat3f switch_rb(Mat3f src) {
  Mat3f bgr(src.rows(), 3);
  bgr.col(0) = src.col(2);
  bgr.col(1) = src.col(1);
  bgr.col(2) = src.col(0);
  return bgr;
}

template <typename T>
RowXf linearize(const Eigen::Map<const Eigen::RowVectorX<T>>& src) {
  const RowXf normalized = src.template cast<float>() / 255.0f;
  return normalized.unaryExpr([](float value) {
    return (value <= 0.04045f) ? (value / 12.92f)
                               : std::pow((value + 0.055f) / 1.055f, 2.4f);
  });
}

template <typename T>
RowXf delinearize(const Eigen::Map<const Eigen::RowVectorX<T>>& src) {
  const RowXf normalized = src.template cast<float>();
  return normalized.unaryExpr([](float value) {
    return (value <= 0.0031308f)
               ? (12.92f * value)
               : (1.055f * std::pow(value, 1.0f / 2.4f) - 0.055f);
  });
}

Mat3f rgb2xyz(const Mat3f& src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << 0.4124564f, 0.3575761f, 0.1804375f,
                   0.2126729f, 0.7151522f, 0.0721750f,
                   0.0193339f, 0.1191920f, 0.9503041f;
  // clang-format on
  return src * transform_mat.transpose();
}

Mat3f xyz2rgb(const Mat3f& src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << 3.2404542f, -1.5371385f, -0.4985314f,
                  -0.9692660f,  1.8760108f,  0.0415560f,
                   0.0556434f, -0.2040259f,  1.0572252f;
  // clang-format on
  return src * transform_mat.transpose();
}

Mat3f xyz2dci_p3(const Mat3f& src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << 0.4865709, 0.2656677, 0.1982173,
                   0.2289746, 0.6917385, 0.0792869,
                   0.0000000, 0.0451134, 1.0439444;
  // clang-format on
  return src * transform_mat.transpose();
}

Mat3f dci_p3_2xyz(const Mat3f& src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << 2.4934969, -0.9313836, -0.4027107,
                  -0.8294889,  1.7626640,  0.0236247,
                   0.0358458, -0.0761724,  0.9568845;
  // clang-format on
  return src * transform_mat.transpose();
}

}  // namespace

namespace psm::detail {

template <typename T>
void DciP3::fromSRGB(const std::span<const T>& src, std::span<T> dst) {
  const Eigen::Map<const Eigen::RowVectorX<T>> map_src(src.data(), src.size());
  RowXf norm_src = linearize(map_src);

  // Assuming BGR as input
  const Mat3fView norm_bgr(norm_src.data(), norm_src.cols() / 3, 3);
  const Mat3f norm_rgb = switch_rb(norm_bgr);
  const Mat3f xyz = rgb2xyz(norm_rgb);
  const Mat3f dci_p3 = xyz2dci_p3(xyz);
  const Mat3f dci_p3_final = switch_rb(dci_p3);

  const Eigen::Map<const RowXf> dci_p3_row(
      dci_p3_final.data(), dci_p3_final.rows() * dci_p3_final.cols());

  RowXf encoded_dci_p3 = delinearize(dci_p3_row);

  const Mat3fView result(encoded_dci_p3.data(), encoded_dci_p3.size() / 3, 3);

  Eigen::Map<Eigen::RowVectorX<T>> dst_map(dst.data(), dst.size());
  dst_map = (result.reshaped<Eigen::RowMajor>() * 255.0f)
                .cwiseMin(255.0f)
                .cwiseMax(0.0f)
                .template cast<T>();
}

template <typename T>
void DciP3::toSRGB(const std::span<const T>& src, std::span<T> dst) {
  const Eigen::Map<const Eigen::RowVectorX<T>> map_src(src.data(), src.size());
  RowXf norm_src = linearize(map_src);

  // Assuming BGR as input
  const Mat3fView norm_bgr(norm_src.data(), norm_src.cols() / 3, 3);
  const Mat3f dci_p3_final = switch_rb(norm_bgr);
  const Mat3f xyz = dci_p3_2xyz(dci_p3_final);
  const Mat3f srgb = xyz2rgb(xyz);
  const Mat3f srgb_bgr = switch_rb(srgb);

  const Eigen::Map<const RowXf> srgb_row(srgb_bgr.data(),
                                         srgb_bgr.rows() * srgb_bgr.cols());
  RowXf encoded_srgb = delinearize(srgb_row);

  const Mat3fView result(encoded_srgb.data(), encoded_srgb.size() / 3, 3);

  Eigen::Map<Eigen::RowVectorX<T>> dst_map(dst.data(), dst.size());
  dst_map = (result.reshaped<Eigen::RowMajor>() * 255.0f)
                .cwiseMin(255.0f)
                .cwiseMax(0.0f)
                .template cast<T>();
}

template void DciP3::fromSRGB<unsigned char>(
    const std::span<const unsigned char>&, std::span<unsigned char>);
template void DciP3::toSRGB<unsigned char>(
    const std::span<const unsigned char>&, std::span<unsigned char>);
}  // namespace psm::detail
