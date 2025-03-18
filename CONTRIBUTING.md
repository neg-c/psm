# CONTRIBUTE.md

Thank you for your interest in contributing to our project! This document
provides guidelines for contributing to the PSM (Prisma) library, with a focus
on adding new color space conversions.

## Table of Contents

- [Code Style](#code-style)
- [Adding a New Color Space](#adding-a-new-color-space)
  - [Step 1: Create Header Files](#step-1-create-header-files)
  - [Step 2: Implement Color Space Conversion](#step-2-implement-color-space-conversion)
  - [Step 3: Update Build Files](#step-3-update-build-files)
  - [Step 4: Write Unit Tests](#step-4-write-unit-tests)
- [Pull Request Process](#pull-request-process)

## Code Style

This project follows the Google C++ style guide with modifications defined in
the `.clang-format` file. Some key style points:

- Indentation: 2 spaces
- Line length: 80 characters maximum
- Braces: Attach style (same line)
- Class member access modifiers are indented with -1 spaces
- Header include organization: grouped by categories

## Adding a New Color Space

The PSM library allows for easy addition of new color space conversions. Below
is a step-by-step guide based on the ProPhoto RGB implementation.

### Step 1: Create Header Files

Create a header file for your color space in `src/psm/include/psm/detail/`
(e.g., `your_color_space.hpp`). We place color spaceheaders in the `detail/`
directory because they're not intended to be used directly by clients, but are
included and exposed through `psm.hpp`:

```cpp
#pragma once

#include <span>

namespace psm {

namespace detail {

class YourColorSpaceClass {
 public:
  YourColorSpaceClass() = delete;

  template <typename T>
  static void fromSRGB(const std::span<const T>& src, std::span<T> dst);
  template <typename T>
  static void toSRGB(const std::span<const T>& src, std::span<T> dst);
};

}  // namespace detail

struct YourColorSpace {
  using type = detail::YourColorSpaceClass;
};

}  // namespace psm
```

Then add your header include to `src/psm/include/psm/psm.hpp`. We use
conditional includes because color spaces are designed as optional modules that
can be enabled or disabled at build time:

```cpp
// ... existing code ...
#if __has_include("detail/your_color_space.hpp")
#include "detail/your_color_space.hpp"
#endif
// ... existing code ...
```

Note that your implementation must satisfy the `ColorSpaceType` concept defined
in `color_space_concept.hpp`. This requires:

1. A tag struct with a nested `type` (your implementation class)
2. Static `toSRGB` and `fromSRGB` methods in the implementation class
3. These methods must accept `std::span` parameters for input and output

This design enables tag-based dispatch for color space conversions.

### Step 2: Implement Color Space Conversion

Create an implementation file in a new directory
`src/psm/your_color_space/your_color_space.cpp`. Use the utility functions
provided in `colorspace.hpp`, `pixel_transformation.hpp`, and `types.hpp` to
simplify your implementation:

```cpp
#include "psm/detail/your_color_space.hpp"

#include <Eigen/Dense>
#include <cmath>

#include "psm/detail/colorspace.hpp"
#include "psm/detail/pixel_transformation.hpp"
#include "psm/detail/types.hpp"

namespace {
// Define color space transformation matrices in anonymous namespace
// These are implementation details not exposed to users
psm::detail::Mat3f xyz2your_color_space(const psm::detail::Mat3f& src) {
  // Define your transformation matrix from XYZ to your color space
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << a11, a12, a13,
                   a21, a22, a23,
                   a31, a32, a33;
  // clang-format on
  return src * transform_mat.transpose();
}

psm::detail::Mat3f your_color_space2xyz(const psm::detail::Mat3f& src) {
  // Define your transformation matrix from your color space to XYZ
  Eigen::Matrix3f transform_mat;
  // clang-format off
  transform_mat << b11, b12, b13,
                   b21, b22, b23,
                   b31, b32, b33;
  // clang-format on
  return src * transform_mat.transpose();
}
}  // namespace

namespace psm::detail {

template <typename T>
void YourColorSpaceClass::fromSRGB(const std::span<const T>& src, std::span<T> dst) {
  const Eigen::Map<const Eigen::RowVectorX<T>> map_src(src.data(), src.size());
  // Apply sRGB gamma decoding from transform::srgb namespace (defined in pixel_transformation.hpp)
  psm::detail::RowXf norm_src = transform::srgb::decode(map_src);

  // Convert from sRGB to XYZ
  // Create a view of the normalized RGB data as a matrix with 3 columns (RGB triplets)
  const psm::detail::Mat3fView norm_rgb(norm_src.data(), norm_src.cols() / 3, 3);
  // Use srgbToXyz utility function from colorspace.hpp to convert to XYZ color space
  const psm::detail::Mat3f xyz = psm::detail::srgbToXyz(norm_rgb);

  // Apply chromatic adaptation if needed (e.g., D65 to D50)
  // For example, with Bradford adaptation:
  //
  // Eigen::Matrix3f bradford;
  // bradford << 1.0478f, 0.0229f, -0.0501f,
  //            0.0295f, 0.9904f, -0.0170f,
  //           -0.0092f, 0.0150f,  0.7521f;
  // const psm::detail::Mat3f adapted_xyz = xyz * bradford.transpose();

  // Convert from XYZ to your color space
  const psm::detail::Mat3f your_color_space = xyz2your_color_space(xyz);

  // Apply appropriate gamma encoding for your color space
  const Eigen::Map<const psm::detail::RowXf> your_color_space_row(
      your_color_space.data(), your_color_space.rows() * your_color_space.cols());

  // Apply gamma correction if needed
  // For example:
  // psm::detail::RowXf encoded = your_color_space_row.unaryExpr([](float value) {
  //   return std::pow(value, 1.0f / gamma);
  // });

  // Map result back to output buffer
  Eigen::Map<Eigen::RowVectorX<T>> dst_map(dst.data(), dst.size());
  // Use denormalize_as utility from types.hpp to convert from normalized float values back to type T
  dst_map = psm::detail::denormalize_as<T>(your_color_space);
}

template <typename T>
void YourColorSpaceClass::toSRGB(const std::span<const T>& src, std::span<T> dst) {
  // Implement conversion back to sRGB (reverse of the above process)
  // See pro_photo_rgb.cpp for a complete example
}

// Explicitly instantiate templates for supported types
template void YourColorSpaceClass::fromSRGB<unsigned char>(
    const std::span<const unsigned char>&, std::span<unsigned char>);
template void YourColorSpaceClass::toSRGB<unsigned char>(
    const std::span<const unsigned char>&, std::span<unsigned char>);
}  // namespace psm::detail
```

### Step 3: Update Build Files

Add your module to `cmake/Options.cmake`. This step is necessary to register
your color space as an optional module in the build system:

```cmake
set(PSM_MODULES adjust_channels orgb adobe_rgb display_p3 pro_photo_rgb your_color_space)
```

Create `src/psm/your_color_space/CMakeLists.txt`:

```cmake
psm_add_module(psm_your_color_space SHARED)

target_sources(
  psm_your_color_space
  PRIVATE your_color_space.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS ${CMAKE_SOURCE_DIR}/src/psm/include FILES
         ${CMAKE_SOURCE_DIR}/src/psm/include/psm/detail/your_color_space.hpp)

find_package(Eigen3 REQUIRED)

target_link_libraries(psm_your_color_space PRIVATE Eigen3::Eigen psm_srgb)
target_compile_features(psm_your_color_space PUBLIC cxx_std_20)
```

### Step 4: Write Unit Tests

Update `test/psm/CMakeLists.txt`. We use `if(TARGET)` because color space
modules are optional and may not be built in all configurations:

```cmake
if(TARGET psm_your_color_space)
  add_subdirectory(your_color_space)
endif()
```

Create `test/psm/your_color_space/CMakeLists.txt`:

```cmake
add_executable(your_color_space_test your_color_space_test.cpp)

target_link_libraries(your_color_space_test PRIVATE psm::your_color_space
                                                   psm_test_utils)

addtests(your_color_space_test)
```

Create `test/psm/your_color_space/your_color_space_test.cpp`:

Your tests should include:

- Testing primary colors (red, green, blue)
- Testing black and white values
- Testing gray values
- Testing round-trip conversion accuracy
- Validating input and output sizes
- Performance testing for bulk conversions

Example test structure from the ProPhoto RGB implementation:

```cpp
TEST_F(YourColorSpaceTest, HandlesPrimaryColors) {
  // Test conversion of RGB primary colors
}

TEST_F(YourColorSpaceTest, HandlesBlackAndWhite) {
  // Test conversion of black and white
}

TEST_F(YourColorSpaceTest, HandlesGrayValues) {
  // Test conversion of various gray levels
}

TEST_F(YourColorSpaceTest, RoundTripConversion) {
  // Test converting to your color space and back to sRGB
}

TEST_F(YourColorSpaceTest, ValidatesInputSize) {
  // Test proper error handling for invalid input
}

TEST_F(YourColorSpaceTest, BulkConversionPerformance) {
  // Test performance with large image data
}
```

## Pull Request Process

1. Ensure your code follows the style guide and passes all tests
2. Update documentation if necessary
3. Create a pull request with a clear description of the changes

For pull requests, we follow conventional commit notation for titles and a
structured what/why/how format for descriptions:

```
feat(colorspace): add ProPhoto RGB color space support (#84)

Add ProPhoto RGB color space implementation to the PSM library,
including:
- Color transformation functions between sRGB and ProPhoto RGB
- D50/D65 white point adaptation using Bradford chromatic adaptation
- Proper gamma encoding/decoding for ProPhoto RGB
- Unit testing
```

Make sure to reference related issues in your PR description using the
appropriate GitHub keywords (e.g., "Closes #123" or "Relates to #456") to
automatically link your PR to the relevant issues.
