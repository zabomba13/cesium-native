#include "JsonObjectWriter.h"
#include "JsonWriter.h"
#include <CesiumGltf/JsonValue.h>
#include <catch2/catch.hpp>
#include <string>
#include <iostream>

using namespace CesiumGltf;
using namespace rapidjson;

using Object = JsonValue::Object;
using Value = JsonValue;
using Array = JsonValue::Array;
using Bool = JsonValue::Bool;
using Null = JsonValue::Null;

TEST_CASE("TestJsonObjectWriter") {
    SECTION("[{}, {}, {}]") {
        CesiumGltf::JsonWriter writer;
        const auto extrasObject =
            Object{{"extras", Array{Object{}, Object{}, Object{}}}};
        writeJsonValue(extrasObject, writer, false);
        REQUIRE(writer.toString() == R"({"extras":[{},{},{}]})");
    }

    SECTION(R"("A": {"B": "C"{}})") {
        CesiumGltf::JsonWriter writer;
        // clang-format off
        const auto extrasObject = Object {{
            "extras", Object {{
                "A", Object {{
                    "B", Object {{
                        "C", Object {}
                    }}
                }}
            }}
        }};
        // clang-format on

        writeJsonValue(extrasObject, writer, false);
        REQUIRE(writer.toString() == R"({"extras":{"A":{"B":{"C":{}}}}})");
    }

    SECTION(R"([[[1 -2,false,null,true,{"emojis": "😂👽🇵🇷"}]]])") {
        CesiumGltf::JsonWriter writer;
        // clang-format off
        const auto extrasObject = Object {{
            "extras", Array {{{
                1.0, -2.0, Bool(false), Null(), Bool(true),
                Object {{ "emojis", "😂👽🇵🇷" }} }}}
        }};
        // clang-format on

        writeJsonValue(extrasObject, writer, false);
        REQUIRE(
            writer.toString() ==
            R"({"extras":[[[1.0,-2.0,false,null,true,{"emojis":"😂👽🇵🇷"}]]]})");
    }

    SECTION("Empty object is serialized correctly") {
        CesiumGltf::JsonWriter writer;
        writeJsonValue(Object{}, writer, false);
        REQUIRE(writer.toString() == "{}");
    }
}