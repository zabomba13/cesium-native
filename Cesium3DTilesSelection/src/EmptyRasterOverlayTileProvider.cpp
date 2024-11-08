#include "EmptyRasterOverlayTileProvider.h"

using namespace CesiumRasterOverlays;

namespace Cesium3DTilesSelection {

EmptyRasterOverlayTileProvider::EmptyRasterOverlayTileProvider(
    const CesiumUtility::IntrusivePointer<const RasterOverlay>& pOwner,
    const CesiumAsync::AsyncSystem& asyncSystem) noexcept
    : RasterOverlayTileProvider(
          pOwner,
          asyncSystem,
          nullptr,
          std::nullopt,
          nullptr,
          nullptr,
          CesiumGeospatial::GeographicProjection(),
          CesiumGeometry::Rectangle()) {}

CesiumAsync::Future<CesiumUtility::ResultPointer<
    CesiumRasterOverlays::LoadedRasterOverlayImage>>
EmptyRasterOverlayTileProvider::loadTileImage(
    CesiumRasterOverlays::RasterOverlayTile& /*overlayTile*/) {
  return this->getAsyncSystem().createResolvedFuture(
      CesiumUtility::ResultPointer<LoadedRasterOverlayImage>{nullptr});
}

} // namespace Cesium3DTilesSelection
