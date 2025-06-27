#include "SliderConfig.hpp"

#include <functional>
#include <map>

namespace psm_gui::controller {

SliderConfig::Config SliderConfig::getConfig(int colorspace) {
  static const std::map<int, Config> configs = {
      {0, {"Brightness", "Contrast", sRGBAdjustment}},
      {1, {"Saturation", "Green-Magenta", adobeRGBAdjustment}},
      {2, {"Red Enhancement", "Blue Balance", displayP3Adjustment}},
      {3, {"Luminance", "Chroma", oRGBAdjustment}}};

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
  int brightness_adjustment = vertical / 2;
  int contrast_adjustment = static_cast<int>(horizontal / 2);

  return psm::Percent{brightness_adjustment + contrast_adjustment,
                      brightness_adjustment,
                      brightness_adjustment + contrast_adjustment};
}

psm::Percent SliderConfig::adobeRGBAdjustment(int vertical, float horizontal) {
  int saturation_adjustment = vertical / 2;
  int green_magenta_adjustment = static_cast<int>(horizontal / 2);

  return psm::Percent{0, saturation_adjustment + green_magenta_adjustment, 0};
}

psm::Percent SliderConfig::displayP3Adjustment(int vertical, float horizontal) {
  int red_adjustment = vertical / 2;
  int blue_adjustment = static_cast<int>(horizontal / 2);

  return psm::Percent{red_adjustment, 0, blue_adjustment};
}

psm::Percent SliderConfig::oRGBAdjustment(int vertical, float horizontal) {
  int luminance_adjustment = vertical / 2;
  int chroma_adjustment = static_cast<int>(horizontal / 2);

  return psm::Percent{luminance_adjustment + chroma_adjustment,
                      luminance_adjustment,
                      luminance_adjustment + chroma_adjustment};
}

}  // namespace psm_gui::controller
