// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/AnimationChannelTarget.h"
#include "CesiumGltf/Library.h"
#include <CesiumUtility/ExtensibleObject.h>
#include <cstdint>

namespace CesiumGltf {
    /**
     * @brief An animation channel combines an animation sampler with a target property being animated.
     */
    struct CESIUMGLTF_API AnimationChannel final : public CesiumUtility::ExtensibleObject {
        static inline constexpr const char* TypeName = "AnimationChannel";

        /**
         * @brief The index of a sampler in this animation used to compute the value for the target.
         *
         * The index of a sampler in this animation used to compute the value for the target, e.g., a node's translation, rotation, or scale (TRS).
         */
        int32_t sampler = -1;

        /**
         * @brief The descriptor of the animated property.
         */
        CesiumGltf::AnimationChannelTarget target;

    };
}
