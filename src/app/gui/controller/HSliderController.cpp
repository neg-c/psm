#include "HSliderController.hpp"

#include "PreviewController.hpp"
#include "SliderConfig.hpp"

namespace psm_gui::controller {
HSliderController::HSliderController(AppState &state) : state_(state) {}

void HSliderController::updateImage() {
  if (state_.image.is_loaded) {
    if (state_.image.is_16bit) {
      // Handle 16-bit image updates
      std::vector<uint16_t>& converted_data = std::get<std::vector<uint16_t>>(state_.image.converted_data);
      std::vector<uint16_t>& display_data = std::get<std::vector<uint16_t>>(state_.image.display_data);
      
      if (display_data.size() != converted_data.size()) {
        display_data.resize(converted_data.size());
      }

      std::copy(converted_data.begin(), converted_data.end(), display_data.begin());

      std::span<uint16_t> output_span{display_data};
      SliderConfig::applyAdjustmentAndConvert(state_, output_span);

      state_.image.is_processed = true;
      PreviewController::forcePreviousUpdate();
    } else {
      // Handle 8-bit image updates (existing logic)
      std::vector<unsigned char>& converted_data = std::get<std::vector<unsigned char>>(state_.image.converted_data);
      std::vector<unsigned char>& display_data = std::get<std::vector<unsigned char>>(state_.image.display_data);
      
      if (display_data.size() != converted_data.size()) {
        display_data.resize(converted_data.size());
      }

      std::copy(converted_data.begin(), converted_data.end(), display_data.begin());

      std::span<unsigned char> output_span{display_data};
      SliderConfig::applyAdjustmentAndConvert(state_, output_span);

      state_.image.is_processed = true;
      PreviewController::forcePreviousUpdate();
    }
  }
}

}  // namespace psm_gui::controller
