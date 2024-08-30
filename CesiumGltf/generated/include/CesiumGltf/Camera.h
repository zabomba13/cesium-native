// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/CameraOrthographic.h"
#include "CesiumGltf/CameraPerspective.h"
#include "CesiumGltf/Library.h"
#include "CesiumGltf/NamedObject.h"

#include <optional>
#include <string>

namespace CesiumGltf {
/**
 * @brief A camera's projection.  A node **MAY** reference a camera to apply a
 * transform to place the camera in the scene.
 */
struct CESIUMGLTF_API Camera final : public CesiumGltf::NamedObject {
  static constexpr const char* TypeName = "Camera";

  /**
   * @brief Known values for Specifies if the camera uses a perspective or
   * orthographic projection.
   */
  struct Type {
    inline static const std::string perspective = "perspective";

    inline static const std::string orthographic = "orthographic";
  };

  /**
   * @brief An orthographic camera containing properties to create an
   * orthographic projection matrix. This property **MUST NOT** be defined when
   * `perspective` is defined.
   */
  std::optional<CesiumGltf::CameraOrthographic> orthographic;

  /**
   * @brief A perspective camera containing properties to create a perspective
   * projection matrix. This property **MUST NOT** be defined when
   * `orthographic` is defined.
   */
  std::optional<CesiumGltf::CameraPerspective> perspective;

  /**
   * @brief Specifies if the camera uses a perspective or orthographic
   * projection.
   *
   * Known values are defined in {@link Type}.
   *
   *
   * Based on this, either the camera's `perspective` or `orthographic` property
   * **MUST** be defined.
   */
  std::string type = Type::perspective;
};
} // namespace CesiumGltf
