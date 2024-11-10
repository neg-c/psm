#ifndef UTILS_HPP
#define UTILS_HPP

#include <Eigen/Dense>

using Mat3f = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;
using Mat4f = Eigen::Matrix<float, Eigen::Dynamic, 4, Eigen::RowMajor>;
using Mat3fView = Eigen::Map<Mat3f>;
using Mat4fView = Eigen::Map<Mat4f>;
using RowXf = Eigen::RowVectorXf;
using RowXfView = Eigen::Map<RowXf>;

namespace utils {

Mat4f scaleTo4d(Mat3f lcc, const Eigen::MatrixXf& alpha);
Mat3f switch_rb(Mat3f src);

}  // namespace utils

#endif  // UTILS_HPP
