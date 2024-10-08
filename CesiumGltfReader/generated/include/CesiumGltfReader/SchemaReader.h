// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include <CesiumGltfReader/Library.h>
#include <CesiumJsonReader/JsonReader.h>
#include <CesiumJsonReader/JsonReaderOptions.h>
#include <CesiumGltf/Schema.h>
#include <gsl/span>
#include <rapidjson/fwd.h>
#include <vector>

namespace CesiumGltf {
  struct Schema;
}

namespace CesiumGltfReader {

/**
 * @brief Reads {@link Schema} instances from JSON.
 */
class CESIUMGLTFREADER_API SchemaReader {
public:
  /**
   * @brief Constructs a new instance.
   */
  SchemaReader();

  /**
   * @brief Gets the options controlling how the JSON is read.
   */
  CesiumJsonReader::JsonReaderOptions& getOptions();

  /**
   * @brief Gets the options controlling how the JSON is read.
   */
  const CesiumJsonReader::JsonReaderOptions& getOptions() const;

  /**
   * @brief Reads an instance of Schema from a byte buffer.
   *
   * @param data The buffer from which to read the instance.
   * @return The result of reading the instance.
   */
  CesiumJsonReader::ReadJsonResult<CesiumGltf::Schema> readFromJson(const gsl::span<const std::byte>& data) const;

  /**
   * @brief Reads an instance of Schema from a rapidJson::Value.
   *
   * @param data The buffer from which to read the instance.
   * @return The result of reading the instance.
   */
  CesiumJsonReader::ReadJsonResult<CesiumGltf::Schema> readFromJson(const rapidjson::Value& value) const;

  /**
   * @brief Reads an array of instances of Schema from a rapidJson::Value.
   *
   * @param data The buffer from which to read the array of instances.
   * @return The result of reading the array of instances.
   */
  CesiumJsonReader::ReadJsonResult<std::vector<CesiumGltf::Schema>> readArrayFromJson(const rapidjson::Value& value) const;

private:
  CesiumJsonReader::JsonReaderOptions _options;
};

} // namespace CesiumGltfReader