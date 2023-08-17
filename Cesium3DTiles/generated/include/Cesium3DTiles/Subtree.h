// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "Cesium3DTiles/Availability.h"
#include "Cesium3DTiles/Buffer.h"
#include "Cesium3DTiles/BufferView.h"
#include "Cesium3DTiles/Library.h"
#include "Cesium3DTiles/MetadataEntity.h"
#include "Cesium3DTiles/PropertyTable.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <cstdint>
#include <vector>

namespace Cesium3DTiles {
/**
 * @brief An object describing the availability of tiles and content in a
 * subtree, as well as availability of children subtrees. May also store
 * metadata for available tiles and content.
 */
struct CESIUM3DTILES_API Subtree final
    : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName = "Subtree";

  /**
   * @brief An array of buffers.
   */
  std::vector<Cesium3DTiles::Buffer> buffers;

  /**
   * @brief An array of buffer views.
   */
  std::vector<Cesium3DTiles::BufferView> bufferViews;

  /**
   * @brief An array of property tables.
   */
  std::vector<Cesium3DTiles::PropertyTable> propertyTables;

  /**
   * @brief The availability of tiles in the subtree. The availability bitstream
   * is a 1D boolean array where tiles are ordered by their level in the subtree
   * and Morton index within that level. A tile's availability is determined by
   * a single bit, 1 meaning a tile exists at that spatial index, and 0 meaning
   * it does not. The number of elements in the array is `(N^subtreeLevels -
   * 1)/(N - 1)` where N is 4 for subdivision scheme `QUADTREE` and 8 for
   * `OCTREE`. Availability may be stored in a buffer view or as a constant
   * value that applies to all tiles. If a non-root tile's availability is 1 its
   * parent tile's availability shall also be 1. `tileAvailability.constant: 0`
   * is disallowed, as subtrees shall have at least one tile.
   */
  Cesium3DTiles::Availability tileAvailability;

  /**
   * @brief An array of content availability objects. If the tile has a single
   * content this array will have one element; if the tile has multiple contents
   * - as supported by 3DTILES_multiple_contents and 3D Tiles 1.1 - this array
   * will have multiple elements.
   */
  std::vector<Cesium3DTiles::Availability> contentAvailability;

  /**
   * @brief The availability of children subtrees. The availability bitstream is
   * a 1D boolean array where subtrees are ordered by their Morton index in the
   * level of the tree immediately below the bottom row of the subtree. A child
   * subtree's availability is determined by a single bit, 1 meaning a subtree
   * exists at that spatial index, and 0 meaning it does not. The number of
   * elements in the array is `N^subtreeLevels` where N is 4 for subdivision
   * scheme `QUADTREE` and 8 for `OCTREE`. Availability may be stored in a
   * buffer view or as a constant value that applies to all child subtrees. If
   * availability is 0 for all child subtrees, then the tileset does not
   * subdivide further.
   */
  Cesium3DTiles::Availability childSubtreeAvailability;

  /**
   * @brief Index of the property table containing tile metadata. Tile metadata
   * only exists for available tiles and is tightly packed by increasing tile
   * index. To access individual tile metadata, implementations may create a
   * mapping from tile indices to tile metadata indices.
   */
  int64_t tileMetadata = int64_t();

  /**
   * @brief An array of indexes to property tables containing content metadata.
   * If the tile has a single content this array will have one element; if the
   * tile has multiple contents - as supported by 3DTILES_multiple_contents and
   * 3D Tiles 1.1 - this array will have multiple elements. Content metadata
   * only exists for available contents and is tightly packed by increasing tile
   * index. To access individual content metadata, implementations may create a
   * mapping from tile indices to content metadata indices.
   */
  std::vector<int64_t> contentMetadata;

  /**
   * @brief Subtree metadata encoded in JSON.
   */
  Cesium3DTiles::MetadataEntity subtreeMetadata;

  /** @brief Indicates whether the {@link buffers} property is defined. */
  bool buffersDefined : 1;
  /** @brief Indicates whether the {@link bufferViews} property is defined. */
  bool bufferViewsDefined : 1;
  /** @brief Indicates whether the {@link propertyTables} property is defined. */
  bool propertyTablesDefined : 1;
  /** @brief Indicates whether the {@link tileAvailability} property is defined. */
  bool tileAvailabilityDefined : 1;
  /** @brief Indicates whether the {@link contentAvailability} property is defined. */
  bool contentAvailabilityDefined : 1;
  /** @brief Indicates whether the {@link childSubtreeAvailability} property is defined. */
  bool childSubtreeAvailabilityDefined : 1;
  /** @brief Indicates whether the {@link tileMetadata} property is defined. */
  bool tileMetadataDefined : 1;
  /** @brief Indicates whether the {@link contentMetadata} property is defined. */
  bool contentMetadataDefined : 1;
  /** @brief Indicates whether the {@link subtreeMetadata} property is defined. */
  bool subtreeMetadataDefined : 1;
};
} // namespace Cesium3DTiles
