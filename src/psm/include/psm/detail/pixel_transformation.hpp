#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

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
 * @brief Normalizes pixel values of any unsigned integer or float type to [0.0, 1.0]
 *
 * - Unsigned integers: divides by std::numeric_limits<Scalar>::max()
 * - Floats: pass-through cast to float (assumes already [0.0, 1.0])
 */
template <typename Derived>
auto normalize_pixels(const Eigen::MatrixBase<Derived>& src) {
  using Scalar = typename Derived::Scalar;
  if constexpr (std::is_floating_point_v<Scalar>) {
    return src.template cast<float>();
  } else if constexpr (std::is_integral_v<Scalar> &&
                       std::is_unsigned_v<Scalar>) {
    const float scale = static_cast<float>(std::numeric_limits<Scalar>::max());
    return src.template cast<float>() / scale;
  } else {
    static_assert(std::is_same_v<Scalar, void>,
                  "normalize_pixels requires unsigned integral or "
                  "floating-point scalars");
  }
}

/**
 * @brief Normalizes 16-bit integer values in range [0, 65535] to floating point values in range [0.0, 1.0]
 *
 * @tparam Derived The derived Eigen expression type
 * @param src Source data to normalize
 * @return A normalized expression with float values between 0.0 and 1.0
 *
 * @note This function works with any compatible Eigen expression (vectors, matrices, blocks, etc.)
 *
 * @code
 * Mat3f pixels = ...; // Some matrix with 16-bit integer values 0-65535
 * auto normalized = normalize16(pixels); // Converts to floating point 0.0-1.0
 * @endcode
 */
template <typename Derived>
auto normalize16(const Eigen::MatrixBase<Derived>& src) {
  return src.template cast<float>() / 65535.0f;
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
  const float max_value =
      static_cast<float>(std::numeric_limits<ScalarType>::max());
  return (src * max_value)
      .cwiseMin(max_value)
      .cwiseMax(0.0f)
      .template cast<ScalarType>();
}

/**
 * @brief Denormalizes floating point values in range [0.0, 1.0] back to 16-bit integers in range [0, 65535]
 *
 * @tparam Derived The derived Eigen expression type
 * @tparam ScalarType The output scalar type (defaults to the input type)
 * @param src Source normalized data (expected range: 0.0-1.0)
 * @return A denormalized expression with values clamped to 0-65535 and cast to ScalarType
 *
 * @note Values outside [0.0, 1.0] will be clamped to valid range
 *
 * @code
 * auto normalized = normalize16(pixels); // Floating point values 0.0-1.0
 * // Process normalized data...
 * auto denormalized = denormalize16<uint16_t>(normalized); // Back to integers 0-65535
 * @endcode
 */
template <typename Derived, typename ScalarType = typename Derived::Scalar>
auto denormalize16(const Eigen::MatrixBase<Derived>& src) {
  return denormalize<Derived, ScalarType>(src);
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
  const float max_value =
      static_cast<float>(std::numeric_limits<ScalarType>::max());
  return (src * max_value)
      .cwiseMin(max_value)
      .cwiseMax(0.0f)
      .template cast<ScalarType>();
}

/**
 * @brief Explicitly specify the output type when denormalizing to 16-bit
 *
 * @tparam ScalarType The desired output scalar type
 * @tparam Derived The derived Eigen expression type
 * @param src Source normalized data (expected range: 0.0-1.0)
 * @return A denormalized expression with values clamped to 0-65535 and cast to ScalarType
 *
 * @note This is useful when you need to explicitly control the output type for 16-bit data
 *
 * @code
 * auto normalized = normalize16(pixels); // Floating point values 0.0-1.0
 * // Process normalized data...
 * auto uint16_result = denormalize_as16<uint16_t>(normalized); // Force uint16_t output
 * @endcode
 */
template <typename ScalarType, typename Derived>
auto denormalize_as16(const Eigen::MatrixBase<Derived>& src) {
  return denormalize_as<ScalarType>(src);
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
  const RowXf normalized = normalize_pixels(src);
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
  const RowXf normalized = normalize_pixels(src);

  return normalized.unaryExpr([](float value) {
    return (value <= 0.04045f) ? (value / 12.92f)
                               : std::pow((value + 0.055f) / 1.055f, 2.4f);
  });
}

/**
 * @brief sRGB decoding for 16-bit data (sRGB to linear)
 *
 * @tparam Derived Eigen expression type
 * @param src Source data
 * @return Linear data (normalized 0.0-1.0)
 */
template <typename Derived>
RowXf decode16(const Eigen::MatrixBase<Derived>& src) {
  // For backwards compatibility; generic decode handles any unsigned depth
  return decode(src);
}

}  // namespace srgb

}  // namespace transform

}  // namespace psm::detail
