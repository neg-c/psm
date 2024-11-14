#include <format>
#include <iostream>
#include <vector>

import psm;

template <typename T>
void print_buffer(const std::span<T>& buffer) {
  for (size_t i = 0; i < buffer.size(); ++i) {
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

  std::span<const unsigned char> src_span(input_image.data(),
                                          input_image.size());
  std::span<unsigned char> result_span(output_image.data(),
                                       output_image.size());

  std::cout << "Input Image (RGBA):\n";
  print_buffer(src_span);

  psm::Color(src_span, result_span, psm::Format::ksRGB, psm::Format::koRGB);

  std::cout << "Output Image (oRGB):\n";
  print_buffer(result_span);

  return 0;
}
