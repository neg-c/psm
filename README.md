# Prisma (`psm`)

## Description

**Prisma (`psm`)** is a modern C++20 library for color space conversions. It
currently supports converting colors from **sRGB** to **oRGB** and aims to
include more color space conversions in the future.

## Features

- **Color Space Conversion**: Supports sRGB to oRGB.
- **C++20 Compatibility**: Built with modern C++20 features.
- **Flexible Integration**: Can be built, installed, or directly included in
  other projects.

## Getting Started

Follow the instructions in the [INSTALL](INSTALL.md) file to build and integrate
Prisma into your project.

## Usage Example

Refer to the example provided in `src/psm_cli` for a simple demonstration. Or to
simply get started:

```cpp
#include "psm/psm.hpp"

int main() {
  // Input data in BGR format
  std::vector<unsigned char> input_image = {
      0,   0,   255,  // Blue pixel (B=0, G=0, R=255)
      0,   255, 0,    // Green pixel (B=0, G=255, R=0)
      255, 0,   0     // Red pixel (B=255, G=0, R=0)
  };

  // Allocate output buffer
  std::vector<unsigned char> output_image(input_image.size());

  // Convert BGR to oRGB and back
  psm::Convert<psm::sRGB, psm::oRGB>(input_image, output_image);
  psm::Convert<psm::oRGB, psm::sRGB>(output_image, output_image);
}
```

## Integration with CMake

Add Prisma (`psm`) to your CMake project:

```cmake
...
find_package(psm REQUIRED)
target_link_libraries(<your_target> PRIVATE psm::psm)
...
```

# License

Prisma is licensed under [LICENSE](LICENSE)
