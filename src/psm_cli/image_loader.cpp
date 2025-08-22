#include "image_loader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace psm_cli {

ImageVariant load_image(const std::string& filepath) {
  int width = 0, height = 0, channels = 0;
  constexpr int target_channels = 3;

  if (stbi_is_16_bit(filepath.c_str())) {
    auto* data = stbi_load_16(filepath.c_str(), &width, &height, &channels,
                              target_channels);
    return ImageData<uint16_t>{data, width, height, target_channels};
  } else {
    auto* data = stbi_load(filepath.c_str(), &width, &height, &channels,
                           target_channels);
    return ImageData<uint8_t>{data, width, height, target_channels};
  }
}

}  // namespace psm_cli
