// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!

#include "registerExtensions.h"

#include "ModelJsonWriter.h"

#include <CesiumGltf/Buffer.h>
#include <CesiumGltf/BufferView.h>
#include <CesiumGltf/ExtensionBufferExtMeshoptCompression.h>
#include <CesiumGltf/ExtensionBufferViewExtMeshoptCompression.h>
#include <CesiumGltf/ExtensionMaterialKhrMaterialsUnlit.h>
#include <CesiumGltf/ExtensionMeshPrimitiveCesiumTileEdges.h>
#include <CesiumGltf/ExtensionMeshPrimitiveExtFeatureMetadata.h>
#include <CesiumGltf/ExtensionMeshPrimitiveKhrDracoMeshCompression.h>
#include <CesiumGltf/ExtensionModelExtFeatureMetadata.h>
#include <CesiumGltf/ExtensionNodeExtMeshGpuInstancing.h>
#include <CesiumGltf/Material.h>
#include <CesiumGltf/MeshPrimitive.h>
#include <CesiumGltf/Model.h>
#include <CesiumGltf/Node.h>
#include <CesiumJsonWriter/ExtensionWriterContext.h>

namespace CesiumGltfWriter {

void registerExtensions(CesiumJsonWriter::ExtensionWriterContext& context) {
  (void)context;
  context.registerExtension<
      CesiumGltf::MeshPrimitive,
      ExtensionMeshPrimitiveKhrDracoMeshCompressionJsonWriter>();
  context.registerExtension<
      CesiumGltf::MeshPrimitive,
      ExtensionMeshPrimitiveExtFeatureMetadataJsonWriter>();
  context.registerExtension<
      CesiumGltf::MeshPrimitive,
      ExtensionMeshPrimitiveCesiumTileEdgesJsonWriter>();
  context.registerExtension<
      CesiumGltf::Material,
      ExtensionMaterialKhrMaterialsUnlitJsonWriter>();
  context.registerExtension<
      CesiumGltf::Node,
      ExtensionNodeExtMeshGpuInstancingJsonWriter>();
  context.registerExtension<
      CesiumGltf::Buffer,
      ExtensionBufferExtMeshoptCompressionJsonWriter>();
  context.registerExtension<
      CesiumGltf::BufferView,
      ExtensionBufferViewExtMeshoptCompressionJsonWriter>();
  context.registerExtension<
      CesiumGltf::Model,
      ExtensionModelExtFeatureMetadataJsonWriter>();
}
} // namespace CesiumGltfWriter
