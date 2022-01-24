// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "Cesium3DTiles/Library.h"

#include <CesiumUtility/ExtensibleObject.h>
#include <CesiumUtility/JsonValue.h>

#include <cstdint>
#include <optional>
#include <string>

namespace Cesium3DTiles {
/**
 * @brief A class property.
 */
struct CESIUM3DTILES_API Property final
    : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName = "Property";

  /**
   * @brief Known values for Element type represented by each property value.
   * `VECN` is a vector of `N` numeric components. `MATN` is an `N ⨉ N` matrix
   * of numeric components stored in column-major order. `ARRAY` is fixed-length
   * when `componentCount` is defined, and is variable-length otherwise.
   */
  struct Type {
    inline static const std::string SINGLE = "SINGLE";

    inline static const std::string VEC2 = "VEC2";

    inline static const std::string VEC3 = "VEC3";

    inline static const std::string VEC4 = "VEC4";

    inline static const std::string MAT2 = "MAT2";

    inline static const std::string MAT3 = "MAT3";

    inline static const std::string MAT4 = "MAT4";

    inline static const std::string ARRAY = "ARRAY";
  };

  /**
   * @brief Known values for Data type of an element's components. When `type`
   * is `SINGLE`, then `componentType` is also the data type of the element.
   * When `componentType` is `ENUM`, `enumType` is required.
   */
  struct ComponentType {
    inline static const std::string INT8 = "INT8";

    inline static const std::string UINT8 = "UINT8";

    inline static const std::string INT16 = "INT16";

    inline static const std::string UINT16 = "UINT16";

    inline static const std::string INT32 = "INT32";

    inline static const std::string UINT32 = "UINT32";

    inline static const std::string INT64 = "INT64";

    inline static const std::string UINT64 = "UINT64";

    inline static const std::string FLOAT32 = "FLOAT32";

    inline static const std::string FLOAT64 = "FLOAT64";

    inline static const std::string BOOLEAN = "BOOLEAN";

    inline static const std::string STRING = "STRING";

    inline static const std::string ENUM = "ENUM";
  };

  /**
   * @brief The name of the property, e.g. for display purposes.
   */
  std::optional<std::string> name;

  /**
   * @brief The description of the property.
   */
  std::optional<std::string> description;

  /**
   * @brief Element type represented by each property value. `VECN` is a vector
   * of `N` numeric components. `MATN` is an `N ⨉ N` matrix of numeric
   * components stored in column-major order. `ARRAY` is fixed-length when
   * `componentCount` is defined, and is variable-length otherwise.
   *
   * Known values are defined in {@link Type}.
   *
   */
  std::string type = Type::SINGLE;

  /**
   * @brief Enum ID as declared in the `enums` dictionary. Required when
   * `componentType` is `ENUM`.
   */
  std::optional<std::string> enumType;

  /**
   * @brief Data type of an element's components. When `type` is `SINGLE`, then
   * `componentType` is also the data type of the element. When `componentType`
   * is `ENUM`, `enumType` is required.
   *
   * Known values are defined in {@link ComponentType}.
   *
   */
  std::string componentType = ComponentType::INT8;

  /**
   * @brief Number of components per element for fixed-length `ARRAY` elements.
   * Always undefined for variable-length `ARRAY` and all other element types.
   */
  std::optional<int64_t> componentCount;

  /**
   * @brief Specifies whether integer values are normalized. This applies when
   * `componentType` is an integer type. For unsigned integer component types,
   * values are normalized between `[0.0, 1.0]`. For signed integer component
   * types, values are normalized between `[-1.0, 1.0]`. For all other component
   * types, this property must be false.
   */
  bool normalized = false;

  /**
   * @brief Maximum allowed value for the property. Only applicable for
   * single-value numeric types, fixed-length arrays of numeric types, `VECN`,
   * and `MATN` types. For single-value numeric types this is a single number.
   * For fixed-length arrays, `VECN`, and `MATN` types, this is an array of
   * component-wise maximum values. The `normalized` property has no effect on
   * the maximum, which always contains integer values.
   */
  CesiumUtility::JsonValue max;

  /**
   * @brief Minimum allowed value for the property. Only applicable for
   * single-value numeric types, fixed-length arrays of numeric types, `VECN`,
   * and `MATN` types. For single-value numeric types this is a single number.
   * For fixed-length arrays, `VECN`, and `MATN` types, this is an array of
   * component-wise minimum values. The `normalized` property has no effect on
   * the minimum, which always contains integer values.
   */
  CesiumUtility::JsonValue min;

  /**
   * @brief If required, the property must be present for every feature of its
   * class. If not required, individual features may include `noData` values, or
   * the entire property may be omitted from a property table or texture. As a
   * result, `noData` has no effect on a required property. Client
   * implementations may use required properties to make performance
   * optimizations.
   */
  bool required = false;

  /**
   * @brief A `noData` value represents missing data — also known as a sentinel
   * value — wherever it appears. If omitted (excluding variable-length `ARRAY`
   * properties), property values exist for all features, and the property is
   * required in property tables or textures instantiating the class. For
   * variable-length `ARRAY` elements, `noData` is implicitly `[]` and the
   * property is never required; an additional `noData` array, such as
   * `["UNSPECIFIED"]`, may be provided if necessary. For fixed-length `ARRAY`
   * properties, `noData` must be an array of length `componentCount`. For
   * `VECN` properties, `noData` must be an array of length `N`. For `MATN`
   * propperties, `noData` must be an array of length `N²`. `BOOLEAN` properties
   * may not specify `noData` values. `ENUM` `noData` values must use a valid
   * enum `name`, not an integer value.
   */
  CesiumUtility::JsonValue noData;

  /**
   * @brief An identifier that describes how this property should be
   * interpreted. The semantic cannot be used by other properties in the class.
   */
  std::optional<std::string> semantic;
};
} // namespace Cesium3DTiles
