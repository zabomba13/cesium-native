#include "NamedObjectJsonHandler.h"

#include "CesiumJsonReader/IJsonHandler.h"
#include "CesiumJsonReader/JsonReaderOptions.h"

#include <CesiumGltf/NamedObject.h>
#include <CesiumJsonReader/ExtensibleObjectJsonHandler.h>

#include <string>
#include <string_view>

namespace CesiumGltfReader {
NamedObjectJsonHandler::NamedObjectJsonHandler(
    const CesiumJsonReader::JsonReaderOptions& context) noexcept
    : CesiumJsonReader::ExtensibleObjectJsonHandler(context), _name() {}

void NamedObjectJsonHandler::reset(
    IJsonHandler* pParent,
    CesiumGltf::NamedObject* pObject) {
  CesiumJsonReader::ExtensibleObjectJsonHandler::reset(pParent, pObject);
}

CesiumJsonReader::IJsonHandler*
NamedObjectJsonHandler::readObjectKeyNamedObject(
    const std::string& objectType,
    const std::string_view& str,
    CesiumGltf::NamedObject& o) {
  // NOLINTNEXTLINE(misc-include-cleaner)
  using std::string_literals::operator""s;
  if ("name"s == str)
    return property("name", this->_name, o.name);
  return this->readObjectKeyExtensibleObject(objectType, str, o);
}
} // namespace CesiumGltfReader
