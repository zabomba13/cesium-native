// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "Cesium3DTiles/ClassStatistics.h"
#include "Cesium3DTiles/Library.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <unordered_map>

namespace Cesium3DTiles {
/**
 * @brief Statistics about entities.
 */
struct CESIUM3DTILES_API Statistics final
    : public CesiumUtility::ExtensibleObject {
  static constexpr const char* TypeName = "Statistics";

  /**
   * @brief A dictionary, where each key corresponds to a class ID in the
   * `classes` dictionary of the metatata schema that was defined for the
   * tileset that contains these statistics. Each value is an object containing
   * statistics about entities that conform to the class.
   */
  std::unordered_map<std::string, Cesium3DTiles::ClassStatistics> classes;
};
} // namespace Cesium3DTiles
