// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#include "SkinJsonHandler.h"
#include "CesiumGltf/Skin.h"

#include <cassert>
#include <string>

using namespace CesiumGltf;

SkinJsonHandler::SkinJsonHandler(const ReaderContext& context) noexcept
    : NamedObjectJsonHandler(context),
      _inverseBindMatrices(),
      _skeleton(),
      _joints() {}

void SkinJsonHandler::reset(
    CesiumJsonReader::IJsonHandler* pParentHandler,
    Skin* pObject) {
  NamedObjectJsonHandler::reset(pParentHandler, pObject);
  this->_pObject = pObject;
}

CesiumJsonReader::IJsonHandler*
SkinJsonHandler::readObjectKey(const std::string_view& str) {
  assert(this->_pObject);
  return this->readObjectKeySkin(Skin::TypeName, str, *this->_pObject);
}

CesiumJsonReader::IJsonHandler* SkinJsonHandler::readObjectKeySkin(
    const std::string& objectType,
    const std::string_view& str,
    Skin& o) {
  using namespace std::string_literals;

  if ("inverseBindMatrices"s == str)
    return property(
        "inverseBindMatrices",
        this->_inverseBindMatrices,
        o.inverseBindMatrices);
  if ("skeleton"s == str)
    return property("skeleton", this->_skeleton, o.skeleton);
  if ("joints"s == str)
    return property("joints", this->_joints, o.joints);

  return this->readObjectKeyNamedObject(objectType, str, *this->_pObject);
}
