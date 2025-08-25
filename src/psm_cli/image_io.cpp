#include "image_io.hpp"

#include <filesystem>
#include <format>
#include <iostream>
#include <type_traits>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace psm_cli {

namespace {
constexpr int kJpegQuality = 95;
}

template <typename T>
ImageData<T>::~ImageData() {
  if (data_) {
    stbi_image_free(data_);
  }
}

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

template <typename DataType>
bool save_image(const std::vector<DataType>& image_data, int width, int height,
                const std::string& output_path) {
  constexpr int channels = 3;

  if constexpr (std::is_same_v<DataType, uint8_t>) {
    // Save as JPEG for 8-bit
    std::filesystem::path path(output_path);
    if (path.extension() != ".jpg" && path.extension() != ".jpeg") {
      path.replace_extension(".jpg");
    }

    std::cout << std::format("Saving 8-bit image as JPEG: {}\n", path.string());
    return stbi_write_jpg(path.string().c_str(), width, height, channels,
                          image_data.data(), kJpegQuality);
  } else {
    // Save as PNG for 16-bit
    std::filesystem::path path(output_path);
    if (path.extension() != ".png") {
      path.replace_extension(".png");
    }

    std::cout << std::format("Saving 16-bit image as PNG: {}\n", path.string());
    return stbi_write_png(path.string().c_str(), width, height, channels,
                          image_data.data(),
                          width * channels * sizeof(DataType));
  }
}

template class ImageData<uint8_t>;
template class ImageData<uint16_t>;

template bool save_image<uint8_t>(const std::vector<uint8_t>&, int, int,
                                  const std::string&);
template bool save_image<uint16_t>(const std::vector<uint16_t>&, int, int,
                                   const std::string&);

}  // namespace psm_cli
