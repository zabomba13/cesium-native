#include "CesiumJsonReader/StringJsonHandler.h"

#include "CesiumJsonReader/IJsonHandler.h"
#include "CesiumJsonReader/JsonHandler.h"

#include <string>
#include <string_view>

namespace CesiumJsonReader {
StringJsonHandler::StringJsonHandler() noexcept = default;

void StringJsonHandler::reset(IJsonHandler* pParent, std::string* pString) {
  JsonHandler::reset(pParent);
  this->_pString = pString;
}

std::string* StringJsonHandler::getObject() noexcept { return this->_pString; }

IJsonHandler* StringJsonHandler::readString(const std::string_view& str) {
  *this->_pString = str;
  return this->parent();
}
} // namespace CesiumJsonReader
