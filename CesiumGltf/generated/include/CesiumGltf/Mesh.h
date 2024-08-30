// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/Library.h"
#include "CesiumGltf/MeshPrimitive.h"
#include "CesiumGltf/NamedObject.h"

#include <vector>

namespace CesiumGltf {
/**
 * @brief A set of primitives to be rendered.  Its global transform is defined
 * by a node that references it.
 */
struct CESIUMGLTF_API Mesh final : public CesiumGltf::NamedObject {
  static constexpr const char* TypeName = "Mesh";

  /**
   * @brief An array of primitives, each defining geometry to be rendered.
   */
  std::vector<CesiumGltf::MeshPrimitive> primitives;

  /**
   * @brief Array of weights to be applied to the morph targets. The number of
   * array elements **MUST** match the number of morph targets.
   */
  std::vector<double> weights;
};
} // namespace CesiumGltf
