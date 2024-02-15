// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "PaddingJsonHandler.h"

#include <Cesium3DTiles/ExtensionContent3dTilesContentVoxels.h>
#include <CesiumJsonReader/ArrayJsonHandler.h>
#include <CesiumJsonReader/ExtensibleObjectJsonHandler.h>
#include <CesiumJsonReader/IntegerJsonHandler.h>
#include <CesiumJsonReader/StringJsonHandler.h>

namespace CesiumJsonReader {
class JsonReaderOptions;
}

namespace Cesium3DTilesReader {
class ExtensionContent3dTilesContentVoxelsJsonHandler
    : public CesiumJsonReader::ExtensibleObjectJsonHandler,
      public CesiumJsonReader::IExtensionJsonHandler {
public:
  using ValueType = Cesium3DTiles::ExtensionContent3dTilesContentVoxels;

  static inline constexpr const char* ExtensionName = "3DTILES_content_voxels";

  ExtensionContent3dTilesContentVoxelsJsonHandler(
      const CesiumJsonReader::JsonReaderOptions& options) noexcept;
  void reset(
      IJsonHandler* pParentHandler,
      Cesium3DTiles::ExtensionContent3dTilesContentVoxels* pObject);

  virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

  virtual void reset(
      IJsonHandler* pParentHandler,
      CesiumUtility::ExtensibleObject& o,
      const std::string_view& extensionName) override;

  virtual IJsonHandler& getHandler() override { return *this; }

protected:
  IJsonHandler* readObjectKeyExtensionContent3dTilesContentVoxels(
      const std::string& objectType,
      const std::string_view& str,
      Cesium3DTiles::ExtensionContent3dTilesContentVoxels& o);

private:
  Cesium3DTiles::ExtensionContent3dTilesContentVoxels* _pObject = nullptr;
  CesiumJsonReader::
      ArrayJsonHandler<int64_t, CesiumJsonReader::IntegerJsonHandler<int64_t>>
          _dimensions;
  PaddingJsonHandler _padding;
  CesiumJsonReader::StringJsonHandler _classProperty;
};
} // namespace Cesium3DTilesReader
