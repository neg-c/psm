#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace psm_cli {

template <typename T>
class ImageData {
 public:
  ImageData(T* ptr, int w, int h, int c)
      : data_(ptr), width_(w), height_(h), channels_(c) {}

  ~ImageData();

  // Non-copyable but movable
  ImageData(const ImageData&) = delete;
  ImageData& operator=(const ImageData&) = delete;

  ImageData(ImageData&& other) noexcept
      : data_(std::exchange(other.data_, nullptr)),
        width_(other.width_),
        height_(other.height_),
        channels_(other.channels_) {}

  ImageData& operator=(ImageData&& other) noexcept {
    if (this != &other) {
      if (data_) {
        stbi_image_free(data_);
      }
      data_ = std::exchange(other.data_, nullptr);
      width_ = other.width_;
      height_ = other.height_;
      channels_ = other.channels_;
    }
    return *this;
  }

  T* data() const { return data_; }
  int width() const { return width_; }
  int height() const { return height_; }
  int channels() const { return channels_; }

  explicit operator bool() const { return data_ != nullptr; }
  size_t size() const {
    return static_cast<size_t>(width_ * height_ * channels_);
  }

 private:
  T* data_ = nullptr;
  int width_ = 0;
  int height_ = 0;
  int channels_ = 0;
};

using ImageVariant = std::variant<ImageData<uint8_t>, ImageData<uint16_t>>;

// Load image with automatic bit depth detection
ImageVariant load_image(const std::string& filepath);

// Save image with appropriate format based on bit depth
template <typename DataType>
bool save_image(const std::vector<DataType>& image_data, int width, int height,
                const std::string& output_path);

}  // namespace psm_cli
