// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "Cesium3DTiles/Library.h"

#include <CesiumUtility/ExtensibleObject.h>
#include <CesiumUtility/JsonValue.h>

#include <string>
#include <unordered_map>

namespace Cesium3DTiles {
/**
 * @brief An object containing a reference to a class from a metadata schema,
 * and property values that conform to the properties of that class.
 */
struct CESIUM3DTILES_API MetadataEntity
    : public CesiumUtility::ExtensibleObject {
  static constexpr const char* TypeName = "MetadataEntity";

  /**
   * @brief The class that property values conform to. The value shall be a
   * class ID declared in the `classes` dictionary of the metadata schema.
   */
  std::string classProperty;

  /**
   * @brief A dictionary, where each key corresponds to a property ID in the
   * class' `properties` dictionary and each value contains the property values.
   * The type of the value shall match the property definition: For `BOOLEAN`
   * use `true` or `false`. For `STRING` use a JSON string. For numeric types
   * use a JSON number. For `ENUM` use a valid enum `name`, not an integer
   * value. For `ARRAY`, `VECN`, and `MATN` types use a JSON array containing
   * values matching the `componentType`. Required properties shall be included
   * in this dictionary.
   */
  std::unordered_map<std::string, CesiumUtility::JsonValue> properties;
};
} // namespace Cesium3DTiles
