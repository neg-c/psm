#pragma once

#include <cstdint>
#include <functional>
#include <span>
#include <vector>

#include "app/AppState.hpp"
#include "psm/percent.hpp"

namespace psm_gui::controller {

class SliderConfig {
 public:
  struct Config {
    std::string vertical_label;
    std::string horizontal_label;
    std::function<psm::Percent(int, float)> adjustment_function;
  };

  static Config getConfig(int colorspace);
  static void updateLabels(AppState& state);
  static psm::Percent getAdjustment(AppState& state);
  
  // Overloaded functions for different data types
  static void applyAdjustmentAndConvert(AppState& state,
                                        std::span<unsigned char> image_span);
  static void applyAdjustmentAndConvert(AppState& state,
                                        std::span<uint16_t> image_span);

 private:
  static psm::Percent sRGBAdjustment(int vertical, float horizontal);
  static psm::Percent adobeRGBAdjustment(int vertical, float horizontal);
  static psm::Percent displayP3Adjustment(int vertical, float horizontal);
  static psm::Percent oRGBAdjustment(int vertical, float horizontal);
};

}  // namespace psm_gui::controller
