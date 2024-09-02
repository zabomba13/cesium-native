#include "Cesium3DTilesContent/GltfConverterResult.h"
#include "CesiumGltf/Accessor.h"
#include "CesiumGltf/Buffer.h"
#include "CesiumGltf/BufferView.h"
#include "CesiumGltf/FeatureId.h"
#include "CesiumGltf/Material.h"
#include "CesiumGltf/MaterialPBRMetallicRoughness.h"
#include "CesiumGltf/Mesh.h"
#include "CesiumGltf/MeshPrimitive.h"
#include "CesiumGltf/Model.h"
#include "CesiumGltf/Node.h"
#include "ConvertTileToGltf.h"

#include <CesiumGltf/ExtensionCesiumRTC.h>
#include <CesiumGltf/ExtensionExtMeshFeatures.h>
#include <CesiumGltf/ExtensionKhrMaterialsUnlit.h>
#include <CesiumGltf/ExtensionModelExtStructuralMetadata.h>
#include <CesiumUtility/Math.h>

#include <catch2/catch.hpp>
#include <catch2/catch_test_macros.hpp>
#include <glm/ext/vector_double3.hpp>
#include <glm/ext/vector_double4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/vector_uint2_sized.hpp>
#include <glm/ext/vector_uint3_sized.hpp>
#include <glm/ext/vector_uint4_sized.hpp>

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <set>
#include <string>
#include <vector>

using namespace CesiumGltf;
using namespace Cesium3DTilesContent;
using namespace CesiumUtility;

template <typename Type>
static void checkBufferContents(
    const std::vector<std::byte>& buffer,
    const std::vector<Type>& expected,
    [[maybe_unused]] const double epsilon = Math::Epsilon6) {
  REQUIRE(buffer.size() == expected.size() * sizeof(Type));
  const int32_t byteStride = sizeof(Type);
  if constexpr (std::is_same_v<Type, glm::vec3>) {
    for (size_t i = 0; i < expected.size(); ++i) {
      const glm::vec3& value =
          *reinterpret_cast<const glm::vec3*>(buffer.data() + i * byteStride);
      const glm::vec3& expectedValue = expected[i];
      REQUIRE(Math::equalsEpsilon(
          static_cast<glm::dvec3>(value),
          static_cast<glm::dvec3>(expectedValue),
          epsilon));
    }
  } else if constexpr (std::is_same_v<Type, glm::vec4>) {
    for (size_t i = 0; i < expected.size(); ++i) {
      const glm::vec4& value =
          *reinterpret_cast<const glm::vec4*>(buffer.data() + i * byteStride);
      const glm::vec4& expectedValue = expected[i];
      REQUIRE(Math::equalsEpsilon(
          static_cast<glm::dvec4>(value),
          static_cast<glm::dvec4>(expectedValue),
          epsilon));
    }
  } else if constexpr (std::is_floating_point_v<Type>) {
    for (size_t i = 0; i < expected.size(); ++i) {
      const Type& value =
          *reinterpret_cast<const Type*>(buffer.data() + i * byteStride);
      const Type& expectedValue = expected[i];
      REQUIRE(value == Approx(expectedValue));
    }
  } else if constexpr (
      std::is_integral_v<Type> || std::is_same_v<Type, glm::u8vec2> ||
      std::is_same_v<Type, glm::u8vec3> || std::is_same_v<Type, glm::u8vec4>) {
    for (size_t i = 0; i < expected.size(); ++i) {
      const Type& value =
          *reinterpret_cast<const Type*>(buffer.data() + i * byteStride);
      const Type& expectedValue = expected[i];
      REQUIRE(value == expectedValue);
    }
  } else {
    FAIL("Buffer check has not been implemented for the given type.");
  }
}

template <typename Type>
static void checkAttribute(
    const Model& gltf,
    const MeshPrimitive& primitive,
    const std::string& attributeSemantic,
    const uint32_t expectedCount) {
  const auto& attributes = primitive.attributes;
  REQUIRE(attributes.find(attributeSemantic) != attributes.end());

  const int32_t accessorId = attributes.at(attributeSemantic);
  REQUIRE(accessorId >= 0);
  const auto accessorIdUint = static_cast<uint32_t>(accessorId);
  const Accessor& accessor = gltf.accessors[accessorIdUint];

  int32_t expectedComponentType = -1;
  std::string expectedType;

  if constexpr (std::is_same_v<Type, glm::vec3>) {
    expectedComponentType = Accessor::ComponentType::FLOAT;
    expectedType = Accessor::Type::VEC3;
  } else if constexpr (std::is_same_v<Type, glm::vec4>) {
    expectedComponentType = Accessor::ComponentType::FLOAT;
    expectedType = Accessor::Type::VEC4;
  } else if constexpr (std::is_same_v<Type, glm::u8vec3>) {
    expectedComponentType = Accessor::ComponentType::UNSIGNED_BYTE;
    expectedType = Accessor::Type::VEC3;
  } else if constexpr (std::is_same_v<Type, glm::u8vec4>) {
    expectedComponentType = Accessor::ComponentType::UNSIGNED_BYTE;
    expectedType = Accessor::Type::VEC4;
  } else if constexpr (std::is_same_v<Type, uint8_t>) {
    expectedComponentType = Accessor::ComponentType::UNSIGNED_BYTE;
    expectedType = Accessor::Type::SCALAR;
  } else {
    FAIL("Accessor check has not been implemented for the given type.");
  }

  CHECK(accessor.byteOffset == 0);
  CHECK(accessor.componentType == expectedComponentType);
  CHECK(accessor.count == expectedCount);
  CHECK(accessor.type == expectedType);

  const auto expectedByteLength =
      static_cast<int64_t>(expectedCount * sizeof(Type));

  const int32_t bufferViewId = accessor.bufferView;
  REQUIRE(bufferViewId >= 0);
  const auto bufferViewIdUint = static_cast<uint32_t>(bufferViewId);
  const BufferView& bufferView = gltf.bufferViews[bufferViewIdUint];
  CHECK(bufferView.byteLength == expectedByteLength);
  CHECK(bufferView.byteOffset == 0);

  const int32_t bufferId = bufferView.buffer;
  REQUIRE(bufferId >= 0);
  const auto bufferIdUint = static_cast<uint32_t>(bufferId);
  const Buffer& buffer = gltf.buffers[static_cast<uint32_t>(bufferIdUint)];
  CHECK(buffer.byteLength == expectedByteLength);
  CHECK(static_cast<int64_t>(buffer.cesium.data.size()) == buffer.byteLength);
}

