#include "utils.hpp"

namespace utils {

Mat4f scaleTo4d(Mat3f lcc, const Eigen::MatrixXf& alpha) {
  Mat4f rgba(lcc.rows(), 4);
  rgba.leftCols(3) = lcc;
  rgba.col(rgba.cols() - 1) = alpha;
  return rgba;
}

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

}  // namespace utils
