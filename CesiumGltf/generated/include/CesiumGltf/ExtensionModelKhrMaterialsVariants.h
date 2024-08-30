// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include <CesiumGltf/ExtensionModelKhrMaterialsVariantsValue.h>
#include <CesiumGltf/Library.h>
#include <CesiumUtility/ExtensibleObject.h>

#include <vector>

namespace CesiumGltf {
/**
 * @brief glTF extension that defines a material variations for mesh primitives
 */
struct CESIUMGLTF_API ExtensionModelKhrMaterialsVariants final
    : public CesiumUtility::ExtensibleObject {
  static constexpr const char* TypeName = "ExtensionModelKhrMaterialsVariants";
  static constexpr const char* ExtensionName = "KHR_materials_variants";

  /**
   * @brief variants
   */
  std::vector<CesiumGltf::ExtensionModelKhrMaterialsVariantsValue> variants;
};
} // namespace CesiumGltf
