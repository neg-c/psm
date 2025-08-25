#include "image_io.hpp"

#include <png.h>
#include <turbojpeg.h>

#include <cstdio>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <type_traits>

namespace psm_cli {

namespace {
constexpr int kDefaultJpegQuality = 95;
constexpr int kTargetChannels = 3;  // RGB

class FileHandle {
 public:
  explicit FileHandle(const std::string& filepath, const char* mode)
      : file_(std::fopen(filepath.c_str(), mode)) {
    if (!file_) {
      throw std::runtime_error(std::format("Cannot open file: {}", filepath));
    }
  }

  ~FileHandle() {
    if (file_) {
      std::fclose(file_);
    }
  }

  FileHandle(const FileHandle&) = delete;
  FileHandle& operator=(const FileHandle&) = delete;

  FILE* get() const { return file_; }

 private:
  FILE* file_;
};

class TurboJpegHandle {
 public:
  explicit TurboJpegHandle(bool decompress = true)
      : handle_(decompress ? tjInitDecompress() : tjInitCompress()) {
    if (!handle_) {
      throw std::runtime_error("Failed to initialize TurboJPEG");
    }
  }

  ~TurboJpegHandle() {
    if (handle_) {
      tjDestroy(handle_);
    }
  }

  TurboJpegHandle(const TurboJpegHandle&) = delete;
  TurboJpegHandle& operator=(const TurboJpegHandle&) = delete;

  tjhandle get() const { return handle_; }

 private:
  tjhandle handle_;
};

}  // anonymous namespace

ImageFormat detect_format(const std::string& filepath) {
  const auto ext = std::filesystem::path(filepath).extension().string();

  if (ext == ".png" || ext == ".PNG") {
    return ImageFormat::PNG;
  } else if (ext == ".jpg" || ext == ".jpeg" || ext == ".JPG" ||
             ext == ".JPEG") {
    return ImageFormat::JPEG;
  }

  return ImageFormat::UNKNOWN;
}

ImageData<uint8_t> load_png_8bit(const std::string& filepath) {
  FileHandle file(filepath, "rb");

  // Check PNG signature
  png_byte header[8];
  if (std::fread(header, 1, 8, file.get()) != 8 || png_sig_cmp(header, 0, 8)) {
    throw std::runtime_error(std::format("Invalid PNG file: {}", filepath));
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) {
    throw std::runtime_error("Failed to create PNG read struct");
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    throw std::runtime_error("Failed to create PNG info struct");
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    throw std::runtime_error("Error reading PNG file");
  }

  png_init_io(png_ptr, file.get());
  png_set_sig_bytes(png_ptr, 8);

  png_read_info(png_ptr, info_ptr);

  const int width = png_get_image_width(png_ptr, info_ptr);
  const int height = png_get_image_height(png_ptr, info_ptr);
  const int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  const int color_type = png_get_color_type(png_ptr, info_ptr);

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png_ptr);
  }
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
    png_set_expand_gray_1_2_4_to_8(png_ptr);
  }
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png_ptr);
  }
  if (color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(png_ptr);
  }
  if (color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_strip_alpha(png_ptr);
  }
  if (bit_depth == 16) {
    png_set_strip_16(png_ptr);
  }

  png_read_update_info(png_ptr, info_ptr);

  std::vector<uint8_t> image_data(height * width * kTargetChannels);
  std::vector<png_bytep> row_pointers(height);

  for (int y = 0; y < height; ++y) {
    row_pointers[y] = image_data.data() + y * width * kTargetChannels;
  }

  png_read_image(png_ptr, row_pointers.data());
  png_read_end(png_ptr, nullptr);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

  return ImageData<uint8_t>(std::move(image_data), width, height,
                            kTargetChannels);
}

ImageData<uint16_t> load_png_16bit(const std::string& filepath) {
  FileHandle file(filepath, "rb");

  png_byte header[8];
  if (std::fread(header, 1, 8, file.get()) != 8 || png_sig_cmp(header, 0, 8)) {
    throw std::runtime_error(std::format("Invalid PNG file: {}", filepath));
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) {
    throw std::runtime_error("Failed to create PNG read struct");
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    throw std::runtime_error("Failed to create PNG info struct");
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    throw std::runtime_error("Error reading PNG file");
  }

  png_init_io(png_ptr, file.get());
  png_set_sig_bytes(png_ptr, 8);

  png_read_info(png_ptr, info_ptr);

  const int width = png_get_image_width(png_ptr, info_ptr);
  const int height = png_get_image_height(png_ptr, info_ptr);
  const int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  const int color_type = png_get_color_type(png_ptr, info_ptr);

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png_ptr);
  }
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
    png_set_expand_gray_1_2_4_to_8(png_ptr);
  }
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png_ptr);
  }
  if (color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(png_ptr);
  }
  if (color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_strip_alpha(png_ptr);
  }
  // Only process truly 16-bit images - don't expand 8-bit
  if (bit_depth != 16) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    throw std::runtime_error("Not a 16-bit PNG image");
  }

  // Handle byte order (PNG is big-endian, we want native)
  if (png_get_bit_depth(png_ptr, info_ptr) == 16) {
    png_set_swap(png_ptr);
  }

  png_read_update_info(png_ptr, info_ptr);

  std::vector<uint16_t> image_data(height * width * kTargetChannels);
  std::vector<png_bytep> row_pointers(height);

  for (int y = 0; y < height; ++y) {
    row_pointers[y] = reinterpret_cast<png_bytep>(image_data.data() +
                                                  y * width * kTargetChannels);
  }

  png_read_image(png_ptr, row_pointers.data());
  png_read_end(png_ptr, nullptr);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

  return ImageData<uint16_t>(std::move(image_data), width, height,
                             kTargetChannels);
}

