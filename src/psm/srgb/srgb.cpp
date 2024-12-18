#include "psm/srgb.hpp"

#include <Eigen/Dense>

namespace psm {

using Mat3f = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;
using Mat4f = Eigen::Matrix<float, Eigen::Dynamic, 4, Eigen::RowMajor>;
using Mat3fView = Eigen::Map<Mat3f>;
using Mat4fView = Eigen::Map<Mat4f>;
using RowXf = Eigen::RowVectorXf;
using RowXfView = Eigen::Map<RowXf>;

class SrgbImpl {
 public:
  SrgbImpl() = default;

  template <typename T>
  void toXYZ(std::span<const T> src, std::span<float> dst) {
    Eigen::Map<const Eigen::RowVectorX<T>> map_src(src.data(), src.size());
    RowXf norm_src = normalize(map_src);
    Mat4fView norm_4d(norm_src.data(), norm_src.cols() / 4, 4);
    Mat3f rgb = switch_rb(norm_4d.leftCols(3));

    Mat3f linear_rgb = rgb.unaryExpr([](float x) {
      return (x <= 0.04045f) ? x / 12.92f
                             : std::pow((x + 0.055f) / 1.055f, 2.4f);
    });

    Eigen::Matrix3f rgb2xyz;
    // clang-format off
    rgb2xyz << 0.4124564f, 0.3575761f, 0.1804375f,
               0.2126729f, 0.7151522f, 0.0721750f,
               0.0193339f, 0.1191920f, 0.9503041f;
    // clang-format on

    Mat3f xyz = linear_rgb * rgb2xyz;
    Mat4f xyza = scaleTo4d(xyz, norm_4d.rightCols(1));

    RowXfView result = RowXfView(xyza.data(), xyza.cols() * xyza.rows());
    Eigen::Map<Eigen::RowVectorX<float>> dst_map(dst.data(), dst.size());
    dst_map = (result * 255.0f).template cast<float>();
  }

  template <typename T>
  void fromXYZ(std::span<float> src, std::span<T> dst) {
    Eigen::Map<const Eigen::RowVectorX<float>> map_src(src.data(), src.size());
    RowXf norm_src = normalize(map_src);
    Mat4fView norm_4d(norm_src.data(), norm_src.cols() / 4, 4);
    Mat3f xyz = norm_4d.leftCols(3);

    Eigen::Matrix3f xyz2rgb;
    // clang-format off
    xyz2rgb <<  3.2404542f, -1.5371385f, -0.4985314f,
                -0.9692660f,  1.8760108f,  0.0415560f,
                0.0556434f, -0.2040259f,  1.0572252f;
    // clang-format on

    Mat3f linear_rgb = xyz * xyz2rgb;

    Mat3f srgb = linear_rgb.unaryExpr([](float x) {
      return (x <= 0.0031308f) ? 12.92f * x
                               : 1.055f * std::pow(x, 1.0f / 2.4f) - 0.055f;
    });

    Mat3f bgr = switch_rb(srgb);
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
};

Mat4f SrgbImpl::scaleTo4d(Mat3f lcc, const Eigen::MatrixXf& alpha) {
  Mat4f rgba(lcc.rows(), 4);
  rgba.leftCols(3) = lcc;
  rgba.col(rgba.cols() - 1) = alpha;
  return rgba;
}

Mat3f SrgbImpl::switch_rb(Mat3f src) {
  Mat3f bgr(src.rows(), 3);
  bgr.col(0) = src.col(2);
  bgr.col(1) = src.col(1);
  bgr.col(2) = src.col(0);
  return bgr;
}

Srgb::Srgb() : impl_(std::make_unique<SrgbImpl>()) {}
Srgb::~Srgb() = default;

Srgb::Srgb(Srgb&&) noexcept = default;
Srgb& Srgb::operator=(Srgb&&) noexcept = default;

template <typename T>
void Srgb::toXYZ(std::span<const T> src, std::span<float> dst) {
  impl_->toXYZ(src, dst);
}
template <typename T>
void Srgb::fromXYZ(std::span<float> src, std::span<T> dst) {
  impl_->fromXYZ(src, dst);
}

template void psm::Srgb::toXYZ<unsigned char>(std::span<const unsigned char>,
                                              std::span<float>);
template void psm::Srgb::fromXYZ<unsigned char>(std::span<float>,
                                                std::span<unsigned char>);
}  // namespace psm
