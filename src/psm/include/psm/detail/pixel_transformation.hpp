#pragma once

#include <cmath>

#include "psm/detail/types.hpp"

namespace psm::detail {

/**
 * @brief Normalizes integer values in range [0, 255] to floating point values in range [0.0, 1.0]
 *
 * @tparam Derived The derived Eigen expression type
 * @param src Source data to normalize
 * @return A normalized expression with float values between 0.0 and 1.0
 *
 * @note This function works with any compatible Eigen expression (vectors, matrices, blocks, etc.)
 *
 * @code
 * Mat3f pixels = ...; // Some matrix with integer values 0-255
 * auto normalized = normalize(pixels); // Converts to floating point 0.0-1.0
 * @endcode
 */
template <typename Derived>
auto normalize(const Eigen::MatrixBase<Derived>& src) {
  return src.template cast<float>() / 255.0f;
}

/**
 * @brief Denormalizes floating point values in range [0.0, 1.0] back to integers in range [0, 255]
 *
 * @tparam Derived The derived Eigen expression type
 * @tparam ScalarType The output scalar type (defaults to the input type)
 * @param src Source normalized data (expected range: 0.0-1.0)
 * @return A denormalized expression with values clamped to 0-255 and cast to ScalarType
 *
 * @note Values outside [0.0, 1.0] will be clamped to valid range
 *
 * @code
 * auto normalized = normalize(pixels); // Floating point values 0.0-1.0
 * // Process normalized data...
 * auto denormalized = denormalize<uint8_t>(normalized); // Back to integers 0-255
 * @endcode
 */
template <typename Derived, typename ScalarType = typename Derived::Scalar>
auto denormalize(const Eigen::MatrixBase<Derived>& src) {
  return (src * 255.0f)
      .cwiseMin(255.0f)
      .cwiseMax(0.0f)
      .template cast<ScalarType>();
}

/**
 * @brief Explicitly specify the output type when denormalizing
 *
 * @tparam ScalarType The desired output scalar type
 * @tparam Derived The derived Eigen expression type
 * @param src Source normalized data (expected range: 0.0-1.0)
 * @return A denormalized expression with values clamped to 0-255 and cast to ScalarType
 *
 * @note This is useful when you need to explicitly control the output type
 *
 * @code
 * auto normalized = normalize(pixels); // Floating point values 0.0-1.0
 * // Process normalized data...
 * auto uint8_result = denormalize_as<uint8_t>(normalized); // Force uint8_t output
 * @endcode
 */
template <typename ScalarType, typename Derived>
auto denormalize_as(const Eigen::MatrixBase<Derived>& src) {
  return (src * 255.0f)
      .cwiseMin(255.0f)
      .cwiseMax(0.0f)
      .template cast<ScalarType>();
}

/**
 * @namespace transform
 * @brief Contains color space transformation utility functions
 */
namespace transform {

/**
 * @namespace gamma
 * @brief Gamma encoding/decoding functions
 */
namespace gamma {
/**
 * @brief Apply gamma encoding (linear to gamma)
 *
 * @tparam Derived Eigen expression type
 * @param src Source data
 * @param gamma Gamma value (default: 2.2)
 * @return Gamma-encoded data (normalized 0.0-1.0)
 */
template <typename Derived>
RowXf encode(const Eigen::MatrixBase<Derived>& src, float gamma = 2.2f) {
  const RowXf normalized = src.template cast<float>();
  return normalized.unaryExpr(
      [gamma](float v) { return std::pow(v, 1.0f / gamma); });
}

/**
 * @brief Apply gamma decoding (gamma to linear)
 *
 * @tparam Derived Eigen expression type
 * @param src Source data
 * @param gamma Gamma value (default: 2.2)
 * @return Linear data (normalized 0.0-1.0)
 */
template <typename Derived>
RowXf decode(const Eigen::MatrixBase<Derived>& src, float gamma = 2.2f) {
  const RowXf normalized = normalize(src);
  return normalized.unaryExpr([gamma](float v) { return std::pow(v, gamma); });
}
}  // namespace gamma

/**
 * @namespace srgb
 * @brief sRGB transfer function encoding/decoding
 */
namespace srgb {
/**
 * @brief sRGB encoding (linear to sRGB)
 *
 * @tparam Derived Eigen expression type
 * @param src Source data
 * @return sRGB encoded data (normalized 0.0-1.0)
 */
template <typename Derived>
RowXf encode(const Eigen::MatrixBase<Derived>& src) {
  const RowXf normalized = src.template cast<float>();

  return normalized.unaryExpr([](float value) {
    return (value <= 0.0031308f)
               ? (12.92f * value)
               : ((1.055f * std::pow(value, 1.0f / 2.4f)) - 0.055f);
  });
}

/**
 * @brief sRGB decoding (sRGB to linear)
 *
 * @tparam Derived Eigen expression type
 * @param src Source data
 * @return Linear data (normalized 0.0-1.0)
 */
template <typename Derived>
RowXf decode(const Eigen::MatrixBase<Derived>& src) {
  const RowXf normalized = normalize(src);

  return normalized.unaryExpr([](float value) {
    return (value <= 0.04045f) ? (value / 12.92f)
                               : std::pow((value + 0.055f) / 1.055f, 2.4f);
  });
}

}  // namespace srgb

}  // namespace transform

}  // namespace psm::detail
