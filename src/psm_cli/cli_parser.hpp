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

// Parse command line arguments
CLIOptions parse_args(int argc, char* argv[]);

// Print usage information
void print_usage(const char* program_name);

// Parse adjust argument string into percentages
psm::Percent parse_adjust_arg(const std::string& adjust_arg);