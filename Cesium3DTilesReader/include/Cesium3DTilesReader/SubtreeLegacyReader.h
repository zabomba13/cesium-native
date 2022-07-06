#pragma once

#include "Cesium3DTilesReader/Library.h"

#include <Cesium3DTiles/Extension3dTilesImplicitTilingSubtreeLegacy.h>
#include <CesiumJsonReader/ExtensionReaderContext.h>

#include <gsl/span>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Cesium3DTilesReader {

/**
 * @brief The result of reading a subtree with
 * {@link SubtreeReaderLegacy::readSubtree}.
 */
struct CESIUM3DTILESREADER_API SubtreeReaderResultLegacy {
  /**
   * @brief The read subtree, or std::nullopt if the subtree could not be read.
   */
  std::optional<Cesium3DTiles::Extension3dTilesImplicitTilingSubtreeLegacy> subtree;

  /**
   * @brief Errors, if any, that occurred during the load process.
   */
  std::vector<std::string> errors;

  /**
   * @brief Warnings, if any, that occurred during the load process.
   */
  std::vector<std::string> warnings;
};

/**
 * @brief Reads subtrees.
 */
class CESIUM3DTILESREADER_API SubtreeReaderLegacy {
public:
  /**
   * @brief Constructs a new instance.
   */
  SubtreeReaderLegacy();

  /**
   * @brief Gets the context used to control how extensions are loaded from a
   * subtree.
   */
  CesiumJsonReader::ExtensionReaderContext& getExtensions();

  /**
   * @brief Gets the context used to control how extensions are loaded from a
   * subtree.
   */
  const CesiumJsonReader::ExtensionReaderContext& getExtensions() const;

  /**
   * @brief Reads a subtree.
   *
   * @param data The buffer from which to read the subtree.
   * @param options Options for how to read the subtree.
   * @return The result of reading the subtree.
   */
  SubtreeReaderResultLegacy readSubtree(const gsl::span<const std::byte>& data) const;

private:
  CesiumJsonReader::ExtensionReaderContext _context;
};

} // namespace Cesium3DTilesReader
