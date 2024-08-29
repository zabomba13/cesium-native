#include <Cesium3DTiles/ClassProperty.h>
#include <Cesium3DTiles/MetadataEntity.h>
#include <Cesium3DTiles/MetadataQuery.h>
#include <Cesium3DTiles/Schema.h>

#include <optional>
#include <string>

namespace Cesium3DTiles {

std::optional<FoundMetadataProperty>
MetadataQuery::findFirstPropertyWithSemantic(
    const Schema& schema,
    const MetadataEntity& entity,
    const std::string& semantic) {
  auto classIt = schema.classes.find(entity.classProperty);
  if (classIt == schema.classes.end()) {
    return std::nullopt;
  }

  const auto& [classId, klass] = *classIt;

  for (const auto& [propertyId, property] : entity.properties) {
    auto propertyIt = klass.properties.find(propertyId);
    if (propertyIt == klass.properties.end()) {
      continue;
    }

    const ClassProperty& classProperty = propertyIt->second;
    if (classProperty.semantic == semantic) {
      return FoundMetadataProperty{
          &classId,
          &klass,
          &propertyId,
          &propertyIt->second,
          &property};
    }
  }

  return std::nullopt;
}

} // namespace Cesium3DTiles
