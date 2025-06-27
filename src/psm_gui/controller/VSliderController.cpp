#include "VSliderController.hpp"

#include "PreviewController.hpp"
#include "SliderConfig.hpp"
#include "psm/adjust_channels.hpp"

namespace psm_gui::controller {
VSliderController::VSliderController(AppState &state) : state_(state) {}

void VSliderController::updateImage() {
  if (state_.io.loaded_image) {
    if (state_.io.display_image.size() != state_.io.converted_image.size()) {
      state_.io.display_image.resize(state_.io.converted_image.size());
    }

    std::copy(state_.io.converted_image.begin(),
              state_.io.converted_image.end(), state_.io.display_image.begin());

    std::span<unsigned char> output_span{state_.io.display_image};
    psm::AdjustChannels(output_span, SliderConfig::getAdjustment(state_));

    state_.io.image_processed = true;
    PreviewController::forcePreviousUpdate();
  }
}

}  // namespace psm_gui::controller
