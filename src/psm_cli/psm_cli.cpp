#include <cstddef>
#include <format>
#include <iostream>
#include <ranges>
#include <vector>

#include "psm/adjust_channels.hpp"
#include "psm/percent.hpp"
#include "psm/psm.hpp"

template <std::ranges::contiguous_range Buffer>
void print_buffer(const Buffer& buffer) {
  for (size_t i = 0; i < std::ranges::size(buffer); ++i) {
    std::cout << std::format("[{}] ", buffer[i]);
  }
  std::cout << "\n";
}

int main() {
  const std::vector<unsigned char> input_image = {
      255, 0,   0,    // Red pixel (R=255, G=0, B=0)
      0,   255, 0,    // Green pixel (R=0, G=255, B=0)
      0,   0,   255,  // Blue pixel (R=0, G=0, B=255)
      255, 255, 0     // Yellow pixel (R=255, G=255, B=0)
  };

  std::vector<unsigned char> output_image(input_image.size());

  std::cout << "Input Image (BGR):\n";
  print_buffer(input_image);

  psm::Convert<psm::sRGB, psm::AdobeRGB>(input_image, output_image);
  psm::AdjustChannels(output_image, psm::Percent(20, 0, 10));
  psm::Convert<psm::AdobeRGB, psm::sRGB>(output_image, output_image);

  psm::Convert<psm::sRGB, psm::oRGB>(output_image, output_image);
  psm::Convert<psm::oRGB, psm::sRGB>(output_image, output_image);

  psm::Convert<psm::sRGB, psm::DisplayP3>(output_image, output_image);
  psm::Convert<psm::DisplayP3, psm::sRGB>(output_image, output_image);

  std::cout << "Output Image (RGB):\n";
  print_buffer(output_image);

  return 0;
}
