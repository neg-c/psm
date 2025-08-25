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
  nfdu8filteritem_t filters[2] = {{"PNG files", "png"},
                                  {"JPEG files", "jpeg, jpg"}};
  nfdopendialogu8args_t args{};
  args.filterList = filters;
  args.filterCount = 2;

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

              const size_t image_size = image_data.size();

              // Determine if this is 16-bit data
              using DataType = std::decay_t<decltype(*image_data.data())>;
              constexpr bool is_16bit = std::is_same_v<DataType, uint16_t>;
              state_.image.is_16bit = is_16bit;

              if constexpr (is_16bit) {
                // Store 16-bit data directly
                std::vector<uint16_t> original_data(image_size);
                std::copy(image_data.data(), image_data.data() + image_size,
                          original_data.begin());
                state_.image.original_data = std::move(original_data);

                // Initialize converted and display data as 16-bit
                state_.image.converted_data = std::vector<uint16_t>(image_size);
                state_.image.display_data = std::vector<uint16_t>(image_size);
              } else {
                // Store 8-bit data directly
                std::vector<unsigned char> original_data(image_size);
                std::copy(image_data.data(), image_data.data() + image_size,
                          original_data.begin());
                state_.image.original_data = std::move(original_data);

                // Initialize converted and display data as 8-bit
                state_.image.converted_data =
                    std::vector<unsigned char>(image_size);
                state_.image.display_data =
                    std::vector<unsigned char>(image_size);
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
  nfdu8filteritem_t filters[2] = {{"PNG files", "png"},
                                  {"JPEG files", "jpeg, jpg"}};
  nfdsavedialogu8args_t args{};
  args.filterList = filters;
  args.defaultName = "converted_image.png";
  args.filterCount = 2;

  nfdresult_t result = NFD_SaveDialogU8_With(&path, &args);
  if (result == NFD_OKAY && state_.image.is_loaded &&
      state_.image.is_processed) {
    state_.image.save_path = std::string(path);
    NFD_FreePathU8(path);

    try {
      bool success = false;

      if (state_.image.is_16bit) {
        // Save 16-bit image
        std::vector<uint16_t> display_data =
            std::get<std::vector<uint16_t>>(state_.image.display_data);
        psm_cli::ImageData<uint16_t> image_data(
            std::move(display_data), state_.image.width, state_.image.height,
            state_.image.channels);
        success = psm_cli::save_image(image_data, state_.image.save_path);
      } else {
        // Save 8-bit image
        std::vector<unsigned char> display_data =
            std::get<std::vector<unsigned char>>(state_.image.display_data);
        psm_cli::ImageData<uint8_t> image_data(
            std::move(display_data), state_.image.width, state_.image.height,
            state_.image.channels);
        success = psm_cli::save_image(image_data, state_.image.save_path);
      }

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
  if (!state_.image.is_loaded) return;

  const size_t image_size = state_.image.getImageSize();

  if (state_.image.is_16bit) {
    // Handle 16-bit image conversion
    std::vector<uint16_t>& original_data =
        std::get<std::vector<uint16_t>>(state_.image.original_data);
    std::vector<uint16_t>& converted_data =
        std::get<std::vector<uint16_t>>(state_.image.converted_data);
    std::vector<uint16_t>& display_data =
        std::get<std::vector<uint16_t>>(state_.image.display_data);

    if (original_data.empty()) return;

    converted_data.resize(image_size);
    display_data.resize(image_size);

    std::span<const uint16_t> input_span{original_data};
    std::span<uint16_t> converted_span{converted_data};

    try {
      // Use shared image processor for consistent color space conversion
      psm_cli::convert_colorspace<uint16_t>(input_span, converted_span, 0,
                                            state_.selected_colorspace);

      std::copy(converted_data.begin(), converted_data.end(),
                display_data.begin());

      if (state_.selected_colorspace ==
          3) {  // oRGB - convert back to sRGB for display
        std::vector<uint16_t> temp_image(display_data.size());
        std::span<uint16_t> temp_span{temp_image};

        psm_cli::convert_colorspace<uint16_t>(std::span<uint16_t>{display_data},
                                              temp_span, 3, 0);  // oRGB to sRGB

        std::copy(temp_image.begin(), temp_image.end(), display_data.begin());
      }

      state_.image.is_processed = true;
    } catch (const std::exception& e) {
      std::cerr << "Error converting 16-bit image: " << e.what() << std::endl;
      state_.image.is_processed = false;
    }
  } else {
    // Handle 8-bit image conversion (existing logic)
    std::vector<unsigned char>& original_data =
        std::get<std::vector<unsigned char>>(state_.image.original_data);
    std::vector<unsigned char>& converted_data =
        std::get<std::vector<unsigned char>>(state_.image.converted_data);
    std::vector<unsigned char>& display_data =
        std::get<std::vector<unsigned char>>(state_.image.display_data);

    if (original_data.empty()) return;

    converted_data.resize(image_size);
    display_data.resize(image_size);

    std::span<const unsigned char> input_span{original_data};
    std::span<unsigned char> converted_span{converted_data};

    try {
      // Use shared image processor for consistent color space conversion
      psm_cli::convert_colorspace<unsigned char>(input_span, converted_span, 0,
                                                 state_.selected_colorspace);

      std::copy(converted_data.begin(), converted_data.end(),
                display_data.begin());

      if (state_.selected_colorspace ==
          3) {  // oRGB - convert back to sRGB for display
        std::vector<unsigned char> temp_image(display_data.size());
        std::span<unsigned char> temp_span{temp_image};

        psm_cli::convert_colorspace<unsigned char>(
            std::span<unsigned char>{display_data}, temp_span, 3,
            0);  // oRGB to sRGB

        std::copy(temp_image.begin(), temp_image.end(), display_data.begin());
      }

      state_.image.is_processed = true;
    } catch (const std::exception& e) {
      std::cerr << "Error converting 8-bit image: " << e.what() << std::endl;
      state_.image.is_processed = false;
    }
  }
}

}  // namespace psm_gui::controller
