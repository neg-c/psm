#include <cstddef>
#include <filesystem>
#include <format>
#include <iostream>
#include <optional>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "cli_parser.hpp"
#include "psm/adjust_channels.hpp"
#include "psm/percent.hpp"
#include "psm/psm.hpp"
#include "stb_image_write.h"

namespace {
constexpr int kJpegQuality = 95;
}  // namespace

namespace conversion {

template <typename TargetColorSpace>
void convert_from(std::string_view from_space,
                  std::span<const unsigned char> input,
                  std::span<unsigned char> output) {
  if (from_space == "sRGB") {
    psm::Convert<psm::sRGB, TargetColorSpace>(input, output);
  } else if (from_space == "AdobeRGB") {
    psm::Convert<psm::AdobeRGB, TargetColorSpace>(input, output);
  } else if (from_space == "DisplayP3") {
    psm::Convert<psm::DisplayP3, TargetColorSpace>(input, output);
  } else if (from_space == "oRGB") {
    psm::Convert<psm::oRGB, TargetColorSpace>(input, output);
  } else if (from_space == "ProPhotoRGB") {
    psm::Convert<psm::ProPhotoRGB, TargetColorSpace>(input, output);
  } else {
    throw std::invalid_argument(
        std::format("Unsupported source color space: {}", from_space));
  }
}

void convert_between(std::string_view from_space, std::string_view to_space,
                     std::span<const unsigned char> input,
                     std::span<unsigned char> output) {
  if (to_space == "sRGB") {
    convert_from<psm::sRGB>(from_space, input, output);
  } else if (to_space == "AdobeRGB") {
    convert_from<psm::AdobeRGB>(from_space, input, output);
  } else if (to_space == "DisplayP3") {
    convert_from<psm::DisplayP3>(from_space, input, output);
  } else if (to_space == "oRGB") {
    convert_from<psm::oRGB>(from_space, input, output);
  } else if (to_space == "ProPhotoRGB") {
    convert_from<psm::ProPhotoRGB>(from_space, input, output);
  } else {
    throw std::invalid_argument(
        std::format("Unsupported target color space: {}", to_space));
  }
}

}  // namespace conversion

int main(int argc, char* argv[]) {
  try {
    const CLIOptions options = parse_args(argc, argv);

    int width = 0;
    int height = 0;
    int channels = 0;

    // Force 3 channels (RGB)
    constexpr int target_channels = 3;

    unsigned char* image_data = stbi_load(options.input_file.c_str(), &width,
                                          &height, &channels, target_channels);

    if (image_data == nullptr) {
      std::cerr << std::format("Failed to load image: {}\n",
                               options.input_file);
      return 1;
    }

    // Create a span directly over the loaded image data - no copy needed
    const size_t image_size = width * height * target_channels;
    std::span<const unsigned char> input_image{image_data, image_size};

    // Create output storage and span
    std::vector<unsigned char> output_image_storage(image_size);
    std::span<unsigned char> output_image{output_image_storage};

    std::cout << std::format("Processing image: {}\n", options.input_file);
    std::cout << std::format("Dimensions: {}x{}, Channels: {}\n", width, height,
                             channels);
    std::cout << std::format("Converting from {} to {}\n", options.from_space,
                             options.to_space);

    conversion::convert_between(options.from_space, options.to_space,
                                input_image, output_image);

    stbi_image_free(image_data);

    if (options.adjust_values) {
      std::cout << std::format("Adjusting channels by R:{}%, G:{}%, B:{}%\n",
                               options.adjust_values->channel(0),
                               options.adjust_values->channel(1),
                               options.adjust_values->channel(2));

      psm::AdjustChannels(output_image, *options.adjust_values);

      // Convert back to original color space if needed
      if (options.from_space != options.to_space) {
        std::vector<unsigned char> temp_image_storage(output_image.size());
        std::span<unsigned char> temp_image{temp_image_storage};

        conversion::convert_between(options.to_space, options.from_space,
                                    output_image, temp_image);
        output_image_storage = std::move(temp_image_storage);
        output_image = std::span<unsigned char>{output_image_storage};
      }
    }

    std::string output_file = options.output_file;
    const std::filesystem::path output_path(output_file);

    if (output_path.extension() != ".jpg" &&
        output_path.extension() != ".jpeg") {
      std::filesystem::path new_path = output_path;
      output_file = new_path.replace_extension(".jpg").string();
      std::cout << std::format("Output will be saved as JPEG: {}\n",
                               output_file);
    }

    const bool save_success =
        stbi_write_jpg(output_file.c_str(), width, height, target_channels,
                       output_image_storage.data(), kJpegQuality);

    if (!save_success) {
      std::cerr << std::format("Failed to save JPEG image: {}\n", output_file);
      return 1;
    }

    std::cout << std::format("Successfully saved output to: {}\n", output_file);

  } catch (const std::exception& e) {
    std::cerr << std::format("Error: {}\n", e.what());
    return 1;
  }

  return 0;
}
