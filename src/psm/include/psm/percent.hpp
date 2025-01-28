#pragma once

#include <array>

namespace psm {

class Percent {
 public:
  constexpr Percent(int c0, int c1, int c2) : channels_{c0, c1, c2} {}

  static constexpr Percent neutral() { return {0, 0, 0}; }
  static constexpr Percent uniform(int value) { return {value, value, value}; }

  constexpr int channel(std::size_t idx) const { return channels_.at(idx); }

  constexpr Percent& operator+=(int value) {
    for (auto& channel : channels_) {
      channel += value;
    }
    return *this;
  }

  constexpr Percent& operator-=(int value) {
    for (auto& channel : channels_) {
      channel -= value;
    }
    return *this;
  }

  constexpr Percent operator+(int value) const {
    return {channels_[0] + value, channels_[1] + value, channels_[2] + value};
  }

  constexpr Percent operator-(int value) const {
    return {channels_[0] - value, channels_[1] - value, channels_[2] - value};
  }

  constexpr bool isNeutral() const {
    return channels_[0] == 0 && channels_[1] == 0 && channels_[2] == 0;
  }

  constexpr bool isUniform() const {
    return channels_[0] == channels_[1] && channels_[1] == channels_[2];
  }

 private:
  std::array<int, 3> channels_;
};

}  // namespace psm
