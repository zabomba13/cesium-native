const fs = require("fs");
const path = require("path");

const getNameFromTitle = require("./getNameFromTitle");
const unindent = require("./unindent");

function generateCombinedWriter(options) {
  const {
    writerOutputDir,
    config,
    namespace,
    writerNamespace,
    rootSchema,
    writers,
  } = options;

  const name = getNameFromTitle(config, rootSchema.title);

  const header = `
        // This file was generated by generate-classes.
        // DO NOT EDIT THIS FILE!
        #pragma once

        // forward declarations
        namespace CesiumJsonWriter {
          class JsonWriter;
          class ExtensionWriterContext;
        } // namespace CesiumJsonWriter

        // forward declarations
        namespace ${namespace} {
          ${writers
            .map((writer) => {
              return writer.writeForwardDeclaration;
            })
            .join("\n")}
        } // namespace ${namespace}

        namespace ${writerNamespace} {
          ${writers
            .map((writer) => {
              return writer.writeDeclaration;
            })
            .join("\n")}

        } // namespace ${writerNamespace}
  `;

  const implementation = `
        // This file was generated by generate-classes.
        // DO NOT EDIT THIS FILE!

        #include "${name}JsonWriter.h"

        #include <CesiumUtility/JsonValue.h>
        #include <CesiumJsonWriter/ExtensionWriterContext.h>
        #include <CesiumJsonWriter/JsonObjectWriter.h>
        #include <CesiumJsonWriter/JsonWriter.h>

        ${writers
          .map((writer) => {
            return writer.writeInclude;
          })
          .join("\n")}

        // NOLINTNEXTLINE(misc-include-cleaner)
        #include <CesiumJsonWriter/writeJsonExtensions.h>

        #include <vector>
        #include <string>
        #include <cstdint>
        #include <unordered_map>
        #include <optional>

        namespace ${writerNamespace} {

        namespace {

        ${writers
          .map((writer) => {
            return writer.writeJsonDeclaration;
          })
          .join("\n")}

        // Forward declaration to avoid circular dependency since some properties
        // are vector of unordered_map and others are unordered_map of vector
        template <typename T>
        [[maybe_unused]] void writeJson(
            const std::vector<T>& list,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& context);

        [[maybe_unused]] void writeJson(
            const std::string& str,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& /* context */) {
          jsonWriter.String(str);
        }

        [[maybe_unused]] void writeJson(
            double val,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& /* context */) {
          jsonWriter.Double(val);
        }

        [[maybe_unused]] void writeJson(
            bool val,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& /* context */) {
          jsonWriter.Bool(val);
        }

        [[maybe_unused]] void writeJson(
            int64_t val,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& /* context */) {
          jsonWriter.Int64(val);
        }

        [[maybe_unused]] void writeJson(
            int32_t val,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& /* context */) {
          jsonWriter.Int64(val);
        }

        [[maybe_unused]] void writeJson(
            const CesiumUtility::JsonValue::Object& obj,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& /* context */) {
          jsonWriter.StartObject();
          for (const auto& item : obj) {
            jsonWriter.Key(item.first);
            CesiumJsonWriter::writeJsonValue(item.second, jsonWriter);
          }
          jsonWriter.EndObject();
        }

        [[maybe_unused]] void writeJson(
            const CesiumUtility::JsonValue& value,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& /* context */) {
            CesiumJsonWriter::writeJsonValue(value, jsonWriter);
        }

        template <typename T>
        [[maybe_unused]] void writeJson(
            const std::unordered_map<std::string, T>& obj,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& context) {
          jsonWriter.StartObject();
          for (const auto& item : obj) {
            jsonWriter.Key(item.first);
            writeJson(item.second, jsonWriter, context);
          }
          jsonWriter.EndObject();
        }

        template <typename T>
        [[maybe_unused]] void writeJson(
            const std::vector<T>& list,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& context) {
          jsonWriter.StartArray();
          for (const T& item : list) {
            writeJson(item, jsonWriter, context);
          }
          jsonWriter.EndArray();
        }

        template <typename T>
        [[maybe_unused]] void writeJson(
            const std::optional<T>& val,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& context) {
          if (val.has_value()) {
            writeJson(*val, jsonWriter, context);
          } else {
            jsonWriter.Null();
          }
        }

        template <typename T>
        void writeExtensibleObject(
            const T& obj,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& context) {

          if (!obj.extensions.empty()) {
            jsonWriter.Key("extensions");
            writeJsonExtensions(obj, jsonWriter, context);
          }

          if (!obj.extras.empty()) {
            jsonWriter.Key("extras");
            writeJson(obj.extras, jsonWriter, context);
          }
        }

        template <typename T>
        void writeNamedObject(
            const T& obj,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& context) {

          if (!obj.name.empty()) {
            jsonWriter.Key("name");
            writeJson(obj.name, jsonWriter, context);
          }

          writeExtensibleObject(obj, jsonWriter, context);
        }

        ${writers
          .map((writer) => {
            return writer.writeBaseJsonDefinition
              ? writer.writeBaseJsonDefinition
              : "";
          })
          .join("\n")}

        ${writers
          .map((writer) => {
            return writer.writeJsonDefinition;
          })
          .join("\n")}

        } // namespace ${writerNamespace}

        ${writers
          .map((writer) => {
            return writer.writeDefinition;
          })
          .join("\n")}

        } // namespace ${writerNamespace}
  `;

  const writerHeaderOutputDir = path.join(writerOutputDir, "generated", "src");
  fs.mkdirSync(writerHeaderOutputDir, { recursive: true });

  const headerOutputPath = path.join(
    writerHeaderOutputDir,
    `${name}JsonWriter.h`
  );

  const implementationOutputPath = path.join(
    writerHeaderOutputDir,
    `${name}JsonWriter.cpp`
  );

  fs.writeFileSync(headerOutputPath, unindent(header), "utf-8");
  fs.writeFileSync(implementationOutputPath, unindent(implementation), "utf-8");
}

module.exports = generateCombinedWriter;