TEST_CASE("Converts simple point cloud to glTF") {
  std::filesystem::path testFilePath = Cesium3DTilesSelection_TEST_DATA_DIR;
  testFilePath = testFilePath / "PointCloud" / "pointCloudPositionsOnly.pnts";
  const int32_t pointsLength = 8;

  GltfConverterResult result = ConvertTileToGltf::fromPnts(testFilePath);

  REQUIRE(result.model);
  Model& gltf = *result.model;

  CHECK(gltf.asset.version == "2.0");

  // Check for single mesh node
  REQUIRE(gltf.nodes.size() == 1);
  Node& node = gltf.nodes[0];
  // clang-format off
  std::vector<double> expectedMatrix = {
      1.0, 0.0, 0.0, 0.0,
      0.0, 0.0, -1.0, 0.0,
      0.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 1.0};
  // clang-format on
  CHECK(node.matrix == expectedMatrix);
  CHECK(node.mesh == 0);

  // Check for a default scene referencing the node
  CHECK(gltf.scene == 0);
  REQUIRE(gltf.scenes.size() == 1);
  REQUIRE(gltf.scenes[0].nodes.size() == 1);
  CHECK(gltf.scenes[0].nodes[0] == 0);

  // Check for single mesh primitive
  REQUIRE(gltf.meshes.size() == 1);
  Mesh& mesh = gltf.meshes[0];
  REQUIRE(mesh.primitives.size() == 1);
  MeshPrimitive& primitive = mesh.primitives[0];
  CHECK(primitive.mode == MeshPrimitive::Mode::POINTS);
  CHECK(primitive.material == 0);

  // Check for single material
  REQUIRE(gltf.materials.size() == 1);
  Material& material = gltf.materials[0];
  CHECK(material.pbrMetallicRoughness);
  CHECK(material.hasExtension<ExtensionKhrMaterialsUnlit>());
  CHECK(gltf.isExtensionUsed(ExtensionKhrMaterialsUnlit::ExtensionName));

  auto attributes = primitive.attributes;
  REQUIRE(attributes.size() == 1);
  REQUIRE(attributes.find("POSITION") != attributes.end());
  CHECK(attributes.at("POSITION") == 0);

  // Check for single accessor
  REQUIRE(gltf.accessors.size() == 1);
  Accessor& accessor = gltf.accessors[0];
  CHECK(accessor.bufferView == 0);
  CHECK(accessor.byteOffset == 0);
  CHECK(accessor.componentType == Accessor::ComponentType::FLOAT);
  CHECK(accessor.count == pointsLength);
  CHECK(accessor.type == Accessor::Type::VEC3);

  const glm::vec3 expectedMin(-3.2968313, -4.0330467, -3.5223078);
  CHECK(accessor.min[0] == Approx(expectedMin.x));
  CHECK(accessor.min[1] == Approx(expectedMin.y));
  CHECK(accessor.min[2] == Approx(expectedMin.z));

  const glm::vec3 expectedMax(3.2968313, 4.0330467, 3.5223078);
  CHECK(accessor.max[0] == Approx(expectedMax.x));
  CHECK(accessor.max[1] == Approx(expectedMax.y));
  CHECK(accessor.max[2] == Approx(expectedMax.z));

  // Check for single bufferView
  REQUIRE(gltf.bufferViews.size() == 1);
  BufferView& bufferView = gltf.bufferViews[0];
  CHECK(bufferView.buffer == 0);
  CHECK(bufferView.byteLength == pointsLength * sizeof(glm::vec3));
  CHECK(bufferView.byteOffset == 0);

  // Check for single buffer
  REQUIRE(gltf.buffers.size() == 1);
  Buffer& buffer = gltf.buffers[0];
  CHECK(buffer.byteLength == pointsLength * sizeof(glm::vec3));
  CHECK(static_cast<int64_t>(buffer.cesium.data.size()) == buffer.byteLength);

  const std::vector<glm::vec3> expectedPositions = {
      glm::vec3(-2.4975082, -0.3252686, -3.5223078),
      glm::vec3(2.3456699, 0.9171584, -3.5223078),
      glm::vec3(-3.2968313, 2.7906193, 0.3055275),
      glm::vec3(1.5463469, 4.03304672, 0.3055275),
      glm::vec3(-1.5463469, -4.03304672, -0.3055275),
      glm::vec3(3.2968313, -2.7906193, -0.3055275),
      glm::vec3(-2.3456699, -0.9171584, 3.5223078),
      glm::vec3(2.4975082, 0.3252686, 3.5223078)};

  checkBufferContents<glm::vec3>(buffer.cesium.data, expectedPositions);

  // Check for RTC extension
  REQUIRE(gltf.hasExtension<CesiumGltf::ExtensionCesiumRTC>());
  CHECK(gltf.isExtensionUsed(ExtensionCesiumRTC::ExtensionName));
  CHECK(gltf.isExtensionRequired(ExtensionCesiumRTC::ExtensionName));
  const auto& rtcExtension =
      result.model->getExtension<CesiumGltf::ExtensionCesiumRTC>();
  const glm::vec3 expectedRtcCenter(
      1215012.8828876,
      -4736313.0511995,
      4081605.2212604);
  CHECK(rtcExtension->center[0] == Approx(expectedRtcCenter.x));
  CHECK(rtcExtension->center[1] == Approx(expectedRtcCenter.y));
  CHECK(rtcExtension->center[2] == Approx(expectedRtcCenter.z));
}

TEST_CASE("Converts point cloud with RGBA to glTF") {
  std::filesystem::path testFilePath = Cesium3DTilesSelection_TEST_DATA_DIR;
  testFilePath = testFilePath / "PointCloud" / "pointCloudRGBA.pnts";
  const int32_t pointsLength = 8;
  const int32_t expectedAttributeCount = 2;

  GltfConverterResult result = ConvertTileToGltf::fromPnts(testFilePath);

  REQUIRE(result.model);
  Model& gltf = *result.model;

  CHECK(gltf.hasExtension<CesiumGltf::ExtensionCesiumRTC>());
  CHECK(gltf.isExtensionUsed(ExtensionCesiumRTC::ExtensionName));
  CHECK(gltf.isExtensionRequired(ExtensionCesiumRTC::ExtensionName));

  CHECK(gltf.scenes.size() == 1);
  CHECK(gltf.nodes.size() == 1);

  REQUIRE(gltf.meshes.size() == 1);
  Mesh& mesh = gltf.meshes[0];
  REQUIRE(mesh.primitives.size() == 1);
  MeshPrimitive& primitive = mesh.primitives[0];
  CHECK(primitive.mode == MeshPrimitive::Mode::POINTS);

  REQUIRE(gltf.materials.size() == 1);
  Material& material = gltf.materials[0];
  CHECK(material.alphaMode == Material::AlphaMode::BLEND);
  CHECK(material.hasExtension<ExtensionKhrMaterialsUnlit>());
  CHECK(gltf.isExtensionUsed(ExtensionKhrMaterialsUnlit::ExtensionName));

  REQUIRE(gltf.accessors.size() == expectedAttributeCount);
  REQUIRE(gltf.bufferViews.size() == expectedAttributeCount);
  REQUIRE(gltf.buffers.size() == expectedAttributeCount);

  auto attributes = primitive.attributes;
  REQUIRE(attributes.size() == expectedAttributeCount);

  // Check that position and color attributes are present
  checkAttribute<glm::vec3>(gltf, primitive, "POSITION", pointsLength);
  checkAttribute<glm::vec4>(gltf, primitive, "COLOR_0", pointsLength);

  // Check color attribute more thoroughly
  uint32_t colorAccessorId = static_cast<uint32_t>(attributes.at("COLOR_0"));
  Accessor& colorAccessor = gltf.accessors[colorAccessorId];
  CHECK(!colorAccessor.normalized);

  auto colorBufferViewId = static_cast<uint32_t>(colorAccessor.bufferView);
  BufferView& colorBufferView = gltf.bufferViews[colorBufferViewId];

  auto colorBufferId = static_cast<uint32_t>(colorBufferView.buffer);
  Buffer& colorBuffer = gltf.buffers[colorBufferId];

  const std::vector<glm::vec4> expectedColors = {
      glm::vec4(0.263174F, 0.315762F, 0.476177F, 0.423529F),
      glm::vec4(0.325036F, 0.708297F, 0.259027F, 0.423529F),
      glm::vec4(0.151058F, 0.353740F, 0.378676F, 0.192156F),
      glm::vec4(0.160443F, 0.067724F, 0.774227F, 0.027450F),
      glm::vec4(0.915750F, 0.056374F, 0.119264F, 0.239215F),
      glm::vec4(0.592438F, 0.632042F, 0.242796F, 0.239215F),
      glm::vec4(0.284452F, 0.127529F, 0.843369F, 0.419607F),
      glm::vec4(0.002932F, 0.091518F, 0.004559F, 0.321568F)};

  checkBufferContents<glm::vec4>(colorBuffer.cesium.data, expectedColors);
}

