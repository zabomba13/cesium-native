// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "ExtensionMeshPrimitiveKhrMaterialsVariantsMappingsValueJsonHandler.h"

#include <CesiumGltf/ExtensionMeshPrimitiveKhrMaterialsVariants.h>
#include <CesiumJsonReader/ArrayJsonHandler.h>
#include <CesiumJsonReader/ExtensibleObjectJsonHandler.h>

namespace CesiumJsonReader {
class JsonReaderOptions;
}

namespace CesiumGltfReader {
class ExtensionMeshPrimitiveKhrMaterialsVariantsJsonHandler
    : public CesiumJsonReader::ExtensibleObjectJsonHandler,
      public CesiumJsonReader::IExtensionJsonHandler {
public:
  using ValueType = CesiumGltf::ExtensionMeshPrimitiveKhrMaterialsVariants;

  static inline constexpr const char* ExtensionName = "KHR_materials_variants";

  ExtensionMeshPrimitiveKhrMaterialsVariantsJsonHandler(
      const CesiumJsonReader::JsonReaderOptions& options
  ) noexcept;
  void reset(
      IJsonHandler* pParentHandler,
      CesiumGltf::ExtensionMeshPrimitiveKhrMaterialsVariants* pObject
  );

  virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

  virtual void reset(
      IJsonHandler* pParentHandler,
      CesiumUtility::ExtensibleObject& o,
      const std::string_view& extensionName
  ) override;

  virtual IJsonHandler& getHandler() override { return *this; }

protected:
  IJsonHandler* readObjectKeyExtensionMeshPrimitiveKhrMaterialsVariants(
      const std::string& objectType,
      const std::string_view& str,
      CesiumGltf::ExtensionMeshPrimitiveKhrMaterialsVariants& o
  );

private:
  CesiumGltf::ExtensionMeshPrimitiveKhrMaterialsVariants* _pObject = nullptr;
  CesiumJsonReader::ArrayJsonHandler<
      CesiumGltf::ExtensionMeshPrimitiveKhrMaterialsVariantsMappingsValue,
      ExtensionMeshPrimitiveKhrMaterialsVariantsMappingsValueJsonHandler>
      _mappings;
};
} // namespace CesiumGltfReader
