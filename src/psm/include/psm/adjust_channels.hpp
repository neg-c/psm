#pragma once

#include <ranges>
#include <span>

#include "percent.hpp"

namespace psm {

namespace detail {

template <typename T>
void adjustChannels(std::span<T> buffer, const Percent& adjust_percentage);

template <typename T>
void AdjustChannelsImpl(std::span<T> buffer, const Percent& adjust_percentage) {
  adjustChannels(buffer, adjust_percentage);
}

}  // namespace detail

template <std::ranges::contiguous_range Range>
void AdjustChannels(Range& buffer, const Percent& adjust_percentage) {
  detail::AdjustChannelsImpl(std::span{buffer}, adjust_percentage);
}

}  // namespace psm
