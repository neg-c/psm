#pragma once

#include "psm/detail/types.hpp"

namespace psm::detail {

/**
 * @brief Convert from sRGB color space to CIE XYZ color space
 * 
 * @param src Source RGB data in sRGB color space
 * @return Data in XYZ color space
 * 
 * @note This assumes the input is in linear space (not gamma-encoded)
 * @note If your input is in standard sRGB, you should first decode the gamma:
 * @code
 * auto linear_rgb = transform::srgb::decode<uint8_t>(input_data);
 * auto xyz = srgbToXyz(linear_rgb);
 * @endcode
 */
inline Mat3f srgbToXyz(const Mat3f& src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << 0.4124564f, 0.3575761f, 0.1804375f,
                   0.2126729f, 0.7151522f, 0.0721750f, 
                   0.0193339f, 0.1191920f, 0.9503041f;
  // clang-format on
  return src * transform_mat.transpose();
}

/**
 * @brief Convert from CIE XYZ color space to sRGB color space
 * 
 * @param src Source data in XYZ color space
 * @return Data in linear RGB color space
 * 
 * @note This produces linear RGB values. To convert to standard sRGB, apply sRGB encoding:
 * @code
 * auto linear_rgb = xyzToSrgb(xyz_data);
 * auto srgb = transform::srgb::encode<float>(linear_rgb);
 * @endcode
 */
inline Mat3f xyzToSrgb(const Mat3f& src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << 3.2404542f, -1.5371385f, -0.4985314f,
                  -0.9692660f,  1.8760108f,  0.0415560f,
                   0.0556434f, -0.2040259f,  1.0572252f;
  // clang-format on
  return src * transform_mat.transpose();
}

}  // namespace psm::detail
