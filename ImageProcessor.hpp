#pragma once

#include <Eigen/Dense>
#include <span>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace cvt {

using Mat3f = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;
using Mat4f = Eigen::Matrix<float, Eigen::Dynamic, 4, Eigen::RowMajor>;
using Mat3fView = Eigen::Map<Mat3f>;
using Mat4fView = Eigen::Map<Mat4f>;
using RowXf = Eigen::RowVectorXf;
using RowXfView = Eigen::Map<RowXf>;

template <typename T>
void Color(std::span<const T> src, std::span<T> dst, Format src_format,
           Format dst_format) {
  auto cvt_format_it =
      conversion_table.find(std::make_pair(src_format, dst_format));

  switch (cvt_format_it->second) {
    case Conversion::ksRGB2oRGB:
      srgb2orgb(src, dst, 4);
      break;
    default:
      throw std::invalid_argument("Unsupported format");
  }
}

}  // namespace cvt
