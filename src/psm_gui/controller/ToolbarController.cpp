#include "ToolbarController.hpp"

#include <nfd.h>

#include <iostream>

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

    // Use stbi_is_16_bit to detect the actual bit depth
    bool is_16bit = stbi_is_16_bit(state_.image.load_path.c_str());

    int width, height, channels;
    constexpr int target_channels = 3;

    if (is_16bit) {
      // Load as 16-bit
      unsigned short* image_data_16 =
          stbi_load_16(state_.image.load_path.c_str(), &width, &height,
                       &channels, target_channels);

      if (image_data_16) {
        state_.image.width = width;
        state_.image.height = height;
        state_.image.channels = target_channels;
        state_.image.is_original_8bit = false;  // This is true 16-bit data

        const size_t image_size = width * height * target_channels;
        state_.image.original_data.resize(image_size);
        std::copy(image_data_16, image_data_16 + image_size,
                  state_.image.original_data.begin());

        stbi_image_free(image_data_16);
        state_.image.is_loaded = true;

        convertImage();
      } else {
        std::cerr << "✗ Failed to load 16-bit image: " << state_.image.load_path
                  << std::endl;
        std::cerr << "  Error: " << stbi_failure_reason() << std::endl;
        state_.image.is_loaded = false;
      }
    } else {
      // Load as 8-bit and store in 16-bit buffer without scaling
      unsigned char* image_data_8 =
          stbi_load(state_.image.load_path.c_str(), &width, &height, &channels,
                    target_channels);

      if (image_data_8) {
        state_.image.width = width;
        state_.image.height = height;
        state_.image.channels = target_channels;
        state_.image.is_original_8bit = true;  // This is 8-bit data

        const size_t image_size = width * height * target_channels;
        state_.image.original_data.resize(image_size);
        // Store 8-bit data in 16-bit buffer without scaling - preserve original values
        for (size_t i = 0; i < image_size; ++i) {
          state_.image.original_data[i] =
              static_cast<std::uint16_t>(image_data_8[i]);
        }

        stbi_image_free(image_data_8);
        state_.image.is_loaded = true;

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

    constexpr int jpeg_quality = 95;

    // Convert to 8-bit for saving (JPEG only supports 8-bit)
    std::vector<std::uint8_t> display_data_8(state_.image.display_data.size());

    if (state_.image.is_original_8bit) {
      // For original 8-bit data, just cast down (values are already in 0-255 range)
      for (size_t i = 0; i < state_.image.display_data.size(); ++i) {
        display_data_8[i] =
            static_cast<std::uint8_t>(state_.image.display_data[i]);
      }
    } else {
      // For original 16-bit data, scale down properly
      for (size_t i = 0; i < state_.image.display_data.size(); ++i) {
        display_data_8[i] =
            static_cast<std::uint8_t>(state_.image.display_data[i] >> 8);
      }
    }

    bool success = stbi_write_jpg(
        state_.image.save_path.c_str(), state_.image.width, state_.image.height,
        state_.image.channels, display_data_8.data(), jpeg_quality);

    if (!success) {
      std::cerr << "✗ Failed to save image: " << state_.image.save_path
                << std::endl;
    }
  }
}

void ToolbarController::updateColorSpace(int colorspace) {
  state_.selected_colorspace = colorspace;
  if (state_.image.is_loaded) {
    convertImage();

    // Update slider configuration for new color space
    SliderConfig::updateLabels(state_);

    // Force preview update after color space conversion
    PreviewController::forcePreviousUpdate();
  }
}

void ToolbarController::convertImage() {
  if (!state_.image.is_loaded || state_.image.original_data.empty()) return;

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

    state_.image.is_processed = true;
  } catch (const std::exception& e) {
    std::cerr << "✗ Error converting image: " << e.what() << std::endl;
    state_.image.is_processed = false;
  }
}

}  // namespace psm_gui::controller
