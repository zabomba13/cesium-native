// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include <CesiumGltfReader/Library.h>
#include <CesiumJsonReader/JsonReader.h>
#include <CesiumJsonReader/JsonReaderOptions.h>
#include <CesiumGltf/AnimationChannel.h>
#include <gsl/span>
#include <rapidjson/fwd.h>
#include <vector>

namespace CesiumGltf {
  struct AnimationChannel;
}

namespace CesiumGltfReader {

/**
 * @brief Reads {@link AnimationChannel} instances from JSON.
 */
class CESIUMGLTFREADER_API AnimationChannelReader {
public:
  /**
   * @brief Constructs a new instance.
   */
  AnimationChannelReader();

  /**
   * @brief Gets the options controlling how the JSON is read.
   */
  CesiumJsonReader::JsonReaderOptions& getOptions();

  /**
   * @brief Gets the options controlling how the JSON is read.
   */
  const CesiumJsonReader::JsonReaderOptions& getOptions() const;

  /**
   * @brief Reads an instance of AnimationChannel from a byte buffer.
   *
   * @param data The buffer from which to read the instance.
   * @return The result of reading the instance.
   */
  CesiumJsonReader::ReadJsonResult<CesiumGltf::AnimationChannel> readFromJson(const gsl::span<const std::byte>& data) const;

  /**
   * @brief Reads an instance of AnimationChannel from a rapidJson::Value.
   *
   * @param data The buffer from which to read the instance.
   * @return The result of reading the instance.
   */
  CesiumJsonReader::ReadJsonResult<CesiumGltf::AnimationChannel> readFromJson(const rapidjson::Value& value) const;

  /**
   * @brief Reads an array of instances of AnimationChannel from a rapidJson::Value.
   *
   * @param data The buffer from which to read the array of instances.
   * @return The result of reading the array of instances.
   */
  CesiumJsonReader::ReadJsonResult<std::vector<CesiumGltf::AnimationChannel>> readArrayFromJson(const rapidjson::Value& value) const;

private:
  CesiumJsonReader::JsonReaderOptions _options;
};

} // namespace CesiumGltfReader