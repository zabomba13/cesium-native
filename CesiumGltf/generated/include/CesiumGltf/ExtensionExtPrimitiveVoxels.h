// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/Library.h"
#include "CesiumGltf/Padding.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <cstdint>
#include <optional>
#include <vector>

namespace CesiumGltf {
/**
 * @brief `EXT_primitive_voxels` extension for a primitive in a glTF model to
 * indicate voxel-based volumetric data
 */
struct CESIUMGLTF_API ExtensionExtPrimitiveVoxels final
    : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName = "ExtensionExtPrimitiveVoxels";
  static inline constexpr const char* ExtensionName = "EXT_primitive_voxels";

  /**
   * @brief Dimensions of the voxel grid. The values are interpreted with
   * respect to the shape of the grid indicated by `EXT_implicit_geometry`.
   */
  std::vector<int64_t> dimensions;

  /**
   * @brief The optional padding of the voxels.
   */
  std::optional<CesiumGltf::Padding> padding;
};
} // namespace CesiumGltf
