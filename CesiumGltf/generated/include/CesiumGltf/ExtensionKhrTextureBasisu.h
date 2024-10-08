// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/Library.h"
#include <CesiumUtility/ExtensibleObject.h>
#include <cstdint>

namespace CesiumGltf {
    /**
     * @brief glTF extension to specify textures using the KTX v2 images with Basis Universal supercompression.
     */
    struct CESIUMGLTF_API ExtensionKhrTextureBasisu final : public CesiumUtility::ExtensibleObject {
        static inline constexpr const char* TypeName = "ExtensionKhrTextureBasisu";
        static inline constexpr const char* ExtensionName = "KHR_texture_basisu";

        /**
         * @brief The index of the image which points to a KTX v2 resource with Basis Universal supercompression.
         */
        int32_t source = -1;

    };
}
