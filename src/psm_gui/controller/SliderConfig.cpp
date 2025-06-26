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
  float brightness_factor = 1.0f + (vertical / 200.0f);
  float contrast_factor = 1.0f + (horizontal / 200.0f);

  return psm::Percent{
      static_cast<int>(brightness_factor * contrast_factor * 100.0f),
      static_cast<int>(brightness_factor * 100.0f),
      static_cast<int>(brightness_factor * contrast_factor * 100.0f)};
}

psm::Percent SliderConfig::adobeRGBAdjustment(int vertical, float horizontal) {
  float saturation_factor = 1.0f + (vertical / 200.0f);
  float green_magenta_factor = 1.0f + (horizontal / 200.0f);

  return psm::Percent{
      static_cast<int>(100.0f),
      static_cast<int>(saturation_factor * green_magenta_factor * 100.0f),
      static_cast<int>(100.0f)};
}

psm::Percent SliderConfig::displayP3Adjustment(int vertical, float horizontal) {
  float red_factor = 1.0f + (vertical / 200.0f);
  float blue_factor = 1.0f + (horizontal / 200.0f);

  return psm::Percent{static_cast<int>(red_factor * 100.0f),
                      static_cast<int>(100.0f),
                      static_cast<int>(blue_factor * 100.0f)};
}

psm::Percent SliderConfig::oRGBAdjustment(int vertical, float horizontal) {
  float luminance_factor = 1.0f + (vertical / 200.0f);
  float chroma_factor = 1.0f + (horizontal / 200.0f);

  return psm::Percent{
      static_cast<int>(luminance_factor * chroma_factor * 100.0f),
      static_cast<int>(luminance_factor * 100.0f),
      static_cast<int>(luminance_factor * chroma_factor * 100.0f)};
}

}  // namespace psm_gui::controller
