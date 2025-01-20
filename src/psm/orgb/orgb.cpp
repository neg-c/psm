#include "psm/detail/orgb.hpp"

#include <Eigen/Dense>
#include <cmath>
#include <numbers>

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
RowXf normalize(const Eigen::Map<const Eigen::RowVectorX<T>>& src) {
  return src.template cast<float>() / 255.0f;
}

float convertToRGBangle(float theta) {
  theta = std::clamp(theta, 0.0f, std::numbers::pi_v<float>);
  if (theta < std::numbers::pi_v<float> / 3.0f) {
    return (3.0f / 2.0f) * theta;
  }
  return std::numbers::pi_v<float> / 2.0f +
         (3.0f / 4.0f) * (theta - std::numbers::pi_v<float> / 3.0f);
}

float convertToOrgbangle(float theta) {
  theta = std::clamp(theta, 0.0f, std::numbers::pi_v<float>);
  if (theta < std::numbers::pi_v<float> / 2.0f) {
    return (2.0f / 3.0f) * theta;
  }

  return std::numbers::pi_v<float> / 3 +
         (4.0f / 3.0f) * (theta - std::numbers::pi_v<float> / 2);
}

Mat3f rgb2lcc(const Mat3f& src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << 0.2990f, 0.5870f, 0.1140f,
                   0.5000f, 0.5000f, -1.0000f,
                   0.8660f, -0.8660f, 0.0000f;
  // clang-format on
  return src * transform_mat;
}

Mat3f lcc2rgb(const Mat3f& lcc) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << 1.0000f, 0.1140f, 0.7436f,
                   1.0000f, 0.1140f, -0.4111f,
                   1.0000f, -0.8660f, 0.1663f;
  // clang-format on
  return lcc * transform_mat;
}

Mat3f lcc2orgb(Mat3f lcc) {
  Mat3f orgb(lcc.rows(), 3);

  for (int i = 0; i < lcc.rows(); ++i) {
    const float L = lcc(i, 0);
    const float C1 = lcc(i, 1);
    const float C2 = lcc(i, 2);

    const float theta = std::atan2(C2, C1);
    const float orgb_theta =
        (theta > 0.0f) ? convertToRGBangle(theta) : -convertToRGBangle(-theta);
    const float angle = orgb_theta - theta;

    Eigen::Matrix2f rotation_matrix;
    // clang-format off
    rotation_matrix << std::cos(angle), -std::sin(angle),
                      std::sin(angle), std::cos(angle);
    // clang-format on
    const Eigen::Vector2f C1C2(C1, C2);
    const Eigen::Vector2f CybCrg = rotation_matrix * C1C2;
    orgb(i, 0) = L;
    orgb(i, 1) = CybCrg(0);
    orgb(i, 2) = CybCrg(1);
  }
  return orgb;
}

Mat3f orgb2lcc(const Mat3f& orgb) {
  Mat3f lcc(orgb.rows(), 3);

  for (int i = 0; i < orgb.rows(); ++i) {
    const float L = orgb(i, 0);
    const float Cyb = orgb(i, 1);
    const float Crg = orgb(i, 2);

    const float theta = std::atan2(Crg, Cyb);
    const float rgb_theta = (theta > 0.0f) ? convertToOrgbangle(theta)
                                           : -convertToOrgbangle(-theta);
    const float angle = rgb_theta - theta;

    Eigen::Matrix2f rotation_matrix;
    // clang-format off
    rotation_matrix << std::cos(angle), -std::sin(angle),
                      std::sin(angle), std::cos(angle);
    // clang-format on
    const Eigen::Vector2f CybCrg(Cyb, Crg);
    const Eigen::Vector2f C1C2 = rotation_matrix * CybCrg;

    lcc(i, 0) = L;
    lcc(i, 1) = C1C2(0);
    lcc(i, 2) = C1C2(1);
  }
  return lcc;
}
}  // namespace

namespace psm::detail {
template <typename T>
void Orgb::fromSRGB(const std::span<T>& src, std::span<T> dst) {
  const Eigen::Map<const Eigen::RowVectorX<T>> map_src(src.data(), src.size());
  RowXf norm_src = normalize(map_src);

  // Assuming BGR as input
  const Mat3fView norm_bgr(norm_src.data(), norm_src.cols() / 3, 3);
  const Mat3f norm_rgb = switch_rb(norm_bgr);
  const Mat3f lcc = rgb2lcc(norm_rgb);
  Mat3f orgb = lcc2orgb(lcc);
  // map [-1, 2] to [0, 1] to preserve data when converting back to sRGB
  Mat3f shifted_orgb = ((orgb.array() + 1.0f) / 3.0f).min(1.0f).max(0.0f);

  const RowXfView result(shifted_orgb.data(),
                         shifted_orgb.cols() * shifted_orgb.rows());
  Eigen::Map<Eigen::RowVectorX<T>> dst_map(dst.data(), dst.size());
  dst_map =
      (result * 255.0f).cwiseMin(255.0f).cwiseMax(0.0f).template cast<T>();
}

template <typename T>
void Orgb::toSRGB(const std::span<T>& src, std::span<T> dst) {
  const Eigen::Map<const Eigen::RowVectorX<T>> map_src(src.data(), src.size());
  RowXf norm_src = map_src.template cast<float>() / 255.0f;

  // Assuming ORGB as input
  Mat3fView norm_obgr(norm_src.data(), norm_src.cols() / 3, 3);

  // remap [0, 1] back to [-1, 2] to preserve data
  const Mat3f unshifted_orgb =
      ((norm_obgr.array() * 3.0f) - 1.0f).min(2.0f).max(-1.0f);

  const Mat3f lcc = orgb2lcc(unshifted_orgb);
  const Mat3f rgb = lcc2rgb(lcc);
  Mat3f bgr = switch_rb(rgb);

  const RowXfView result(bgr.data(), bgr.cols() * bgr.rows());
  Eigen::Map<Eigen::RowVectorX<T>> dst_map(dst.data(), dst.size());
  dst_map =
      (result * 255.0f).cwiseMin(255.0f).cwiseMax(0.0f).template cast<T>();
}

template void Orgb::fromSRGB<unsigned char>(const std::span<unsigned char>&,
                                            std::span<unsigned char>);
template void Orgb::toSRGB<unsigned char>(const std::span<unsigned char>&,
                                          std::span<unsigned char>);
}  // namespace psm::detail
