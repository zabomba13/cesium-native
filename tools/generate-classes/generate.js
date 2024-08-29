const fs = require("fs");
const getNameFromTitle = require("./getNameFromTitle");
const indent = require("./indent");
const lodash = require("lodash");
const NameFormatters = require("./NameFormatters");
const path = require("path");
const resolveProperty = require("./resolveProperty");
const unindent = require("./unindent");

function generate(options, schema, writers) {
  const {
    schemaCache,
    outputDir,
    readerOutputDir,
    config,
    namespace,
    readerNamespace,
    writerNamespace,
    extensions,
  } = options;

  const baseName = getNameFromTitle(config, schema.title);
  const prefix =
    schema.prefix && schema.prefix !== baseName ? schema.prefix : "";
  const name = prefix + baseName;
  const thisConfig = config.classes[schema.title] || {};

  console.log(`Generating ${name}`);

  schemaCache.pushContext(schema);

  let baseSchema;
  if (schema.allOf && schema.allOf.length > 0 && schema.allOf[0].$ref) {
    baseSchema = schemaCache.load(schema.allOf[0].$ref);
  } else if (schema.$ref) {
    baseSchema = schemaCache.load(schema.$ref);
  }

  let base = "CesiumUtility::ExtensibleObject";
  if (baseSchema !== undefined) {
    base = getNameFromTitle(config, baseSchema.title);
  }

  const required = schema.required || [];

  const properties = Object.keys(schema.properties || {})
    .map((key) =>
      resolveProperty(
        schemaCache,
        config,
        schema,
        name,
        key,
        schema.properties[key],
        required,
        namespace,
        readerNamespace,
        writerNamespace
      )
    )
    .filter((property) => property !== undefined);

  const localTypes = lodash.uniq(
    lodash.flatten(properties.map((property) => property.localTypes))
  );

  schemaCache.popContext();

  let headers = lodash.uniq([
    `"${namespace}/Library.h"`,
    NameFormatters.getIncludeFromName(base, namespace),
    ...lodash.flatten(properties.map((property) => property.headers)),
  ]);

  // Prevent header from including itself for recursive types like Tile
  headers = headers.filter((header) => {
    return header !== `"${namespace}/${name}.h"`;
  });

  headers.sort();

  // prettier-ignore
  const header = `
        // This file was generated by generate-classes.
        // DO NOT EDIT THIS FILE!
        #pragma once

        ${headers.map((header) => `#include ${header}`).join("\n")}

        namespace ${namespace} {
            /**
             * @brief ${schema.description ? schema.description : schema.title}
             */
            struct ${namespace.toUpperCase()}_API ${name}${thisConfig.toBeInherited ? "Spec" : (thisConfig.isBaseClass ? "" : " final")} : public ${base} {
                static inline constexpr const char* TypeName = "${name}";
                ${thisConfig.extensionName ? `static inline constexpr const char* ExtensionName = "${thisConfig.extensionName}";` : ""}

                ${indent(localTypes.join("\n\n"), 16)}

                ${indent(
                  properties
                    .map((property) => formatProperty(property))
                    .filter(propertyText => propertyText !== undefined)
                    .join("\n\n"),
                  16
                )}
                ${thisConfig.toBeInherited ? privateSpecConstructor(name) : ""}
            };
        }
    `;

  const headerOutputDir = path.join(
    outputDir,
    "generated",
    "include",
    namespace
  );
  fs.mkdirSync(headerOutputDir, { recursive: true });
  const headerOutputPath = path.join(
    headerOutputDir,
    `${name}${thisConfig.toBeInherited ? "Spec" : ""}.h`
  );
  fs.writeFileSync(headerOutputPath, unindent(header), "utf-8");

  let jsonHandlerHeaders = lodash.uniq([
    NameFormatters.getJsonHandlerIncludeFromName(base, readerNamespace),
    `<${namespace}/${name}.h>`,
    ...lodash.flatten(properties.map((property) => property.readerHeaders)),
  ]);

  // Prevent header from including itself for recursive types like Tile
  jsonHandlerHeaders = jsonHandlerHeaders.filter((readerHeader) => {
    return readerHeader !== `"${name}JsonHandler.h"`;
  });

  jsonHandlerHeaders.sort();

  const readerLocalTypes = lodash.uniq(
    lodash.flatten(properties.map((property) => property.readerLocalTypes))
  );

  const baseJsonHandler = NameFormatters.getJsonHandlerName(
    base,
    readerNamespace
  );

  // prettier-ignore
  const jsonHandlerHeader = `
        // This file was generated by generate-classes.
        // DO NOT EDIT THIS FILE!
        #pragma once

        ${jsonHandlerHeaders.map((header) => `#include ${header}`).join("\n")}

        namespace CesiumJsonReader {
          class JsonReaderOptions;
        }

        namespace ${readerNamespace} {
          class ${name}JsonHandler : public ${baseJsonHandler}${thisConfig.extensionName ? `, public CesiumJsonReader::IExtensionJsonHandler` : ""} {
          public:
            using ValueType = ${namespace}::${name};

            ${thisConfig.extensionName ? `static inline constexpr const char* ExtensionName = "${thisConfig.extensionName}";` : ""}

            ${name}JsonHandler(const CesiumJsonReader::JsonReaderOptions& options) noexcept;
            void reset(IJsonHandler* pParentHandler, ${namespace}::${name}* pObject);

            virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

            ${thisConfig.extensionName ? `
            virtual void reset(IJsonHandler* pParentHandler, CesiumUtility::ExtensibleObject& o, const std::string_view& extensionName) override;

            virtual IJsonHandler& getHandler() override { return *this; }
            ` : ""}

          protected:
            IJsonHandler* readObjectKey${name}(const std::string& objectType, const std::string_view& str, ${namespace}::${name}& o);

          private:
            ${indent(readerLocalTypes.join("\n\n"), 12)}
            ${namespace}::${name}* _pObject = nullptr;
            ${indent(
              properties
                .map((property) => formatReaderProperty(property))
                .join("\n"),
              12
            )}
          };
        }
  `;

  const jsonHandlerHeaderOutputDir = path.join(
    readerOutputDir,
    "generated",
    "src"
  );
  fs.mkdirSync(jsonHandlerHeaderOutputDir, { recursive: true });

  const jsonHandlerHeaderOutputPath = path.join(
    jsonHandlerHeaderOutputDir,
    name + "JsonHandler.h"
  );
  fs.writeFileSync(
    jsonHandlerHeaderOutputPath,
    unindent(jsonHandlerHeader),
    "utf-8"
  );

  const readerHeader = `
    // This file was generated by generate-classes.
    // DO NOT EDIT THIS FILE!
    #pragma once

    #include <${readerNamespace}/Library.h>
    #include <CesiumJsonReader/JsonReader.h>
    #include <CesiumJsonReader/JsonReaderOptions.h>
    #include <${namespace}/${name}.h>
    #include <gsl/span>
    #include <rapidjson/fwd.h>
    #include <vector>

    namespace ${namespace} {
      struct ${name};
    }

    namespace ${readerNamespace} {

    /**
     * @brief Reads {@link ${name}} instances from JSON.
     */
    class ${readerNamespace.toUpperCase()}_API ${name}Reader {
    public:
      /**
       * @brief Constructs a new instance.
       */
      ${name}Reader();

      /**
       * @brief Gets the options controlling how the JSON is read.
       */
      CesiumJsonReader::JsonReaderOptions& getOptions();

      /**
       * @brief Gets the options controlling how the JSON is read.
       */
      const CesiumJsonReader::JsonReaderOptions& getOptions() const;

      /**
       * @brief Reads an instance of ${name} from a byte buffer.
       *
       * @param data The buffer from which to read the instance.
       * @return The result of reading the instance.
       */
      CesiumJsonReader::ReadJsonResult<${namespace}::${name}> readFromJson(const gsl::span<const std::byte>& data) const;

      /**
       * @brief Reads an instance of ${name} from a rapidJson::Value.
       *
       * @param data The buffer from which to read the instance.
       * @return The result of reading the instance.
       */
      CesiumJsonReader::ReadJsonResult<${namespace}::${name}> readFromJson(const rapidjson::Value& value) const;

      /**
       * @brief Reads an array of instances of ${name} from a rapidJson::Value.
       *
       * @param data The buffer from which to read the array of instances.
       * @return The result of reading the array of instances.
       */
      CesiumJsonReader::ReadJsonResult<std::vector<${namespace}::${name}>> readArrayFromJson(const rapidjson::Value& value) const;

    private:
      CesiumJsonReader::JsonReaderOptions _options;
    };

    } // namespace ${readerNamespace}`;

  const readerHeaderOutputDir = path.join(
    readerOutputDir,
    "generated",
    "include",
    readerNamespace
  );
  fs.mkdirSync(readerHeaderOutputDir, { recursive: true });

  const readerHeaderOutputPath = path.join(
    readerHeaderOutputDir,
    name + "Reader.h"
  );
  fs.writeFileSync(readerHeaderOutputPath, unindent(readerHeader), "utf-8");

  const jsonHandlerLocalTypesImpl = lodash.uniq(
    lodash.flatten(properties.map((property) => property.readerLocalTypesImpl))
  );

  const jsonHandlerHeadersImpl = lodash.uniq([
    ...lodash.flatten(properties.map((property) => property.readerHeadersImpl)),
  ]);
  jsonHandlerHeadersImpl.sort();

  function generateJsonHandlerOptionsInitializerList(properties, varName) {
    const initializerList = properties
      .filter((p) => p.readerType.toLowerCase().indexOf("jsonhandler") != -1)
      .map(
        (p) =>
          `_${p.cppSafeName}(${
            p.schemas && p.schemas.length > 0 ? varName : ""
          })`
      )
      .join(", ");
    return initializerList == "" ? "" : ", " + initializerList;
  }
  // prettier-ignore
  const jsonHandlerImpl = `
        // This file was generated by generate-classes.
        // DO NOT EDIT THIS FILE!
        #include "${name}JsonHandler.h"

        #include <${namespace}/${name}.h>
        ${jsonHandlerHeadersImpl.map((header) => `#include ${header}`).join("\n")}
        #include <${readerNamespace}/${name}Reader.h>
        #include <CesiumJsonReader/JsonReader.h>
        #include <CesiumJsonReader/ArrayJsonHandler.h>
        #include <CesiumUtility/Assert.h>
        #include "registerReaderExtensions.h"
        #include <string>

        namespace ${readerNamespace} {

        ${name}JsonHandler::${name}JsonHandler(const CesiumJsonReader::JsonReaderOptions& options) noexcept : ${baseJsonHandler}(options)${generateJsonHandlerOptionsInitializerList(properties, 'options')} {}

        void ${name}JsonHandler::reset(CesiumJsonReader::IJsonHandler* pParentHandler, ${namespace}::${name}* pObject) {
          ${baseJsonHandler}::reset(pParentHandler, pObject);
          this->_pObject = pObject;
        }

        CesiumJsonReader::IJsonHandler* ${name}JsonHandler::readObjectKey(const std::string_view& str) {
          CESIUM_ASSERT(this->_pObject);
          return this->readObjectKey${name}(${namespace}::${name}::TypeName, str, *this->_pObject);
        }

        ${thisConfig.extensionName ? `
        void ${name}JsonHandler::reset(CesiumJsonReader::IJsonHandler* pParentHandler, CesiumUtility::ExtensibleObject& o, const std::string_view& extensionName) {
          std::any& value =
              o.extensions.emplace(extensionName, ${namespace}::${name}())
                  .first->second;
          this->reset(
              pParentHandler,
              &std::any_cast<${namespace}::${name}&>(value));
        }
        ` : ""}

        CesiumJsonReader::IJsonHandler* ${name}JsonHandler::readObjectKey${name}(const std::string& objectType, const std::string_view& str, ${namespace}::${name}& o) {
          using std::string_literals::operator""s; // NOLINT(misc-include-cleaner)

          ${properties.length > 0 ? `
          ${indent(
            properties
              .map((property) => formatReaderPropertyImpl(property))
              .join("\n"),
            10
          )}` : `(void)o;`}

          return this->readObjectKey${NameFormatters.removeNamespace(base)}(objectType, str, *this->_pObject);
        }

        ${name}Reader::${name}Reader() { registerReaderExtensions(this->_options); }

        CesiumJsonReader::JsonReaderOptions& ${name}Reader::getOptions() {
          return this->_options;
        }

        const CesiumJsonReader::JsonReaderOptions& ${name}Reader::getOptions() const {
          return this->_options;
        }

        CesiumJsonReader::ReadJsonResult<${namespace}::${name}> ${name}Reader::readFromJson(const gsl::span<const std::byte>& data) const {
          ${name}JsonHandler handler(this->_options);
          return CesiumJsonReader::JsonReader::readJson(data, handler);
        }

        CesiumJsonReader::ReadJsonResult<${namespace}::${name}> ${name}Reader::readFromJson(const rapidjson::Value& value) const {
          ${name}JsonHandler handler(this->_options);
          return CesiumJsonReader::JsonReader::readJson(value, handler);
        }

        CesiumJsonReader::ReadJsonResult<std::vector<${namespace}::${name}>> ${name}Reader::readArrayFromJson(const rapidjson::Value& value) const {
          CesiumJsonReader::ArrayJsonHandler<${namespace}::${name}, ${name}JsonHandler> handler(this->_options);
          return CesiumJsonReader::JsonReader::readJson(value, handler);
        }

        ${indent(jsonHandlerLocalTypesImpl.join("\n\n"), 8)}
        } // namespace ${readerNamespace}
  `;

  const writeForwardDeclaration = `struct ${name};`;

  const writeInclude = `#include <${namespace}/${name}.h>`;

  const writeDeclaration = `
        struct ${name}JsonWriter {
          using ValueType = ${namespace}::${name};

          ${
            thisConfig.extensionName
              ? `static inline constexpr const char* ExtensionName = "${thisConfig.extensionName}";`
              : ""
          }

          static void write(
              const ${namespace}::${name}& obj,
              CesiumJsonWriter::JsonWriter& jsonWriter,
              const CesiumJsonWriter::ExtensionWriterContext& context);
        };
  `;

  const writeJsonDeclaration = `
        void writeJson(
            const ${namespace}::${name}& obj,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& context);
  `;

  const writeDefinition = `
        void ${name}JsonWriter::write(
            const ${namespace}::${name}& obj,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& context) {
          writeJson(obj, jsonWriter, context);
        }
  `;

  let writeBaseJsonDefinition;
  let writeJsonDefinition;

  if (thisConfig.isBaseClass) {
    writeBaseJsonDefinition = `
        template <typename T>
        void write${NameFormatters.getWriterName(name)}(
            const T& obj,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& context) {

          ${indent(
            properties
              .map((property) => formatWriterPropertyImpl(property))
              .join("\n\n"),
            10
          )}

          write${NameFormatters.getWriterName(base)}(obj, jsonWriter, context);
        }
    `;

    writeJsonDefinition = `
        void writeJson(
            const ${namespace}::${name}& obj,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& context) {
          jsonWriter.StartObject();

          write${NameFormatters.getWriterName(name)}(obj, jsonWriter, context);

          jsonWriter.EndObject();
        }
    `;
  } else {
    writeJsonDefinition = `
        void writeJson(
            const ${namespace}::${name}& obj,
            CesiumJsonWriter::JsonWriter& jsonWriter,
            const CesiumJsonWriter::ExtensionWriterContext& context) {
          jsonWriter.StartObject();

          ${indent(
            properties
              .map((property) => formatWriterPropertyImpl(property))
              .join("\n\n"),
            10
          )}

          write${NameFormatters.getWriterName(base)}(obj, jsonWriter, context);

          jsonWriter.EndObject();
        }
    `;
  }

  const writeExtensionsRegistration = `
        ${
          extensions[schema.title]
            ? extensions[schema.title]
                .map((extension) => {
                  return `context.registerExtension<${namespace}::${name}, ${extension.className}JsonWriter>();`;
                })
                .join("\n")
            : ""
        }
  `;

  writers.push({
    writeInclude,
    writeForwardDeclaration,
    writeDeclaration,
    writeJsonDeclaration,
    writeDefinition,
    writeJsonDefinition,
    writeBaseJsonDefinition,
    writeExtensionsRegistration,
  });

  if (options.oneHandlerFile) {
    const readerSourceOutputPath = path.join(
      jsonHandlerHeaderOutputDir,
      "GeneratedJsonHandlers.cpp"
    );
    fs.appendFileSync(
      readerSourceOutputPath,
      unindent(jsonHandlerImpl),
      "utf-8"
    );
  } else {
    const readerSourceOutputPath = path.join(
      jsonHandlerHeaderOutputDir,
      name + "JsonHandler.cpp"
    );
    fs.writeFileSync(
      readerSourceOutputPath,
      unindent(jsonHandlerImpl),
      "utf-8"
    );
  }

  const schemas = lodash.flatten(
    properties.map((property) => property.schemas)
  );
  if (baseSchema && !base.includes("::")) {
    schemas.push(baseSchema);
  }

  return lodash.uniq(schemas);
}

