// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "ExtensionNodeMaxarMeshVariantsMappingsValueJsonHandler.h"
#include <CesiumGltf/ExtensionNodeMaxarMeshVariants.h>
#include <CesiumJsonReader/ArrayJsonHandler.h>
#include <CesiumJsonReader/ExtensibleObjectJsonHandler.h>

namespace CesiumJsonReader {
  class JsonReaderOptions;
}

namespace CesiumGltfReader {
  class ExtensionNodeMaxarMeshVariantsJsonHandler : public CesiumJsonReader::ExtensibleObjectJsonHandler, public CesiumJsonReader::IExtensionJsonHandler {
  public:
    using ValueType = CesiumGltf::ExtensionNodeMaxarMeshVariants;

    static inline constexpr const char* ExtensionName = "MAXAR_mesh_variants";

    ExtensionNodeMaxarMeshVariantsJsonHandler(const CesiumJsonReader::JsonReaderOptions& options) noexcept;
    void reset(IJsonHandler* pParentHandler, CesiumGltf::ExtensionNodeMaxarMeshVariants* pObject);

    virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

    virtual void reset(IJsonHandler* pParentHandler, CesiumUtility::ExtensibleObject& o, const std::string_view& extensionName) override;

    virtual IJsonHandler& getHandler() override { return *this; }

  protected:
    IJsonHandler* readObjectKeyExtensionNodeMaxarMeshVariants(const std::string& objectType, const std::string_view& str, CesiumGltf::ExtensionNodeMaxarMeshVariants& o);

  private:

    CesiumGltf::ExtensionNodeMaxarMeshVariants* _pObject = nullptr;
    CesiumJsonReader::ArrayJsonHandler<CesiumGltf::ExtensionNodeMaxarMeshVariantsMappingsValue, ExtensionNodeMaxarMeshVariantsMappingsValueJsonHandler> _mappings;
  };
}
