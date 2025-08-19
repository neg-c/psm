#include "ToolbarController.hpp"

#include <nfd.h>

#include "PreviewController.hpp"
#include "SliderConfig.hpp"
#include "psm/adjust_channels.hpp"
#include "psm/detail/orgb.hpp"
#include "psm/detail/pro_photo_rgb.hpp"
#include "psm/detail/srgb.hpp"
#include "psm/percent.hpp"
#include "psm/psm.hpp"

#ifndef STB_IMAGE_INCLUDED
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#ifndef STBI_WRITE_INCLUDED
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif

#include <iostream>
#include <span>
#include <string>
#include <vector>

namespace psm_gui::controller {

ToolbarController::ToolbarController(AppState& state) : state_(state) {}

void ToolbarController::loadImage() {
  nfdu8char_t* path = nullptr;
  nfdu8filteritem_t filters[1] = {{"Image files", "png,jpeg,jpg,webp"}};
  nfdopendialogu8args_t args{};
  args.filterList = filters;
  args.filterCount = 1;

  nfdresult_t result = NFD_OpenDialogU8_With(&path, &args);
  if (result == NFD_OKAY) {
    state_.image.load_path = std::string(path);
    NFD_FreePathU8(path);

    std::cout << "Loading image: " << state_.image.load_path << std::endl;

    // Use stbi_is_16_bit to detect the actual bit depth
    bool is_16bit = stbi_is_16_bit(state_.image.load_path.c_str());

    if (is_16bit) {
      std::cout << "  Detected: 16-bit image" << std::endl;
    } else {
      std::cout << "  Detected: 8-bit image" << std::endl;
    }

    int width, height, channels;
    constexpr int target_channels = 3;

    if (is_16bit) {
      // Load as 16-bit
      unsigned short* image_data_16 =
          stbi_load_16(state_.image.load_path.c_str(), &width, &height,
                       &channels, target_channels);

      if (image_data_16) {
        // 16-bit image loaded successfully
        std::cout << "✓ Successfully loaded 16-bit image" << std::endl;
        std::cout << "  Dimensions: " << width << "x" << height << " pixels"
                  << std::endl;
        std::cout << "  Channels: " << channels << " (converted to "
                  << target_channels << ")" << std::endl;
        std::cout << "  Bit depth: 16-bit (0-65535 range)" << std::endl;

        state_.image.width = width;
        state_.image.height = height;
        state_.image.channels = target_channels;
        state_.image.bit_depth = AppState::ImageData::BitDepth::BITS_16;

        const size_t image_size = width * height * target_channels;
        state_.image.original_data = std::vector<std::uint16_t>(image_size);
        auto& original_data_16 =
            std::get<std::vector<std::uint16_t>>(state_.image.original_data);
        std::copy(image_data_16, image_data_16 + image_size,
                  original_data_16.begin());

        stbi_image_free(image_data_16);
        state_.image.is_loaded = true;

        std::cout << "  Memory usage: "
                  << (image_size * sizeof(std::uint16_t) / 1024.0 / 1024.0)
                  << " MB" << std::endl;
        std::cout << "  Processing 16-bit data with full precision..."
                  << std::endl;

        convertImage();
      } else {
        std::cerr << "✗ Failed to load 16-bit image: " << state_.image.load_path
                  << std::endl;
        std::cerr << "  Error: " << stbi_failure_reason() << std::endl;
        state_.image.is_loaded = false;
      }
    } else {
      // Load as 8-bit
      unsigned char* image_data_8 =
          stbi_load(state_.image.load_path.c_str(), &width, &height, &channels,
                    target_channels);

      if (image_data_8) {
        // 8-bit image loaded successfully
        std::cout << "✓ Successfully loaded 8-bit image" << std::endl;
        std::cout << "  Dimensions: " << width << "x" << height << " pixels"
                  << std::endl;
        std::cout << "  Channels: " << channels << " (converted to "
                  << target_channels << ")" << std::endl;
        std::cout << "  Bit depth: 8-bit (0-255 range)" << std::endl;

        state_.image.width = width;
        state_.image.height = height;
        state_.image.channels = target_channels;
        state_.image.bit_depth = AppState::ImageData::BitDepth::BITS_8;

        const size_t image_size = width * height * target_channels;
        state_.image.original_data = std::vector<std::uint8_t>(image_size);
        auto& original_data_8 =
            std::get<std::vector<std::uint8_t>>(state_.image.original_data);
        std::copy(image_data_8, image_data_8 + image_size,
                  original_data_8.begin());

        stbi_image_free(image_data_8);
        state_.image.is_loaded = true;

        std::cout << "  Memory usage: "
                  << (image_size * sizeof(std::uint8_t) / 1024.0 / 1024.0)
                  << " MB" << std::endl;
        std::cout << "  Processing 8-bit data..." << std::endl;

        convertImage();
      } else {
        std::cerr << "✗ Failed to load 8-bit image: " << state_.image.load_path
                  << std::endl;
        std::cerr << "  Error: " << stbi_failure_reason() << std::endl;
        state_.image.is_loaded = false;
      }
    }
  }
}

void ToolbarController::saveImage() {
  nfdu8char_t* path = nullptr;
  nfdu8filteritem_t filters[1] = {{"Image files", "png,jpeg,jpg,webp"}};
  nfdsavedialogu8args_t args{};
  args.filterList = filters;
  args.defaultName = "converted_image.jpg";
  args.filterCount = 1;

  nfdresult_t result = NFD_SaveDialogU8_With(&path, &args);
  if (result == NFD_OKAY && state_.image.is_loaded &&
      state_.image.is_processed) {
    state_.image.save_path = std::string(path);
    NFD_FreePathU8(path);

    std::cout << "Saving image to: " << state_.image.save_path << std::endl;

    constexpr int jpeg_quality = 95;
    bool success = false;

    if (state_.image.bit_depth == AppState::ImageData::BitDepth::BITS_16) {
      std::cout << "  Converting 16-bit data to 8-bit for saving..."
                << std::endl;
      // Convert 16-bit data to 8-bit for saving
      auto& display_data_16 = std::get<std::vector<std::uint16_t>>(state_.image.display_data);
      std::vector<std::uint8_t> display_data_8(display_data_16.size());

      for (size_t i = 0; i < display_data_16.size(); ++i) {
        display_data_8[i] = static_cast<std::uint8_t>(display_data_16[i] >> 8);
      }

      success =
          stbi_write_jpg(state_.image.save_path.c_str(), state_.image.width,
                         state_.image.height, state_.image.channels,
                         display_data_8.data(), jpeg_quality);
    } else {
      std::cout << "  Saving 8-bit data directly..." << std::endl;
      // 8-bit data can be saved directly
      auto& display_data_8 = std::get<std::vector<std::uint8_t>>(state_.image.display_data);
      success =
          stbi_write_jpg(state_.image.save_path.c_str(), state_.image.width,
                         state_.image.height, state_.image.channels,
                         display_data_8.data(), jpeg_quality);
    }

    if (success) {
      std::cout << "✓ Image saved successfully" << std::endl;
      std::cout << "  Dimensions: " << state_.image.width << "x"
                << state_.image.height << " pixels" << std::endl;
      std::cout << "  Quality: " << jpeg_quality << "%" << std::endl;
    } else {
      std::cerr << "✗ Failed to save image: " << state_.image.save_path
                << std::endl;
    }
  }
}

void ToolbarController::updateColorSpace(int colorspace) {
  // Get color space name for logging
  std::string colorspace_name;
  switch (colorspace) {
    case 0:
      colorspace_name = "sRGB";
      break;
    case 1:
      colorspace_name = "Adobe RGB";
      break;
    case 2:
      colorspace_name = "Display P3";
      break;
    case 3:
      colorspace_name = "oRGB";
      break;
    default:
      colorspace_name = "sRGB";
      break;
  }

  std::cout << "Switching to " << colorspace_name << " color space..."
            << std::endl;

  state_.selected_colorspace = colorspace;
  if (state_.image.is_loaded) {
    // Log the bit depth being processed
    if (state_.image.bit_depth == AppState::ImageData::BitDepth::BITS_16) {
      std::cout << "  Re-converting 16-bit image with new color space..."
                << std::endl;
    } else {
      std::cout << "  Re-converting 8-bit image with new color space..."
                << std::endl;
    }

    convertImage();

    // Update slider configuration for new color space
    SliderConfig::updateLabels(state_);

    // Force preview update after color space conversion
    PreviewController::forcePreviousUpdate();

    std::cout << "✓ Color space conversion completed" << std::endl;
  }
}

void ToolbarController::convertImage() {
  if (!state_.image.is_loaded ||
      (!std::holds_alternative<std::vector<std::uint8_t>>(
           state_.image.original_data) &&
       !std::holds_alternative<std::vector<std::uint16_t>>(
           state_.image.original_data)))
    return;

  const size_t image_size = state_.image.getImageSize();

  // Get color space name for logging
  std::string colorspace_name;
  switch (state_.selected_colorspace) {
    case 0:
      colorspace_name = "sRGB";
      break;
    case 1:
      colorspace_name = "Adobe RGB";
      break;
    case 2:
      colorspace_name = "Display P3";
      break;
    case 3:
      colorspace_name = "oRGB";
      break;
    default:
      colorspace_name = "sRGB";
      break;
  }

  try {
    switch (state_.selected_colorspace) {
      case 0:  // sRGB
        psm::Convert<psm::sRGB, psm::sRGB>(input_span, converted_span);
        break;
      case 1:  // AdobeRGB
        psm::Convert<psm::sRGB, psm::AdobeRGB>(input_span, converted_span);
        break;
      case 2:  // DisplayP3
        psm::Convert<psm::sRGB, psm::DisplayP3>(input_span, converted_span);
        break;
      case 3:  // oRGB
        psm::Convert<psm::sRGB, psm::oRGB>(input_span, converted_span);
        break;
      case 4:
        psm::Convert<psm::sRGB, psm::ProPhotoRGB>(input_span, converted_span);
        break;
      default:
        psm::Convert<psm::sRGB, psm::sRGB>(input_span, converted_span);
    }
  } else {
    // Handle 8-bit data
    std::cout << "  Processing 8-bit data (0-255 range)" << std::endl;

    state_.image.converted_data = std::vector<std::uint8_t>(image_size);
    state_.image.display_data = std::vector<std::uint8_t>(image_size);

    auto& original_data = std::get<std::vector<std::uint8_t>>(state_.image.original_data);
    auto& converted_data = std::get<std::vector<std::uint8_t>>(state_.image.converted_data);
    auto& display_data = std::get<std::vector<std::uint8_t>>(state_.image.display_data);

    std::span<const std::uint8_t> input_span{original_data};
    std::span<std::uint8_t> converted_span{converted_data};

    try {
      switch (state_.selected_colorspace) {
        case 0:  // sRGB
          psm::Convert<psm::sRGB, psm::sRGB>(input_span, converted_span);
          break;
        case 1:  // AdobeRGB
          psm::Convert<psm::sRGB, psm::AdobeRGB>(input_span, converted_span);
          break;
        case 2:  // DisplayP3
          psm::Convert<psm::sRGB, psm::DisplayP3>(input_span, converted_span);
          break;
        case 3:  // oRGB
          psm::Convert<psm::sRGB, psm::oRGB>(input_span, converted_span);
          break;
        default:
          psm::Convert<psm::sRGB, psm::sRGB>(input_span, converted_span);
      }

      std::copy(converted_data.begin(), converted_data.end(),
                display_data.begin());

      if (state_.selected_colorspace == 3) {  // oRGB
        std::cout << "  Applying oRGB to sRGB conversion for display..."
                  << std::endl;
        std::vector<std::uint8_t> temp_image(display_data.size());
        std::span<std::uint8_t> temp_span{temp_image};

        psm::Convert<psm::oRGB, psm::sRGB>(
            std::span<std::uint8_t>{display_data}, temp_span);

        std::copy(temp_image.begin(), temp_image.end(), display_data.begin());
      }

      state_.image.is_processed = true;
      std::cout << "✓ 8-bit image conversion completed successfully"
                << std::endl;
    } catch (const std::exception& e) {
      std::cerr << "✗ Error converting 8-bit image: " << e.what() << std::endl;
      state_.image.is_processed = false;
    }
  }
}

}  // namespace psm_gui::controller
