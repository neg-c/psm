#pragma once

#include <string>
#include <utility>
#include <vector>

namespace psm_gui {

// Define conversion options as an enum
enum class ConversionType { ADOBE_RGB, DISPLAY_P3, PRO_PHOTO_RGB, ORGB };

// Conversion registry to manage available conversions
class ConversionRegistry {
 public:
  ConversionRegistry() {
    // Initialize the available conversions
    conversion_options = {{ConversionType::ADOBE_RGB, "Adobe RGB"},
                          {ConversionType::DISPLAY_P3, "Display P3"},
                          {ConversionType::PRO_PHOTO_RGB, "ProPhoto RGB"},
                          {ConversionType::ORGB, "oRGB"}};
  }

  // Get all available conversion options
  const std::vector<std::pair<ConversionType, std::string>>& getOptions()
      const {
    return conversion_options;
  }

  // Get conversion name by type
  const std::string& getConversionName(ConversionType type) const {
    for (const auto& option : conversion_options) {
      if (option.first == type) {
        return option.second;
      }
    }
    static const std::string empty;
    return empty;
  }

  // Get conversion name by index
  const std::string& getConversionName(int index) const {
    if (index >= 0 && index < conversion_options.size()) {
      return conversion_options[index].second;
    }
    static const std::string empty;
    return empty;
  }

  // Get conversion type by index
  ConversionType getConversionType(int index) const {
    if (index >= 0 && index < conversion_options.size()) {
      return conversion_options[index].first;
    }
    return ConversionType::ADOBE_RGB;  // Default
  }

 private:
  std::vector<std::pair<ConversionType, std::string>> conversion_options;
};

}  // namespace psm_gui
