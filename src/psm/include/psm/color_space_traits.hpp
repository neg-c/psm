#pragma once
#include <span>
#include <concepts>

namespace psm::detail {

// Primary template for tag mapping
template<typename Tag>
struct ColorSpaceFromTag;

// Helper alias
template<typename Tag>
using ColorSpaceFromTag_t = typename ColorSpaceFromTag<Tag>::type;

// Concept that defines requirements for color space types
template<typename T>
concept ColorSpaceType = requires(const std::span<float>& src, std::span<float> dst) {
    // Must have static toSRGB and fromSRGB methods
    { T::toSRGB(src, dst) } -> std::same_as<void>;
    { T::fromSRGB(src, dst) } -> std::same_as<void>;
    
    // Must have a tag_type
    typename T::tag_type;
};

}  // namespace psm::detail
