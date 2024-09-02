#include "CesiumGltf/PropertyTableView.h"

#include "CesiumGltf/Buffer.h"
#include "CesiumGltf/BufferView.h"
#include "CesiumGltf/ClassProperty.h"
#include "CesiumGltf/ExtensionModelExtStructuralMetadata.h"
#include "CesiumGltf/Model.h"
#include "CesiumGltf/PropertyArrayView.h"
#include "CesiumGltf/PropertyTable.h"
#include "CesiumGltf/PropertyTableProperty.h"
#include "CesiumGltf/PropertyTablePropertyView.h"
#include "CesiumGltf/PropertyType.h"
#include "CesiumGltf/PropertyView.h"
#include "CesiumGltf/Schema.h"

#include <glm/common.hpp>
#include <gsl/span>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace CesiumGltf {
template <typename T>
static PropertyViewStatusType checkOffsetsBuffer(
    const gsl::span<const std::byte>& offsetBuffer,
    size_t valuesBufferSize,
    size_t instanceCount,
    bool checkBitSize,
    PropertyViewStatusType offsetsNotSortedError,
    PropertyViewStatusType offsetOutOfBoundsError) noexcept {
  if (offsetBuffer.size() % sizeof(T) != 0) {
    return PropertyTablePropertyViewStatus::
        ErrorBufferViewSizeNotDivisibleByTypeSize;
  }

  const size_t size = offsetBuffer.size() / sizeof(T);
  if (size != instanceCount + 1) {
    return PropertyTablePropertyViewStatus::
        ErrorBufferViewSizeDoesNotMatchPropertyTableCount;
  }

  const gsl::span<const T> offsetValues(
      reinterpret_cast<const T*>(offsetBuffer.data()),
      size);

  for (size_t i = 1; i < offsetValues.size(); ++i) {
    if (offsetValues[i] < offsetValues[i - 1]) {
      return offsetsNotSortedError;
    }
  }

  if (checkBitSize) {
    if (offsetValues.back() / 8 <= valuesBufferSize) {
      return PropertyTablePropertyViewStatus::Valid;
    }

    return offsetOutOfBoundsError;
  }

  if (offsetValues.back() <= valuesBufferSize) {
    return PropertyTablePropertyViewStatus::Valid;
  }

  return offsetOutOfBoundsError;
}

template <typename T>
static PropertyViewStatusType checkStringAndArrayOffsetsBuffers(
    const gsl::span<const std::byte>& arrayOffsets,
    const gsl::span<const std::byte>& stringOffsets,
    size_t valuesBufferSize,
    PropertyComponentType stringOffsetType,
    size_t propertyTableCount) noexcept {
  const auto status = checkOffsetsBuffer<T>(
      arrayOffsets,
      stringOffsets.size(),
      propertyTableCount,
      false,
      PropertyTablePropertyViewStatus::ErrorArrayOffsetsNotSorted,
      PropertyTablePropertyViewStatus::ErrorArrayOffsetOutOfBounds);

  if (status != PropertyTablePropertyViewStatus::Valid) {
    return status;
  }

  const T* pValue = reinterpret_cast<const T*>(arrayOffsets.data());

  switch (stringOffsetType) {
  case PropertyComponentType::Uint8:
    return checkOffsetsBuffer<uint8_t>(
        stringOffsets,
        valuesBufferSize,
        pValue[propertyTableCount] / sizeof(T),
        false,
        PropertyTablePropertyViewStatus::ErrorStringOffsetsNotSorted,
        PropertyTablePropertyViewStatus::ErrorStringOffsetOutOfBounds);
  case PropertyComponentType::Uint16:
    return checkOffsetsBuffer<uint16_t>(
        stringOffsets,
        valuesBufferSize,
        pValue[propertyTableCount] / sizeof(T),
        false,
        PropertyTablePropertyViewStatus::ErrorStringOffsetsNotSorted,
        PropertyTablePropertyViewStatus::ErrorStringOffsetOutOfBounds);
  case PropertyComponentType::Uint32:
    return checkOffsetsBuffer<uint32_t>(
        stringOffsets,
        valuesBufferSize,
        pValue[propertyTableCount] / sizeof(T),
        false,
        PropertyTablePropertyViewStatus::ErrorStringOffsetsNotSorted,
        PropertyTablePropertyViewStatus::ErrorStringOffsetOutOfBounds);
  case PropertyComponentType::Uint64:
    return checkOffsetsBuffer<uint64_t>(
        stringOffsets,
        valuesBufferSize,
        pValue[propertyTableCount] / sizeof(T),
        false,
        PropertyTablePropertyViewStatus::ErrorStringOffsetsNotSorted,
        PropertyTablePropertyViewStatus::ErrorStringOffsetOutOfBounds);
  default:
    return PropertyTablePropertyViewStatus::ErrorInvalidStringOffsetType;
  }
}

