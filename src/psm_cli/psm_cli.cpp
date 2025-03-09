#include <cstddef>
#include <filesystem>
#include <format>
#include <iostream>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// Add image loading/saving library
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "cli_parser.hpp"
#include "psm/adjust_channels.hpp"
#include "psm/percent.hpp"
#include "psm/psm.hpp"
#include "stb_image_write.h"

namespace conversion {

// Color space conversion function that handles runtime selection
template <typename TargetColorSpace>
void convert_from(const std::string& from_space,
                  const std::vector<unsigned char>& input,
                  std::vector<unsigned char>& output) {
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
    throw std::invalid_argument("Unsupported source color space: " +
                                from_space);
  }
}

// Dispatcher for color space conversion
void convert_between(const std::string& from_space, const std::string& to_space,
                     const std::vector<unsigned char>& input,
                     std::vector<unsigned char>& output) {
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
    throw std::invalid_argument("Unsupported target color space: " + to_space);
  }
}

}  // namespace conversion

int main(int argc, char* argv[]) {
  try {
    // Parse command line arguments
    CLIOptions options = parse_args(argc, argv);

    // Load image
    int width, height, channels;
    unsigned char* image_data =
        stbi_load(options.input_file.c_str(), &width, &height, &channels,
                  3  // Force 3 channels (RGB)
        );

    if (!image_data) {
      std::cerr << "Failed to load image: " << options.input_file << "\n";
      return 1;
    }

    // Convert to vector
    std::vector<unsigned char> input_image(image_data,
                                           image_data + width * height * 3);
    std::vector<unsigned char> output_image(input_image.size());

    // Free original image data
    stbi_image_free(image_data);

    std::cout << "Processing image: " << options.input_file << "\n";
    std::cout << "Dimensions: " << width << "x" << height
              << ", Channels: " << channels << "\n";
    std::cout << "Converting from " << options.from_space << " to "
              << options.to_space << "\n";

    // Convert to target color space
    conversion::convert_between(options.from_space, options.to_space,
                                input_image, output_image);

    // Apply channel adjustments if provided
    if (options.adjust_values) {
      std::cout << "Adjusting channels by R:"
                << options.adjust_values->channel(0)
                << "%, G:" << options.adjust_values->channel(1)
                << "%, B:" << options.adjust_values->channel(2) << "%\n";

      psm::AdjustChannels(output_image, *options.adjust_values);

      // Convert back to original color space if needed
      if (options.from_space != options.to_space) {
        std::vector<unsigned char> temp_image(output_image.size());
        conversion::convert_between(options.to_space, options.from_space,
                                    output_image, temp_image);
        output_image = std::move(temp_image);
      }
    }

    // Ensure output path has .jpg extension
    std::string output_file = options.output_file;
    std::filesystem::path output_path(output_file);

    if (output_path.extension() != ".jpg" &&
        output_path.extension() != ".jpeg") {
      output_file = output_path.replace_extension(".jpg").string();
      std::cout << "Output will be saved as JPEG: " << output_file << "\n";
    }

    // Save as JPEG
    bool save_success =
        stbi_write_jpg(output_file.c_str(), width, height,
                       3,  // Always RGB
                       output_image.data(),
                       95  // Quality - slightly higher than before
        );

    if (!save_success) {
      std::cerr << "Failed to save JPEG image: " << output_file << "\n";
      return 1;
    }

    std::cout << "Successfully saved output to: " << output_file << "\n";

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
