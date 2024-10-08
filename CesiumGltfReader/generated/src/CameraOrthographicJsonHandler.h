// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include <CesiumGltf/CameraOrthographic.h>
#include <CesiumJsonReader/DoubleJsonHandler.h>
#include <CesiumJsonReader/ExtensibleObjectJsonHandler.h>

namespace CesiumJsonReader {
  class JsonReaderOptions;
}

namespace CesiumGltfReader {
  class CameraOrthographicJsonHandler : public CesiumJsonReader::ExtensibleObjectJsonHandler {
  public:
    using ValueType = CesiumGltf::CameraOrthographic;

    CameraOrthographicJsonHandler(const CesiumJsonReader::JsonReaderOptions& options) noexcept;
    void reset(IJsonHandler* pParentHandler, CesiumGltf::CameraOrthographic* pObject);

    virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

  protected:
    IJsonHandler* readObjectKeyCameraOrthographic(const std::string& objectType, const std::string_view& str, CesiumGltf::CameraOrthographic& o);

  private:

    CesiumGltf::CameraOrthographic* _pObject = nullptr;
    CesiumJsonReader::DoubleJsonHandler _xmag;
    CesiumJsonReader::DoubleJsonHandler _ymag;
    CesiumJsonReader::DoubleJsonHandler _zfar;
    CesiumJsonReader::DoubleJsonHandler _znear;
  };
}