TEST_CASE("Converts point cloud with RGB to glTF") {
  std::filesystem::path testFilePath = Cesium3DTilesSelection_TEST_DATA_DIR;
  testFilePath = testFilePath / "PointCloud" / "pointCloudRGB.pnts";
  const int32_t pointsLength = 8;
  const int32_t expectedAttributeCount = 2;

  GltfConverterResult result = ConvertTileToGltf::fromPnts(testFilePath);

  REQUIRE(result.model);
  Model& gltf = *result.model;

  CHECK(gltf.hasExtension<CesiumGltf::ExtensionCesiumRTC>());
  CHECK(gltf.isExtensionUsed(ExtensionCesiumRTC::ExtensionName));
  CHECK(gltf.isExtensionRequired(ExtensionCesiumRTC::ExtensionName));

  CHECK(gltf.scenes.size() == 1);
  CHECK(gltf.nodes.size() == 1);

  REQUIRE(gltf.meshes.size() == 1);
  Mesh& mesh = gltf.meshes[0];
  REQUIRE(mesh.primitives.size() == 1);
  MeshPrimitive& primitive = mesh.primitives[0];
  CHECK(primitive.mode == MeshPrimitive::Mode::POINTS);

  REQUIRE(gltf.materials.size() == 1);
  Material& material = gltf.materials[0];
  CHECK(material.alphaMode == Material::AlphaMode::OPAQUE);
  CHECK(material.hasExtension<ExtensionKhrMaterialsUnlit>());
  CHECK(gltf.isExtensionUsed(ExtensionKhrMaterialsUnlit::ExtensionName));

  REQUIRE(gltf.accessors.size() == expectedAttributeCount);
  REQUIRE(gltf.bufferViews.size() == expectedAttributeCount);
  REQUIRE(gltf.buffers.size() == expectedAttributeCount);

  auto attributes = primitive.attributes;
  REQUIRE(attributes.size() == expectedAttributeCount);

  // Check that position and color attributes are present
  checkAttribute<glm::vec3>(gltf, primitive, "POSITION", pointsLength);
  checkAttribute<glm::vec3>(gltf, primitive, "COLOR_0", pointsLength);

  // Check color attribute more thoroughly
  uint32_t colorAccessorId = static_cast<uint32_t>(attributes.at("COLOR_0"));
  Accessor& colorAccessor = gltf.accessors[colorAccessorId];
  CHECK(!colorAccessor.normalized);

  auto colorBufferViewId = static_cast<uint32_t>(colorAccessor.bufferView);
  BufferView& colorBufferView = gltf.bufferViews[colorBufferViewId];

  auto colorBufferId = static_cast<uint32_t>(colorBufferView.buffer);
  Buffer& colorBuffer = gltf.buffers[colorBufferId];

  const std::vector<glm::vec3> expectedColors = {
      glm::vec3(0.263174F, 0.315762F, 0.476177F),
      glm::vec3(0.325036F, 0.708297F, 0.259027F),
      glm::vec3(0.151058F, 0.353740F, 0.378676F),
      glm::vec3(0.160443F, 0.067724F, 0.774227F),
      glm::vec3(0.915750F, 0.056374F, 0.119264F),
      glm::vec3(0.592438F, 0.632042F, 0.242796F),
      glm::vec3(0.284452F, 0.127529F, 0.843369F),
      glm::vec3(0.002932F, 0.091518F, 0.004559F)};

  checkBufferContents<glm::vec3>(colorBuffer.cesium.data, expectedColors);
}

TEST_CASE("Converts point cloud with RGB565 to glTF") {
  std::filesystem::path testFilePath = Cesium3DTilesSelection_TEST_DATA_DIR;
  testFilePath = testFilePath / "PointCloud" / "pointCloudRGB565.pnts";
  const int32_t pointsLength = 8;
  const int32_t expectedAttributeCount = 2;

  GltfConverterResult result = ConvertTileToGltf::fromPnts(testFilePath);

  REQUIRE(result.model);
  Model& gltf = *result.model;

  CHECK(gltf.hasExtension<CesiumGltf::ExtensionCesiumRTC>());
  CHECK(gltf.isExtensionUsed(ExtensionCesiumRTC::ExtensionName));
  CHECK(gltf.isExtensionRequired(ExtensionCesiumRTC::ExtensionName));

  CHECK(gltf.scenes.size() == 1);
  CHECK(gltf.nodes.size() == 1);

  REQUIRE(gltf.meshes.size() == 1);
  Mesh& mesh = gltf.meshes[0];
  REQUIRE(mesh.primitives.size() == 1);
  MeshPrimitive& primitive = mesh.primitives[0];
  CHECK(primitive.mode == MeshPrimitive::Mode::POINTS);

  REQUIRE(gltf.materials.size() == 1);
  Material& material = gltf.materials[0];
  CHECK(material.alphaMode == Material::AlphaMode::OPAQUE);
  CHECK(material.hasExtension<ExtensionKhrMaterialsUnlit>());
  CHECK(gltf.isExtensionUsed(ExtensionKhrMaterialsUnlit::ExtensionName));

  REQUIRE(gltf.accessors.size() == expectedAttributeCount);
  REQUIRE(gltf.bufferViews.size() == expectedAttributeCount);
  REQUIRE(gltf.buffers.size() == expectedAttributeCount);

  auto attributes = primitive.attributes;
  REQUIRE(attributes.size() == expectedAttributeCount);

  // Check that position and color attributes are present
  checkAttribute<glm::vec3>(gltf, primitive, "POSITION", pointsLength);
  checkAttribute<glm::vec3>(gltf, primitive, "COLOR_0", pointsLength);

  // Check color attribute more thoroughly
  uint32_t colorAccessorId = static_cast<uint32_t>(attributes.at("COLOR_0"));
  Accessor& colorAccessor = gltf.accessors[colorAccessorId];
  CHECK(!colorAccessor.normalized);

  auto colorBufferViewId = static_cast<uint32_t>(colorAccessor.bufferView);
  BufferView& colorBufferView = gltf.bufferViews[colorBufferViewId];

  auto colorBufferId = static_cast<uint32_t>(colorBufferView.buffer);
  Buffer& colorBuffer = gltf.buffers[colorBufferId];

  const std::vector<glm::vec3> expectedColors = {
      glm::vec3(0.2666808F, 0.3100948F, 0.4702556F),
      glm::vec3(0.3024152F, 0.7123886F, 0.2333824F),
      glm::vec3(0.1478017F, 0.3481712F, 0.3813029F),
      glm::vec3(0.1478017F, 0.0635404F, 0.7379118F),
      glm::vec3(0.8635347F, 0.0560322F, 0.1023452F),
      glm::vec3(0.5694675F, 0.6282104F, 0.2333824F),
      glm::vec3(0.2666808F, 0.1196507F, 0.7993773F),
      glm::vec3(0.0024058F, 0.0891934F, 0.0024058F)};

  checkBufferContents<glm::vec3>(colorBuffer.cesium.data, expectedColors);
}

TEST_CASE("Converts point cloud with CONSTANT_RGBA") {
  std::filesystem::path testFilePath = Cesium3DTilesSelection_TEST_DATA_DIR;
  testFilePath = testFilePath / "PointCloud" / "pointCloudConstantRGBA.pnts";
  GltfConverterResult result = ConvertTileToGltf::fromPnts(testFilePath);
  const int32_t pointsLength = 8;

  REQUIRE(result.model);
  Model& gltf = *result.model;

  CHECK(gltf.hasExtension<CesiumGltf::ExtensionCesiumRTC>());
  CHECK(gltf.isExtensionUsed(ExtensionCesiumRTC::ExtensionName));
  CHECK(gltf.isExtensionRequired(ExtensionCesiumRTC::ExtensionName));

  CHECK(gltf.scenes.size() == 1);
  CHECK(gltf.nodes.size() == 1);

  REQUIRE(gltf.meshes.size() == 1);
  Mesh& mesh = gltf.meshes[0];
  REQUIRE(mesh.primitives.size() == 1);
  MeshPrimitive& primitive = mesh.primitives[0];
  CHECK(primitive.mode == MeshPrimitive::Mode::POINTS);
  CHECK(primitive.material == 0);

  CHECK(gltf.buffers.size() == 1);
  CHECK(gltf.bufferViews.size() == 1);
  CHECK(gltf.accessors.size() == 1);

  checkAttribute<glm::vec3>(gltf, primitive, "POSITION", pointsLength);

  REQUIRE(gltf.materials.size() == 1);
  Material& material = gltf.materials[0];
  REQUIRE(material.pbrMetallicRoughness);
  MaterialPBRMetallicRoughness& pbrMetallicRoughness =
      material.pbrMetallicRoughness.value();
  const auto& baseColorFactor = pbrMetallicRoughness.baseColorFactor;

  // Check that CONSTANT_RGBA is stored in the material base color
  const glm::vec4 expectedConstantRGBA =
      glm::vec4(1.0F, 1.0F, 0.0F, 51.0F / 255.0F);
  CHECK(baseColorFactor[0] == Approx(expectedConstantRGBA.x));
  CHECK(baseColorFactor[1] == Approx(expectedConstantRGBA.y));
  CHECK(baseColorFactor[2] == Approx(expectedConstantRGBA.z));
  CHECK(baseColorFactor[3] == Approx(expectedConstantRGBA.w));

  CHECK(material.alphaMode == Material::AlphaMode::BLEND);
  CHECK(material.hasExtension<ExtensionKhrMaterialsUnlit>());
  CHECK(gltf.isExtensionUsed(ExtensionKhrMaterialsUnlit::ExtensionName));
}

