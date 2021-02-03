#include "JsonObjectWriter.h"
#include <CesiumGltf/JsonValue.h>
#include <functional>
#include <stack>
#include <utility>

using namespace CesiumGltf;
using namespace rapidjson;

void primitiveWriter(const JsonValue& item, Writer<StringBuffer>& j);
void recursiveArrayWriter(
    const JsonValue::Array& array,
    Writer<StringBuffer>& j);
void recursiveObjectWriter(
    const JsonValue::Object& object,
    Writer<StringBuffer>& j);

void primitiveWriter(const JsonValue& item, Writer<StringBuffer>& j) {
    if (item.isBool()) {
        j.Bool(std::get<JsonValue::Bool>(item.value));
    }

    else if (item.isNull()) {
        j.Null();
    }

    else if (item.isNumber()) {
        j.Double(std::get<JsonValue::Number>(item.value));
    }

    else if (item.isString()) {
        j.String(std::get<JsonValue::String>(item.value).c_str());
    }
}

void recursiveArrayWriter(
    const JsonValue::Array& array,
    Writer<StringBuffer>& j) {
    j.StartArray();
    for (const auto& item : array) {
        if (item.isArray()) {
            recursiveArrayWriter(std::get<JsonValue::Array>(item.value), j);
        }

        else if (item.isObject()) {
            recursiveObjectWriter(std::get<JsonValue::Object>(item.value), j);
        }

        else {
            primitiveWriter(item, j);
        }
    }
    j.EndArray();
}

void recursiveObjectWriter(
    const CesiumGltf::JsonValue::Object& object,
    rapidjson::Writer<rapidjson::StringBuffer>& j) {

    j.StartObject();
    for (const auto& [key, item] : object) {
        j.Key(key.c_str());
        if (item.isArray()) {
            recursiveArrayWriter(std::get<JsonValue::Array>(item.value), j);
        }

        if (item.isObject()) {
            recursiveObjectWriter(std::get<JsonValue::Object>(item.value), j);
        }

        else {
            primitiveWriter(item, j);
        }
    }
    j.EndObject();
}

void CesiumGltf::writeJsonObject(
    const JsonValue::Object& object,
    rapidjson::Writer<rapidjson::StringBuffer>& jsonWriter) {
    recursiveObjectWriter(object, jsonWriter);
}