function formatProperty(property) {
  if (!property.type) {
    return undefined;
  }

  let result = "";

  result += `/**\n * @brief ${property.briefDoc || property.cppSafeName}\n`;
  if (property.fullDoc) {
    result += ` *\n * ${property.fullDoc.split("\n").join("\n * ")}\n`;
  }

  result += ` */\n`;

  result += `${property.type} ${property.cppSafeName}`;

  if (property.defaultValue !== undefined) {
    result += " = " + property.defaultValue;
  } else if (property.needsInitialization) {
    result += " = " + property.type + "()";
  }

  result += ";";

  return result;
}

function formatReaderProperty(property) {
  return `${property.readerType} _${property.cppSafeName};`;
}

function formatReaderPropertyImpl(property) {
  return `if ("${property.name}"s == str) return property("${property.name}", this->_${property.cppSafeName}, o.${property.cppSafeName});`;
}

function formatWriterPropertyImpl(property) {
  let result = "";

  const type = property.type;
  const defaultValue = property.defaultValueWriter || property.defaultValue;

  const isId = property.requiredId !== undefined;
  const isRequiredEnum = property.requiredEnum === true;
  const isVector = type.startsWith("std::vector");
  const isMap = type.startsWith("std::unordered_map");
  const isOptional = type.startsWith("std::optional");

  const hasDefaultValueGuard =
    !isId && !isRequiredEnum && defaultValue !== undefined;
  const hasDefaultVectorGuard = hasDefaultValueGuard && isVector;
  const hasEmptyGuard = isVector || isMap;
  const hasOptionalGuard = isOptional;
  const hasNegativeIndexGuard = isId;
  const hasGuard =
    hasDefaultValueGuard ||
    hasEmptyGuard ||
    hasOptionalGuard ||
    hasNegativeIndexGuard;

  if (hasDefaultVectorGuard) {
    result += `static const ${type} ${property.cppSafeName}Default = ${defaultValue};\n`;
    result += `if (obj.${property.cppSafeName} != ${property.cppSafeName}Default) {\n`;
  } else if (hasDefaultValueGuard) {
    result += `if (obj.${property.cppSafeName} != ${defaultValue}) {\n`;
  } else if (hasEmptyGuard) {
    result += `if (!obj.${property.cppSafeName}.empty()) {\n`;
  } else if (hasNegativeIndexGuard) {
    result += `if (obj.${property.cppSafeName} > -1) {\n`;
  } else if (hasOptionalGuard) {
    result += `if (obj.${property.cppSafeName}.has_value()) {\n`;
  }

  result += `jsonWriter.Key("${property.name}");\n`;
  result += `writeJson(obj.${property.cppSafeName}, jsonWriter, context);\n`;

  if (hasGuard) {
    result += "}\n";
  }

  return result;
}

function privateSpecConstructor(name) {
  return `
    private:
      /**
       * @brief This class is not meant to be instantiated directly. Use {@link ${name}} instead.
       */
      ${name}Spec() = default;
      friend struct ${name};
  `;
}

module.exports = generate;
