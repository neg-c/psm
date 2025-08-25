#include "cli_parser.hpp"

#include <iostream>
#include <stdexcept>
#include <string_view>

void print_usage(const char* program_name) {
  std::cout
      << "Usage: " << program_name << " [options]\n"
      << "Options:\n"
      << "  -i, --input FILE       Input image file\n"
      << "  -o, --output FILE      Output image file\n"
      << "  -f, --from COLORSPACE  Source color space (sRGB, AdobeRGB, "
         "DisplayP3, oRGB, ProPhotoRGB)\n"
      << "  -t, --to COLORSPACE    Target color space (sRGB, AdobeRGB, "
         "DisplayP3, oRGB, ProPhotoRGB)\n"
      << "  -a, --adjust R,G,B     Adjust channels by percent (e.g., 10,5,-5)\n"
      << "  -h, --help             Show this help message\n";
}

psm::Percent parse_adjust_arg(const std::string& adjust_arg) {
  // Expected format: "R,G,B" where each is a percentage value
  size_t first_comma = adjust_arg.find(',');
  size_t second_comma = adjust_arg.find(',', first_comma + 1);

  if (first_comma == std::string::npos || second_comma == std::string::npos) {
    throw std::invalid_argument("Invalid adjust format. Expected: R,G,B");
  }

  int r_percent = std::stoi(adjust_arg.substr(0, first_comma));
  int g_percent = std::stoi(
      adjust_arg.substr(first_comma + 1, second_comma - first_comma - 1));
  int b_percent = std::stoi(adjust_arg.substr(second_comma + 1));

  // Create a Percent object with the three channel values
  return psm::Percent{r_percent, g_percent, b_percent};
}

CLIOptions parse_args(int argc, char* argv[]) {
  CLIOptions options;

  for (int i = 1; i < argc; ++i) {
    std::string_view arg = argv[i];

    if (arg == "-h" || arg == "--help") {
      print_usage(argv[0]);
      exit(0);
    } else if (arg == "-i" || arg == "--input") {
      if (++i < argc)
        options.input_file = argv[i];
      else
        throw std::runtime_error("Missing input file");
    } else if (arg == "-o" || arg == "--output") {
      if (++i < argc)
        options.output_file = argv[i];
      else
        throw std::runtime_error("Missing output file");
    } else if (arg == "-f" || arg == "--from") {
      if (++i < argc)
        options.from_space = argv[i];
      else
        throw std::runtime_error("Missing source color space");
    } else if (arg == "-t" || arg == "--to") {
      if (++i < argc)
        options.to_space = argv[i];
      else
        throw std::runtime_error("Missing target color space");
    } else if (arg == "-a" || arg == "--adjust") {
      if (++i < argc) {
        options.adjust_values = parse_adjust_arg(argv[i]);
      } else {
        throw std::runtime_error("Missing adjust values");
      }
    } else {
      throw std::runtime_error(std::string("Unknown option: ") +
                               std::string(arg));
    }
  }

  if (options.input_file.empty() || options.output_file.empty()) {
    throw std::runtime_error("Input and output files are required");
  }

  return options;
}