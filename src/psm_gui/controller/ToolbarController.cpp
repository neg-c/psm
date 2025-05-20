#include "ToolbarController.hpp"

#include <nfd.h>

#include <iostream>

namespace psm_gui::controller {

ToolbarController::ToolbarController(AppState &state) : state_(state) {}

void ToolbarController::loadImage() {
  nfdu8char_t *path = nullptr;
  nfdu8filteritem_t filters[1] = {{"Image files", "png,jpeg,jpg,webp"}};
  nfdopendialognargs_t args{};
  args.filterList = filters;
  args.filterCount = 1;

  nfdresult_t result = NFD_OpenDialogU8_With(&path, &args);
  if (result == NFD_OKAY) {
    state_.io.load_path_ = std::string(path);
    NFD_FreePathU8(path);
  }
}

void ToolbarController::saveImage() {
  nfdu8char_t *path = nullptr;
  nfdu8filteritem_t filters[1] = {{"Image files", "png,jpeg,jpg,webp"}};
  nfdsavedialogu8args_t args{};
  args.filterList = filters;
  args.defaultName = "converted_image.png";
  args.filterCount = 1;
  nfdresult_t result = NFD_SaveDialogU8_With(&path, &args);
}

}  // namespace psm_gui::controller
