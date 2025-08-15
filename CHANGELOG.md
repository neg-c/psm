# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to
[Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.1] - 2025-08-15

### Fixed

- **Windows GUI Application**: Fixed missing DLL dependencies (`glfw3.dll`, `nfd.dll`) in Windows installer packages by properly installing runtime dependencies using `$<TARGET_RUNTIME_DLLS:psm_gui>` in the installation configuration.
- **Windows GUI Application**: Configured GUI application to launch without console window by setting `/SUBSYSTEM:WINDOWS` and `/ENTRY:mainCRTStartup` linker options.

## [1.1.0] - 2025-07-15

### Added

- **PSM GUI Demo Tool**:
  - Introduced a new GUI demo tool (`psm_gui`) for interactive color space conversion and image processing.
  - Features include:
    - Image loading and saving with native file dialogs (NFD).
    - Real-time color space conversion between sRGB, AdobeRGB, DisplayP3, and oRGB.
    - Modern UI with ImGui, including toolbar, control panels, and preview area.
    - Vertical and horizontal sliders for channel and color adjustments, with color space-specific schemes.
    - Pixel color picker and magnifying glass for pixel-level inspection.
    - Responsive image preview with aspect ratio preservation and OpenGL rendering.
    - Centralized state management and intuitive controls for color grading workflows.
  - Built by default; can be disabled with `-DBUILD_PSM_GUI=OFF`.

### Changed

- **GUI Enhancements**:
  - Improved layout and panel grid logic for better usability.
  - Added modern slider styles and dynamic slider labels based on color space.
  - Implemented color space-specific adjustment algorithms for professional color grading.
  - Enhanced preview area with OpenGL texture management and aspect ratio preservation.
  - Improved state management and encapsulation for maintainability.
  - Refactored code for better organization, readability, and maintainability.

### Fixed

- Addressed build issues related to GLEW and OpenGL dependencies, especially for Linux and Windows.
- Fixed image preview artifacts by setting proper OpenGL texture alignment.
- Resolved issues with slider sensitivity and image update mechanisms for smoother adjustments.
- Fixed transitive dependency issues with libxcrypt and vcpkg baseline updates for GCC 15 compatibility.

### Documentation

- Added comprehensive documentation for the GUI demo tool in `README.md` and `INSTALL.md`.
- Documented system dependencies and installation instructions for major Linux distributions.
- Clarified build options and tool usage in project documentation.

### Infrastructure

- Updated CMake options to allow flexible builds (e.g., `BUILD_PSM_GUI`, `BUILD_PSM_CLI`).
- Improved CI/CD workflows to handle new dependencies and build configurations.

## [1.0.0] - 2025-03-10

### Breaking Changes

- **Standardized RGB/BGR Color Format Handling**:
  - Changed the expected color format from BGR to RGB across the codebase.
  - Updated color space implementations to work with RGB input directly.
  - **Migration Note**: Code that previously relied on BGR format will need to
    be updated.

- **Const Correctness in Color Space Conversions**:
  - Made source parameters const in color space conversion functions.
  - Updated ColorSpaceType concept to require const source parameters.
  - **Migration Note**: This should be transparent for most users but may affect
    custom color space implementations.

- **Refactored Percent Class**:
  - Converted `Percent` struct to a class with proper encapsulation.
  - Replaced direct member access with `channel(idx)` method.
  - Added arithmetic operators (`+=`, `-=`, `+`, `-`) for easier manipulation.
  - Introduced `neutral()` and `uniform(value)` factory methods.
  - Added equality and inequality operators.
  - **Migration Note**: Update code to use the new accessor methods instead of
    direct member access.

### Added

- **New Color Spaces**:
  - **Display P3**: Added support for Display P3 color space with D65 white
    point and sRGB transfer function.
  - **ProPhoto RGB**: Added support for ProPhoto RGB color space with D50 white
    point and proper gamma encoding/decoding.

- **Command-Line Interface**:
  - Added a new CLI tool (`psm_cli`) for color space conversion and channel
    adjustment.
  - Supports all color spaces in the library (sRGB, AdobeRGB, DisplayP3, oRGB,
    ProPhotoRGB).
  - Uses STB image libraries for image I/O.

