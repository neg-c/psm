#pragma once

#include <span>

#include "psm/percent.hpp"

namespace psm::detail {

template <typename T>
void adjustChannels(std::span<T> buffer, const Percent& adjust_percentage);

}  // namespace psm::detail
