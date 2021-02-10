#include "JsonObjectWriter.h"
#include "BufferViewWriter.h"
#include <magic_enum.hpp>
#include <stdexcept>
#include <type_traits>

void CesiumGltf::writeBufferView(
    const std::vector<BufferView>& bufferViews,
    CesiumGltf::JsonWriter& jsonWriter) {

    if (bufferViews.empty()) {
        return;
    }

    auto& j = jsonWriter;
    j.Key("bufferViews");
    j.StartArray();
    for (const auto& buffer : bufferViews) {
        j.StartObject();

        j.Key("buffer");
        j.Int(buffer.buffer);

        if (buffer.byteOffset >= 0) {
            j.Key("byteOffset");
            j.Int64(buffer.byteOffset);
        }

        j.Key("byteLength");
        j.Int64(buffer.byteLength);

        if (buffer.byteStride && buffer.byteStride >= 0) {
            j.Key("byteStride");
            j.Int64(*buffer.byteStride);
        }

        if (buffer.target) {
            j.Key("target");
            j.Int(magic_enum::enum_integer(*buffer.target));
        }

        if (!buffer.name.empty()) {
            j.Key("name");
            j.String(buffer.name.c_str());
        }

        if (!buffer.extras.empty()) {
            j.Key("extras");
            writeJsonObject(buffer.extras, j);
        }

        j.EndObject();
        // TODO: extensions / extras
    }
    j.EndArray();
}