- **Visual Documentation**:
  - Added comprehensive visual examples showing color space conversions and
    channel adjustments.
  - Demonstrates practical benefits of different color spaces, particularly
    highlighting oRGB's perceptually uniform channels.

- **Input Validation**:
  - Added validation check in `Convert` function to ensure input buffer size is
    a multiple of 3.
  - Throws `std::invalid_argument` if validation fails.

- **Testing Improvements**:
  - Added comprehensive test suites for Adobe RGB, Display P3, and oRGB color
    spaces.
  - Added unit tests for `psm::Percent` class.
  - Added container compatibility test suite to verify compatibility with
    various C++ container types.
  - Improved color space conversion test utilities with standardized matchers.

### Changed

- **CMake Enhancements**:
  - Replaced custom `PSM_BUILD_TESTS` option with standard `BUILD_TESTING` flag
    from CTest module.
  - Conditionally build tests based on module availability.
  - Fixed span assignment with proper data copy in sRGB conversion functions.

- **Code Organization**:
  - Centralized common color space utilities into detail module.
  - Grouped Eigen3 types used in the codebase for DRYer code.
  - Grouped RGB2XYZ and XYZ2RGB conversions in a separate module.
  - Grouped gamma correction, transfer function, and normalization of data in a
    separate module.

### Infrastructure

- **CI/CD Improvements**:
  - Added test status reporting workflow and badge.
  - Configured clang-tidy to use separate configs for src and test directories.
  - Added test-specific clang-tidy configuration to handle test-specific
    patterns.

- **Documentation**:
  - Added CONTRIBUTE.md file with color space guide.
  - Clarified RGB/BGR input format support in documentation.

### Migration Guide

#### RGB/BGR Format Change

```cpp
// Old (v0.3.0-alpha) - Expected BGR format
std::vector<unsigned char> bgr_data = {0, 0, 255, 0, 255, 0, 255, 0, 0}; // BGR: Red, Green, Blue
psm::Convert<psm::sRGB, psm::oRGB>(bgr_data, output);
// New (v1.0.0) - Expects RGB format
std::vector<unsigned char> rgb_data = {255, 0, 0, 0, 255, 0, 0, 0, 255}; // RGB: Red, Green, Blue
psm::Convert<psm::sRGB, psm::oRGB>(rgb_data, output);
```

#### Percent Type Change

```cpp
// Old (v0.3.0-alpha)
psm::Percent<int> percent;
percent.channel0_ = 100;
percent.channel1_ = 75;
percent.channel2_ = 50;
// New (v1.0.0)
// Using constructor
psm::Percent percent(10, 20, 30);
// Or using factory methods
auto neutral = psm::Percent::neutral();
auto uniform = psm::Percent::uniform(15);
// Accessing channels
int channel0 = percent.channel(0);
int channel1 = percent.channel(1);
int channel2 = percent.channel(2);
// Using arithmetic operators
percent += 5;
percent -= 3;
auto result = percent + 10;
auto result2 = result - 5;
// Checking properties
bool isNeutral = neutral.isNeutral();    // true
bool isUniform = uniform.isUniform();    // true
// Comparison operators
bool equals = (psm::Percent(10, 20, 30) == psm::Percent(10, 20, 30));    // true
bool notEquals = (psm::Percent(10, 20, 30) != psm::Percent(10, 20, 31)); // true
```

#### Using the CLI Tool

```bash
# Convert from sRGB to oRGB
psm_cli --input input.png --output output.png --from srgb --to orgb
# Adjust channels of oRGB image
psm_cli --input input.jpg --output output.jpg --space ProPhotoRGB --adjust 110,90,95
```

## [0.3.0-alpha] - 2025-01-26

### Breaking Changes

- **Refactored `Percent` Type**:
  - The `Percent` struct now uses `int` instead of `float` for channel values.
  - Updated `AdjustChannels` to handle `int`-based percentages with explicit
    casting.
  - **Migration Note**: Since percentages were already treated as whole numbers,
    this change should have minimal functional impact.

