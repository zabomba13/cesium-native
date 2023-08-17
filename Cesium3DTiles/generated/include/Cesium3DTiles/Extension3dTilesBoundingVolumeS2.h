// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "Cesium3DTiles/Library.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <string>

namespace Cesium3DTiles {
/**
 * @brief 3D Tiles extension for S2 bounding volumes.
 */
struct CESIUM3DTILES_API Extension3dTilesBoundingVolumeS2 final
    : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName =
      "Extension3dTilesBoundingVolumeS2";
  static inline constexpr const char* ExtensionName =
      "3DTILES_bounding_volume_S2";

  /**
   * @brief A hexadecimal representation of the S2CellId. Tokens shall be
   * lower-case, shall not contain whitespace and shall have trailing zeros
   * stripped.
   */
  std::string token;

  /**
   * @brief The minimum height of the tile, specified in meters above (or below)
   * the WGS84 ellipsoid.
   */
  double minimumHeight = double();

  /**
   * @brief The maximum height of the tile, specified in meters above (or below)
   * the WGS84 ellipsoid.
   */
  double maximumHeight = double();

  /** @brief Indicates whether the {@link token} property is defined. */
  bool tokenDefined : 1;
  /** @brief Indicates whether the {@link minimumHeight} property is defined. */
  bool minimumHeightDefined : 1;
  /** @brief Indicates whether the {@link maximumHeight} property is defined. */
  bool maximumHeightDefined : 1;
};
} // namespace Cesium3DTiles
