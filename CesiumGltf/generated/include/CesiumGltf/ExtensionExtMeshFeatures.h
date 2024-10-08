// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/FeatureId.h"
#include "CesiumGltf/Library.h"
#include <CesiumUtility/ExtensibleObject.h>
#include <vector>

namespace CesiumGltf {
    /**
     * @brief An object describing feature IDs for a mesh primitive.
     */
    struct CESIUMGLTF_API ExtensionExtMeshFeatures final : public CesiumUtility::ExtensibleObject {
        static inline constexpr const char* TypeName = "ExtensionExtMeshFeatures";
        static inline constexpr const char* ExtensionName = "EXT_mesh_features";

        /**
         * @brief An array of feature ID sets.
         */
        std::vector<CesiumGltf::FeatureId> featureIds;

    };
}