### Added

- **Enhanced CI/CD Workflow**:
  - Automated `CHANGELOG.md` validation and version entry checks.
  - Automatically includes version-specific changelogs in release drafts.
  - Simplified manual steps in creating release notes for smoother release
    management.
- **Unit Testing System**:
  - Integrated GTest as the unit testing framework.
  - Added a `Testing.cmake` module to configure GTest via vcpkg.
  - Introduced the `AddTests` macro for easy test configuration and discovery
    using the GoogleTest module.

### Changed

- **CMake Enhancements**:
  - Introduced `psm_add_module()` function for standardized module registration.
  - Automatically creates `psm::` namespace aliases for all modules.
  - Updated module `CMakeLists.txt` to use `psm_add_module()` for consistency.
  - Fixed issues where modules weren't exported in default builds.
  - Installed the `psm_add_module()` CMake script for developer use.

### Migration Guide

#### AdjustChannels Percent Type

- Previous versions used `float` for the `Percent` struct. Update your code to
  use `int` for channel adjustment percentages.
- Example:

  ```cpp
  // Old (v0.2.0-alpha)
  psm::Percent<float> percent = {100.0f, 75.0f, 50.0f};
  psm::AdjustChannels<psm::sRGB>(image, percent);

  // New (v0.3.0-alpha)
  psm::Percent<int> percent = {100, 75, 50};
  psm::AdjustChannels<psm::sRGB>(image, percent);
  ```

#### Adding Unit Test

- Example:
  ```cmake
  AddTests(orgb_test)
  ```

#### Optional Module setup

- Should me used instead of `add_library` for optional modules(build using
  WITH\_ flag) to properly handle aliasing.
- Example:
  ```cmake
  psm_add_module(psm_new_module SHARED)
  ```

## [0.2.0-alpha] - 2025-01-23

### Breaking Changes

- Modularized the library into separate components with optional builds:
  - `psm::psm`: Includes all modules (default build).
  - `psm::orgb`: Handles sRGB <-> oRGB conversions.
  - `psm::adobe_rgb`: Handles sRGB <-> Adobe RGB conversions.
- Introduced optional linking for `psma::adjust_channels`. It is no longer
  included in `psm::psm` by default and must be explicitly linked.
- Refactored internal implementation:
  - Moved color space classes (`oRGB`, `sRGB`, `AdobeRGB`) to the `psm::detail`
    namespace to hide implementation details from library users.
  - Reorganized headers to separate public and private interfaces.
  - Channel adjustment logic centralized under `AdjustChannels` in a dedicated
    public header.

### Added

- **Adobe RGB Support**:
  - Implemented bidirectional sRGB <-> Adobe RGB conversion.
- **Modular Builds**:
  - Introduced CMake options for selective module builds:
    - `WITH_ORGB`
    - `WITH_ADOBE_RGB`
    - `WITH_ADJUST_CHANNELS`
  - Dynamic module handling in build system for better configurability.
- Improved documentation:
  - Added guidance for linking individual modules.
  - Updated README with modular build instructions and examples.
- Expanded example usage in the CLI to demonstrate the `AdjustChannels`
  functionality.

### Changed

- Modernized type system:
  - Replaced the trait-based color space type system with a concept-based design
    using C++20 concepts.
  - Introduced `ColorSpaceType` concepts and tag-based lookups for cleaner and
    more idiomatic template constraints.
- Optimized color space pipeline:
  - Simplified color space conversion logic using tag-based design.
  - Removed redundant type checks and streamlined the conversion process.
- Improved encapsulation:
  - Implementation headers moved to the `detail/` directory to clearly separate
    internal logic from the public API.
  - Reduced the public API surface to minimize misuse by library clients.

### Infrastructure

- Improved build system:
  - Added `Options.cmake` to simplify module build configuration.
  - Made module installation configurable based on enabled modules.
- Integrated `clang-tidy` for C++20 best practices:
  - Enabled checks for template metaprogramming, range safety, and modern
    idioms.
- Added `.gitattributes` for consistent line endings across platforms.

### Migration Guide