PropertyTableView::PropertyTableView(
    const Model& model,
    const PropertyTable& propertyTable)
    : _pModel{&model},
      _pPropertyTable{&propertyTable},
      _pClass{nullptr},
      _status() {
  const auto* pMetadata =
      model.getExtension<ExtensionModelExtStructuralMetadata>();
  if (!pMetadata) {
    _status = PropertyTableViewStatus::ErrorMissingMetadataExtension;
    return;
  }

  const std::optional<Schema>& schema = pMetadata->schema;
  if (!schema) {
    _status = PropertyTableViewStatus::ErrorMissingSchema;
    return;
  }

  auto classIter = schema->classes.find(_pPropertyTable->classProperty);
  if (classIter == schema->classes.end()) {
    _status = PropertyTableViewStatus::ErrorClassNotFound;
    return;
  }

  _pClass = &classIter->second;
}

const ClassProperty*
PropertyTableView::getClassProperty(const std::string& propertyId) const {
  if (_status != PropertyTableViewStatus::Valid) {
    return nullptr;
  }

  auto propertyIter = _pClass->properties.find(propertyId);
  if (propertyIter == _pClass->properties.end()) {
    return nullptr;
  }

  return &propertyIter->second;
}

PropertyViewStatusType PropertyTableView::getBufferSafe(
    int32_t bufferViewIdx,
    gsl::span<const std::byte>& buffer) const noexcept {
  buffer = {};

  const BufferView* pBufferView =
      CesiumGltf::Model::getSafe(&_pModel->bufferViews, bufferViewIdx);
  if (!pBufferView) {
    return PropertyTablePropertyViewStatus::ErrorInvalidValueBufferView;
  }

  const Buffer* pBuffer =
      CesiumGltf::Model::getSafe(&_pModel->buffers, pBufferView->buffer);
  if (!pBuffer) {
    return PropertyTablePropertyViewStatus::ErrorInvalidValueBuffer;
  }

  if (pBufferView->byteOffset + pBufferView->byteLength >
      static_cast<int64_t>(pBuffer->cesium.data.size())) {
    return PropertyTablePropertyViewStatus::ErrorBufferViewOutOfBounds;
  }

  buffer = gsl::span<const std::byte>(
      pBuffer->cesium.data.data() + pBufferView->byteOffset,
      static_cast<size_t>(pBufferView->byteLength));
  return PropertyTablePropertyViewStatus::Valid;
}

