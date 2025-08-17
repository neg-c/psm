#include "HSliderController.hpp"

#include "PreviewController.hpp"
#include "SliderConfig.hpp"

namespace psm_gui::controller {
HSliderController::HSliderController(AppState& state) : state_(state) {}

void HSliderController::updateImage() {
  if (state_.image.is_loaded) {
    const size_t image_size = state_.image.getImageSize();

    // Handle 16-bit data
    if (state_.image.bit_depth == AppState::ImageData::BitDepth::BITS_16) {
      if (!std::holds_alternative<std::vector<std::uint16_t>>(
              state_.image.display_data)) {
        state_.image.display_data = std::vector<std::uint16_t>(image_size);
      }
      if (!std::holds_alternative<std::vector<std::uint16_t>>(
              state_.image.converted_data)) {
        state_.image.converted_data = std::vector<std::uint16_t>(image_size);
      }

      auto& converted_data = state_.image.getConvertedData<std::uint16_t>();
      auto& display_data = state_.image.getDisplayData<std::uint16_t>();

      std::copy(converted_data.begin(), converted_data.end(),
                display_data.begin());

      std::span<std::uint16_t> output_span{display_data};
      SliderConfig::applyAdjustmentAndConvertT(state_, output_span);
    } else {
      // Handle 8-bit data
      if (!std::holds_alternative<std::vector<std::uint8_t>>(
              state_.image.display_data)) {
        state_.image.display_data = std::vector<std::uint8_t>(image_size);
      }
      if (!std::holds_alternative<std::vector<std::uint8_t>>(
              state_.image.converted_data)) {
        state_.image.converted_data = std::vector<std::uint8_t>(image_size);
      }

      auto& converted_data = state_.image.getConvertedData<std::uint8_t>();
      auto& display_data = state_.image.getDisplayData<std::uint8_t>();

      std::copy(converted_data.begin(), converted_data.end(),
                display_data.begin());

      std::span<std::uint8_t> output_span{display_data};
      SliderConfig::applyAdjustmentAndConvertT(state_, output_span);
    }

    state_.image.is_processed = true;
    PreviewController::forcePreviousUpdate();
  }
}

}  // namespace psm_gui::controller