#### Modular Linking

- To link only the required modules:
  ```cmake
  target_link_libraries(my_project PRIVATE psm::orgb)
  ```
- To include all modules:
  ```cmake
  target_link_libraries(my_project PRIVATE psm::psm)
  ```

#### AdjustChannels Usage

##### Example: Channel Adjusting for AdobeRGB

```cpp
#include <psm/psm.hpp>
#include <psm/adjust_channels.hpp>
...
std::vector<unsigned char> input_image = load_image();
std::vector<unsigned char> output_image(input_image.size());

// Convert from sRGB to Adobe RGB
psm::Convert<psm::sRGB, psm::AdobeRGB>(input_image, output_image);
// Channel 0 increase by 20%, Channel 1 leave as is, Channel 2 decrease by 10%
psm::AdjustChannels(output_image, psm::Percent(20.0f, 0.0f, -10.0f));
// Convert back to sRGB
psm::Convert<psm::AdobeRGB, psm::sRGB>(output_image, input_image);
```

## [0.1.0-alpha] - 2024-01-05

### Breaking Changes

- Renamed `Color()` to `Convert()` for better API clarity
- Changed from runtime format enums to compile-time template parameters
- Changed color conversion pipeline to use float-point precision internally
- Reverted to `unsigned char` with **[0,1]** float range mapping for better data
  preservation
- Input data is now expected in **BGR** format

### Added

- Direct container support in conversion functions
- Improved color space conversion accuracy
- Type-trait based compile-time color space validation
- Static utility functions for color space operations

### Changed

- Restructured ORGB into a separate module
- Optimized color conversion pipeline
  - Direct sRGB-oRGB conversion
  - Removed unnecessary intermediate steps
- Improved implementation organization
  - Moved color space traits to detail namespace
  - Replaced runtime checks with compile-time validation
  - Converted color space classes to static utilities

### Infrastructure

- Improved public interface installation
- Replaced header guards with `#pragma once`
- Cleaned up includes based on IWYU analysis
- Updated documentation to reflect new API limitations and features

### Migration Guide

#### Basic Usage

```cpp
// Old (v0.0.1-alpha)
std::vector<unsigned char> input_image = load_image();
std::vector<unsigned char> output_image(input_image.size());

std::span<const unsigned char> src_span(input_image.data(), input_image.size());
std::span<unsigned char> result_span(output_image.data(), output_image.size());

psm::Color(src_span, result_span, psm::Format::ksRGB, psm::Format::koRGB);

// New (v0.1.0-alpha)
std::vector<unsigned char> input_image = load_image();
std::vector<unsigned char> output_image(input_image.size());

psm::Convert<psm::sRGB, psm::oRGB>(input_image, output_image);
```

## [0.0.1-alpha] - 2024-12-09

### Added

- Initial release of the Prisma (PSM) color space conversion library
- Core color space conversion functionality
  - RGB color space support
  - Optimized RGB (ORGB) color space implementation
- Command-line interface (CLI) tool for color space conversions
- CMake build system with vcpkg integration
- Cross-platform support for Windows and Linux
- Package generation support
  - Windows: NSIS installer and ZIP archive
  - Linux: DEB, RPM, and TGZ packages
- Comprehensive documentation
  - Installation guide
  - API documentation
  - Usage examples
- Automated CI/CD pipeline
  - Automated builds for Windows and Linux
  - Code formatting checks
  - Release automation

[1.1.1]: https://github.com/neg-c/psm/releases/tag/v1.1.1
[1.1.0]: https://github.com/neg-c/psm/releases/tag/v1.1.0
[1.0.0]: https://github.com/neg-c/psm/releases/tag/v1.0.0
[0.3.0-alpha]: https://github.com/neg-c/psm/releases/tag/v0.3.0-alpha
[0.2.0-alpha]: https://github.com/neg-c/psm/releases/tag/v0.2.0-alpha
[0.1.0-alpha]: https://github.com/neg-c/psm/releases/tag/v0.1.0-alpha
[0.0.1-alpha]: https://github.com/neg-c/psm/releases/tag/v0.0.1-alpha