ImageData<uint8_t> load_jpeg(const std::string& filepath) {
  // Read file into memory
  std::ifstream file(filepath, std::ios::binary | std::ios::ate);
  if (!file) {
    throw std::runtime_error(
        std::format("Cannot open JPEG file: {}", filepath));
  }

  const auto file_size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> file_data(file_size);
  if (!file.read(reinterpret_cast<char*>(file_data.data()), file_size)) {
    throw std::runtime_error(
        std::format("Cannot read JPEG file: {}", filepath));
  }

  // Initialize TurboJPEG
  TurboJpegHandle tj_handle(true);

  int width, height, subsamp, colorspace;
  if (tjDecompressHeader3(tj_handle.get(), file_data.data(), file_data.size(),
                          &width, &height, &subsamp, &colorspace) != 0) {
    throw std::runtime_error(
        std::format("Cannot decode JPEG header: {}", tjGetErrorStr()));
  }

  // Allocate output buffer
  std::vector<uint8_t> image_data(width * height * kTargetChannels);

  // Decompress
  if (tjDecompress2(tj_handle.get(), file_data.data(), file_data.size(),
                    image_data.data(), width, 0, height, TJPF_RGB, 0) != 0) {
    throw std::runtime_error(
        std::format("Cannot decompress JPEG: {}", tjGetErrorStr()));
  }

  return ImageData<uint8_t>(std::move(image_data), width, height,
                            kTargetChannels);
}

// Helper function to detect PNG bit depth
int detect_png_bit_depth(const std::string& filepath) {
  FileHandle file(filepath, "rb");

  // Check PNG signature
  png_byte header[8];
  if (std::fread(header, 1, 8, file.get()) != 8 || png_sig_cmp(header, 0, 8)) {
    throw std::runtime_error("Invalid PNG file");
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) throw std::runtime_error("Failed to create PNG read struct");

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    throw std::runtime_error("Failed to create PNG info struct");
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    throw std::runtime_error("Error reading PNG header");
  }

  png_init_io(png_ptr, file.get());
  png_set_sig_bytes(png_ptr, 8);
  png_read_info(png_ptr, info_ptr);

  const int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return bit_depth;
}

ImageVariant load_image(const std::string& filepath) {
  const auto format = detect_format(filepath);

  std::cout << std::format("Loading {} image: {}\n",
                           format == ImageFormat::PNG ? "PNG" : "JPEG",
                           filepath);

  switch (format) {
    case ImageFormat::PNG: {
      // Detect actual bit depth and load accordingly
      const int bit_depth = detect_png_bit_depth(filepath);
      std::cout << std::format("Detected PNG bit depth: {}\n", bit_depth);

      if (bit_depth == 16) {
        return load_png_16bit(filepath);
      } else {
        return load_png_8bit(filepath);
      }
    }
    case ImageFormat::JPEG:
      return load_jpeg(filepath);
    default:
      throw std::runtime_error(
          std::format("Unsupported image format: {}", filepath));
  }
}

bool save_png_8bit(const ImageData<uint8_t>& image,
                   const std::string& filepath) {
  try {
    std::cout << std::format("Saving 8-bit PNG: {}x{} channels={} size={}\n",
                             image.width(), image.height(), image.channels(),
                             image.size());

    FileHandle file(filepath, "wb");

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                  nullptr, nullptr, nullptr);
    if (!png_ptr) return false;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
      png_destroy_write_struct(&png_ptr, nullptr);
      return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
      png_destroy_write_struct(&png_ptr, &info_ptr);
      return false;
    }

    png_init_io(png_ptr, file.get());

    png_set_IHDR(png_ptr, info_ptr, image.width(), image.height(), 8,
                 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    // Write image data row by row to ensure correct stride
    const int row_stride = image.width() * kTargetChannels;
    const uint8_t* src_data = image.data();

    for (int y = 0; y < image.height(); ++y) {
      png_write_row(png_ptr, const_cast<png_bytep>(src_data + y * row_stride));
    }
    png_write_end(png_ptr, nullptr);

    png_destroy_write_struct(&png_ptr, &info_ptr);

    std::cout << std::format("Saved 8-bit PNG: {}\n", filepath);
    return true;

  } catch (const std::exception& e) {
    std::cerr << std::format("Failed to save PNG: {}\n", e.what());
    return false;
  }
}

