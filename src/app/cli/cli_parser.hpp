#pragma once

#include <optional>
#include <string>

#include "psm/percent.hpp"

struct CLIOptions {
  std::string input_file;
  std::string output_file;
  std::string from_space = "sRGB";
  std::string to_space = "sRGB";
  std::optional<psm::Percent> adjust_values;
};

CLIOptions parse_args(int argc, char* argv[]);

void print_usage(const char* program_name);

psm::Percent parse_adjust_arg(const std::string& adjust_arg);