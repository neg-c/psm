#include "SliderConfig.hpp"

#include <functional>
#include <map>

namespace psm_gui::controller {

SliderConfig::Config SliderConfig::getConfig(int colorspace) {
  static const std::map<int, Config> configs = {
      {0, {"Luminance", "Contrast", sRGBAdjustment}},
      {1, {"Saturation", "Magenta-Green", adobeRGBAdjustment}},
      {2, {"Contrast", "Red-Orange", displayP3Adjustment}},
      {3, {"Chromaticity", "Temperature", oRGBAdjustment}}};

  auto it = configs.find(colorspace);
  return (it != configs.end()) ? it->second : configs.at(0);
}

void SliderConfig::updateLabels(AppState& state) {
  Config config = getConfig(state.selected_colorspace);
  state.sliders.vertical_label = config.vertical_label;
  state.sliders.horizontal_label = config.horizontal_label;
}

psm::Percent SliderConfig::getAdjustment(AppState& state) {
  Config config = getConfig(state.selected_colorspace);
  return config.adjustment_function(state.sliders.vertical_slider,
                                    state.sliders.horizontal_slider);
}

psm::Percent SliderConfig::sRGBAdjustment(int vertical, float horizontal) {
  int luminance_adjustment = vertical / 2;
  int contrast_adjustment = static_cast<int>(horizontal / 2);

  return psm::Percent{luminance_adjustment + contrast_adjustment,
                      luminance_adjustment + contrast_adjustment,
                      luminance_adjustment + contrast_adjustment};
}

psm::Percent SliderConfig::adobeRGBAdjustment(int vertical, float horizontal) {
  int saturation_adjustment = vertical / 2;
  int green_magenta_adjustment = static_cast<int>(horizontal / 2);

  return psm::Percent{-green_magenta_adjustment / 2,
                      saturation_adjustment + green_magenta_adjustment,
                      green_magenta_adjustment / 2};
}

psm::Percent SliderConfig::displayP3Adjustment(int vertical, float horizontal) {
  int contrast_adjustment = vertical / 2;
  int red_orange_adjustment = static_cast<int>(horizontal / 2);

  return psm::Percent{contrast_adjustment + red_orange_adjustment,
                      contrast_adjustment,
                      contrast_adjustment - red_orange_adjustment / 2};
}

psm::Percent SliderConfig::oRGBAdjustment(int vertical, float horizontal) {
  int chromaticity_adjustment = vertical / 2;
  int temperature_adjustment = static_cast<int>(horizontal / 2);

  return psm::Percent{chromaticity_adjustment + temperature_adjustment,
                      chromaticity_adjustment,
                      chromaticity_adjustment - temperature_adjustment};
}

}  // namespace psm_gui::controller
