# Prisma (`psm`)

## Description

**Prisma (`psm`)** is a modern C++20 library for color space conversions. It
currently supports converting colors between **sRGB**, **Adobe RGB**, **DISPLAY-P3**,
and **oRGB** color spaces.

## Features

- **Flexible Color Space Conversion**:
  - Convert between any supported color spaces (sRGB, Adobe RGB, oRGB, Display P3)
  - All conversions maintain color accuracy through proper intermediate
    transformations
- **Channel Adjustment**: Allows percentage-based adjustment of individual color
  channels
- **C++20 Compatibility**: Built with modern C++20 features
- **Flexible Integration**: Can be built, installed, or directly included in
  other projects

## Limitations

- Additional channels (like alpha) are ignored during conversion

## Getting Started

Follow the instructions in the [INSTALL](INSTALL.md) file to build and integrate
Prisma into your project.

## Usage Example

Here's a simple example showing color space conversion from sRGB to Adobe RGB,
channel adjustments, and conversion back to sRGB for display:

```cpp
#include "psm/psm.hpp"

int main() {
    // Input data in BGR format
    const std::vector<unsigned char> input_image = {
        0,   0,   255,  // Red pixel (B=0, G=0, R=255)
        0,   255, 0,    // Green pixel (B=0, G=255, R=0)
        255, 0,   0     // Blue pixel (B=255, G=0, R=0)
    };

    // Convert between any supported color spaces
    auto output_image = psm::Convert<psm::sRGB, psm::AdobeRGB>(input_image);

    // Adjust channels in any color space (AdobeRGB in this case)
    // Create Percent object with channel adjustments
    psm::Percent adjustments{20, -10, 0};  // +20% blue, -10% green, 0% red
    psm::AdjustChannels(output_image, adjustments);

    // Convert through oRGB and back to sRGB
    output_image = psm::Convert<psm::AdobeRGB, psm::oRGB>(output_image);
    output_image = psm::Convert<psm::oRGB, psm::sRGB>(output_image);
}
```

## Integration with CMake

Add Prisma (`psm`) to your CMake project:

```cmake
find_package(psm REQUIRED)

# Link core functionality (includes oRGB,AdobeRGB, and AdjustChannels utility)
target_link_libraries(<your_target> PRIVATE psm::psm)

# Optionally link specific color space modules
target_link_libraries(<your_target> PRIVATE
    psm::orgb      # Link oRGB support
    psm::adobe_rgb     # Link Adobe RGB support
)
```

Each color space and utility is provided as a separate module that can be linked independently. Available modules:
- `psm::orgb` - Support for oRGB color space
- `psm::adobe_rgb` - Support for Adobe RGB color space
- `psm::adjust_channels` - Support for channel adjustment utilities
- `psm::display_p3` - Support for Display P3 color space
# License

Prisma is licensed under [LICENSE](LICENSE)
