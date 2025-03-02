#pragma once

#include <Eigen/Dense>

/**
 * @namespace psm::detail
 * @brief Contains implementation details for the PSM (Pixel Space Management) library.
 */
namespace psm::detail {

/** @brief Row-major matrix of floats with dynamic rows and 3 columns */
using Mat3f = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;

/** @brief Row-major matrix of floats with dynamic rows and 4 columns */
using Mat4f = Eigen::Matrix<float, Eigen::Dynamic, 4, Eigen::RowMajor>;

/** @brief View into a row-major 3-column float matrix */
using Mat3fView = Eigen::Map<Mat3f>;

/** @brief View into a row-major 4-column float matrix */
using Mat4fView = Eigen::Map<Mat4f>;

/** @brief Dynamic-sized row vector of floats */
using RowXf = Eigen::RowVectorXf;

/** @brief View into a dynamic-sized row vector of floats */
using RowXfView = Eigen::Map<RowXf>;

}  // namespace psm::detail