TEST_CASE("Converts point cloud with quantized positions to glTF") {
  std::filesystem::path testFilePath = Cesium3DTilesSelection_TEST_DATA_DIR;
  testFilePath = testFilePath / "PointCloud" / "pointCloudQuantized.pnts";
  const int32_t pointsLength = 8;
  const int32_t expectedAttributeCount = 2;

  GltfConverterResult result = ConvertTileToGltf::fromPnts(testFilePath);

  REQUIRE(result.model);
  Model& gltf = *result.model;

  CHECK(!gltf.hasExtension<CesiumGltf::ExtensionCesiumRTC>());
  CHECK(!gltf.isExtensionUsed(ExtensionCesiumRTC::ExtensionName));
  CHECK(!gltf.isExtensionRequired(ExtensionCesiumRTC::ExtensionName));

  CHECK(gltf.scenes.size() == 1);
  CHECK(gltf.nodes.size() == 1);

  REQUIRE(gltf.meshes.size() == 1);
  Mesh& mesh = gltf.meshes[0];
  REQUIRE(mesh.primitives.size() == 1);
  MeshPrimitive& primitive = mesh.primitives[0];
  CHECK(primitive.mode == MeshPrimitive::Mode::POINTS);

  REQUIRE(gltf.materials.size() == 1);
  Material& material = gltf.materials[0];
  CHECK(material.hasExtension<ExtensionKhrMaterialsUnlit>());
  CHECK(gltf.isExtensionUsed(ExtensionKhrMaterialsUnlit::ExtensionName));

  REQUIRE(gltf.accessors.size() == expectedAttributeCount);
  REQUIRE(gltf.bufferViews.size() == expectedAttributeCount);
  REQUIRE(gltf.buffers.size() == expectedAttributeCount);

  auto attributes = primitive.attributes;
  REQUIRE(attributes.size() == expectedAttributeCount);

  // Check that position and color attributes are present
  checkAttribute<glm::vec3>(gltf, primitive, "POSITION", pointsLength);
  checkAttribute<glm::vec3>(gltf, primitive, "COLOR_0", pointsLength);

  // Check position attribute more thoroughly
  uint32_t positionAccessorId =
      static_cast<uint32_t>(attributes.at("POSITION"));
  Accessor& positionAccessor = gltf.accessors[positionAccessorId];
  CHECK(!positionAccessor.normalized);

  const glm::vec3 expectedMin(1215009.59, -4736317.08, 4081601.7);
  CHECK(positionAccessor.min[0] == Approx(expectedMin.x));
  CHECK(positionAccessor.min[1] == Approx(expectedMin.y));
  CHECK(positionAccessor.min[2] == Approx(expectedMin.z));

  const glm::vec3 expectedMax(1215016.18, -4736309.02, 4081608.74);
  CHECK(positionAccessor.max[0] == Approx(expectedMax.x));
  CHECK(positionAccessor.max[1] == Approx(expectedMax.y));
  CHECK(positionAccessor.max[2] == Approx(expectedMax.z));

  auto positionBufferViewId =
      static_cast<uint32_t>(positionAccessor.bufferView);
  BufferView& positionBufferView = gltf.bufferViews[positionBufferViewId];

  auto positionBufferId = static_cast<uint32_t>(positionBufferView.buffer);
  Buffer& positionBuffer = gltf.buffers[positionBufferId];

  const std::vector<glm::vec3> expectedPositions = {
      glm::vec3(1215010.39, -4736313.38, 4081601.7),
      glm::vec3(1215015.23, -4736312.13, 4081601.7),
      glm::vec3(1215009.59, -4736310.26, 4081605.53),
      glm::vec3(1215014.43, -4736309.02, 4081605.53),
      glm::vec3(1215011.34, -4736317.08, 4081604.92),
      glm::vec3(1215016.18, -4736315.84, 4081604.92),
      glm::vec3(1215010.54, -4736313.97, 4081608.74),
      glm::vec3(1215015.38, -4736312.73, 4081608.74)};

  checkBufferContents<glm::vec3>(positionBuffer.cesium.data, expectedPositions);
}

TEST_CASE("Converts point cloud with normals to glTF") {
  std::filesystem::path testFilePath = Cesium3DTilesSelection_TEST_DATA_DIR;
  testFilePath = testFilePath / "PointCloud" / "pointCloudNormals.pnts";
  const int32_t pointsLength = 8;
  const int32_t expectedAttributeCount = 3;

  GltfConverterResult result = ConvertTileToGltf::fromPnts(testFilePath);

  REQUIRE(result.model);
  Model& gltf = *result.model;

  CHECK(gltf.hasExtension<CesiumGltf::ExtensionCesiumRTC>());
  CHECK(gltf.isExtensionUsed(ExtensionCesiumRTC::ExtensionName));
  CHECK(gltf.isExtensionRequired(ExtensionCesiumRTC::ExtensionName));

  CHECK(gltf.scenes.size() == 1);
  CHECK(gltf.nodes.size() == 1);

  REQUIRE(gltf.meshes.size() == 1);
  Mesh& mesh = gltf.meshes[0];
  REQUIRE(mesh.primitives.size() == 1);
  MeshPrimitive& primitive = mesh.primitives[0];
  CHECK(primitive.mode == MeshPrimitive::Mode::POINTS);

  REQUIRE(gltf.materials.size() == 1);
  Material& material = gltf.materials[0];
  CHECK(!material.hasExtension<ExtensionKhrMaterialsUnlit>());
  CHECK(!gltf.isExtensionUsed(ExtensionKhrMaterialsUnlit::ExtensionName));

  REQUIRE(gltf.accessors.size() == expectedAttributeCount);
  REQUIRE(gltf.bufferViews.size() == expectedAttributeCount);
  REQUIRE(gltf.buffers.size() == expectedAttributeCount);

  auto attributes = primitive.attributes;
  REQUIRE(attributes.size() == expectedAttributeCount);

  // Check that position, color, and normal attributes are present
  checkAttribute<glm::vec3>(gltf, primitive, "POSITION", pointsLength);
  checkAttribute<glm::vec3>(gltf, primitive, "COLOR_0", pointsLength);
  checkAttribute<glm::vec3>(gltf, primitive, "NORMAL", pointsLength);

  // Check normal attribute more thoroughly
  uint32_t normalAccessorId = static_cast<uint32_t>(attributes.at("NORMAL"));
  Accessor& normalAccessor = gltf.accessors[normalAccessorId];

  auto normalBufferViewId = static_cast<uint32_t>(normalAccessor.bufferView);
  BufferView& normalBufferView = gltf.bufferViews[normalBufferViewId];

  auto normalBufferId = static_cast<uint32_t>(normalBufferView.buffer);
  Buffer& normalBuffer = gltf.buffers[normalBufferId];

  const std::vector<glm::vec3> expectedNormals = {
      glm::vec3(-0.9854088, 0.1667507, 0.0341110),
      glm::vec3(-0.5957704, 0.5378777, 0.5964436),
      glm::vec3(-0.5666092, -0.7828890, -0.2569800),
      glm::vec3(-0.5804154, -0.7226123, 0.3754320),
      glm::vec3(-0.8535281, -0.1291752, -0.5047805),
      glm::vec3(0.7557975, 0.1243999, 0.6428800),
      glm::vec3(0.1374090, -0.2333731, -0.9626296),
      glm::vec3(-0.0633145, 0.9630424, 0.2618022)};

  checkBufferContents<glm::vec3>(normalBuffer.cesium.data, expectedNormals);
}

