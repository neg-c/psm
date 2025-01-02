#include <format>
#include <iostream>
#include <ranges>
#include <vector>

#include "psm/orgb.hpp"
#include "psm/psm.hpp"

template <std::ranges::contiguous_range Buffer>
void print_buffer(const Buffer& buffer) {
  for (size_t i = 0; i < std::ranges::size(buffer); ++i) {
    std::cout << std::format("[{}] ", buffer[i]);
  }
  std::cout << "\n";
}

int main() {
  std::vector<unsigned char> input_image = {
      255, 0,   0,   255,  // Red pixel (R=255, G=0, B=0, A=255)
      0,   255, 0,   255,  // Green pixel (R=0, G=255, B=0, A=255)
      0,   0,   255, 255,  // Blue pixel (R=0, G=0, B=255, A=255)
      255, 255, 0,   255   // Yellow pixel (R=255, G=255, B=0, A=255)
  };

  std::vector<unsigned char> output_image(input_image.size());

  std::cout << "Input Image (RGBA):\n";
  print_buffer(std::span{input_image});

  psm::Convert<psm::sRGB, psm::oRGB>(input_image, output_image);
  psm::Convert<psm::oRGB, psm::sRGB>(output_image, output_image);

  std::cout << "Output Image (BGR):\n";
  print_buffer(output_image);

  return 0;
}
