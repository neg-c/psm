#include <cstddef>
#include <format>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <ranges>

#include "psm/psm.hpp"

template <std::ranges::contiguous_range Buffer>
void print_buffer(const Buffer& buffer) {
  for (size_t i = 0; i < std::ranges::size(buffer); ++i) {
    std::cout << std::format("[{}] ", buffer[i]);
  }
  std::cout << "\n";
}

static int window(const std::basic_string<char>& wndName, const cv::Mat& roi,
                  bool freeze) {
  cv::namedWindow(wndName, cv::WINDOW_NORMAL);
  cv::setWindowProperty(wndName, cv::WND_PROP_FULLSCREEN, cv::WND_PROP_TOPMOST);
  cv::imshow(wndName, roi);
  if (freeze) {
    while (true) {
      int c = cv::waitKey();
      if (c == 27) {
        break;
      }
    }
  }
  return 0;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <image_path>\n";
    return 1;
  }

  cv::Mat image = cv::imread(argv[1], cv::IMREAD_UNCHANGED);
  cv::Mat out_image(image.rows, image.cols, CV_8UC3);

  if (image.empty()) {
    std::cerr << "Error: Could not load image " << argv[1] << "\n";
    return 1;
  }

  // Use image directly without conversion
  if (image.channels() != 3 && image.channels() != 4) {
    std::cerr << "Error: Image must be RGB or RGBA\n";
    return 1;
  }

  std::span<unsigned char> in_span(
      image.ptr(), image.ptr() + image.total() * image.channels());
  std::span<unsigned char> out_span(
      out_image.ptr(),
      out_image.ptr() + out_image.total() * out_image.channels());

  psm::Convert<psm::sRGB, psm::oRGB>(in_span, out_span);
  psm::Convert<psm::oRGB, psm::sRGB>(out_span, out_span);
  window("kfjdalkf", image, false);
  window("kfjdalkdfsadff", out_image, true);

  return 0;
}
