// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include <Cesium3DTiles/Library.h>
#include <Cesium3DTiles/PropertyStatistics.h>
#include <CesiumUtility/ExtensibleObject.h>

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

namespace Cesium3DTiles {
/**
 * @brief Statistics about entities that conform to a class that was defined in
 * a metadata schema.
 */
struct CESIUM3DTILES_API ClassStatistics final
    : public CesiumUtility::ExtensibleObject {
  static constexpr const char* TypeName = "ClassStatistics";

  /**
   * @brief The number of entities that conform to the class.
   */
  std::optional<int64_t> count;

  /**
   * @brief A dictionary, where each key corresponds to a property ID in the
   * class' `properties` dictionary and each value is an object containing
   * statistics about property values.
   */
  std::unordered_map<std::string, Cesium3DTiles::PropertyStatistics> properties;
};
} // namespace Cesium3DTiles
