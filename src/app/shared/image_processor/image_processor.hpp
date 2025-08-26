#pragma once

#include <optional>
#include <span>
#include <string_view>
#include <vector>

#include "image_io/image_io.hpp"

struct CLIOptions;

namespace psm_cli {

namespace conversion {

template <typename TargetColorSpace, typename DataType>
void convert_from(std::string_view from_space, std::span<const DataType> input,
                  std::span<DataType> output);

template <typename DataType>
void convert_between(std::string_view from_space, std::string_view to_space,
                     std::span<const DataType> input,
                     std::span<DataType> output);

}  // namespace conversion

template <typename DataType>
void convert_colorspace(std::span<const DataType> input,
                        std::span<DataType> output, int from_colorspace_id,
                        int to_colorspace_id);

template <typename DataType>
std::vector<DataType> process_image(const ImageData<DataType>& image_data,
                                    const CLIOptions& options);

}  // namespace psm_cli
