// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include <CesiumGltf/ExtensionMeshPrimitiveExtStructuralMetadata.h>
#include <CesiumJsonReader/ArrayJsonHandler.h>
#include <CesiumJsonReader/ExtensibleObjectJsonHandler.h>
#include <CesiumJsonReader/IntegerJsonHandler.h>

namespace CesiumJsonReader {
class JsonReaderOptions;
} // namespace CesiumJsonReader

namespace CesiumGltfReader {
class ExtensionMeshPrimitiveExtStructuralMetadataJsonHandler
    : public CesiumJsonReader::ExtensibleObjectJsonHandler,
      public CesiumJsonReader::IExtensionJsonHandler {
public:
  using ValueType = CesiumGltf::ExtensionMeshPrimitiveExtStructuralMetadata;

  static constexpr const char* ExtensionName = "EXT_structural_metadata";

  explicit ExtensionMeshPrimitiveExtStructuralMetadataJsonHandler(
      const CesiumJsonReader::JsonReaderOptions& options) noexcept;
  void reset(
      IJsonHandler* pParentHandler,
      CesiumGltf::ExtensionMeshPrimitiveExtStructuralMetadata* pObject);

  IJsonHandler* readObjectKey(const std::string_view& str) override;

  void reset(
      IJsonHandler* pParentHandler,
      CesiumUtility::ExtensibleObject& o,
      const std::string_view& extensionName) override;

  IJsonHandler& getHandler() override { return *this; }

protected:
  IJsonHandler* readObjectKeyExtensionMeshPrimitiveExtStructuralMetadata(
      const std::string& objectType,
      const std::string_view& str,
      CesiumGltf::ExtensionMeshPrimitiveExtStructuralMetadata& o);

private:
  CesiumGltf::ExtensionMeshPrimitiveExtStructuralMetadata* _pObject = nullptr;
  CesiumJsonReader::
      ArrayJsonHandler<int32_t, CesiumJsonReader::IntegerJsonHandler<int32_t>>
          _propertyTextures;
  CesiumJsonReader::
      ArrayJsonHandler<int32_t, CesiumJsonReader::IntegerJsonHandler<int32_t>>
          _propertyAttributes;
};
} // namespace CesiumGltfReader
