#include <format>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <variant>

#include "cli_parser.hpp"
#include "image_io.hpp"
#include "image_processor.hpp"

int main(int argc, char* argv[]) {
  const auto options = parse_args(argc, argv);
  std::cout << std::format("Loading image: {}\n", options.input_file);
  auto image_variant = psm_cli::load_image(options.input_file);
  const bool success = std::visit(
      [&options](auto& image_data) -> bool {
        using DataType = std::decay_t<decltype(*image_data.data())>;
        if (!image_data) {
          std::cerr << std::format("Failed to load image: {}\n",
                                   options.input_file);
          return false;
        }
        auto processed_data =
            psm_cli::process_image<DataType>(image_data, options);
        const bool save_success = psm_cli::save_image<DataType>(
            processed_data, image_data.width(), image_data.height(),
            options.output_file);
        if (!save_success) {
          std::cerr << std::format("Failed to save image: {}\n",
                                   options.output_file);
          return false;
        }
        std::cout << std::format("Successfully processed and saved image\n");
        return true;
      },
      image_variant);

  return success ? 0 : 1;
}
