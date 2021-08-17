// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "Library.h"

#include <CesiumUtility/ExtensibleObject.h>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#define CODEGEN_API CESIUM3DTILES_API

namespace Cesium3DTiles {

struct CODEGEN_API Asset : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName = "Asset";

  std::string version;

  std::optional<std::string> tilesetVersion;
};

struct CODEGEN_API BoundingVolume : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName = "BoundingVolume";

  std::optional<std::vector<double>> box;

  std::optional<std::vector<double>> region;

  std::optional<std::vector<double>> sphere;
};

struct CODEGEN_API TileContent : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName = "TileContent";

  std::optional<BoundingVolume> boundingVolume;

  std::string uri;
};

struct CODEGEN_API Tile : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName = "Tile";

  enum class Refine { ADD, REPLACE };

  BoundingVolume boundingVolume;

  std::optional<BoundingVolume> viewerRequestVolume;

  double geometricError;

  std::optional<Refine> refine;

  std::vector<double> transform =
      {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

  std::optional<TileContent> content;

  std::optional<std::vector<Tile>> children;
};

struct CODEGEN_API TilesetProperties : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName = "TilesetProperties";

  double maximum;

  double minimum;
};

struct CODEGEN_API Tileset : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName = "Tileset";

  struct CODEGEN_API Properties : public CesiumUtility::ExtensibleObject {
    static inline constexpr const char* TypeName = "Properties";

    std::unordered_map<std::string, TilesetProperties> additionalProperties;
  };

  Asset asset;

  std::optional<Properties> properties;

  double geometricError;

  Tile root;

  std::optional<std::vector<std::string>> extensionsUsed;

  std::optional<std::vector<std::string>> extensionsRequired;
};

struct CODEGEN_API FeatureTable : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName = "FeatureTable";

  struct CODEGEN_API BinaryBodyReference
      : public CesiumUtility::ExtensibleObject {
    static inline constexpr const char* TypeName = "BinaryBodyReference";

    enum class ComponentType {
      BYTE,
      UNSIGNED_BYTE,
      SHORT,
      UNSIGNED_SHORT,
      INT,
      UNSIGNED_INT,
      FLOAT,
      DOUBLE
    };

    double byteOffset;

    std::optional<ComponentType> componentType;
  };

  struct CODEGEN_API Property {
    static inline constexpr const char* TypeName = "Property";

    std::optional<BinaryBodyReference> v0;
    std::optional<std::vector<double>> v1;
    std::optional<double> v2;
  };
  struct CODEGEN_API GlobalPropertyScalar {
    static inline constexpr const char* TypeName = "GlobalPropertyScalar";

    struct CODEGEN_API Variant0 : public CesiumUtility::ExtensibleObject {
      static inline constexpr const char* TypeName = "Variant0";

      double byteOffset;
    };

    std::optional<Variant0> v0;
    std::optional<std::vector<double>> v1;
    std::optional<double> v2;
  };
  struct CODEGEN_API GlobalPropertyCartesian3 {
    static inline constexpr const char* TypeName = "GlobalPropertyCartesian3";

    struct CODEGEN_API Variant0 : public CesiumUtility::ExtensibleObject {
      static inline constexpr const char* TypeName = "Variant0";

      double byteOffset;
    };

    std::optional<Variant0> v0;
    std::optional<std::vector<double>> v1;
  };
  struct CODEGEN_API GlobalPropertyCartesian4 {
    static inline constexpr const char* TypeName = "GlobalPropertyCartesian4";

    struct CODEGEN_API Variant0 : public CesiumUtility::ExtensibleObject {
      static inline constexpr const char* TypeName = "Variant0";

      double byteOffset;
    };

    std::optional<Variant0> v0;
    std::optional<std::vector<double>> v1;
  };

  std::unordered_map<std::string, Property> additionalProperties;
};

struct CODEGEN_API PntsFeatureTable : public FeatureTable {
  static inline constexpr const char* TypeName = "PntsFeatureTable";

  std::optional<BinaryBodyReference> POSITION;

  std::optional<BinaryBodyReference> POSITION_QUANTIZED;

  std::optional<BinaryBodyReference> RGBA;

  std::optional<BinaryBodyReference> RGB;

  std::optional<BinaryBodyReference> RGB565;

  std::optional<BinaryBodyReference> NORMAL;

  std::optional<BinaryBodyReference> NORMAL_OCT16P;

  std::optional<BinaryBodyReference> BATCH_ID;

  std::optional<GlobalPropertyScalar> POINTS_LENGTH;

  std::optional<GlobalPropertyCartesian3> RTC_CENTER;

  std::optional<GlobalPropertyCartesian3> QUANTIZED_VOLUME_OFFSET;

  std::optional<GlobalPropertyCartesian3> QUANTIZED_VOLUME_SCALE;

  std::optional<GlobalPropertyCartesian4> CONSTANT_RGBA;

  std::optional<GlobalPropertyScalar> BATCH_LENGTH;
};

} // namespace Cesium3DTiles

#undef CODEGEN_API
