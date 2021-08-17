// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "WriterLibrary.h"

#include <Cesium3DTiles/Tileset.h>
#include <CesiumJsonWriter/ExtensionWriterContext.h>
#include <CesiumJsonWriter/JsonWriter.h>

namespace Cesium3DTiles {

struct AssetWriter {
  using ValueType = Asset;

  static void write(
      const Asset& obj,
      CesiumJsonWriter::JsonWriter& jsonWriter,
      const CesiumJsonWriter::ExtensionWriterContext& context);
};
struct BoundingVolumeWriter {
  using ValueType = BoundingVolume;

  static void write(
      const BoundingVolume& obj,
      CesiumJsonWriter::JsonWriter& jsonWriter,
      const CesiumJsonWriter::ExtensionWriterContext& context);
};
struct TileContentWriter {
  using ValueType = TileContent;

  static void write(
      const TileContent& obj,
      CesiumJsonWriter::JsonWriter& jsonWriter,
      const CesiumJsonWriter::ExtensionWriterContext& context);
};
struct TileWriter {
  using ValueType = Tile;

  struct RefineWriter {
    static void write(
        const Tile::Refine& obj,
        CesiumJsonWriter::JsonWriter& jsonWriter,
        const CesiumJsonWriter::ExtensionWriterContext& context);
  };

  static void write(
      const Tile& obj,
      CesiumJsonWriter::JsonWriter& jsonWriter,
      const CesiumJsonWriter::ExtensionWriterContext& context);
};
struct TilesetPropertiesWriter {
  using ValueType = TilesetProperties;

  static void write(
      const TilesetProperties& obj,
      CesiumJsonWriter::JsonWriter& jsonWriter,
      const CesiumJsonWriter::ExtensionWriterContext& context);
};
struct TilesetWriter {
  using ValueType = Tileset;

  struct PropertiesWriter {
    using ValueType = Tileset::Properties;

    static void write(
        const Tileset::Properties& obj,
        CesiumJsonWriter::JsonWriter& jsonWriter,
        const CesiumJsonWriter::ExtensionWriterContext& context);
  };

  static void write(
      const Tileset& obj,
      CesiumJsonWriter::JsonWriter& jsonWriter,
      const CesiumJsonWriter::ExtensionWriterContext& context);
};
struct FeatureTableWriter {
  using ValueType = FeatureTable;

  struct BinaryBodyReferenceWriter {
    using ValueType = FeatureTable::BinaryBodyReference;

    struct ComponentTypeWriter {
      static void write(
          const FeatureTable::BinaryBodyReference::ComponentType& obj,
          CesiumJsonWriter::JsonWriter& jsonWriter,
          const CesiumJsonWriter::ExtensionWriterContext& context);
    };

    static void write(
        const FeatureTable::BinaryBodyReference& obj,
        CesiumJsonWriter::JsonWriter& jsonWriter,
        const CesiumJsonWriter::ExtensionWriterContext& context);
  };

  struct PropertyWriter {
    using ValueType = FeatureTable::Property;

    static void write(
        const FeatureTable::Property& obj,
        CesiumJsonWriter::JsonWriter& jsonWriter,
        const CesiumJsonWriter::ExtensionWriterContext& context);
  };

  struct GlobalPropertyScalarWriter {
    using ValueType = FeatureTable::GlobalPropertyScalar;

    struct Variant0Writer {
      using ValueType = FeatureTable::GlobalPropertyScalar::Variant0;

      static void write(
          const FeatureTable::GlobalPropertyScalar::Variant0& obj,
          CesiumJsonWriter::JsonWriter& jsonWriter,
          const CesiumJsonWriter::ExtensionWriterContext& context);
    };

    static void write(
        const FeatureTable::GlobalPropertyScalar& obj,
        CesiumJsonWriter::JsonWriter& jsonWriter,
        const CesiumJsonWriter::ExtensionWriterContext& context);
  };

  struct GlobalPropertyCartesian3Writer {
    using ValueType = FeatureTable::GlobalPropertyCartesian3;

    struct Variant0Writer {
      using ValueType = FeatureTable::GlobalPropertyCartesian3::Variant0;

      static void write(
          const FeatureTable::GlobalPropertyCartesian3::Variant0& obj,
          CesiumJsonWriter::JsonWriter& jsonWriter,
          const CesiumJsonWriter::ExtensionWriterContext& context);
    };

    static void write(
        const FeatureTable::GlobalPropertyCartesian3& obj,
        CesiumJsonWriter::JsonWriter& jsonWriter,
        const CesiumJsonWriter::ExtensionWriterContext& context);
  };

  struct GlobalPropertyCartesian4Writer {
    using ValueType = FeatureTable::GlobalPropertyCartesian4;

    struct Variant0Writer {
      using ValueType = FeatureTable::GlobalPropertyCartesian4::Variant0;

      static void write(
          const FeatureTable::GlobalPropertyCartesian4::Variant0& obj,
          CesiumJsonWriter::JsonWriter& jsonWriter,
          const CesiumJsonWriter::ExtensionWriterContext& context);
    };

    static void write(
        const FeatureTable::GlobalPropertyCartesian4& obj,
        CesiumJsonWriter::JsonWriter& jsonWriter,
        const CesiumJsonWriter::ExtensionWriterContext& context);
  };

  static void write(
      const FeatureTable& obj,
      CesiumJsonWriter::JsonWriter& jsonWriter,
      const CesiumJsonWriter::ExtensionWriterContext& context);
};
struct PntsFeatureTableWriter {
  using ValueType = PntsFeatureTable;

  static void write(
      const PntsFeatureTable& obj,
      CesiumJsonWriter::JsonWriter& jsonWriter,
      const CesiumJsonWriter::ExtensionWriterContext& context);
};

} // namespace Cesium3DTiles
