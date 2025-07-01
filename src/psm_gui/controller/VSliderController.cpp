#include "VSliderController.hpp"

#include "PreviewController.hpp"
#include "SliderConfig.hpp"

namespace psm_gui::controller {
VSliderController::VSliderController(AppState &state) : state_(state) {}

void VSliderController::updateImage() {
  if (state_.image.is_loaded) {
    if (state_.image.display_data.size() !=
        state_.image.converted_data.size()) {
      state_.image.display_data.resize(state_.image.converted_data.size());
    }

    std::copy(state_.image.converted_data.begin(),
              state_.image.converted_data.end(),
              state_.image.display_data.begin());

    std::span<unsigned char> output_span{state_.image.display_data};
    SliderConfig::applyAdjustmentAndConvert(state_, output_span);

    state_.image.is_processed = true;
    PreviewController::forcePreviousUpdate();
  }
}

}  // namespace psm_gui::controller
