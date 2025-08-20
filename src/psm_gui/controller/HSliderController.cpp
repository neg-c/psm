#include "HSliderController.hpp"

#include "PreviewController.hpp"
#include "SliderConfig.hpp"

namespace psm_gui::controller {
HSliderController::HSliderController(AppState& state) : state_(state) {}

void HSliderController::updateImage() {
  if (state_.image.is_loaded) {
    const size_t image_size = state_.image.getImageSize();

    if (state_.image.display_data.size() != image_size) {
      state_.image.display_data.resize(image_size);
    }
    if (state_.image.converted_data.size() != image_size) {
      state_.image.converted_data.resize(image_size);
    }

    std::copy(state_.image.converted_data.begin(),
              state_.image.converted_data.end(),
              state_.image.display_data.begin());

    std::span<std::uint16_t> output_span{state_.image.display_data};
    SliderConfig::applyAdjustmentAndConvertT(state_, output_span);

    state_.image.is_processed = true;
    PreviewController::forcePreviousUpdate();
  }
}

}  // namespace psm_gui::controller
