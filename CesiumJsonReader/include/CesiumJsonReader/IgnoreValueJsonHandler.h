#pragma once

#include "IJsonHandler.h"
#include "Library.h"

#include <cstdint>

namespace CesiumJsonReader {
class CESIUMJSONREADER_API IgnoreValueJsonHandler : public IJsonHandler {
public:
  void reset(IJsonHandler* pParent) noexcept;

  IJsonHandler* readNull() override;
  IJsonHandler* readBool(bool b) override;
  IJsonHandler* readInt32(int32_t i) override;
  IJsonHandler* readUint32(uint32_t i) override;
  IJsonHandler* readInt64(int64_t i) override;
  IJsonHandler* readUint64(uint64_t i) override;
  IJsonHandler* readDouble(double d) override;
  IJsonHandler* readString(const std::string_view& str) override;
  IJsonHandler* readObjectStart() override;
  IJsonHandler* readObjectKey(const std::string_view& str) override;
  IJsonHandler* readObjectEnd() override;
  IJsonHandler* readArrayStart() override;
  IJsonHandler* readArrayEnd() override;

  void reportWarning(
      const std::string& warning,
      std::vector<std::string>&& context = std::vector<std::string>()) override;

  IJsonHandler* parent() noexcept;

private:
  IJsonHandler* _pParent = nullptr;
  int32_t _depth = 0;
};
} // namespace CesiumJsonReader
