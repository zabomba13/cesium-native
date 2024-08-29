#include "CesiumUtility/SpanHelper.h"

#include <catch2/catch_test_macros.hpp>
#include <gsl/span>

#include <cstddef>
#include <cstdint>
#include <vector>

TEST_CASE("reintepretCastSpan") {
  std::vector<std::byte> data(7 * sizeof(float));
  auto* value = reinterpret_cast<float*>(data.data());
  value[0] = 0.0F;
  value[1] = 2.5F;
  value[2] = 1.0F;
  value[3] = 3.4F;
  value[4] = 0.7F;
  value[5] = 1.0F;
  value[6] = 2.9F;

  gsl::span<std::byte> byteView(data.data(), data.size());
  gsl::span<float> floatView =
      CesiumUtility::reintepretCastSpan<float>(byteView);
  REQUIRE(floatView.size() == 7);
  REQUIRE(floatView[0] == 0.0F);
  REQUIRE(floatView[1] == 2.5F);
  REQUIRE(floatView[2] == 1.0F);
  REQUIRE(floatView[3] == 3.4F);
  REQUIRE(floatView[4] == 0.7F);
  REQUIRE(floatView[5] == 1.0F);
  REQUIRE(floatView[6] == 2.9F);

  std::vector<int32_t> intData(1, -1);
  gsl::span<int32_t> intSpan = intData;
  gsl::span<uint32_t> uintSpan =
      CesiumUtility::reintepretCastSpan<uint32_t>(intSpan);
  REQUIRE(uintSpan.size() == 2);
  REQUIRE(uintSpan[0] == 1);
  REQUIRE(uintSpan[1] == uint32_t(-1));
}
