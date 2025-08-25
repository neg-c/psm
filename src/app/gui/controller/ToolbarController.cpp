#include "ToolbarController.hpp"

#include <nfd.h>

#include <iostream>
#include <span>
#include <string>
#include <vector>

#include "PreviewController.hpp"
#include "SliderConfig.hpp"
#include "image_io/image_io.hpp"
#include "image_processor/image_processor.hpp"
#include "psm/adjust_channels.hpp"
#include "psm/detail/orgb.hpp"
#include "psm/detail/pro_photo_rgb.hpp"
#include "psm/detail/srgb.hpp"
#include "psm/percent.hpp"
#include "psm/psm.hpp"

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

    try {
      auto image_variant = psm_cli::load_image(state_.image.load_path);

      // Visit the variant to handle both 8-bit and 16-bit images
      std::visit(
          [this](auto& image_data) {
            if (image_data) {
              state_.image.width = image_data.width();
              state_.image.height = image_data.height();
              state_.image.channels = image_data.channels();

              // Convert to 8-bit for GUI display (GUI currently expects uint8_t)
              const size_t image_size = image_data.size();
              state_.image.original_data.resize(image_size);

              if constexpr (std::is_same_v<
                                std::decay_t<decltype(*image_data.data())>,
                                uint8_t>) {
                // Already 8-bit, direct copy
                std::copy(image_data.data(), image_data.data() + image_size,
                          state_.image.original_data.begin());
              } else {
                // Convert 16-bit to 8-bit for display
                const uint16_t* src = image_data.data();
                for (size_t i = 0; i < image_size; ++i) {
                  state_.image.original_data[i] =
                      static_cast<uint8_t>(src[i] / 257);
                }
              }

              state_.image.is_loaded = true;
              convertImage();
            } else {
              state_.image.is_loaded = false;
            }
          },
          image_variant);

    } catch (const std::exception& e) {
      std::cerr << "Failed to load image: " << state_.image.load_path << " - "
                << e.what() << std::endl;
      state_.image.is_loaded = false;
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

    try {
      // Create ImageData object for saving
      psm_cli::ImageData<uint8_t> image_data(
          state_.image.display_data,  // This is already std::vector<uint8_t>
          state_.image.width, state_.image.height, state_.image.channels);

      bool success = psm_cli::save_image(image_data, state_.image.save_path);

      if (!success) {
        std::cerr << "Failed to save image: " << state_.image.save_path
                  << std::endl;
      }

    } catch (const std::exception& e) {
      std::cerr << "Failed to save image: " << state_.image.save_path << " - "
                << e.what() << std::endl;
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
    // Use shared image processor for consistent color space conversion
    psm_cli::convert_colorspace<unsigned char>(input_span, converted_span, 0,
                                               state_.selected_colorspace);

    std::copy(state_.image.converted_data.begin(),
              state_.image.converted_data.end(),
              state_.image.display_data.begin());

    if (state_.selected_colorspace ==
        3) {  // oRGB - convert back to sRGB for display
      std::vector<unsigned char> temp_image(state_.image.display_data.size());
      std::span<unsigned char> temp_span{temp_image};

      psm_cli::convert_colorspace<unsigned char>(
          std::span<unsigned char>{state_.image.display_data}, temp_span, 3,
          0);  // oRGB to sRGB

      std::copy(temp_image.begin(), temp_image.end(),
                state_.image.display_data.begin());
    }

    state_.image.is_processed = true;
  } catch (const std::exception& e) {
    std::cerr << "Error converting image: " << e.what() << std::endl;
    state_.image.is_processed = false;
  }
}

}  // namespace psm_gui::controller
