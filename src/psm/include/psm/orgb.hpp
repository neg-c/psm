#ifndef ORGB_HPP
#define ORGB_HPP

#include <Eigen/Dense>
#include <span>

namespace psm {

using Mat3f = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;
using Mat4f = Eigen::Matrix<float, Eigen::Dynamic, 4, Eigen::RowMajor>;
using Mat3fView = Eigen::Map<Mat3f>;
using Mat4fView = Eigen::Map<Mat4f>;
using RowXf = Eigen::RowVectorXf;
using RowXfView = Eigen::Map<RowXf>;

Mat4f scaleTo4d(Mat3f lcc, const Eigen::MatrixXf& alpha);
Mat3f switch_rb(Mat3f src);
template <typename T>
RowXf normalize(const Eigen::Map<const Eigen::RowVectorX<T>>& src) {
  return src.template cast<float>() / 255.0f;
}

double convertToRGBangle(double theta);
double convertToORGBangle(double theta);
Mat3f rgb2lcc(Mat3f src);
Mat3f lcc2rgb(Mat3f lcc);
Mat3f lcc2orgb(Mat3f lcc);
Mat3f orgb2lcc(const Mat3f& orgb);

template <typename T>
void srgb2orgb(std::span<const T> src, std::span<T> dst, std::size_t channel) {
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

  RowXfView result =
      RowXfView(bgra.data(), bgra.cols() * bgra.rows() * channel);

  Eigen::Map<Eigen::RowVectorX<T>> dst_map(dst.data(), dst.size());
  dst_map = (result * 255).cwiseMin(255).cwiseMax(0).template cast<T>();
}

}  // namespace psm
#endif  // ORGB_HPP
