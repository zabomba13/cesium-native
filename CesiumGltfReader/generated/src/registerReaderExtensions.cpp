// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!

#include "registerReaderExtensions.h"

#include <CesiumJsonReader/JsonReaderOptions.h>

#include <CesiumGltf/Model.h>
#include <CesiumGltf/MeshPrimitive.h>
#include <CesiumGltf/Node.h>
#include <CesiumGltf/Buffer.h>
#include <CesiumGltf/BufferView.h>
#include <CesiumGltf/Material.h>
#include <CesiumGltf/Texture.h>
#include <CesiumGltf/TextureInfo.h>
#include <CesiumGltf/MaterialOcclusionTextureInfo.h>
#include <CesiumGltf/MaterialNormalTextureInfo.h>
#include <CesiumGltf/PropertyTextureProperty.h>
#include <CesiumGltf/FeatureIdTexture.h>

#include "ExtensionCesiumRTCJsonHandler.h"
#include "ExtensionModelExtStructuralMetadataJsonHandler.h"
#include "ExtensionModelKhrMaterialsVariantsJsonHandler.h"
#include "ExtensionModelMaxarMeshVariantsJsonHandler.h"
#include "ExtensionCesiumTileEdgesJsonHandler.h"
#include "ExtensionExtMeshFeaturesJsonHandler.h"
#include "ExtensionMeshPrimitiveExtStructuralMetadataJsonHandler.h"
#include "ExtensionKhrDracoMeshCompressionJsonHandler.h"
#include "ExtensionMeshPrimitiveKhrMaterialsVariantsJsonHandler.h"
#include "ExtensionCesiumPrimitiveOutlineJsonHandler.h"
#include "ExtensionKhrGaussianSplattingJsonHandler.h"
#include "ExtensionExtInstanceFeaturesJsonHandler.h"
#include "ExtensionExtMeshGpuInstancingJsonHandler.h"
#include "ExtensionNodeMaxarMeshVariantsJsonHandler.h"
#include "ExtensionBufferExtMeshoptCompressionJsonHandler.h"
#include "ExtensionBufferViewExtMeshoptCompressionJsonHandler.h"
#include "ExtensionKhrMaterialsUnlitJsonHandler.h"
#include "ExtensionKhrTextureBasisuJsonHandler.h"
#include "ExtensionTextureWebpJsonHandler.h"
#include "ExtensionKhrTextureTransformJsonHandler.h"
#include "ExtensionKhrTextureTransformJsonHandler.h"
#include "ExtensionKhrTextureTransformJsonHandler.h"
#include "ExtensionKhrTextureTransformJsonHandler.h"
#include "ExtensionKhrTextureTransformJsonHandler.h"

namespace CesiumGltfReader {

void registerReaderExtensions(CesiumJsonReader::JsonReaderOptions& options) {
  (void)options;
  options.registerExtension<CesiumGltf::Model, ExtensionCesiumRTCJsonHandler>();
options.registerExtension<CesiumGltf::Model, ExtensionModelExtStructuralMetadataJsonHandler>();
options.registerExtension<CesiumGltf::Model, ExtensionModelKhrMaterialsVariantsJsonHandler>();
options.registerExtension<CesiumGltf::Model, ExtensionModelMaxarMeshVariantsJsonHandler>();
options.registerExtension<CesiumGltf::MeshPrimitive, ExtensionCesiumTileEdgesJsonHandler>();
options.registerExtension<CesiumGltf::MeshPrimitive, ExtensionExtMeshFeaturesJsonHandler>();
options.registerExtension<CesiumGltf::MeshPrimitive, ExtensionMeshPrimitiveExtStructuralMetadataJsonHandler>();
options.registerExtension<CesiumGltf::MeshPrimitive, ExtensionKhrDracoMeshCompressionJsonHandler>();
options.registerExtension<CesiumGltf::MeshPrimitive, ExtensionMeshPrimitiveKhrMaterialsVariantsJsonHandler>();
options.registerExtension<CesiumGltf::MeshPrimitive, ExtensionCesiumPrimitiveOutlineJsonHandler>();
options.registerExtension<CesiumGltf::MeshPrimitive, ExtensionKhrGaussianSplattingJsonHandler>();
options.registerExtension<CesiumGltf::Node, ExtensionExtInstanceFeaturesJsonHandler>();
options.registerExtension<CesiumGltf::Node, ExtensionExtMeshGpuInstancingJsonHandler>();
options.registerExtension<CesiumGltf::Node, ExtensionNodeMaxarMeshVariantsJsonHandler>();
options.registerExtension<CesiumGltf::Buffer, ExtensionBufferExtMeshoptCompressionJsonHandler>();
options.registerExtension<CesiumGltf::BufferView, ExtensionBufferViewExtMeshoptCompressionJsonHandler>();
options.registerExtension<CesiumGltf::Material, ExtensionKhrMaterialsUnlitJsonHandler>();
options.registerExtension<CesiumGltf::Texture, ExtensionKhrTextureBasisuJsonHandler>();
options.registerExtension<CesiumGltf::Texture, ExtensionTextureWebpJsonHandler>();
options.registerExtension<CesiumGltf::TextureInfo, ExtensionKhrTextureTransformJsonHandler>();
options.registerExtension<CesiumGltf::MaterialOcclusionTextureInfo, ExtensionKhrTextureTransformJsonHandler>();
options.registerExtension<CesiumGltf::MaterialNormalTextureInfo, ExtensionKhrTextureTransformJsonHandler>();
options.registerExtension<CesiumGltf::PropertyTextureProperty, ExtensionKhrTextureTransformJsonHandler>();
options.registerExtension<CesiumGltf::FeatureIdTexture, ExtensionKhrTextureTransformJsonHandler>();
}
} // namespace CesiumGltfReader
