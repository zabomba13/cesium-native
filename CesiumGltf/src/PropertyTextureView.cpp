#include "CesiumGltf/PropertyTextureView.h"

#include "CesiumGltf/ClassProperty.h"
#include "CesiumGltf/ExtensionModelExtStructuralMetadata.h"
#include "CesiumGltf/ImageCesium.h"
#include "CesiumGltf/Model.h"
#include "CesiumGltf/PropertyTexture.h"
#include "CesiumGltf/PropertyTexturePropertyView.h"
#include "CesiumGltf/PropertyView.h"
#include "CesiumGltf/Texture.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace CesiumGltf {
PropertyTextureView::PropertyTextureView(
    const Model& model,
    const PropertyTexture& propertyTexture) noexcept
    : _pModel(&model),
      _pPropertyTexture(&propertyTexture),
      _pClass(nullptr),
      _status() {
  const auto* pMetadata =
      model.getExtension<ExtensionModelExtStructuralMetadata>();

  if (!pMetadata) {
    this->_status = PropertyTextureViewStatus::ErrorMissingMetadataExtension;
    return;
  }

  if (!pMetadata->schema) {
    this->_status = PropertyTextureViewStatus::ErrorMissingSchema;
    return;
  }

  const auto& classIt =
      pMetadata->schema->classes.find(propertyTexture.classProperty);
  if (classIt == pMetadata->schema->classes.end()) {
    this->_status = PropertyTextureViewStatus::ErrorClassNotFound;
    return;
  }

  this->_pClass = &classIt->second;
}

const ClassProperty*
PropertyTextureView::getClassProperty(const std::string& propertyId) const {
  if (_status != PropertyTextureViewStatus::Valid) {
    return nullptr;
  }

  auto propertyIter = _pClass->properties.find(propertyId);
  if (propertyIter == _pClass->properties.end()) {
    return nullptr;
  }

  return &propertyIter->second;
}

PropertyViewStatusType PropertyTextureView::getTextureSafe(
    const int32_t textureIndex,
    int32_t& samplerIndex,
    int32_t& imageIndex) const noexcept {
  if (textureIndex < 0 ||
      static_cast<size_t>(textureIndex) >= _pModel->textures.size()) {
    return PropertyTexturePropertyViewStatus::ErrorInvalidTexture;
  }

  const Texture& texture = _pModel->textures[static_cast<size_t>(textureIndex)];
  samplerIndex = texture.sampler;
  imageIndex = texture.source;

  return PropertyTexturePropertyViewStatus::Valid;
}

PropertyViewStatusType
PropertyTextureView::checkSampler(const int32_t samplerIndex) const noexcept {
  if (samplerIndex < 0 ||
      static_cast<size_t>(samplerIndex) >= _pModel->samplers.size()) {
    return PropertyTexturePropertyViewStatus::ErrorInvalidSampler;
  }

  // TODO: check if sampler filter values are supported

  return PropertyTexturePropertyViewStatus::Valid;
}

PropertyViewStatusType
PropertyTextureView::checkImage(const int32_t imageIndex) const noexcept {
  if (imageIndex < 0 ||
      static_cast<size_t>(imageIndex) >= _pModel->images.size()) {
    return PropertyTexturePropertyViewStatus::ErrorInvalidImage;
  }

  const ImageCesium& image =
      _pModel->images[static_cast<size_t>(imageIndex)].cesium;

  if (image.width < 1 || image.height < 1) {
    return PropertyTexturePropertyViewStatus::ErrorEmptyImage;
  }

  if (image.bytesPerChannel > 1) {
    return PropertyTexturePropertyViewStatus::ErrorInvalidBytesPerChannel;
  }

  return PropertyTexturePropertyViewStatus::Valid;
}

/*static*/ PropertyViewStatusType PropertyTextureView::checkChannels(
    const std::vector<int64_t>& channels,
    const ImageCesium& image) noexcept {
  if (channels.empty() || channels.size() > 4) {
    return PropertyTexturePropertyViewStatus::ErrorInvalidChannels;
  }

  auto imageChannelCount = static_cast<int64_t>(image.channels);
  for (int64_t channel : channels) {
    if (channel < 0 || channel >= imageChannelCount) {
      return PropertyTexturePropertyViewStatus::ErrorInvalidChannels;
    }
  }

  return PropertyTexturePropertyViewStatus::Valid;
}

} // namespace CesiumGltf
