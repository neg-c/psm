#include "ToolbarController.hpp"

#include <nfd.h>

#include "PreviewController.hpp"
#include "SliderConfig.hpp"
#include "psm/adjust_channels.hpp"
#include "psm/detail/orgb.hpp"
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

ToolbarController::ToolbarController(AppState &state) : state_(state) {}

void ToolbarController::loadImage() {
  nfdu8char_t *path = nullptr;
  nfdu8filteritem_t filters[1] = {{"Image files", "png,jpeg,jpg,webp"}};
  nfdopendialogu8args_t args{};
  args.filterList = filters;
  args.filterCount = 1;

  nfdresult_t result = NFD_OpenDialogU8_With(&path, &args);
  if (result == NFD_OKAY) {
    state_.image.load_path = std::string(path);
    NFD_FreePathU8(path);

    int width, height, channels;
    constexpr int target_channels = 3;

    unsigned char *image_data =
        stbi_load(state_.image.load_path.c_str(), &width, &height, &channels,
                  target_channels);

    if (image_data) {
      state_.image.width = width;
      state_.image.height = height;
      state_.image.channels = target_channels;

      const size_t image_size = width * height * target_channels;
      state_.image.original_data.resize(image_size);
      std::copy(image_data, image_data + image_size,
                state_.image.original_data.begin());

      stbi_image_free(image_data);
      state_.image.is_loaded = true;

      convertImage();
    } else {
      std::cerr << "Failed to load image: " << state_.image.load_path
                << std::endl;
      state_.image.is_loaded = false;
    }
  }
}

void ToolbarController::saveImage() {
  nfdu8char_t *path = nullptr;
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
    bool success = stbi_write_jpg(
        state_.image.save_path.c_str(), state_.image.width, state_.image.height,
        state_.image.channels, state_.image.display_data.data(), jpeg_quality);

    if (!success) {
      std::cerr << "Failed to save image: " << state_.image.save_path
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
  state_.image.converted_data.resize(image_size);
  state_.image.display_data.resize(image_size);

  std::span<const unsigned char> input_span{state_.image.original_data};
  std::span<unsigned char> converted_span{state_.image.converted_data};

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

    std::copy(state_.image.converted_data.begin(),
              state_.image.converted_data.end(),
              state_.image.display_data.begin());

    if (state_.selected_colorspace == 3) {  // oRGB
      std::vector<unsigned char> temp_image(state_.image.display_data.size());
      std::span<unsigned char> temp_span{temp_image};

      psm::Convert<psm::oRGB, psm::sRGB>(
          std::span<unsigned char>{state_.image.display_data}, temp_span);

      std::copy(temp_image.begin(), temp_image.end(),
                state_.image.display_data.begin());
    }

    state_.image.is_processed = true;
  } catch (const std::exception &e) {
    std::cerr << "Error converting image: " << e.what() << std::endl;
    state_.image.is_processed = false;
  }
}

}  // namespace psm_gui::controller
