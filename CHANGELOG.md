# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to
[Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0-alpha] - 2024-01-05

### Breaking Changes
- Renamed `Color()` to `Convert()` for better API clarity
- Changed from runtime format enums to compile-time template parameters
- Changed color conversion pipeline to use float-point precision internally
- Reverted to `unsigned char` with **[0,1]** float range mapping for better data preservation
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

[0.1.0-alpha]: https://github.com/neg-c/psm/releases/tag/v0.1.0-alpha
[0.0.1-alpha]: https://github.com/neg-c/psm/releases/tag/v0.0.1-alpha
