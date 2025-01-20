#pragma once
#include <concepts>
#include <span>

namespace psm::detail {

template <typename Tag>
concept ColorSpaceType = requires {
  typename Tag::type;  // Must have an implementation type
} && requires(const std::span<float>& src, std::span<float> dst) {
  // Implementation must have static toSRGB and fromSRGB methods
  { Tag::type::toSRGB(src, dst) } -> std::same_as<void>;
  { Tag::type::fromSRGB(src, dst) } -> std::same_as<void>;
};

template <typename Tag>
  requires ColorSpaceType<Tag>
using ColorSpaceImpl = typename Tag::type;

}  // namespace psm::detail
