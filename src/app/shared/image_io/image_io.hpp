#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace psm_cli {

enum class ImageFormat { PNG, JPEG, UNKNOWN };

template <typename T>
class ImageData {
 public:
  ImageData(std::vector<T> data, int w, int h, int c)
      : data_(std::move(data)), width_(w), height_(h), channels_(c) {}

  ImageData() : width_(0), height_(0), channels_(0) {}

  // Non-copyable but movable
  ImageData(const ImageData&) = delete;
  ImageData& operator=(const ImageData&) = delete;

  ImageData(ImageData&&) = default;
  ImageData& operator=(ImageData&&) = default;

  T* data() { return data_.data(); }
  const T* data() const { return data_.data(); }
  int width() const { return width_; }
  int height() const { return height_; }
  int channels() const { return channels_; }

  explicit operator bool() const { return !data_.empty(); }
  size_t size() const { return data_.size(); }

 private:
  std::vector<T> data_;
  int width_ = 0;
  int height_ = 0;
  int channels_ = 0;
};

using ImageVariant = std::variant<ImageData<uint8_t>, ImageData<uint16_t>>;

ImageFormat detect_format(const std::string& filepath);

int detect_png_bit_depth(const std::string& filepath);

ImageVariant load_image(const std::string& filepath);

template <typename DataType>
bool save_image(const ImageData<DataType>& image_data,
                const std::string& output_path);

ImageData<uint8_t> load_png_8bit(const std::string& filepath);
ImageData<uint16_t> load_png_16bit(const std::string& filepath);
ImageData<uint8_t> load_jpeg(const std::string& filepath);

bool save_png_8bit(const ImageData<uint8_t>& image,
                   const std::string& filepath);
bool save_png_16bit(const ImageData<uint16_t>& image,
                    const std::string& filepath);
bool save_jpeg(const ImageData<uint8_t>& image, const std::string& filepath,
               int quality = 95);

}  // namespace psm_cli
