// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include <CesiumGltf/RegionSlice.h>
#include <CesiumJsonReader/ArrayJsonHandler.h>
#include <CesiumJsonReader/DoubleJsonHandler.h>
#include <CesiumJsonReader/ExtensibleObjectJsonHandler.h>

namespace CesiumJsonReader {
class JsonReaderOptions;
}

namespace CesiumGltfReader {
class RegionSliceJsonHandler
    : public CesiumJsonReader::ExtensibleObjectJsonHandler {
public:
  using ValueType = CesiumGltf::RegionSlice;

  RegionSliceJsonHandler(
      const CesiumJsonReader::JsonReaderOptions& options) noexcept;
  void reset(IJsonHandler* pParentHandler, CesiumGltf::RegionSlice* pObject);

  virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

protected:
  IJsonHandler* readObjectKeyRegionSlice(
      const std::string& objectType,
      const std::string_view& str,
      CesiumGltf::RegionSlice& o);

private:
  CesiumGltf::RegionSlice* _pObject = nullptr;
  CesiumJsonReader::DoubleJsonHandler _minHeight;
  CesiumJsonReader::DoubleJsonHandler _maxHeight;
  CesiumJsonReader::
      ArrayJsonHandler<double, CesiumJsonReader::DoubleJsonHandler>
          _minAngle;
  CesiumJsonReader::
      ArrayJsonHandler<double, CesiumJsonReader::DoubleJsonHandler>
          _maxAngle;
};
} // namespace CesiumGltfReader