bool save_png_16bit(const ImageData<uint16_t>& image,
                    const std::string& filepath) {
  try {
    std::cout << std::format("Saving 16-bit PNG: {}x{} channels={} size={}\n",
                             image.width(), image.height(), image.channels(),
                             image.size());

    FileHandle file(filepath, "wb");

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                  nullptr, nullptr, nullptr);
    if (!png_ptr) return false;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
      png_destroy_write_struct(&png_ptr, nullptr);
      return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
      png_destroy_write_struct(&png_ptr, &info_ptr);
      return false;
    }

    png_init_io(png_ptr, file.get());

    png_set_IHDR(png_ptr, info_ptr, image.width(), image.height(), 16,
                 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    // For 16-bit PNG on little-endian systems, we need to swap bytes
    png_set_swap(png_ptr);

    // Write image data row by row
    const int row_stride = image.width() * kTargetChannels;
    const uint16_t* src_data = image.data();

    for (int y = 0; y < image.height(); ++y) {
      png_write_row(png_ptr, reinterpret_cast<png_const_bytep>(src_data +
                                                               y * row_stride));
    }
    png_write_end(png_ptr, nullptr);

    png_destroy_write_struct(&png_ptr, &info_ptr);

    std::cout << std::format("Saved 16-bit PNG: {}\n", filepath);
    return true;

  } catch (const std::exception& e) {
    std::cerr << std::format("Failed to save 16-bit PNG: {}\n", e.what());
    return false;
  }
}

bool save_jpeg(const ImageData<uint8_t>& image, const std::string& filepath,
               int quality) {
  try {
    TurboJpegHandle tj_handle(false);

    unsigned char* jpeg_buf = nullptr;
    unsigned long jpeg_size = 0;

    if (tjCompress2(tj_handle.get(), image.data(), image.width(), 0,
                    image.height(), TJPF_RGB, &jpeg_buf, &jpeg_size, TJSAMP_444,
                    quality, 0) != 0) {
      throw std::runtime_error(
          std::format("JPEG compression failed: {}", tjGetErrorStr()));
    }

    std::ofstream file(filepath, std::ios::binary);
    if (!file.write(reinterpret_cast<const char*>(jpeg_buf), jpeg_size)) {
      tjFree(jpeg_buf);
      throw std::runtime_error("Failed to write JPEG file");
    }

    tjFree(jpeg_buf);

    std::cout << std::format("Saved JPEG: {}\n", filepath);
    return true;

  } catch (const std::exception& e) {
    std::cerr << std::format("Failed to save JPEG: {}\n", e.what());
    return false;
  }
}

template <typename DataType>
bool save_image(const ImageData<DataType>& image_data,
                const std::string& output_path) {
  auto path = std::filesystem::path(output_path);
  const auto format = detect_format(output_path);

  if constexpr (std::is_same_v<DataType, uint8_t>) {
    if (format == ImageFormat::JPEG || path.extension().empty()) {
      if (path.extension().empty()) {
        path.replace_extension(".jpg");
      }
      return save_jpeg(image_data, path.string(), kDefaultJpegQuality);
    } else {
      if (path.extension().empty()) {
        path.replace_extension(".png");
      }
      return save_png_8bit(image_data, path.string());
    }
  } else {
    // 16-bit images: handle format conversion
    if (format == ImageFormat::JPEG || path.extension().empty()) {
      // Convert 16-bit to 8-bit for JPEG output
      std::cout << std::format(
          "--️WARNING: JPEG format does not support 16-bit images.\n");
      std::cout << std::format(
          "Converting 16-bit to 8-bit for JPEG output: {}\n",
          path.extension().empty() ? path.replace_extension(".jpg").string()
                                   : path.string());
      std::cout << std::format(
          "--Use PNG format to preserve full 16-bit precision.\n");

      std::vector<uint8_t> converted_data(image_data.size());
      const uint16_t* src = image_data.data();

      // Convert 16-bit to 8-bit by dividing by 257 (65535/255 ≈ 257)
      for (size_t i = 0; i < image_data.size(); ++i) {
        converted_data[i] = static_cast<uint8_t>(src[i] / 257);
      }

      ImageData<uint8_t> converted_image(
          std::move(converted_data), image_data.width(), image_data.height(),
          image_data.channels());

      return save_jpeg(converted_image, path.string(), kDefaultJpegQuality);
    } else {
      // Save as 16-bit PNG
      if (path.extension().empty()) {
        path.replace_extension(".png");
      }
      return save_png_16bit(image_data, path.string());
    }
  }
}

// Explicit template instantiations
template bool save_image<uint8_t>(const ImageData<uint8_t>&,
                                  const std::string&);
template bool save_image<uint16_t>(const ImageData<uint16_t>&,
                                   const std::string&);

}  // namespace psm_cli
