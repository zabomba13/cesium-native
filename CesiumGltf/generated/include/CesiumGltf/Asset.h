// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/Library.h"
#include <CesiumUtility/ExtensibleObject.h>
#include <optional>
#include <string>

namespace CesiumGltf {
    /**
     * @brief Metadata about the glTF asset.
     */
    struct CESIUMGLTF_API Asset final : public CesiumUtility::ExtensibleObject {
        static inline constexpr const char* TypeName = "Asset";

        /**
         * @brief A copyright message suitable for display to credit the content creator.
         */
        std::optional<std::string> copyright;

        /**
         * @brief Tool that generated this glTF model.  Useful for debugging.
         */
        std::optional<std::string> generator;

        /**
         * @brief The glTF version in the form of `<major>.<minor>` that this asset targets.
         */
        std::string version;

        /**
         * @brief The minimum glTF version in the form of `<major>.<minor>` that this asset targets. This property **MUST NOT** be greater than the asset version.
         */
        std::optional<std::string> minVersion;

    };
}
