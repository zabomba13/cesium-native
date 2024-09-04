#pragma once

#include "GltfConverterResult.h"

#include <CesiumAsync/Future.h>
#include <CesiumGltf/Model.h>
#include <CesiumGltfReader/GltfReader.h>

#include <gsl/span>

namespace Cesium3DTilesContent {
struct AssetFetcher;

struct I3dmToGltfConverter {
  static CesiumAsync::Future<GltfConverterResult> convert(
      const gsl::span<const std::byte>& instancesBinary,
      const CesiumGltfReader::GltfReaderOptions& options,
      const AssetFetcher& assetFetcher);
};
} // namespace Cesium3DTilesContent