PropertyViewStatusType PropertyTableView::getArrayOffsetsBufferSafe(
    int32_t arrayOffsetsBufferView,
    PropertyComponentType arrayOffsetType,
    size_t valuesBufferSize,
    size_t propertyTableCount,
    bool checkBitsSize,
    gsl::span<const std::byte>& arrayOffsetsBuffer) const noexcept {
  const PropertyViewStatusType bufferStatus =
      getBufferSafe(arrayOffsetsBufferView, arrayOffsetsBuffer);
  if (bufferStatus != PropertyTablePropertyViewStatus::Valid) {
    return bufferStatus;
  }

  switch (arrayOffsetType) {
  case PropertyComponentType::Uint8:
    return checkOffsetsBuffer<uint8_t>(
        arrayOffsetsBuffer,
        valuesBufferSize,
        propertyTableCount,
        checkBitsSize,
        PropertyTablePropertyViewStatus::ErrorArrayOffsetsNotSorted,
        PropertyTablePropertyViewStatus::ErrorArrayOffsetOutOfBounds);
  case PropertyComponentType::Uint16:
    return checkOffsetsBuffer<uint16_t>(
        arrayOffsetsBuffer,
        valuesBufferSize,
        propertyTableCount,
        checkBitsSize,
        PropertyTablePropertyViewStatus::ErrorArrayOffsetsNotSorted,
        PropertyTablePropertyViewStatus::ErrorArrayOffsetOutOfBounds);
  case PropertyComponentType::Uint32:
    return checkOffsetsBuffer<uint32_t>(
        arrayOffsetsBuffer,
        valuesBufferSize,
        propertyTableCount,
        checkBitsSize,
        PropertyTablePropertyViewStatus::ErrorArrayOffsetsNotSorted,
        PropertyTablePropertyViewStatus::ErrorArrayOffsetOutOfBounds);
  case PropertyComponentType::Uint64:
    return checkOffsetsBuffer<uint64_t>(
        arrayOffsetsBuffer,
        valuesBufferSize,
        propertyTableCount,
        checkBitsSize,
        PropertyTablePropertyViewStatus::ErrorArrayOffsetsNotSorted,
        PropertyTablePropertyViewStatus::ErrorArrayOffsetOutOfBounds);
  default:
    return PropertyTablePropertyViewStatus::ErrorInvalidArrayOffsetType;
  }
}

PropertyViewStatusType PropertyTableView::getStringOffsetsBufferSafe(
    int32_t stringOffsetsBufferView,
    PropertyComponentType stringOffsetType,
    size_t valuesBufferSize,
    size_t propertyTableCount,
    gsl::span<const std::byte>& stringOffsetsBuffer) const noexcept {
  const PropertyViewStatusType bufferStatus =
      getBufferSafe(stringOffsetsBufferView, stringOffsetsBuffer);
  if (bufferStatus != PropertyTablePropertyViewStatus::Valid) {
    return bufferStatus;
  }

  switch (stringOffsetType) {
  case PropertyComponentType::Uint8:
    return checkOffsetsBuffer<uint8_t>(
        stringOffsetsBuffer,
        valuesBufferSize,
        propertyTableCount,
        false,
        PropertyTablePropertyViewStatus::ErrorStringOffsetsNotSorted,
        PropertyTablePropertyViewStatus::ErrorStringOffsetOutOfBounds);
  case PropertyComponentType::Uint16:
    return checkOffsetsBuffer<uint16_t>(
        stringOffsetsBuffer,
        valuesBufferSize,
        propertyTableCount,
        false,
        PropertyTablePropertyViewStatus::ErrorStringOffsetsNotSorted,
        PropertyTablePropertyViewStatus::ErrorStringOffsetOutOfBounds);
  case PropertyComponentType::Uint32:
    return checkOffsetsBuffer<uint32_t>(
        stringOffsetsBuffer,
        valuesBufferSize,
        propertyTableCount,
        false,
        PropertyTablePropertyViewStatus::ErrorStringOffsetsNotSorted,
        PropertyTablePropertyViewStatus::ErrorStringOffsetOutOfBounds);
  case PropertyComponentType::Uint64:
    return checkOffsetsBuffer<uint64_t>(
        stringOffsetsBuffer,
        valuesBufferSize,
        propertyTableCount,
        false,
        PropertyTablePropertyViewStatus::ErrorStringOffsetsNotSorted,
        PropertyTablePropertyViewStatus::ErrorStringOffsetOutOfBounds);
  default:
    return PropertyTablePropertyViewStatus::ErrorInvalidStringOffsetType;
  }
}

