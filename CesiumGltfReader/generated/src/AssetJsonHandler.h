// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include <CesiumGltf/Asset.h>
#include <CesiumJsonReader/ExtensibleObjectJsonHandler.h>
#include <CesiumJsonReader/StringJsonHandler.h>

namespace CesiumJsonReader {
  class JsonReaderOptions;
}

namespace CesiumGltfReader {
  class AssetJsonHandler : public CesiumJsonReader::ExtensibleObjectJsonHandler {
  public:
    using ValueType = CesiumGltf::Asset;

    AssetJsonHandler(const CesiumJsonReader::JsonReaderOptions& options) noexcept;
    void reset(IJsonHandler* pParentHandler, CesiumGltf::Asset* pObject);

    virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

  protected:
    IJsonHandler* readObjectKeyAsset(const std::string& objectType, const std::string_view& str, CesiumGltf::Asset& o);

  private:

    CesiumGltf::Asset* _pObject = nullptr;
    CesiumJsonReader::StringJsonHandler _copyright;
    CesiumJsonReader::StringJsonHandler _generator;
    CesiumJsonReader::StringJsonHandler _version;
    CesiumJsonReader::StringJsonHandler _minVersion;
  };
}