TEST_CASE("Converts point cloud with oct-encoded normals to glTF") {
  std::filesystem::path testFilePath = Cesium3DTilesSelection_TEST_DATA_DIR;
  testFilePath =
      testFilePath / "PointCloud" / "pointCloudNormalsOctEncoded.pnts";
  const int32_t pointsLength = 8;
  const int32_t expectedAttributeCount = 3;

  GltfConverterResult result = ConvertTileToGltf::fromPnts(testFilePath);

  REQUIRE(result.model);
  Model& gltf = *result.model;

  CHECK(gltf.hasExtension<CesiumGltf::ExtensionCesiumRTC>());
  CHECK(gltf.isExtensionUsed(ExtensionCesiumRTC::ExtensionName));
  CHECK(gltf.isExtensionRequired(ExtensionCesiumRTC::ExtensionName));

  CHECK(gltf.scenes.size() == 1);
  CHECK(gltf.nodes.size() == 1);

  REQUIRE(gltf.meshes.size() == 1);
  Mesh& mesh = gltf.meshes[0];
  REQUIRE(mesh.primitives.size() == 1);
  MeshPrimitive& primitive = mesh.primitives[0];
  CHECK(primitive.mode == MeshPrimitive::Mode::POINTS);

  REQUIRE(gltf.materials.size() == 1);
  Material& material = gltf.materials[0];
  CHECK(!material.hasExtension<ExtensionKhrMaterialsUnlit>());
  CHECK(!gltf.isExtensionUsed(ExtensionKhrMaterialsUnlit::ExtensionName));

  REQUIRE(gltf.accessors.size() == expectedAttributeCount);
  REQUIRE(gltf.bufferViews.size() == expectedAttributeCount);
  REQUIRE(gltf.buffers.size() == expectedAttributeCount);

  auto attributes = primitive.attributes;
  REQUIRE(attributes.size() == expectedAttributeCount);

  // Check that position, color, and normal attributes are present
  checkAttribute<glm::vec3>(gltf, primitive, "POSITION", pointsLength);
  checkAttribute<glm::vec3>(gltf, primitive, "COLOR_0", pointsLength);
  checkAttribute<glm::vec3>(gltf, primitive, "NORMAL", pointsLength);

  // Check normal attribute more thoroughly
  uint32_t normalAccessorId = static_cast<uint32_t>(attributes.at("NORMAL"));
  Accessor& normalAccessor = gltf.accessors[normalAccessorId];
  CHECK(!normalAccessor.normalized);

  auto normalBufferViewId = static_cast<uint32_t>(normalAccessor.bufferView);
  BufferView& normalBufferView = gltf.bufferViews[normalBufferViewId];

  auto normalBufferId = static_cast<uint32_t>(normalBufferView.buffer);
  Buffer& normalBuffer = gltf.buffers[normalBufferId];

  const std::vector<glm::vec3> expectedNormals = {
      glm::vec3(-0.9856477, 0.1634960, 0.0420418),
      glm::vec3(-0.5901730, 0.5359042, 0.6037402),
      glm::vec3(-0.5674310, -0.7817938, -0.2584963),
      glm::vec3(-0.5861990, -0.7179291, 0.3754308),
      glm::vec3(-0.8519385, -0.1283743, -0.5076620),
      glm::vec3(0.7587127, 0.1254564, 0.6392304),
      glm::vec3(0.1354662, -0.2292506, -0.9638947),
      glm::vec3(-0.0656172, 0.9640687, 0.2574214)};

  checkBufferContents<glm::vec3>(normalBuffer.cesium.data, expectedNormals);
}

std::set<int32_t>
getUniqueBufferIds(const std::vector<BufferView>& bufferViews) {
  std::set<int32_t> result;
  for (const auto& bufferView : bufferViews) {
    result.insert(bufferView.buffer);
  }

  return result;
}

TEST_CASE("Converts point cloud with batch IDs to glTF with "
          "EXT_structural_metadata") {
  std::filesystem::path testFilePath = Cesium3DTilesSelection_TEST_DATA_DIR;
  testFilePath = testFilePath / "PointCloud" / "pointCloudBatched.pnts";
  const int32_t pointsLength = 8;

  GltfConverterResult result = ConvertTileToGltf::fromPnts(testFilePath);

  REQUIRE(result.model);
  Model& gltf = *result.model;

  // The correctness of the model extension is thoroughly tested in
  // TestUpgradeBatchTableToExtStructuralMetadata
  CHECK(gltf.hasExtension<ExtensionModelExtStructuralMetadata>());

  CHECK(gltf.scenes.size() == 1);
  CHECK(gltf.nodes.size() == 1);
  REQUIRE(gltf.meshes.size() == 1);
  Mesh& mesh = gltf.meshes[0];
  REQUIRE(mesh.primitives.size() == 1);
  MeshPrimitive& primitive = mesh.primitives[0];
  CHECK(primitive.mode == MeshPrimitive::Mode::POINTS);

  auto* primitiveExtension = primitive.getExtension<ExtensionExtMeshFeatures>();
  REQUIRE(primitiveExtension);
  REQUIRE(primitiveExtension->featureIds.size() == 1);
  FeatureId& featureId = primitiveExtension->featureIds[0];
  CHECK(featureId.featureCount == 8);
  CHECK(featureId.attribute == 0);
  CHECK(featureId.propertyTable == 0);

  CHECK(gltf.materials.size() == 1);

  // The file has three metadata properties:
  // - "name": string scalars in JSON
  // - "dimensions": float vec3s in binary
  // - "id": int scalars in binary
  // There are three accessors (one per primitive attribute)
  // and four additional buffer views:
  // - "name" string data buffer view
  // - "name" string offsets buffer view
  // - "dimensions" buffer view
  // - "id" buffer view
  REQUIRE(gltf.accessors.size() == 3);
  REQUIRE(gltf.bufferViews.size() == 7);

  // There are also three added buffers:
  // - binary data in the batch table
  // - string data of "name"
  // - string offsets for the data for "name"
  REQUIRE(gltf.buffers.size() == 6);
  std::set<int32_t> bufferSet = getUniqueBufferIds(gltf.bufferViews);
  CHECK(bufferSet.size() == 6);

  auto attributes = primitive.attributes;
  REQUIRE(attributes.size() == 3);

  // Check that position, normal, and feature ID attributes are present
  checkAttribute<glm::vec3>(gltf, primitive, "POSITION", pointsLength);
  checkAttribute<glm::vec3>(gltf, primitive, "NORMAL", pointsLength);
  checkAttribute<uint8_t>(gltf, primitive, "_FEATURE_ID_0", pointsLength);

  // Check feature ID attribute more thoroughly
  uint32_t featureIdAccessorId =
      static_cast<uint32_t>(attributes.at("_FEATURE_ID_0"));
  Accessor& featureIdAccessor = gltf.accessors[featureIdAccessorId];

  auto featureIdBufferViewId =
      static_cast<uint32_t>(featureIdAccessor.bufferView);
  BufferView& featureIdBufferView = gltf.bufferViews[featureIdBufferViewId];

  auto featureIdBufferId = static_cast<uint32_t>(featureIdBufferView.buffer);
  Buffer& featureIdBuffer = gltf.buffers[featureIdBufferId];

  const std::vector<uint8_t> expectedFeatureIDs = {5, 5, 6, 6, 7, 0, 3, 1};
  checkBufferContents<uint8_t>(featureIdBuffer.cesium.data, expectedFeatureIDs);
}

