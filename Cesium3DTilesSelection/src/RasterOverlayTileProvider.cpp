#include "Cesium3DTilesSelection/RasterOverlayTileProvider.h"

#include "Cesium3DTilesSelection/IPrepareRendererResources.h"
#include "Cesium3DTilesSelection/RasterMappedTo3DTile.h"
#include "Cesium3DTilesSelection/RasterOverlay.h"
#include "Cesium3DTilesSelection/RasterOverlayTile.h"
#include "Cesium3DTilesSelection/TileID.h"
#include "Cesium3DTilesSelection/TilesetExternals.h"
#include "Cesium3DTilesSelection/spdlog-cesium.h"

#include <CesiumAsync/IAssetResponse.h>
#include <CesiumGltfReader/GltfReader.h>
#include <CesiumUtility/Tracing.h>
#include <CesiumUtility/joinToString.h>

using namespace CesiumAsync;
using namespace CesiumGeometry;
using namespace CesiumGeospatial;
using namespace CesiumGltf;
using namespace CesiumGltfReader;
using namespace CesiumUtility;

namespace Cesium3DTilesSelection {

/*static*/ CesiumGltfReader::GltfReader
    RasterOverlayTileProvider::_gltfReader{};

RasterOverlayTileProvider::RasterOverlayTileProvider(
    const CesiumUtility::IntrusivePointer<const RasterOverlay>& pOwner,
    const CesiumAsync::AsyncSystem& asyncSystem,
    const std::shared_ptr<IAssetAccessor>& pAssetAccessor) noexcept
    : _pOwner(const_intrusive_cast<RasterOverlay>(pOwner)),
      _asyncSystem(asyncSystem),
      _pAssetAccessor(pAssetAccessor),
      _credit(std::nullopt),
      _pPrepareRendererResources(nullptr),
      _pLogger(nullptr),
      _projection(CesiumGeospatial::GeographicProjection()),
      _coverageRectangle(CesiumGeospatial::GeographicProjection::
                             computeMaximumProjectedRectangle()),
      _pPlaceholder(),
      _tileDataBytes(0),
      _totalTilesCurrentlyLoading(0),
      _throttledTilesCurrentlyLoading(0) {
  this->_pPlaceholder = new RasterOverlayTile(*this);
}

RasterOverlayTileProvider::RasterOverlayTileProvider(
    const CesiumUtility::IntrusivePointer<const RasterOverlay>& pOwner,
    const CesiumAsync::AsyncSystem& asyncSystem,
    const std::shared_ptr<IAssetAccessor>& pAssetAccessor,
    std::optional<Credit> credit,
    const std::shared_ptr<IPrepareRendererResources>& pPrepareRendererResources,
    const std::shared_ptr<spdlog::logger>& pLogger,
    const CesiumGeospatial::Projection& projection,
    const Rectangle& coverageRectangle) noexcept
    : _pOwner(const_intrusive_cast<RasterOverlay>(pOwner)),
      _asyncSystem(asyncSystem),
      _pAssetAccessor(pAssetAccessor),
      _credit(credit),
      _pPrepareRendererResources(pPrepareRendererResources),
      _pLogger(pLogger),
      _projection(projection),
      _coverageRectangle(coverageRectangle),
      _pPlaceholder(nullptr),
      _tileDataBytes(0),
      _totalTilesCurrentlyLoading(0),
      _throttledTilesCurrentlyLoading(0) {}

RasterOverlayTileProvider::~RasterOverlayTileProvider() noexcept {
  // Explicitly release the placeholder first, because RasterOverlayTiles must
  // be destroyed before the tile provider that created them.
  if (this->_pPlaceholder) {
    assert(this->_pPlaceholder->getReferenceCount() == 1);
    this->_pPlaceholder = nullptr;
  }
}

CesiumUtility::IntrusivePointer<RasterOverlayTile>
RasterOverlayTileProvider::getTile(
    const CesiumGeometry::Rectangle& rectangle,
    const glm::dvec2& targetScreenPixels) {
  if (this->_pPlaceholder) {
    return this->_pPlaceholder;
  }

  if (!rectangle.overlaps(this->_coverageRectangle)) {
    return nullptr;
  }

  return new RasterOverlayTile(*this, targetScreenPixels, rectangle);
}

void RasterOverlayTileProvider::removeTile(RasterOverlayTile* pTile) noexcept {
  assert(pTile->getReferenceCount() == 0);

  this->_tileDataBytes -= int64_t(pTile->getImage().pixelData.size());
}

void RasterOverlayTileProvider::loadTile(RasterOverlayTile& tile) {
  if (this->_pPlaceholder) {
    // Refuse to load placeholders.
    return;
  }

  // Already loading or loaded, do nothing.
  if (tile.getState() != RasterLoadState::Unloaded)
    return;

  // Don't let this tile be destroyed while it's loading.
  tile.setState(RasterLoadState::Loading);

  // TODO, this needs a real callback and data passed in
  UrlResponseDataMap responsesByUrl;
  this->doLoad(tile, false, responsesByUrl, nullptr);
}

CesiumAsync::Future<RasterLoadResult>
RasterOverlayTileProvider::loadTileThrottled(
    RasterOverlayTile& tile,
    const UrlResponseDataMap& responsesByUrl,
    RasterProcessingCallback rasterCallback) {
  return this->doLoad(tile, true, responsesByUrl, rasterCallback);
}

void RasterOverlayTileProvider::getLoadTileThrottledWork(
    RasterOverlayTile& tile,
    RequestData& outRequest,
    RasterProcessingCallback& outCallback) {
  if (tile.getState() != RasterLoadState::Unloaded)
    return;

  getLoadTileImageWork(tile, outRequest, outCallback);
}

CesiumAsync::Future<RasterLoadResult>
RasterOverlayTileProvider::loadTileImageFromUrl(
    const std::string& url,
    uint16_t statusCode,
    const gsl::span<const std::byte>& data,
    LoadTileImageFromUrlOptions&& options) const {

  return this->getAsyncSystem().runInWorkerThread(
      [options = std::move(options),
       url = url,
       statusCode = statusCode,
       data = data,
       asyncSystem = this->getAsyncSystem(),
       Ktx2TranscodeTargets =
           this->getOwner().getOptions().ktx2TranscodeTargets]() mutable {
        CESIUM_TRACE("load image");

        if (statusCode != 0 && statusCode < 200 || statusCode >= 300) {
          std::string message = "Image response code " +
                                std::to_string(statusCode) + " for " + url;
          return asyncSystem.createResolvedFuture<RasterLoadResult>(
              RasterLoadResult{
                  std::nullopt,
                  options.rectangle,
                  std::move(options.credits),
                  {message},
                  {},
                  options.moreDetailAvailable});
        }

        if (data.empty()) {
          if (options.allowEmptyImages) {
            return asyncSystem.createResolvedFuture<RasterLoadResult>(
                RasterLoadResult{
                    CesiumGltf::ImageCesium(),
                    options.rectangle,
                    std::move(options.credits),
                    {},
                    {},
                    options.moreDetailAvailable});
          }
          return asyncSystem.createResolvedFuture<RasterLoadResult>(
              RasterLoadResult{
                  std::nullopt,
                  options.rectangle,
                  std::move(options.credits),
                  {"Image response for " + url + " is empty."},
                  {},
                  options.moreDetailAvailable});
        }

        CesiumGltfReader::ImageReaderResult loadedImage =
            RasterOverlayTileProvider::_gltfReader.readImage(
                data,
                Ktx2TranscodeTargets);

        if (!loadedImage.errors.empty()) {
          loadedImage.errors.push_back("Image url: " + url);
        }
        if (!loadedImage.warnings.empty()) {
          loadedImage.warnings.push_back("Image url: " + url);
        }

        return asyncSystem.createResolvedFuture<RasterLoadResult>(
            RasterLoadResult{
                loadedImage.image,
                options.rectangle,
                std::move(options.credits),
                std::move(loadedImage.errors),
                std::move(loadedImage.warnings),
                options.moreDetailAvailable});
      });
}

namespace {

/**
 * @brief Processes the given `RasterLoadResult`
 *
 * This function is intended to be called on the worker thread.
 *
 * If the given `loadedImage` contains no valid image data, then a
 * `RasterLoadResult` with the state `RasterLoadState::Failed` will
 * be returned.
 *
 * Otherwise, the image data will be passed to
 * `IPrepareRendererResources::prepareRasterInLoadThread`, and the function
 * will return a `RasterLoadResult` with the image, the prepared renderer
 * resources, and the state `RasterLoadState::Loaded`.
 *
 * @param tileId The {@link TileID} - only used for logging
 * @param pPrepareRendererResources The `IPrepareRendererResources`
 * @param pLogger The logger
 * @param loadedImage The `LoadedRasterOverlayImage`
 * @param rendererOptions Renderer options
 * @return The `RasterLoadResult`
 */
static RasterLoadResult prepareLoadResultImage(
    const std::shared_ptr<IPrepareRendererResources>& pPrepareRendererResources,
    const std::shared_ptr<spdlog::logger>& pLogger,
    RasterLoadResult&& loadResult,
    const std::any& rendererOptions) {

  if (!loadResult.requestData.url.empty()) {
    // A url was requested, don't need to do anything
    return std::move(loadResult);
  }

  if (!loadResult.image.has_value()) {
    SPDLOG_LOGGER_ERROR(
        pLogger,
        "Failed to load image for tile {}:\n- {}",
        "TODO",
        // Cesium3DTilesSelection::TileIdUtilities::createTileIdString(tileId),
        CesiumUtility::joinToString(loadResult.errors, "\n- "));
    loadResult.state = RasterLoadState::Failed;
    return std::move(loadResult);
  }

  if (!loadResult.warnings.empty()) {
    SPDLOG_LOGGER_WARN(
        pLogger,
        "Warnings while loading image for tile {}:\n- {}",
        "TODO",
        // Cesium3DTilesSelection::TileIdUtilities::createTileIdString(tileId),
        CesiumUtility::joinToString(loadResult.warnings, "\n- "));
  }

  CesiumGltf::ImageCesium& image = loadResult.image.value();

  const int32_t bytesPerPixel = image.channels * image.bytesPerChannel;
  const int64_t requiredBytes =
      static_cast<int64_t>(image.width) * image.height * bytesPerPixel;
  if (image.width > 0 && image.height > 0 &&
      image.pixelData.size() >= static_cast<size_t>(requiredBytes)) {
    CESIUM_TRACE(
        "Prepare Raster " + std::to_string(image.width) + "x" +
        std::to_string(image.height) + "x" + std::to_string(image.channels) +
        "x" + std::to_string(image.bytesPerChannel));

    void* pRendererResources = nullptr;
    if (pPrepareRendererResources) {
      pRendererResources = pPrepareRendererResources->prepareRasterInLoadThread(
          image,
          rendererOptions);
    }

    loadResult.state = RasterLoadState::Loaded;
    loadResult.pRendererResources = pRendererResources;

    return std::move(loadResult);
  }

  loadResult.pRendererResources = nullptr;
  loadResult.state = RasterLoadState::Failed;
  loadResult.moreDetailAvailable = false;

  return std::move(loadResult);
}

} // namespace

CesiumAsync::Future<RasterLoadResult> RasterOverlayTileProvider::doLoad(
    RasterOverlayTile& tile,
    bool isThrottledLoad,
    const UrlResponseDataMap& responsesByUrl,
    RasterProcessingCallback rasterCallback) {
  // CESIUM_TRACE_USE_TRACK_SET(this->_loadingSlots);

  this->beginTileLoad(isThrottledLoad);

  // Keep the tile and tile provider alive while the async operation is in
  // progress.
  IntrusivePointer<RasterOverlayTile> pTile = &tile;
  IntrusivePointer<RasterOverlayTileProvider> thiz = this;

  assert(rasterCallback);

  return rasterCallback(tile, this, responsesByUrl)
      .thenInWorkerThread(
          [pPrepareRendererResources = this->getPrepareRendererResources(),
           pLogger = this->getLogger(),
           rendererOptions = this->_pOwner->getOptions().rendererOptions](
              RasterLoadResult&& loadResult) {
            return prepareLoadResultImage(
                pPrepareRendererResources,
                pLogger,
                std::move(loadResult),
                rendererOptions);
          })
      .thenInMainThread(
          [thiz, pTile, isThrottledLoad](RasterLoadResult&& result) noexcept {
            if (result.state == RasterLoadState::RequestRequired)
              return thiz->_asyncSystem.createResolvedFuture<RasterLoadResult>(
                  std::move(result));

            pTile->_rectangle = result.rectangle;
            pTile->_pRendererResources = result.pRendererResources;
            assert(result.image.has_value());
            pTile->_image = std::move(result.image.value());
            pTile->_tileCredits = std::move(result.credits);
            pTile->_moreDetailAvailable =
                result.moreDetailAvailable
                    ? RasterOverlayTile::MoreDetailAvailable::Yes
                    : RasterOverlayTile::MoreDetailAvailable::No;
            pTile->setState(result.state);

            thiz->_tileDataBytes += int64_t(pTile->getImage().pixelData.size());

            thiz->finalizeTileLoad(isThrottledLoad);
            return thiz->_asyncSystem.createResolvedFuture<RasterLoadResult>(
                std::move(result));
          })
      .catchInMainThread(
          [thiz, pTile, isThrottledLoad](const std::exception& /*e*/) {
            pTile->_pRendererResources = nullptr;
            pTile->_image = {};
            pTile->_tileCredits = {};
            pTile->_moreDetailAvailable =
                RasterOverlayTile::MoreDetailAvailable::No;
            pTile->setState(RasterLoadState::Failed);

            thiz->finalizeTileLoad(isThrottledLoad);

            RasterLoadResult result;
            result.state = RasterLoadState::Failed;

            return thiz->_asyncSystem.createResolvedFuture<RasterLoadResult>(
                std::move(result));
          });
}

void RasterOverlayTileProvider::beginTileLoad(bool isThrottledLoad) noexcept {
  ++this->_totalTilesCurrentlyLoading;
  if (isThrottledLoad) {
    ++this->_throttledTilesCurrentlyLoading;
  }
}

void RasterOverlayTileProvider::finalizeTileLoad(
    bool isThrottledLoad) noexcept {
  --this->_totalTilesCurrentlyLoading;
  if (isThrottledLoad) {
    --this->_throttledTilesCurrentlyLoading;
  }
}
} // namespace Cesium3DTilesSelection
