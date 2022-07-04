// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "Cesium3DTiles/Extension3dTilesImplicitTilingSubtreesLegacy.h"
#include "Cesium3DTiles/Library.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <cstdint>

namespace Cesium3DTiles {
/**
 * @brief This extension allows a tile to be implicitly subdivided. Tile and
 * content availability is stored in subtrees which are referenced externally.
 */
struct CESIUM3DTILES_API Extension3dTilesImplicitTilingLegacy final
    : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName =
      "Extension3dTilesImplicitTilingLegacy";
  static inline constexpr const char* ExtensionName = "3DTILES_implicit_tiling";

  /**
   * @brief Known values for A string describing the subdivision scheme used
   * within the tileset.
   */
  struct SubdivisionScheme {
    inline static const std::string QUADTREE = "QUADTREE";

    inline static const std::string OCTREE = "OCTREE";
  };

  /**
   * @brief A string describing the subdivision scheme used within the tileset.
   *
   * Known values are defined in {@link SubdivisionScheme}.
   *
   */
  std::string subdivisionScheme = SubdivisionScheme::QUADTREE;

  /**
   * @brief The number of distinct levels in each subtree. For example, a
   * quadtree with `subtreeLevels = 2` will have subtrees with 5 nodes (one root
   * and 4 children)
   */
  int64_t subtreeLevels = int64_t();

  /**
   * @brief The level of the deepest available tile. Levels are numbered from 0
   * starting at the tile with the `3DTILES_implicit_tiling` extension. This
   * tile's children are at level 1, the children's children are at level 2, and
   * so on.
   */
  int64_t maximumLevel = int64_t();

  /**
   * @brief An object describing the location of subtree files.
   */
  Cesium3DTiles::Extension3dTilesImplicitTilingSubtreesLegacy subtrees;
};
} // namespace Cesium3DTiles
