#pragma once

namespace psm {

// Primary template declaration
template <typename Tag>
struct ColorSpace;

// Type-trait lookup helper
template <typename Tag>
using ColorSpace_t = typename ColorSpace<Tag>::Type;

}  // namespace psm