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
    state_.io.load_path_ = std::string(path);
    NFD_FreePathU8(path);

    int width, height, channels;
    constexpr int target_channels = 3;

    unsigned char *image_data = stbi_load(state_.io.load_path_.c_str(), &width,
                                          &height, &channels, target_channels);

    if (image_data) {
      state_.io.width = width;
      state_.io.height = height;
      state_.io.channels = target_channels;

      const size_t image_size = width * height * target_channels;
      state_.io.original_image.resize(image_size);
      std::copy(image_data, image_data + image_size,
                state_.io.original_image.begin());

      stbi_image_free(image_data);
      state_.io.loaded_image = true;

      convertImage();
    } else {
      std::cerr << "Failed to load image: " << state_.io.load_path_
                << std::endl;
      state_.io.loaded_image = false;
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
  if (result == NFD_OKAY && state_.io.loaded_image &&
      state_.io.image_processed) {
    state_.io.save_path_ = std::string(path);
    NFD_FreePathU8(path);

    constexpr int jpeg_quality = 95;
    bool success = stbi_write_jpg(state_.io.save_path_.c_str(), state_.io.width,
                                  state_.io.height, state_.io.channels,
                                  state_.io.display_image.data(), jpeg_quality);

    if (!success) {
      std::cerr << "Failed to save image: " << state_.io.save_path_
                << std::endl;
    }
  }
}

void ToolbarController::updateColorSpace(int colorspace) {
  state_.selected_colorspace = colorspace;
  if (state_.io.loaded_image) {
    convertImage();

    // Update slider configuration for new color space
    SliderConfig::updateLabels(state_);

    // Force preview update after color space conversion
    PreviewController::forcePreviousUpdate();
  }
}

void ToolbarController::convertImage() {
  if (!state_.io.loaded_image || state_.io.original_image.empty()) return;

  const size_t image_size =
      state_.io.width * state_.io.height * state_.io.channels;
  state_.io.converted_image.resize(image_size);
  state_.io.display_image.resize(image_size);

  std::span<const unsigned char> input_span{state_.io.original_image};
  std::span<unsigned char> converted_span{state_.io.converted_image};

  try {
    switch (state_.selected_colorspace) {
      case 0:
        psm::Convert<psm::sRGB, psm::sRGB>(input_span, converted_span);
        break;
      case 1:
        psm::Convert<psm::sRGB, psm::AdobeRGB>(input_span, converted_span);
        break;
      case 2:
        psm::Convert<psm::sRGB, psm::DisplayP3>(input_span, converted_span);
        break;
      case 3:
        psm::Convert<psm::sRGB, psm::oRGB>(input_span, converted_span);
        break;
      default:
        psm::Convert<psm::sRGB, psm::sRGB>(input_span, converted_span);
    }

    std::copy(state_.io.converted_image.begin(),
              state_.io.converted_image.end(), state_.io.display_image.begin());

    state_.io.image_processed = true;
  } catch (const std::exception &e) {
    std::cerr << "Error converting image: " << e.what() << std::endl;
    state_.io.image_processed = false;
  }
}

}  // namespace psm_gui::controller
