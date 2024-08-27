// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "NamedObjectJsonHandler.h"

#include <CesiumGltf/Texture.h>
#include <CesiumJsonReader/IntegerJsonHandler.h>

namespace CesiumJsonReader {
class JsonReaderOptions;
}

namespace CesiumGltfReader {
class TextureJsonHandler : public CesiumGltfReader::NamedObjectJsonHandler {
public:
  using ValueType = CesiumGltf::Texture;

  TextureJsonHandler(const CesiumJsonReader::JsonReaderOptions& options
  ) noexcept;
  void reset(IJsonHandler* pParentHandler, CesiumGltf::Texture* pObject);

  virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

protected:
  IJsonHandler* readObjectKeyTexture(
      const std::string& objectType,
      const std::string_view& str,
      CesiumGltf::Texture& o
  );

private:
  CesiumGltf::Texture* _pObject = nullptr;
  CesiumJsonReader::IntegerJsonHandler<int32_t> _sampler;
  CesiumJsonReader::IntegerJsonHandler<int32_t> _source;
};
} // namespace CesiumGltfReader
