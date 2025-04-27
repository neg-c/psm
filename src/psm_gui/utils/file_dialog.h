#pragma once

#include <string>

namespace psm_gui {

/**
 * @brief Opens a file dialog to select an image file
 *
 * @return std::string Path to the selected file, or empty string if canceled
 */
std::string OpenImageFileDialog();

}  // namespace psm_gui
