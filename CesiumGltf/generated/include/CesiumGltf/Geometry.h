// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/Box.h"
#include "CesiumGltf/Cylinder.h"
#include "CesiumGltf/Ellipsoid.h"
#include "CesiumGltf/Library.h"
#include "CesiumGltf/Region.h"
#include "CesiumGltf/Sphere.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <optional>

namespace CesiumGltf {
/**
 * @brief A definition of implicit 3D geometry.
 */
struct CESIUMGLTF_API Geometry final : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName = "Geometry";

  /**
   * @brief box
   */
  std::optional<CesiumGltf::Box> box;

  /**
   * @brief cylinder
   */
  std::optional<CesiumGltf::Cylinder> cylinder;

  /**
   * @brief sphere
   */
  std::optional<CesiumGltf::Sphere> sphere;

  /**
   * @brief ellipsoid
   */
  std::optional<CesiumGltf::Ellipsoid> ellipsoid;

  /**
   * @brief region
   */
  std::optional<CesiumGltf::Region> region;
};
} // namespace CesiumGltf
