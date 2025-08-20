#include "SliderConfig.hpp"

#include <algorithm>
#include <functional>
#include <map>

#include "psm/adjust_channels.hpp"
#include "psm/psm.hpp"

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
  state.controls.vertical_label = config.vertical_label;
  state.controls.horizontal_label = config.horizontal_label;
}

psm::Percent SliderConfig::getAdjustment(AppState& state) {
  Config config = getConfig(state.selected_colorspace);
  return config.adjustment_function(state.controls.vertical_slider,
                                    state.controls.horizontal_slider);
}

template <typename T>
void SliderConfig::applyAdjustmentAndConvertT(AppState& state,
                                              std::span<T> image_span) {
  if (state.image.is_original_8bit) {
    std::vector<std::uint8_t> temp_8bit(image_span.size());
    for (size_t i = 0; i < image_span.size(); ++i) {
      temp_8bit[i] = static_cast<std::uint8_t>(image_span[i]);
    }
    std::span<std::uint8_t> span_8bit{temp_8bit};

    psm::AdjustChannels(span_8bit, getAdjustment(state));

    if (state.selected_colorspace == 3) {  // oRGB
      std::vector<std::uint8_t> temp_image(span_8bit.size());
      std::span<std::uint8_t> temp_span{temp_image};
      psm::Convert<psm::oRGB, psm::sRGB>(span_8bit, temp_span);
      std::copy(temp_image.begin(), temp_image.end(), span_8bit.begin());
    }

    std::transform(temp_8bit.cbegin(), temp_8bit.cend(), image_span.begin(),
                   [](std::uint8_t val) { return static_cast<T>(val); });
  } else {
    psm::AdjustChannels(image_span, getAdjustment(state));

    if (state.selected_colorspace == 3) {  // oRGB
      std::vector<T> temp_image(image_span.size());
      std::span<T> temp_span{temp_image};

      psm::Convert<psm::oRGB, psm::sRGB>(image_span, temp_span);

      std::copy(temp_image.begin(), temp_image.end(), image_span.begin());
    }
  }
}

// Explicit template instantiation for 16-bit only
template void SliderConfig::applyAdjustmentAndConvertT<std::uint16_t>(
    AppState& state, std::span<std::uint16_t> image_span);

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
