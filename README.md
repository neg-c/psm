# Prisma (`psm`)

## Description

**Prisma (`psm`)** is a modern C++20 library for color space conversions. It
currently supports converting colors between **sRGB**, **Adobe RGB**, and
**oRGB** color spaces.

## Features

- **Flexible Color Space Conversion**:
  - Convert between any supported color spaces (sRGB, Adobe RGB, oRGB)
  - All conversions maintain color accuracy through proper intermediate
    transformations
- **Channel Adjustment**: Allows percentage-based adjustment of individual color
  channels
- **C++20 Compatibility**: Built with modern C++20 features
- **Flexible Integration**: Can be built, installed, or directly included in
  other projects

## Limitations

- Currently only processes input data as BGR format
- Additional channels (like alpha) are ignored during conversion

## Getting Started

Follow the instructions in the [INSTALL](INSTALL.md) file to build and integrate
Prisma into your project.

## Usage Example

Here's a simple example showing color space conversion from sRGB to Adobe RGB,
channel adjustments, and conversion through oRGB back to sRGB for display:

```cpp
#include "psm/psm.hpp"

int main() {
    // Input data in BGR format
    std::vector<unsigned char> input_image = {
        0,   0,   255,  // Red pixel (B=0, G=0, R=255)
        0,   255, 0,    // Green pixel (B=0, G=255, R=0)
        255, 0,   0     // Blue pixel (B=255, G=0, R=0)
    };

    // Allocate output buffer
    std::vector<unsigned char> output_image(input_image.size());

    // Convert between any supported color spaces
    psm::Convert<psm::sRGB, psm::AdobeRGB>(input_image, output_image);

    //  adjust channels in any color space(AdobeRGB in this case)
    psm::Percent adjustments{
        .channel0_ = 20.0f,  // Increase first channel by 20%
        .channel1_ = -10.0f, // Decrease second channel by 10%
        .channel2_ = 0.0f    // Leave third channel unchanged
    };
    psm::adjustChannels(output_image, adjustments);

    psm::Convert<psm::AdobeRGB, psm::oRGB>(output_image, output_image);
    // Convert back to sRGB
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
