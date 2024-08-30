// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/Library.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <cstdint>

namespace CesiumGltf {
/**
 * @brief glTF extension for indicating that some edges of a primitive's
 * triangles should be outlined.
 */
struct CESIUMGLTF_API ExtensionCesiumPrimitiveOutline final
    : public CesiumUtility::ExtensibleObject {
  static constexpr const char* TypeName = "ExtensionCesiumPrimitiveOutline";
  static constexpr const char* ExtensionName = "CESIUM_primitive_outline";

  /**
   * @brief The index of the accessor providing the list of highlighted lines at
   * the edge of this primitive's triangles.
   */
  int32_t indices = -1;
};
} // namespace CesiumGltf
