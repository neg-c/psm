#include "SliderController.hpp"

#include "psm/adjust_channels.hpp"
#include "psm/percent.hpp"

namespace psm_gui::controller {
SliderController::SliderController(AppState &state) : state_(state) {}

void SliderController::updateImage() {
  if (state_.io.loaded_image) {
    std::span<const unsigned char> input_span{state_.io.original_image};
    psm::AdjustChannels(input_span,
                        psm::Percent{state_.sliders.vertical_slider, 0, 0});
  }
}

}  // namespace psm_gui::controller
