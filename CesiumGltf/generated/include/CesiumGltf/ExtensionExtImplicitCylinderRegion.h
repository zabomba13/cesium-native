// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/Library.h"

#include <CesiumUtility/ExtensibleObject.h>

namespace CesiumGltf {
/**
 * @brief Extension of `KHR_implicit_shapes.shape` to represent an implicit
 * cylinder region in a glTF model.
 */
struct CESIUMGLTF_API ExtensionExtImplicitCylinderRegion final
    : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName =
      "ExtensionExtImplicitCylinderRegion";
  static inline constexpr const char* ExtensionName =
      "EXT_implicit_cylinder_region";

  /**
   * @brief The inner radius of the cylinder region along the X and Z axes, in
   * meters.
   */
  double minRadius = double();

  /**
   * @brief The outer radius of the cylinder region along the X and Z axes, in
   * meters.
   */
  double maxRadius = double();

  /**
   * @brief The height of the cylinder in meters along the Y-axis, in meters.
   */
  double height = double();

  /**
   * @brief The minimum angle of the cylinder region in radians. In other words,
   * this is the angle where the cylinder region starts. Must be in the range
   * [-pi, pi].
   */
  double minAngle = -3.14159265359;

  /**
   * @brief The maximum angle of the cylinder region in radians. In other words,
   * this is the angle where the cylinder region ends. Must be in the range
   * [-pi, pi].
   */
  double maxAngle = 3.14159265359;
};
} // namespace CesiumGltf
