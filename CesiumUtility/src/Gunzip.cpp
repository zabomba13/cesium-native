#include "CesiumUtility/Gunzip.h"

#include <gsl/span>
#include <zconf-ng.h>

#include <cstddef>
#include <cstdint>
#include <vector>
#define ZLIB_CONST
#include "zlib-ng.h"

constexpr uint32_t CHUNK = 65536;

bool CesiumUtility::isGzip(const gsl::span<const std::byte>& data) {
  if (data.size() < 3) {
    return false;
  }
  return data[0] == std::byte{31} && data[1] == std::byte{139};
}

bool CesiumUtility::gunzip(
    const gsl::span<const std::byte>& data,
    std::vector<std::byte>& out) {
  unsigned int index = 0;
  zng_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  int ret = zng_inflateInit2(&strm, 16 + MAX_WBITS);
  if (ret != Z_OK) {
    return false;
  }

  strm.avail_in = static_cast<uInt>(data.size());
  strm.next_in = reinterpret_cast<const Bytef*>(data.data());

  do {
    out.resize(index + CHUNK);
    strm.next_out = reinterpret_cast<Bytef*>(&out[index]);
    strm.avail_out = CHUNK;
    ret = zng_inflate(&strm, Z_NO_FLUSH);
    switch (ret) {
    case Z_NEED_DICT:
    case Z_DATA_ERROR:
    case Z_MEM_ERROR:
      zng_inflateEnd(&strm);
      return false;
    }
    index += CHUNK - strm.avail_out;
  } while (ret != Z_STREAM_END);

  zng_inflateEnd(&strm);
  out.resize(index);
  return true;
}
