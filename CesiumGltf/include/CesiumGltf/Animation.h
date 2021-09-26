// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/AnimationChannel.h"
#include "CesiumGltf/AnimationSampler.h"
#include "CesiumGltf/Library.h"
#include "CesiumGltf/NamedObject.h"
#include <vector>

namespace CesiumGltf {
/**
 * @brief A keyframe animation.
 */
struct CESIUMGLTF_API Animation final : public NamedObject {
  static inline constexpr const char* TypeName = "Animation";

  /**
   * @brief An array of animation channels, each of which targets an animation's
   * sampler at a node's property. Different channels of the same animation
   * **MUST NOT** have the same targets.
   */
  std::vector<AnimationChannel> channels;

  /**
   * @brief An array of animation samplers that combines input and output
   * accessors with an interpolation algorithm to define a keyframe graph (but
   * not its target).
   */
  std::vector<AnimationSampler> samplers;
};
} // namespace CesiumGltf
