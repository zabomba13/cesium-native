// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/Library.h"
#include "CesiumGltf/NamedObject.h"

#include <cstdint>
#include <vector>

namespace CesiumGltf {
/**
 * @brief The root nodes of a scene.
 */
struct CESIUMGLTF_API Scene final : public CesiumGltf::NamedObject {
  static constexpr const char* TypeName = "Scene";

  /**
   * @brief The indices of each root node.
   */
  std::vector<int32_t> nodes;
};
} // namespace CesiumGltf
