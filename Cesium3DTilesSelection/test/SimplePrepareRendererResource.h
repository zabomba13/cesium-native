#pragma once

#include "Cesium3DTilesSelection/IPrepareRendererResources.h"
#include "Cesium3DTilesSelection/Tile.h"
#include "CesiumRasterOverlays/RasterOverlayTile.h"

#include <catch2/catch_test_macros.hpp>

#include <atomic>

namespace Cesium3DTilesSelection {
class SimplePrepareRendererResource
    : public Cesium3DTilesSelection::IPrepareRendererResources {
public:
  class AllocationResult {
  public:
    explicit AllocationResult(std::atomic<size_t>& allocCount)
        : _allocCount(allocCount) {
      ++_allocCount;
    }

    ~AllocationResult() noexcept { --_allocCount; }

    AllocationResult(const AllocationResult& other) = default;
    AllocationResult& operator=(const AllocationResult& other) = delete;
    AllocationResult(AllocationResult&& other) noexcept = default;
    AllocationResult& operator=(AllocationResult&& other) noexcept = delete;

  private:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
    std::atomic<size_t>& _allocCount;
  };

  ~SimplePrepareRendererResource() noexcept override {
    CHECK(_totalAllocation == 0);
  }

  SimplePrepareRendererResource(const SimplePrepareRendererResource& other) =
      delete;

  SimplePrepareRendererResource&
  operator=(const SimplePrepareRendererResource& other) = delete;

  SimplePrepareRendererResource(
      SimplePrepareRendererResource&& other) noexcept = delete;
  SimplePrepareRendererResource&

  operator=(SimplePrepareRendererResource&& other) noexcept = delete;

  CesiumAsync::Future<TileLoadResultAndRenderResources> prepareInLoadThread(
      const CesiumAsync::AsyncSystem& asyncSystem,
      TileLoadResult&& tileLoadResult,
      const glm::dmat4& /*transform*/,
      const std::any& /*rendererOptions*/) override {
    return asyncSystem.createResolvedFuture(TileLoadResultAndRenderResources{
        std::move(tileLoadResult),
        new AllocationResult(_totalAllocation)});
  }

  void* prepareInMainThread(
      Cesium3DTilesSelection::Tile& /*tile*/,
      void* pLoadThreadResult) override {
    if (pLoadThreadResult) {
      auto* loadThreadResult =
          reinterpret_cast<AllocationResult*>(pLoadThreadResult);
      // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
      delete loadThreadResult;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    return new AllocationResult{_totalAllocation};
  }

  void free(
      Cesium3DTilesSelection::Tile& /*tile*/,
      void* pLoadThreadResult,
      void* pMainThreadResult) noexcept override {
    if (pMainThreadResult) {
      auto* mainThreadResult =
          reinterpret_cast<AllocationResult*>(pMainThreadResult);
      // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
      delete mainThreadResult;
    }

    if (pLoadThreadResult) {
      auto* loadThreadResult =
          reinterpret_cast<AllocationResult*>(pLoadThreadResult);
      // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
      delete loadThreadResult;
    }
  }

  void* prepareRasterInLoadThread(
      CesiumGltf::ImageCesium& /*image*/,
      const std::any& /*rendererOptions*/) override {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory
    return new AllocationResult{_totalAllocation};
  }

  void* prepareRasterInMainThread(
      CesiumRasterOverlays::RasterOverlayTile& /*rasterTile*/,
      void* pLoadThreadResult) override {
    if (pLoadThreadResult) {
      auto* loadThreadResult =
          reinterpret_cast<AllocationResult*>(pLoadThreadResult);
      // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
      delete loadThreadResult;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    return new AllocationResult(_totalAllocation);
  }

  void freeRaster(
      const CesiumRasterOverlays::RasterOverlayTile& /*rasterTile*/,
      void* pLoadThreadResult,
      void* pMainThreadResult) noexcept override {
    if (pMainThreadResult) {
      auto* mainThreadResult =
          reinterpret_cast<AllocationResult*>(pMainThreadResult);
      // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
      delete mainThreadResult;
    }

    if (pLoadThreadResult) {
      auto* loadThreadResult =
          reinterpret_cast<AllocationResult*>(pLoadThreadResult);
      // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
      delete loadThreadResult;
    }
  }

  void attachRasterInMainThread(
      const Cesium3DTilesSelection::Tile& /*tile*/,
      int32_t /*overlayTextureCoordinateID*/,
      const CesiumRasterOverlays::RasterOverlayTile& /*rasterTile*/,
      void* /*pMainThreadRendererResources*/,
      const glm::dvec2& /*translation*/,
      const glm::dvec2& /*scale*/) override {}

  void detachRasterInMainThread(
      const Cesium3DTilesSelection::Tile& /*tile*/,
      int32_t /*overlayTextureCoordinateID*/,
      const CesiumRasterOverlays::RasterOverlayTile& /*rasterTile*/,
      void* /*pMainThreadRendererResources*/) noexcept override {}

private:
  std::atomic<size_t> _totalAllocation;
};
} // namespace Cesium3DTilesSelection
