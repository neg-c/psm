#pragma once

#include <span>
#include <string_view>
#include <vector>

#include "cli_parser.hpp"
#include "image_loader.hpp"

namespace psm_cli {

namespace conversion {

// Convert between color spaces with templated data types
template <typename TargetColorSpace, typename DataType>
void convert_from(std::string_view from_space, std::span<const DataType> input,
                  std::span<DataType> output);

template <typename DataType>
void convert_between(std::string_view from_space, std::string_view to_space,
                     std::span<const DataType> input,
                     std::span<DataType> output);

}  // namespace conversion

// Process image with color space conversion and optional channel adjustments
template <typename DataType>
std::vector<DataType> process_image(const ImageData<DataType>& image_data,
                                    const CLIOptions& options);

}  // namespace psm_cli
