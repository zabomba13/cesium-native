// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "Cesium3DTiles/Library.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <cstdint>
#include <optional>
#include <string>

namespace Cesium3DTiles {
/**
 * @brief A contiguous subset of a buffer
 */
struct CESIUM3DTILES_API BufferView final
    : public CesiumUtility::ExtensibleObject {
  static constexpr const char* TypeName = "BufferView";

  /**
   * @brief The index of the buffer.
   */
  int64_t buffer = int64_t();

  /**
   * @brief The offset into the buffer in bytes.
   */
  int64_t byteOffset = int64_t();

  /**
   * @brief The total byte length of the buffer view.
   */
  int64_t byteLength = int64_t();

  /**
   * @brief The name of the `bufferView`.
   */
  std::optional<std::string> name;
};
} // namespace Cesium3DTiles
