// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/Library.h"
#include "CesiumGltf/PropertyAttribute.h"
#include "CesiumGltf/PropertyTable.h"
#include "CesiumGltf/PropertyTexture.h"
#include "CesiumGltf/Schema.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <optional>
#include <string>
#include <vector>

namespace CesiumGltf {
/**
 * @brief glTF extension that provides structural metadata about vertices,
 * texels, and features in a glTF asset.
 */
struct CESIUMGLTF_API ExtensionModelExtStructuralMetadata final
    : public CesiumUtility::ExtensibleObject {
  static constexpr const char* TypeName = "ExtensionModelExtStructuralMetadata";
  static constexpr const char* ExtensionName = "EXT_structural_metadata";

  /**
   * @brief An object defining classes and enums.
   */
  std::optional<CesiumGltf::Schema> schema;

  /**
   * @brief The URI (or IRI) of the external schema file.
   */
  std::optional<std::string> schemaUri;

  /**
   * @brief An array of property table definitions, which may be referenced by
   * index.
   */
  std::vector<CesiumGltf::PropertyTable> propertyTables;

  /**
   * @brief An array of property texture definitions, which may be referenced by
   * index.
   */
  std::vector<CesiumGltf::PropertyTexture> propertyTextures;

  /**
   * @brief An array of property attribute definitions, which may be referenced
   * by index.
   */
  std::vector<CesiumGltf::PropertyAttribute> propertyAttributes;
};
} // namespace CesiumGltf
