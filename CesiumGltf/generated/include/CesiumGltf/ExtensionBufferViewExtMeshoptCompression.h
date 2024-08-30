// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include <CesiumGltf/Library.h>
#include <CesiumUtility/ExtensibleObject.h>

#include <cstdint>
#include <string>

namespace CesiumGltf {
/**
 * @brief Compressed data for bufferView.
 */
struct CESIUMGLTF_API ExtensionBufferViewExtMeshoptCompression final
    : public CesiumUtility::ExtensibleObject {
  static constexpr const char* TypeName =
      "ExtensionBufferViewExtMeshoptCompression";
  static constexpr const char* ExtensionName = "EXT_meshopt_compression";

  /**
   * @brief Known values for The compression mode.
   */
  struct Mode {
    inline static const std::string ATTRIBUTES = "ATTRIBUTES";

    inline static const std::string TRIANGLES = "TRIANGLES";

    inline static const std::string INDICES = "INDICES";
  };

  /**
   * @brief Known values for The compression filter.
   */
  struct Filter {
    inline static const std::string NONE = "NONE";

    inline static const std::string OCTAHEDRAL = "OCTAHEDRAL";

    inline static const std::string QUATERNION = "QUATERNION";

    inline static const std::string EXPONENTIAL = "EXPONENTIAL";
  };

  /**
   * @brief The index of the buffer with compressed data.
   */
  int32_t buffer = -1;

  /**
   * @brief The offset into the buffer in bytes.
   */
  int64_t byteOffset = 0;

  /**
   * @brief The length of the compressed data in bytes.
   */
  int64_t byteLength = int64_t();

  /**
   * @brief The stride, in bytes.
   */
  int64_t byteStride = int64_t();

  /**
   * @brief The number of elements.
   */
  int64_t count = int64_t();

  /**
   * @brief The compression mode.
   *
   * Known values are defined in {@link Mode}.
   *
   */
  std::string mode = Mode::ATTRIBUTES;

  /**
   * @brief The compression filter.
   *
   * Known values are defined in {@link Filter}.
   *
   */
  std::string filter = Filter::NONE;
};
} // namespace CesiumGltf
