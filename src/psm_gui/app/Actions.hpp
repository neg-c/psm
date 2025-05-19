#pragma once

#include <functional>

namespace psm_gui {

struct Actions {
  std::function<void()> onLoad;
  std::function<void()> onSave;
};

}  // namespace psm_gui
