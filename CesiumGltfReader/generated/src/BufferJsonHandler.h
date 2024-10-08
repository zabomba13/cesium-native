// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "NamedObjectJsonHandler.h"
#include <CesiumGltf/Buffer.h>
#include <CesiumJsonReader/IntegerJsonHandler.h>
#include <CesiumJsonReader/StringJsonHandler.h>

namespace CesiumJsonReader {
  class JsonReaderOptions;
}

namespace CesiumGltfReader {
  class BufferJsonHandler : public CesiumGltfReader::NamedObjectJsonHandler {
  public:
    using ValueType = CesiumGltf::Buffer;

    BufferJsonHandler(const CesiumJsonReader::JsonReaderOptions& options) noexcept;
    void reset(IJsonHandler* pParentHandler, CesiumGltf::Buffer* pObject);

    virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

  protected:
    IJsonHandler* readObjectKeyBuffer(const std::string& objectType, const std::string_view& str, CesiumGltf::Buffer& o);

  private:

    CesiumGltf::Buffer* _pObject = nullptr;
    CesiumJsonReader::StringJsonHandler _uri;
    CesiumJsonReader::IntegerJsonHandler<int64_t> _byteLength;
  };
}
