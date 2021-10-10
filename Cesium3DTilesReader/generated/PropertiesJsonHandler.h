// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "Cesium3DTiles/Properties.h"
#include "Cesium3DTiles/ReaderContext.h"
#include "CesiumJsonReader/DoubleJsonHandler.h"
#include "ExtensibleObjectJsonHandler.h"

namespace Cesium3DTiles {
struct ReaderContext;

class PropertiesJsonHandler : public ExtensibleObjectJsonHandler {
public:
  using ValueType = Properties;

  PropertiesJsonHandler(const ReaderContext& context) noexcept;
  void reset(IJsonHandler* pParentHandler, Properties* pObject);

  virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

protected:
  IJsonHandler* readObjectKeyProperties(
      const std::string& objectType,
      const std::string_view& str,
      Properties& o);

private:
  Properties* _pObject = nullptr;
  CesiumJsonReader::DoubleJsonHandler _maximum;
  CesiumJsonReader::DoubleJsonHandler _minimum;
};
} // namespace Cesium3DTiles
