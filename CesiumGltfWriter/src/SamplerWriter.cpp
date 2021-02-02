#include "SamplerWriter.h"
#include <magic_enum.hpp>

void CesiumGltf::writeSampler(
    const std::vector<Sampler>& samplers,
    rapidjson::Writer<rapidjson::StringBuffer>& jsonWriter
) {

    if (samplers.empty()) {
        return;
    }

    auto& j = jsonWriter;

    j.StartArray();
    for (const auto& sampler : samplers) {
        j.StartObject();

        if (sampler.magFilter) {
            j.Key("magFilter");
            j.Int(magic_enum::enum_integer(*sampler.magFilter));
        }

        if (sampler.minFilter) {
            j.Key("minFilter");
            j.Int(magic_enum::enum_integer(*sampler.minFilter));
        }

        if (sampler.wrapS != Sampler::WrapS::REPEAT) {
            j.Key("wrapS");
            j.Int(magic_enum::enum_integer(sampler.wrapS));
        }

        if (sampler.wrapT != Sampler::WrapT::REPEAT) {
            j.Key("wrapT");
            j.Int(magic_enum::enum_integer(sampler.wrapT));
        }

        if (!sampler.name.empty())  {
            j.Key("name");
            j.String(sampler.name.c_str());
        }

        // todo extras / extensions
        j.EndObject();

    }
    j.EndArray();
}