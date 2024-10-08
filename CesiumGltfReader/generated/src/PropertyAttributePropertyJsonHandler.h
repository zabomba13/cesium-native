// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include <CesiumGltf/PropertyAttributeProperty.h>
#include <CesiumJsonReader/ExtensibleObjectJsonHandler.h>
#include <CesiumJsonReader/JsonObjectJsonHandler.h>
#include <CesiumJsonReader/StringJsonHandler.h>

namespace CesiumJsonReader {
  class JsonReaderOptions;
}

namespace CesiumGltfReader {
  class PropertyAttributePropertyJsonHandler : public CesiumJsonReader::ExtensibleObjectJsonHandler {
  public:
    using ValueType = CesiumGltf::PropertyAttributeProperty;

    PropertyAttributePropertyJsonHandler(const CesiumJsonReader::JsonReaderOptions& options) noexcept;
    void reset(IJsonHandler* pParentHandler, CesiumGltf::PropertyAttributeProperty* pObject);

    virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

  protected:
    IJsonHandler* readObjectKeyPropertyAttributeProperty(const std::string& objectType, const std::string_view& str, CesiumGltf::PropertyAttributeProperty& o);

  private:

    CesiumGltf::PropertyAttributeProperty* _pObject = nullptr;
    CesiumJsonReader::StringJsonHandler _attribute;
    CesiumJsonReader::JsonObjectJsonHandler _offset;
    CesiumJsonReader::JsonObjectJsonHandler _scale;
    CesiumJsonReader::JsonObjectJsonHandler _max;
    CesiumJsonReader::JsonObjectJsonHandler _min;
  };
}