PropertyTablePropertyView<std::string_view>
PropertyTableView::getStringPropertyValues(
    const ClassProperty& classProperty,
    const PropertyTableProperty& propertyTableProperty) const {
  if (classProperty.array) {
    return PropertyTablePropertyView<std::string_view>(
        PropertyTablePropertyViewStatus::ErrorArrayTypeMismatch);
  }

  if (classProperty.type != ClassProperty::Type::STRING) {
    return PropertyTablePropertyView<std::string_view>(
        PropertyTablePropertyViewStatus::ErrorTypeMismatch);
  }

  gsl::span<const std::byte> values;
  auto status = getBufferSafe(propertyTableProperty.values, values);
  if (status != PropertyTablePropertyViewStatus::Valid) {
    return PropertyTablePropertyView<std::string_view>(status);
  }

  const PropertyComponentType offsetType =
      convertStringOffsetTypeStringToPropertyComponentType(
          propertyTableProperty.stringOffsetType);
  if (offsetType == PropertyComponentType::None) {
    return PropertyTablePropertyView<std::string_view>(
        PropertyTablePropertyViewStatus::ErrorInvalidStringOffsetType);
  }

  gsl::span<const std::byte> stringOffsets;
  status = getStringOffsetsBufferSafe(
      propertyTableProperty.stringOffsets,
      offsetType,
      values.size(),
      static_cast<size_t>(_pPropertyTable->count),
      stringOffsets);
  if (status != PropertyTablePropertyViewStatus::Valid) {
    return PropertyTablePropertyView<std::string_view>(status);
  }

  return PropertyTablePropertyView<std::string_view>(
      propertyTableProperty,
      classProperty,
      _pPropertyTable->count,
      values,
      gsl::span<const std::byte>(),
      stringOffsets,
      PropertyComponentType::None,
      offsetType);
}

PropertyTablePropertyView<PropertyArrayView<bool>>
PropertyTableView::getBooleanArrayPropertyValues(
    const ClassProperty& classProperty,
    const PropertyTableProperty& propertyTableProperty) const {
  if (!classProperty.array) {
    return PropertyTablePropertyView<PropertyArrayView<bool>>(
        PropertyTablePropertyViewStatus::ErrorArrayTypeMismatch);
  }

  if (classProperty.type != ClassProperty::Type::BOOLEAN) {
    return PropertyTablePropertyView<PropertyArrayView<bool>>(
        PropertyTablePropertyViewStatus::ErrorTypeMismatch);
  }

  gsl::span<const std::byte> values;
  auto status = getBufferSafe(propertyTableProperty.values, values);
  if (status != PropertyTablePropertyViewStatus::Valid) {
    return PropertyTablePropertyView<PropertyArrayView<bool>>(status);
  }

  const int64_t fixedLengthArrayCount = classProperty.count.value_or(0);
  if (fixedLengthArrayCount > 0 && propertyTableProperty.arrayOffsets >= 0) {
    return PropertyTablePropertyView<PropertyArrayView<bool>>(
        PropertyTablePropertyViewStatus::ErrorArrayCountAndOffsetBufferCoexist);
  }

  if (fixedLengthArrayCount <= 0 && propertyTableProperty.arrayOffsets < 0) {
    return PropertyTablePropertyView<PropertyArrayView<bool>>(
        PropertyTablePropertyViewStatus::
            ErrorArrayCountAndOffsetBufferDontExist);
  }

  // Handle fixed-length arrays
  if (fixedLengthArrayCount > 0) {
    auto maxRequiredBytes = static_cast<size_t>(glm::ceil(
        static_cast<double>(_pPropertyTable->count * fixedLengthArrayCount) /
        8.0));

    if (values.size() < maxRequiredBytes) {
      return PropertyTablePropertyView<PropertyArrayView<bool>>(
          PropertyTablePropertyViewStatus::
              ErrorBufferViewSizeDoesNotMatchPropertyTableCount);
    }

    return PropertyTablePropertyView<PropertyArrayView<bool>>(
        propertyTableProperty,
        classProperty,
        _pPropertyTable->count,
        values);
  }

  // Handle variable-length arrays
  const PropertyComponentType arrayOffsetType =
      convertArrayOffsetTypeStringToPropertyComponentType(
          propertyTableProperty.arrayOffsetType);
  if (arrayOffsetType == PropertyComponentType::None) {
    return PropertyTablePropertyView<PropertyArrayView<bool>>(
        PropertyTablePropertyViewStatus::ErrorInvalidArrayOffsetType);
  }

  constexpr bool checkBitsSize = true;
  gsl::span<const std::byte> arrayOffsets;
  status = getArrayOffsetsBufferSafe(
      propertyTableProperty.arrayOffsets,
      arrayOffsetType,
      values.size(),
      static_cast<size_t>(_pPropertyTable->count),
      checkBitsSize,
      arrayOffsets);
  if (status != PropertyTablePropertyViewStatus::Valid) {
    return PropertyTablePropertyView<PropertyArrayView<bool>>(status);
  }

  return PropertyTablePropertyView<PropertyArrayView<bool>>(
      propertyTableProperty,
      classProperty,
      _pPropertyTable->count,
      values,
      arrayOffsets,
      gsl::span<const std::byte>(),
      arrayOffsetType,
      PropertyComponentType::None);
}

