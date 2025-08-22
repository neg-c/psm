#include "image_processor.hpp"

#include <format>
#include <iostream>
#include <stdexcept>

#include "psm/adjust_channels.hpp"
#include "psm/psm.hpp"

namespace psm_cli {

namespace conversion {

template <typename TargetColorSpace, typename DataType>
void convert_from(std::string_view from_space, std::span<const DataType> input,
                  std::span<DataType> output) {
  if (from_space == "sRGB") {
    psm::Convert<psm::sRGB, TargetColorSpace>(input, output);
  } else if (from_space == "AdobeRGB") {
    psm::Convert<psm::AdobeRGB, TargetColorSpace>(input, output);
  } else if (from_space == "DisplayP3") {
    psm::Convert<psm::DisplayP3, TargetColorSpace>(input, output);
  } else if (from_space == "oRGB") {
    psm::Convert<psm::oRGB, TargetColorSpace>(input, output);
  } else if (from_space == "ProPhotoRGB") {
    psm::Convert<psm::ProPhotoRGB, TargetColorSpace>(input, output);
  } else {
    throw std::invalid_argument(
        std::format("Unsupported source color space: {}", from_space));
  }
}

template <typename DataType>
void convert_between(std::string_view from_space, std::string_view to_space,
                     std::span<const DataType> input,
                     std::span<DataType> output) {
  if (to_space == "sRGB") {
    convert_from<psm::sRGB>(from_space, input, output);
  } else if (to_space == "AdobeRGB") {
    convert_from<psm::AdobeRGB>(from_space, input, output);
  } else if (to_space == "DisplayP3") {
    convert_from<psm::DisplayP3>(from_space, input, output);
  } else if (to_space == "oRGB") {
    convert_from<psm::oRGB>(from_space, input, output);
  } else if (to_space == "ProPhotoRGB") {
    convert_from<psm::ProPhotoRGB>(from_space, input, output);
  } else {
    throw std::invalid_argument(
        std::format("Unsupported target color space: {}", to_space));
  }
}

// Explicit template instantiations
template void convert_from<psm::sRGB, uint8_t>(std::string_view,
                                               std::span<const uint8_t>,
                                               std::span<uint8_t>);
template void convert_from<psm::AdobeRGB, uint8_t>(std::string_view,
                                                   std::span<const uint8_t>,
                                                   std::span<uint8_t>);
template void convert_from<psm::DisplayP3, uint8_t>(std::string_view,
                                                    std::span<const uint8_t>,
                                                    std::span<uint8_t>);
template void convert_from<psm::oRGB, uint8_t>(std::string_view,
                                               std::span<const uint8_t>,
                                               std::span<uint8_t>);
template void convert_from<psm::ProPhotoRGB, uint8_t>(std::string_view,
                                                      std::span<const uint8_t>,
                                                      std::span<uint8_t>);

template void convert_from<psm::sRGB, uint16_t>(std::string_view,
                                                std::span<const uint16_t>,
                                                std::span<uint16_t>);
template void convert_from<psm::AdobeRGB, uint16_t>(std::string_view,
                                                    std::span<const uint16_t>,
                                                    std::span<uint16_t>);
template void convert_from<psm::DisplayP3, uint16_t>(std::string_view,
                                                     std::span<const uint16_t>,
                                                     std::span<uint16_t>);
template void convert_from<psm::oRGB, uint16_t>(std::string_view,
                                                std::span<const uint16_t>,
                                                std::span<uint16_t>);
template void convert_from<psm::ProPhotoRGB, uint16_t>(
    std::string_view, std::span<const uint16_t>, std::span<uint16_t>);

template void convert_between<uint8_t>(std::string_view, std::string_view,
                                       std::span<const uint8_t>,
                                       std::span<uint8_t>);
template void convert_between<uint16_t>(std::string_view, std::string_view,
                                        std::span<const uint16_t>,
                                        std::span<uint16_t>);

}  // namespace conversion

template <typename DataType>
std::vector<DataType> process_image(const ImageData<DataType>& image_data,
                                    const CLIOptions& options) {
  const size_t image_size = image_data.size();
  std::span<const DataType> input_image{image_data.data(), image_size};

  // Create output storage
  std::vector<DataType> output_storage(image_size);
  std::span<DataType> output_image{output_storage};

  std::cout << std::format("Processing {}x{} image with {}-bit precision\n",
                           image_data.width(), image_data.height(),
                           sizeof(DataType) * 8);
  std::cout << std::format("Converting from {} to {}\n", options.from_space,
                           options.to_space);

  // Perform color space conversion
  conversion::convert_between<DataType>(options.from_space, options.to_space,
                                        input_image, output_image);

  // Apply channel adjustments if requested
  if (options.adjust_values) {
    std::cout << std::format("Adjusting channels by R:{}%, G:{}%, B:{}%\n",
                             options.adjust_values->channel(0),
                             options.adjust_values->channel(1),
                             options.adjust_values->channel(2));

    psm::AdjustChannels(output_image, *options.adjust_values);

    // Convert back to original color space if needed
    if (options.from_space != options.to_space) {
      std::vector<DataType> temp_storage(output_image.size());
      std::span<DataType> temp_image{temp_storage};

      conversion::convert_between<DataType>(
          options.to_space, options.from_space, output_image, temp_image);
      output_storage = std::move(temp_storage);
    }
  }

  return output_storage;
}

// Explicit template instantiations
template std::vector<uint8_t> process_image<uint8_t>(const ImageData<uint8_t>&,
                                                     const CLIOptions&);
template std::vector<uint16_t> process_image<uint16_t>(
    const ImageData<uint16_t>&, const CLIOptions&);

}  // namespace psm_cli
