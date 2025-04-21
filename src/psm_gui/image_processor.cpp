#include "image_processor.h"

#include <algorithm>

namespace psm_gui {

bool ImageProcessor::ProcessImage(const AppState& state,
                                  std::vector<unsigned char>& imageData,
                                  int width, int height, int channels) {
  if (imageData.empty() || width <= 0 || height <= 0 || channels <= 0) {
    return false;
  }

  std::span<unsigned char> dataSpan(imageData);

  switch (state.getCurrentConversionType()) {
    case ConversionType::ADOBE_RGB:
      ConvertColorSpace<psm::AdobeRGB>(dataSpan);
      break;

    case ConversionType::DISPLAY_P3:
      ConvertColorSpace<psm::DisplayP3>(dataSpan);
      break;

    case ConversionType::PRO_PHOTO_RGB:
      ConvertColorSpace<psm::ProPhotoRGB>(dataSpan);
      break;

    case ConversionType::ORGB:
      ConvertColorSpace<psm::oRGB>(dataSpan);
      break;

    default:
      return false;
  }

  return true;
}

template <typename DstColorSpace>
void ImageProcessor::ConvertColorSpace(std::span<unsigned char> data) {
  // Create a copy of the data for the conversion
  std::vector<unsigned char> result(data.begin(), data.end());

  // Convert from sRGB to the destination color space
  psm::Convert<psm::sRGB, DstColorSpace>(data, result);

  // Copy the result back to the original data
  std::copy(result.begin(), result.end(), data.begin());
}

}  // namespace psm_gui