TEST_CASE("Converts point cloud with per-point properties to glTF with "
          "EXT_structural_metadata") {
  std::filesystem::path testFilePath = Cesium3DTilesSelection_TEST_DATA_DIR;
  testFilePath =
      testFilePath / "PointCloud" / "pointCloudWithPerPointProperties.pnts";
  const int32_t pointsLength = 8;

  GltfConverterResult result = ConvertTileToGltf::fromPnts(testFilePath);

  REQUIRE(result.model);
  Model& gltf = *result.model;

  // The correctness of the model extension is thoroughly tested in
  // TestUpgradeBatchTableToExtStructuralMetadata
  CHECK(gltf.hasExtension<ExtensionModelExtStructuralMetadata>());

  CHECK(gltf.scenes.size() == 1);
  CHECK(gltf.nodes.size() == 1);
  REQUIRE(gltf.meshes.size() == 1);
  Mesh& mesh = gltf.meshes[0];
  REQUIRE(mesh.primitives.size() == 1);
  MeshPrimitive& primitive = mesh.primitives[0];
  CHECK(primitive.mode == MeshPrimitive::Mode::POINTS);

  auto* primitiveExtension = primitive.getExtension<ExtensionExtMeshFeatures>();
  REQUIRE(primitiveExtension);
  REQUIRE(primitiveExtension->featureIds.size() == 1);
  FeatureId& featureId = primitiveExtension->featureIds[0];
  // Check for implicit feature IDs
  CHECK(featureId.featureCount == pointsLength);
  CHECK(!featureId.attribute);
  CHECK(featureId.propertyTable == 0);

  CHECK(gltf.materials.size() == 1);

  // The file has three binary metadata properties:
  // - "temperature": float scalars
  // - "secondaryColor": float vec3s
  // - "id": unsigned short scalars
  // There are two accessors (one per primitive attribute)
  // and three additional buffer views:
  // - temperature buffer view
  // - secondary color buffer view
  // - id buffer view
  REQUIRE(gltf.accessors.size() == 2);
  REQUIRE(gltf.bufferViews.size() == 5);

  // There is only one added buffer containing all the binary values.
  REQUIRE(gltf.buffers.size() == 3);
  std::set<int32_t> bufferSet = getUniqueBufferIds(gltf.bufferViews);
  CHECK(bufferSet.size() == 3);

  auto attributes = primitive.attributes;
  REQUIRE(attributes.size() == 2);
  REQUIRE(attributes.find("_FEATURE_ID_0") == attributes.end());

  // Check that position and color  attributes are present
  checkAttribute<glm::vec3>(gltf, primitive, "POSITION", pointsLength);
  checkAttribute<glm::vec3>(gltf, primitive, "COLOR_0", pointsLength);
}

TEST_CASE("Converts point cloud with Draco compression to glTF") {
  std::filesystem::path testFilePath = Cesium3DTilesSelection_TEST_DATA_DIR;
  testFilePath = testFilePath / "PointCloud" / "pointCloudDraco.pnts";
  const int32_t pointsLength = 8;

  GltfConverterResult result = ConvertTileToGltf::fromPnts(testFilePath);

  REQUIRE(result.model);
  Model& gltf = *result.model;

  CHECK(gltf.hasExtension<CesiumGltf::ExtensionCesiumRTC>());
  CHECK(gltf.isExtensionUsed(ExtensionCesiumRTC::ExtensionName));
  CHECK(gltf.isExtensionRequired(ExtensionCesiumRTC::ExtensionName));
  // The correctness of the model extension is thoroughly tested in
  // TestUpgradeBatchTableToExtStructuralMetadata
  CHECK(gltf.hasExtension<ExtensionModelExtStructuralMetadata>());

  CHECK(gltf.scenes.size() == 1);
  CHECK(gltf.nodes.size() == 1);
  REQUIRE(gltf.meshes.size() == 1);
  Mesh& mesh = gltf.meshes[0];
  REQUIRE(mesh.primitives.size() == 1);
  MeshPrimitive& primitive = mesh.primitives[0];
  CHECK(primitive.mode == MeshPrimitive::Mode::POINTS);

  auto* primitiveExtension = primitive.getExtension<ExtensionExtMeshFeatures>();
  REQUIRE(primitiveExtension);
  REQUIRE(primitiveExtension->featureIds.size() == 1);
  FeatureId& featureId = primitiveExtension->featureIds[0];
  // Check for implicit feature IDs
  CHECK(featureId.featureCount == pointsLength);
  CHECK(!featureId.attribute);
  CHECK(featureId.propertyTable == 0);

  REQUIRE(gltf.materials.size() == 1);
  Material& material = gltf.materials[0];
  CHECK(!material.hasExtension<ExtensionKhrMaterialsUnlit>());
  CHECK(!gltf.isExtensionUsed(ExtensionKhrMaterialsUnlit::ExtensionName));

  // The file has three binary metadata properties:
  // - "temperature": float scalars
  // - "secondaryColor": float vec3s
  // - "id": unsigned short scalars
  // There are three accessors (one per primitive attribute)
  // and three additional buffer views:
  // - temperature buffer view
  // - secondary color buffer view
  // - id buffer view
  REQUIRE(gltf.accessors.size() == 3);
  REQUIRE(gltf.bufferViews.size() == 6);

  // There is only one added buffer containing all the binary values.
  REQUIRE(gltf.buffers.size() == 4);

  auto attributes = primitive.attributes;
  REQUIRE(attributes.size() == 3);

  // Check that position, color, and normal attributes are present
  checkAttribute<glm::vec3>(gltf, primitive, "POSITION", pointsLength);
  checkAttribute<glm::vec3>(gltf, primitive, "COLOR_0", pointsLength);
  checkAttribute<glm::vec3>(gltf, primitive, "NORMAL", pointsLength);

  // Check each attribute more thoroughly
  {
    uint32_t accessorId = static_cast<uint32_t>(attributes.at("POSITION"));
    Accessor& accessor = gltf.accessors[accessorId];

    const glm::vec3 expectedMin(-4.9270443F, -3.9144449F, -4.8131480F);
    CHECK(accessor.min[0] == Approx(expectedMin.x));
    CHECK(accessor.min[1] == Approx(expectedMin.y));
    CHECK(accessor.min[2] == Approx(expectedMin.z));

    const glm::vec3 expectedMax(3.7791683F, 4.8152132F, 3.2142156F);
    CHECK(accessor.max[0] == Approx(expectedMax.x));
    CHECK(accessor.max[1] == Approx(expectedMax.y));
    CHECK(accessor.max[2] == Approx(expectedMax.z));

    auto bufferViewId = static_cast<uint32_t>(accessor.bufferView);
    BufferView& bufferView = gltf.bufferViews[bufferViewId];

    auto bufferId = static_cast<uint32_t>(bufferView.buffer);
    Buffer& buffer = gltf.buffers[bufferId];

    std::vector<glm::vec3> expected = {
        glm::vec3(-4.9270443F, 0.8337686F, 0.1705846F),
        glm::vec3(-2.9789500F, 2.6891474F, 2.9824265F),
        glm::vec3(-2.8329495F, -3.9144449F, -1.2851576F),
        glm::vec3(-2.9022198F, -3.6128526F, 1.8772986F),
        glm::vec3(-4.2673778F, -0.6459517F, -2.5240305F),
        glm::vec3(3.7791683F, 0.6222278F, 3.2142156F),
        glm::vec3(0.6870481F, -1.1670776F, -4.8131480F),
        glm::vec3(-0.3168385F, 4.8152132F, 1.3087492F),
    };
    checkBufferContents<glm::vec3>(buffer.cesium.data, expected);
  }

  {
    uint32_t accessorId = static_cast<uint32_t>(attributes.at("COLOR_0"));
    Accessor& accessor = gltf.accessors[accessorId];
    CHECK(!accessor.normalized);

    auto bufferViewId = static_cast<uint32_t>(accessor.bufferView);
    BufferView& bufferView = gltf.bufferViews[bufferViewId];

    auto bufferId = static_cast<uint32_t>(bufferView.buffer);
    Buffer& buffer = gltf.buffers[bufferId];

    std::vector<glm::vec3> expected = {
        glm::vec3(0.4761772F, 0.6870308F, 0.3250369F),
        glm::vec3(0.1510580F, 0.3537409F, 0.3786762F),
        glm::vec3(0.7742273F, 0.0016869F, 0.9157501F),
        glm::vec3(0.5924380F, 0.6320426F, 0.2427963F),
        glm::vec3(0.8433697F, 0.6730490F, 0.0029323F),
        glm::vec3(0.0001751F, 0.1087111F, 0.6661169F),
        glm::vec3(0.7299188F, 0.7299188F, 0.9489649F),
        glm::vec3(0.1801442F, 0.2348952F, 0.5795466F),
    };
    checkBufferContents<glm::vec3>(buffer.cesium.data, expected);
  }

  {
    uint32_t accessorId = static_cast<uint32_t>(attributes.at("NORMAL"));
    Accessor& accessor = gltf.accessors[accessorId];

    auto bufferViewId = static_cast<uint32_t>(accessor.bufferView);
    BufferView& bufferView = gltf.bufferViews[bufferViewId];

    auto bufferId = static_cast<uint32_t>(bufferView.buffer);
    Buffer& buffer = gltf.buffers[bufferId];

    // The Draco-decoded normals are slightly different from the values
    // derived by manually decoding the uncompressed oct-encoded normals,
    // hence the less precise comparison.
    std::vector<glm::vec3> expected{
        glm::vec3(-0.9824559F, 0.1803542F, 0.0474616F),
        glm::vec3(-0.5766854F, 0.5427628F, 0.6106081F),
        glm::vec3(-0.5725988F, -0.7802446F, -0.2516918F),
        glm::vec3(-0.5705807F, -0.7345407F, 0.36727036F),
        glm::vec3(-0.8560267F, -0.1281128F, -0.5008047F),
        glm::vec3(0.7647877F, 0.11264316F, 0.63435888F),
        glm::vec3(0.1301889F, -0.23434004F, -0.9633979F),
        glm::vec3(-0.0450783F, 0.9616723F, 0.2704703F),
    };
    checkBufferContents<glm::vec3>(
        buffer.cesium.data,
        expected,
        Math::Epsilon1);
  }
}

