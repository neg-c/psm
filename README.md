# Prisma (`psm`)

## Description
**Prisma (`psm`)** is a modern C++20 library for color space conversions. It currently supports converting colors from **sRGB** to **oRGB** and aims to include more color space conversions in the future.

## Features
- **Color Space Conversion**: Supports sRGB to oRGB.
- **C++20 Compatibility**: Built with modern C++20 features.
- **Flexible Integration**: Can be built, installed, or directly included in other projects.

## Getting Started
Follow the instructions in the `INSTALL` file to build and integrate Prisma into your project.

## Usage Example
Refer to the example provided in `src/psm_cli` for a simple demonstration.
Or to simply get started:
```cpp
#include "psm/psm.hpp"

int main() {
  // load_image is your placeholder for and rgba image
  std::vector<unsigned char> input_image = load_image(); 

  std::vector<unsigned char> output_image(input_image.size());

  std::span<const unsigned char> src_span(input_image.data(),
                                          input_image.size());
  std::span<unsigned char> result_span(output_image.data(),
                                       output_image.size());

  psm::Color(src_span, result_span, psm::Format::ksRGB, psm::Format::koRGB);

  return 0;
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