PropertyTablePropertyView<PropertyArrayView<std::string_view>>
PropertyTableView::getStringArrayPropertyValues(
    const ClassProperty& classProperty,
    const PropertyTableProperty& propertyTableProperty) const {
  if (!classProperty.array) {
    return PropertyTablePropertyView<PropertyArrayView<std::string_view>>(
        PropertyTablePropertyViewStatus::ErrorArrayTypeMismatch);
  }

  if (classProperty.type != ClassProperty::Type::STRING) {
    return PropertyTablePropertyView<PropertyArrayView<std::string_view>>(
        PropertyTablePropertyViewStatus::ErrorTypeMismatch);
  }

  gsl::span<const std::byte> values;
  auto status = getBufferSafe(propertyTableProperty.values, values);
  if (status != PropertyTablePropertyViewStatus::Valid) {
    return PropertyTablePropertyView<PropertyArrayView<std::string_view>>(
        status);
  }

  // Check if array is fixed or variable length
  const int64_t fixedLengthArrayCount = classProperty.count.value_or(0);
  if (fixedLengthArrayCount > 0 && propertyTableProperty.arrayOffsets >= 0) {
    return PropertyTablePropertyView<PropertyArrayView<std::string_view>>(
        PropertyTablePropertyViewStatus::ErrorArrayCountAndOffsetBufferCoexist);
  }

  if (fixedLengthArrayCount <= 0 && propertyTableProperty.arrayOffsets < 0) {
    return PropertyTablePropertyView<PropertyArrayView<std::string_view>>(
        PropertyTablePropertyViewStatus::
            ErrorArrayCountAndOffsetBufferDontExist);
  }

  // Get string offset type
  const PropertyComponentType stringOffsetType =
      convertStringOffsetTypeStringToPropertyComponentType(
          propertyTableProperty.stringOffsetType);
  if (stringOffsetType == PropertyComponentType::None) {
    return PropertyTablePropertyView<PropertyArrayView<std::string_view>>(
        PropertyTablePropertyViewStatus::ErrorInvalidStringOffsetType);
  }

  if (propertyTableProperty.stringOffsets < 0) {
    return PropertyTablePropertyView<PropertyArrayView<std::string_view>>(
        PropertyTablePropertyViewStatus::ErrorInvalidStringOffsetBufferView);
  }

  // Handle fixed-length arrays
  if (fixedLengthArrayCount > 0) {
    gsl::span<const std::byte> stringOffsets;
    status = getStringOffsetsBufferSafe(
        propertyTableProperty.stringOffsets,
        stringOffsetType,
        values.size(),
        static_cast<size_t>(_pPropertyTable->count * fixedLengthArrayCount),
        stringOffsets);
    if (status != PropertyTablePropertyViewStatus::Valid) {
      return PropertyTablePropertyView<PropertyArrayView<std::string_view>>(
          status);
    }

    return PropertyTablePropertyView<PropertyArrayView<std::string_view>>(
        propertyTableProperty,
        classProperty,
        _pPropertyTable->count,
        values,
        gsl::span<const std::byte>(),
        stringOffsets,
        PropertyComponentType::None,
        stringOffsetType);
  }

  // Get array offset type
  const PropertyComponentType arrayOffsetType =
      convertArrayOffsetTypeStringToPropertyComponentType(
          propertyTableProperty.arrayOffsetType);
  if (arrayOffsetType == PropertyComponentType::None) {
    return PropertyTablePropertyView<PropertyArrayView<std::string_view>>(
        PropertyTablePropertyViewStatus::ErrorInvalidArrayOffsetType);
  }

  if (propertyTableProperty.arrayOffsets < 0) {
    return PropertyTablePropertyView<PropertyArrayView<std::string_view>>(
        PropertyTablePropertyViewStatus::ErrorInvalidArrayOffsetBufferView);
  }

  // Handle variable-length arrays
  gsl::span<const std::byte> stringOffsets;
  status = getBufferSafe(propertyTableProperty.stringOffsets, stringOffsets);
  if (status != PropertyTablePropertyViewStatus::Valid) {
    return PropertyTablePropertyView<PropertyArrayView<std::string_view>>(
        status);
  }

  gsl::span<const std::byte> arrayOffsets;
  status = getBufferSafe(propertyTableProperty.arrayOffsets, arrayOffsets);
  if (status != PropertyTablePropertyViewStatus::Valid) {
    return PropertyTablePropertyView<PropertyArrayView<std::string_view>>(
        status);
  }

  switch (arrayOffsetType) {
  case PropertyComponentType::Uint8:
    status = checkStringAndArrayOffsetsBuffers<uint8_t>(
        arrayOffsets,
        stringOffsets,
        values.size(),
        stringOffsetType,
        static_cast<size_t>(_pPropertyTable->count));
    break;
  case PropertyComponentType::Uint16:
    status = checkStringAndArrayOffsetsBuffers<uint16_t>(
        arrayOffsets,
        stringOffsets,
        values.size(),
        stringOffsetType,
        static_cast<size_t>(_pPropertyTable->count));
    break;
  case PropertyComponentType::Uint32:
    status = checkStringAndArrayOffsetsBuffers<uint32_t>(
        arrayOffsets,
        stringOffsets,
        values.size(),
        stringOffsetType,
        static_cast<size_t>(_pPropertyTable->count));
    break;
  case PropertyComponentType::Uint64:
    status = checkStringAndArrayOffsetsBuffers<uint64_t>(
        arrayOffsets,
        stringOffsets,
        values.size(),
        stringOffsetType,
        static_cast<size_t>(_pPropertyTable->count));
    break;
  default:
    status = PropertyTablePropertyViewStatus::ErrorInvalidArrayOffsetType;
    break;
  }

  if (status != PropertyTablePropertyViewStatus::Valid) {
    return PropertyTablePropertyView<PropertyArrayView<std::string_view>>(
        status);
  }

  return PropertyTablePropertyView<PropertyArrayView<std::string_view>>(
      propertyTableProperty,
      classProperty,
      _pPropertyTable->count,
      values,
      arrayOffsets,
      stringOffsets,
      arrayOffsetType,
      stringOffsetType);
}
} // namespace CesiumGltf
