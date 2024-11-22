#include "psm/orgb.hpp"

#include <Eigen/Dense>
#include <memory>
#include <numbers>

namespace psm {

using Mat3f = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;
using Mat4f = Eigen::Matrix<float, Eigen::Dynamic, 4, Eigen::RowMajor>;
using Mat3fView = Eigen::Map<Mat3f>;
using Mat4fView = Eigen::Map<Mat4f>;
using RowXf = Eigen::RowVectorXf;
using RowXfView = Eigen::Map<RowXf>;

class OrgbImpl {
 public:
  OrgbImpl() = default;

  template <typename T>
  void srgb2orgb(std::span<const T> src, std::span<T> dst,
                 std::size_t channel) {
    Eigen::Map<const Eigen::RowVectorX<T>> map_src(src.data(), src.size());

    RowXf norm_src = normalize(map_src);
    Mat4fView norm_4d(norm_src.data(), norm_src.cols() / channel, channel);
    Mat3f norm_rgb = switch_rb(norm_4d.leftCols(3));
    Mat3f lcc = rgb2lcc(norm_rgb);
    Mat3f orgb = lcc2orgb(lcc);

    Mat3f back_lcc = orgb2lcc(orgb);
    Mat3f rgb = lcc2rgb(back_lcc);
    Mat3f bgr = switch_rb(rgb);
    Mat4f bgra = scaleTo4d(bgr, norm_4d.rightCols(1));

    RowXfView result = RowXfView(bgra.data(), bgra.cols() * bgra.rows());

    Eigen::Map<Eigen::RowVectorX<T>> dst_map(dst.data(), dst.size());
    dst_map = (result * 255).cwiseMin(255).cwiseMax(0).template cast<T>();
  }

 private:
  Mat4f scaleTo4d(Mat3f lcc, const Eigen::MatrixXf& alpha);
  Mat3f switch_rb(Mat3f src);
  template <typename T>
  RowXf normalize(const Eigen::Map<const Eigen::RowVectorX<T>>& src) {
    return src.template cast<float>() / 255.0f;
  }
  double convertToRGBangle(double theta);
  double convertToOrgbangle(double theta);
  Mat3f rgb2lcc(Mat3f src);
  Mat3f lcc2rgb(Mat3f lcc);
  Mat3f lcc2orgb(Mat3f lcc);
  Mat3f orgb2lcc(const Mat3f& orgb);
};

Mat4f OrgbImpl::scaleTo4d(Mat3f lcc, const Eigen::MatrixXf& alpha) {
  Mat4f rgba(lcc.rows(), 4);
  rgba.leftCols(3) = lcc;
  rgba.col(rgba.cols() - 1) = alpha;
  return rgba;
}

Mat3f OrgbImpl::switch_rb(Mat3f src) {
  Mat3f bgr(src.rows(), 3);
  bgr.col(0) = src.col(2);
  bgr.col(1) = src.col(1);
  bgr.col(2) = src.col(0);
  return bgr;
}

double OrgbImpl::convertToRGBangle(double theta) {
  if (theta < std::numbers::pi / 3) {
    return (3.0f / 2.0f) * theta;
  } else if (theta <= std::numbers::pi && theta >= std::numbers::pi / 3) {
    return std::numbers::pi / 2 +
           (3.0f / 4.0f) * (theta - std::numbers::pi / 3);
  }
  return -1337;
}

double OrgbImpl::convertToOrgbangle(double theta) {
  if (theta < std::numbers::pi / 2) {
    return (2.0f / 3.0f) * theta;
  } else if (theta <= std::numbers::pi && theta >= std::numbers::pi / 2) {
    return std::numbers::pi / 3 +
           (4.0f / 3.0f) * (theta - std::numbers::pi / 2);
  }
  return -1337;
}

Mat3f OrgbImpl::rgb2lcc(Mat3f src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
    transform_mat << 0.2990f, 0.5870f, 0.1140f,
                     0.5000f, 0.5000f, -1.0000f,
                     0.8660f, -0.8660f, 0.0000f;
  // clang-format on
  return src * transform_mat;
}

Mat3f OrgbImpl::lcc2rgb(Mat3f lcc) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
    transform_mat << 1.0000f, 0.1140f, 0.7436f,
                     1.0000f, 0.1140f, -0.4111f,
                     1.0000f, -0.8660f, 0.1663f;
  // clang-format on
  return lcc * transform_mat;
}

Mat3f OrgbImpl::lcc2orgb(Mat3f lcc) {
  Mat3f orgb(lcc.rows(), 3);

  for (int i = 0; i < lcc.rows(); ++i) {
    double L = lcc(i, 0);
    double C1 = lcc(i, 1);
    double C2 = lcc(i, 2);

    double theta = std::atan2(C2, C1);
    double orgb_theta =
        (theta > 0.0f) ? convertToRGBangle(theta) : -convertToRGBangle(-theta);
    double angle = orgb_theta - theta;

    Eigen::Matrix2f rotation_matrix;
    // clang-format off
    rotation_matrix << std::cos(angle), -std::sin(angle),
                       std::sin(angle), std::cos(angle);
    // clang-format on
    Eigen::Vector2f C1C2(C1, C2);
    Eigen::Vector2f CybCrg = rotation_matrix * C1C2;
    orgb(i, 0) = L;
    orgb(i, 1) = CybCrg(0);
    orgb(i, 2) = CybCrg(1);
  }
  return orgb;
}

Mat3f OrgbImpl::orgb2lcc(const Mat3f& orgb) {
  Mat3f lcc(orgb.rows(), 3);

  for (int i = 0; i < orgb.rows(); ++i) {
    double L = orgb(i, 0);
    double Cyb = orgb(i, 1);
    double Crg = orgb(i, 2);

    double theta = std::atan2(Crg, Cyb);
    double rgb_theta = (theta > 0.0f) ? convertToOrgbangle(theta)
                                      : -convertToOrgbangle(-theta);
    double angle = rgb_theta - theta;

    Eigen::Matrix2f rotation_matrix;
    // clang-format off
    rotation_matrix << std::cos(angle), -std::sin(angle),
                       std::sin(angle), std::cos(angle);
    // clang-format on
    Eigen::Vector2f CybCrg(Cyb, Crg);
    Eigen::Vector2f C1C2 = rotation_matrix * CybCrg;

    lcc(i, 0) = L;
    lcc(i, 1) = C1C2(0);
    lcc(i, 2) = C1C2(1);
  }
  return lcc;
}

Orgb::Orgb() : impl_(std::make_unique<OrgbImpl>()) {}
Orgb::~Orgb() = default;

Orgb::Orgb(Orgb&&) noexcept = default;
Orgb& Orgb::operator=(Orgb&&) noexcept = default;

template <typename T>
void Orgb::convert(std::span<const T> src, std::span<T> dst) {
  impl_->srgb2orgb(src, dst, 4);
}

template void psm::Orgb::convert<unsigned char>(std::span<const unsigned char>,
                                                std::span<unsigned char>);
}  // namespace psm
