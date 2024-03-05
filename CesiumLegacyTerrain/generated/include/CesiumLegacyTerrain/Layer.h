// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumLegacyTerrain/AvailabilityRectangle.h"
#include "CesiumLegacyTerrain/Library.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace CesiumLegacyTerrain {
/**
 * @brief A legacy terrain layer.json.
 */
struct CESIUMLEGACYTERRAIN_API Layer final
    : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName = "Layer";

  /**
   * @brief The attribution (credit) string for the terrain.
   */
  std::string attribution = "";

  /**
   * @brief The tile availability information. The outer array is indexed by
   * tile level. The inner array is a list of rectangles of availability at that
   * level. Tiles themselves may also contain further availability information
   * for their subtree.
   */
  std::vector<std::vector<CesiumLegacyTerrain::AvailabilityRectangle>>
      available;

  /**
   * @brief The bounding box of the terrain, expressed as west, south, east,
   * north in degrees.
   */
  std::vector<double> bounds = {-180, -90, 180, 90};

  /**
   * @brief The description of this terrain tileset.
   */
  std::string description = "";

  /**
   * @brief The extensions available for this tileset.
   */
  std::vector<std::string> extensionsProperty;

  /**
   * @brief The format of the terrain tiles. Should be `"quantized-mesh-1.0"`.
   */
  std::string format = "quantized-mesh-1.0";

  /**
   * @brief The maximum level for which there are any available tiles.
   */
  int64_t maxzoom = int64_t();

  /**
   * @brief The minimum level for which there are any available tiles.
   */
  int64_t minzoom = 0;

  /**
   * @brief The levels at metadata is found in tiles. For example, if this value
   * is 10, then metadata is found at levels 0, 10, 20, etc.
   */
  std::optional<int64_t> metadataAvailability;

  /**
   * @brief The name of this terrain tileset.
   */
  std::string name = "Terrain";

  /**
   * @brief The map projection of this tileset. Valid values are `"EPSG:4326"`
   * and `"EPSG:3857"`.
   */
  std::string projection = "EPSG:4326";

  /**
   * @brief The tiling scheme. The only valid value is `"tms"`.
   */
  std::string scheme = "tms";

  /**
   * @brief The URL templates from which to obtain tiles.
   */
  std::vector<std::string> tiles;

  /**
   * @brief The version of this tileset.
   */
  std::string version = "1.0.0";
};
} // namespace CesiumLegacyTerrain