TEST_CASE("Converts point cloud with partial Draco compression to glTF") {
  std::filesystem::path testFilePath = Cesium3DTilesSelection_TEST_DATA_DIR;
  testFilePath = testFilePath / "PointCloud" / "pointCloudDracoPartial.pnts";
  const int32_t pointsLength = 8;

  GltfConverterResult result = ConvertTileToGltf::fromPnts(testFilePath);

  REQUIRE(result.model);
  Model& gltf = *result.model;

  CHECK(gltf.hasExtension<CesiumGltf::ExtensionCesiumRTC>());
  CHECK(gltf.isExtensionUsed(ExtensionCesiumRTC::ExtensionName));
  CHECK(gltf.isExtensionRequired(ExtensionCesiumRTC::ExtensionName));
  CHECK(gltf.hasExtension<ExtensionModelExtStructuralMetadata>());

  CHECK(gltf.scenes.size() == 1);
  CHECK(gltf.nodes.size() == 1);
  REQUIRE(gltf.meshes.size() == 1);
  Mesh& mesh = gltf.meshes[0];
  REQUIRE(mesh.primitives.size() == 1);
  MeshPrimitive& primitive = mesh.primitives[0];
  CHECK(primitive.mode == MeshPrimitive::Mode::POINTS);

  auto* primitiveExtension = primitive.getExtension<ExtensionExtMeshFeatures>();
  REQUIRE(primitiveExtension);
  REQUIRE(primitiveExtension->featureIds.size() == 1);
  FeatureId& featureId = primitiveExtension->featureIds[0];
  // Check for implicit feature IDs
  CHECK(featureId.featureCount == pointsLength);
  CHECK(!featureId.attribute);
  CHECK(featureId.propertyTable == 0);

  REQUIRE(gltf.materials.size() == 1);
  Material& material = gltf.materials[0];
  CHECK(!material.hasExtension<ExtensionKhrMaterialsUnlit>());
  CHECK(!gltf.isExtensionUsed(ExtensionKhrMaterialsUnlit::ExtensionName));

  // The file has three binary metadata properties:
  // - "temperature": float scalars
  // - "secondaryColor": float vec3s
  // - "id": unsigned short scalars
  // There are three accessors (one per primitive attribute)
  // and three additional buffer views:
  // - temperature buffer view
  // - secondary color buffer view
  // - id buffer view
  REQUIRE(gltf.accessors.size() == 3);
  REQUIRE(gltf.bufferViews.size() == 6);

  // There is only one added buffer containing all the binary values.
  REQUIRE(gltf.buffers.size() == 4);

  auto attributes = primitive.attributes;
  REQUIRE(attributes.size() == 3);

  // Check that position, color, and normal attributes are present
  checkAttribute<glm::vec3>(gltf, primitive, "POSITION", pointsLength);
  checkAttribute<glm::vec3>(gltf, primitive, "COLOR_0", pointsLength);
  checkAttribute<glm::vec3>(gltf, primitive, "NORMAL", pointsLength);

  // Check each attribute more thoroughly
  {
    uint32_t accessorId = static_cast<uint32_t>(attributes.at("POSITION"));
    Accessor& accessor = gltf.accessors[accessorId];

    const glm::vec3 expectedMin(-4.9270443F, -3.9144449F, -4.8131480F);
    CHECK(accessor.min[0] == Approx(expectedMin.x));
    CHECK(accessor.min[1] == Approx(expectedMin.y));
    CHECK(accessor.min[2] == Approx(expectedMin.z));

    const glm::vec3 expectedMax(3.7791683F, 4.8152132F, 3.2142156F);
    CHECK(accessor.max[0] == Approx(expectedMax.x));
    CHECK(accessor.max[1] == Approx(expectedMax.y));
    CHECK(accessor.max[2] == Approx(expectedMax.z));

    auto bufferViewId = static_cast<uint32_t>(accessor.bufferView);
    BufferView& bufferView = gltf.bufferViews[bufferViewId];

    auto bufferId = static_cast<uint32_t>(bufferView.buffer);
    Buffer& buffer = gltf.buffers[bufferId];

    std::vector<glm::vec3> expected = {
        glm::vec3(-4.9270443F, 0.8337686F, 0.1705846F),
        glm::vec3(-2.9789500F, 2.6891474F, 2.9824265F),
        glm::vec3(-2.8329495F, -3.9144449F, -1.2851576F),
        glm::vec3(-2.9022198F, -3.6128526F, 1.8772986F),
        glm::vec3(-4.2673778F, -0.6459517F, -2.5240305F),
        glm::vec3(3.7791683F, 0.6222278F, 3.2142156F),
        glm::vec3(0.6870481F, -1.1670776F, -4.8131480F),
        glm::vec3(-0.3168385F, 4.8152132F, 1.3087492F),
    };
    checkBufferContents<glm::vec3>(buffer.cesium.data, expected);
  }

  {
    uint32_t accessorId = static_cast<uint32_t>(attributes.at("COLOR_0"));
    Accessor& accessor = gltf.accessors[accessorId];
    CHECK(!accessor.normalized);

    auto bufferViewId = static_cast<uint32_t>(accessor.bufferView);
    BufferView& bufferView = gltf.bufferViews[bufferViewId];

    auto bufferId = static_cast<uint32_t>(bufferView.buffer);
    Buffer& buffer = gltf.buffers[bufferId];

    std::vector<glm::vec3> expected = {
        glm::vec3(0.4761772F, 0.6870308F, 0.3250369F),
        glm::vec3(0.1510580F, 0.3537409F, 0.3786762F),
        glm::vec3(0.7742273F, 0.0016869F, 0.9157501F),
        glm::vec3(0.5924380F, 0.6320426F, 0.2427963F),
        glm::vec3(0.8433697F, 0.6730490F, 0.0029323F),
        glm::vec3(0.0001751F, 0.1087111F, 0.6661169F),
        glm::vec3(0.7299188F, 0.7299188F, 0.9489649F),
        glm::vec3(0.1801442F, 0.2348952F, 0.5795466F),
    };
    checkBufferContents<glm::vec3>(buffer.cesium.data, expected);
  }

  {
    uint32_t accessorId = static_cast<uint32_t>(attributes.at("NORMAL"));
    Accessor& accessor = gltf.accessors[accessorId];

    auto bufferViewId = static_cast<uint32_t>(accessor.bufferView);
    BufferView& bufferView = gltf.bufferViews[bufferViewId];

    auto bufferId = static_cast<uint32_t>(bufferView.buffer);
    Buffer& buffer = gltf.buffers[bufferId];

    const std::vector<glm::vec3> expected = {
        glm::vec3(-0.9854088, 0.1667507, 0.0341110),
        glm::vec3(-0.5957704, 0.5378777, 0.5964436),
        glm::vec3(-0.5666092, -0.7828890, -0.2569800),
        glm::vec3(-0.5804154, -0.7226123, 0.3754320),
        glm::vec3(-0.8535281, -0.1291752, -0.5047805),
        glm::vec3(0.7557975, 0.1243999, 0.6428800),
        glm::vec3(0.1374090, -0.2333731, -0.9626296),
        glm::vec3(-0.0633145, 0.9630424, 0.2618022)};

    checkBufferContents<glm::vec3>(buffer.cesium.data, expected);
  }
}

