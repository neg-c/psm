#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace psm_cli {

// Save image with appropriate format based on bit depth
template <typename DataType>
bool save_image(const std::vector<DataType>& image_data, int width, int height,
                const std::string& output_path);

}  // namespace psm_cli
