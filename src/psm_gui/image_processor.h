#pragma once

#include <span>
#include <vector>

#include "app_state.h"
#include "psm/psm.hpp"

namespace psm_gui {

class ImageProcessor {
 public:
  // Process the image based on the current conversion type
  static bool ProcessImage(const AppState& state,
                           std::vector<unsigned char>& imageData, int width,
                           int height, int channels);

 private:
  // PSM-specific conversions
  template <typename DstColorSpace>
  static void ConvertColorSpace(std::span<unsigned char> data);
};

}  // namespace psm_gui