TEST_CASE("Converts batched point cloud with Draco compression to glTF") {
  std::filesystem::path testFilePath = Cesium3DTilesSelection_TEST_DATA_DIR;
  testFilePath = testFilePath / "PointCloud" / "pointCloudDracoBatched.pnts";
  const int32_t pointsLength = 8;

  GltfConverterResult result = ConvertTileToGltf::fromPnts(testFilePath);

  REQUIRE(result.model);
  Model& gltf = *result.model;

  // The correctness of the model extension is thoroughly tested in
  // TestUpgradeBatchTableToExtStructuralMetadata
  CHECK(gltf.hasExtension<ExtensionModelExtStructuralMetadata>());

  CHECK(gltf.scenes.size() == 1);
  CHECK(gltf.nodes.size() == 1);
  REQUIRE(gltf.meshes.size() == 1);
  Mesh& mesh = gltf.meshes[0];
  REQUIRE(mesh.primitives.size() == 1);
  MeshPrimitive& primitive = mesh.primitives[0];
  CHECK(primitive.mode == MeshPrimitive::Mode::POINTS);

  auto* primitiveExtension = primitive.getExtension<ExtensionExtMeshFeatures>();
  REQUIRE(primitiveExtension);
  REQUIRE(primitiveExtension->featureIds.size() == 1);
  FeatureId& featureId = primitiveExtension->featureIds[0];
  CHECK(featureId.featureCount == 8);
  CHECK(featureId.attribute == 0);
  CHECK(featureId.propertyTable == 0);

  CHECK(gltf.materials.size() == 1);

  // The file has three metadata properties:
  // - "name": string scalars in JSON
  // - "dimensions": float vec3s in binary
  // - "id": int scalars in binary
  // There are four accessors (one per primitive attribute)
  // and four additional buffer views:
  // - "name" string data buffer view
  // - "name" string offsets buffer view
  // - "dimensions" buffer view
  // - "id" buffer view
  REQUIRE(gltf.accessors.size() == 4);
  REQUIRE(gltf.bufferViews.size() == 8);

  // There are also three added buffers:
  // - binary data in the batch table
  // - string data of "name"
  // - string offsets for the data for "name"
  REQUIRE(gltf.buffers.size() == 7);
  std::set<int32_t> bufferSet = getUniqueBufferIds(gltf.bufferViews);
  CHECK(bufferSet.size() == 7);

  auto attributes = primitive.attributes;
  REQUIRE(attributes.size() == 4);

  // Check that position, normal, and feature ID attributes are present
  checkAttribute<glm::vec3>(gltf, primitive, "POSITION", pointsLength);
  checkAttribute<glm::vec3>(gltf, primitive, "COLOR_0", pointsLength);
  checkAttribute<glm::vec3>(gltf, primitive, "NORMAL", pointsLength);
  checkAttribute<uint8_t>(gltf, primitive, "_FEATURE_ID_0", pointsLength);

  // Check each attribute more thoroughly
  {
    uint32_t accessorId = static_cast<uint32_t>(attributes.at("POSITION"));
    Accessor& accessor = gltf.accessors[accessorId];

    const glm::vec3 expectedMin(-4.9270443F, -3.9144449F, -4.8131480F);
    CHECK(accessor.min[0] == Approx(expectedMin.x));
    CHECK(accessor.min[1] == Approx(expectedMin.y));
    CHECK(accessor.min[2] == Approx(expectedMin.z));

    const glm::vec3 expectedMax(3.7791683F, 4.8152132F, 3.2142156F);
    CHECK(accessor.max[0] == Approx(expectedMax.x));
    CHECK(accessor.max[1] == Approx(expectedMax.y));
    CHECK(accessor.max[2] == Approx(expectedMax.z));

    auto bufferViewId = static_cast<uint32_t>(accessor.bufferView);
    BufferView& bufferView = gltf.bufferViews[bufferViewId];

    auto bufferId = static_cast<uint32_t>(bufferView.buffer);
    Buffer& buffer = gltf.buffers[bufferId];

    std::vector<glm::vec3> expected = {
        glm::vec3(-4.9270443F, 0.8337686F, 0.1705846F),
        glm::vec3(-2.9789500F, 2.6891474F, 2.9824265F),
        glm::vec3(-2.8329495F, -3.9144449F, -1.2851576F),
        glm::vec3(-2.9022198F, -3.6128526F, 1.8772986F),
        glm::vec3(-4.2673778F, -0.6459517F, -2.5240305F),
        glm::vec3(3.7791683F, 0.6222278F, 3.2142156F),
        glm::vec3(0.6870481F, -1.1670776F, -4.8131480F),
        glm::vec3(-0.3168385F, 4.8152132F, 1.3087492F),
    };
    checkBufferContents<glm::vec3>(buffer.cesium.data, expected);
  }

  {
    uint32_t accessorId = static_cast<uint32_t>(attributes.at("COLOR_0"));
    Accessor& accessor = gltf.accessors[accessorId];
    CHECK(!accessor.normalized);

    auto bufferViewId = static_cast<uint32_t>(accessor.bufferView);
    BufferView& bufferView = gltf.bufferViews[bufferViewId];

    auto bufferId = static_cast<uint32_t>(bufferView.buffer);
    Buffer& buffer = gltf.buffers[bufferId];

    std::vector<glm::vec3> expected = {
        glm::vec3(0.4761772F, 0.6870308F, 0.3250369F),
        glm::vec3(0.1510580F, 0.3537409F, 0.3786762F),
        glm::vec3(0.7742273F, 0.0016869F, 0.9157501F),
        glm::vec3(0.5924380F, 0.6320426F, 0.2427963F),
        glm::vec3(0.8433697F, 0.6730490F, 0.0029323F),
        glm::vec3(0.0001751F, 0.1087111F, 0.6661169F),
        glm::vec3(0.7299188F, 0.7299188F, 0.9489649F),
        glm::vec3(0.1801442F, 0.2348952F, 0.5795466F),
    };
    checkBufferContents<glm::vec3>(buffer.cesium.data, expected);
  }

  {
    uint32_t accessorId = static_cast<uint32_t>(attributes.at("NORMAL"));
    Accessor& accessor = gltf.accessors[accessorId];

    auto bufferViewId = static_cast<uint32_t>(accessor.bufferView);
    BufferView& bufferView = gltf.bufferViews[bufferViewId];

    auto bufferId = static_cast<uint32_t>(bufferView.buffer);
    Buffer& buffer = gltf.buffers[bufferId];

    // The Draco-decoded normals are slightly different from the values
    // derived by manually decoding the uncompressed oct-encoded normals,
    // hence the less precise comparison.
    std::vector<glm::vec3> expected{
        glm::vec3(-0.9824559F, 0.1803542F, 0.0474616F),
        glm::vec3(-0.5766854F, 0.5427628F, 0.6106081F),
        glm::vec3(-0.5725988F, -0.7802446F, -0.2516918F),
        glm::vec3(-0.5705807F, -0.7345407F, 0.36727036F),
        glm::vec3(-0.8560267F, -0.1281128F, -0.5008047F),
        glm::vec3(0.7647877F, 0.11264316F, 0.63435888F),
        glm::vec3(0.1301889F, -0.23434004F, -0.9633979F),
        glm::vec3(-0.0450783F, 0.9616723F, 0.2704703F),
    };
    checkBufferContents<glm::vec3>(
        buffer.cesium.data,
        expected,
        Math::Epsilon1);
  }

  {
    uint32_t accessorId = static_cast<uint32_t>(attributes.at("_FEATURE_ID_0"));
    Accessor& accessor = gltf.accessors[accessorId];

    auto bufferViewId = static_cast<uint32_t>(accessor.bufferView);
    BufferView& bufferView = gltf.bufferViews[bufferViewId];

    auto bufferId = static_cast<uint32_t>(bufferView.buffer);
    Buffer& buffer = gltf.buffers[bufferId];

    const std::vector<uint8_t> expected = {5, 5, 6, 6, 7, 0, 3, 1};
    checkBufferContents<uint8_t>(buffer.cesium.data, expected);
  }
}
