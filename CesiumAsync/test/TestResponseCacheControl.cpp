#include "ResponseCacheControl.h"

#include <CesiumAsync/HttpHeaders.h>

#include <catch2/catch_test_macros.hpp>

#include <optional>

using namespace CesiumAsync;

TEST_CASE("Test parsing cache-control header") {
  SECTION("Header has no cache-control header") {
    HttpHeaders responseHeader{
        {"Response-Header-1", "Response-Value-1"},
        {"Response-Header-2", "Response-Value-2"},
    };
    std::optional<ResponseCacheControl> cacheControl =
        ResponseCacheControl::parseFromResponseHeaders(responseHeader);
    REQUIRE(cacheControl == std::nullopt);
  }

  SECTION("Header has cache-control header") {
    HttpHeaders responseHeader{
        {"Cache-Control",
         "Must-Revalidate, No-Cache, No-Store, No-Transform, Public, Private, "
         "Proxy-Revalidate, Max-Age = 1000,  S-Maxage = 10"},
    };

    std::optional<ResponseCacheControl> cacheControl =
        ResponseCacheControl::parseFromResponseHeaders(responseHeader);
    REQUIRE(cacheControl != std::nullopt);

    if (!cacheControl) {
      return;
    }

    CHECK(cacheControl->mustRevalidate());
    CHECK(cacheControl->noCache());
    CHECK(cacheControl->noStore());
    CHECK(cacheControl->noTransform());
    CHECK(cacheControl->accessControlPublic());
    CHECK(cacheControl->accessControlPrivate());
    CHECK(cacheControl->proxyRevalidate());
    CHECK(cacheControl->maxAgeExists() == true);
    CHECK(cacheControl->maxAgeValue() == 1000);
    CHECK(cacheControl->sharedMaxAgeExists() == true);
    CHECK(cacheControl->sharedMaxAgeValue() == 10);
  }

  SECTION("Header has cache-control header with only some directive") {
    HttpHeaders responseHeader{
        {"Cache-Control",
         "Must-Revalidate, No-Cache, No-Store, Public, Private, Max-Age = "
         "1000,  S-Maxage = 10"},
    };

    std::optional<ResponseCacheControl> cacheControl =
        ResponseCacheControl::parseFromResponseHeaders(responseHeader);
    REQUIRE(cacheControl != std::nullopt);

    if (!cacheControl) {
      return;
    }

    CHECK(cacheControl->mustRevalidate());
    CHECK(cacheControl->noCache());
    CHECK(cacheControl->noStore());
    CHECK(cacheControl->noTransform() == false);
    CHECK(cacheControl->accessControlPublic());
    CHECK(cacheControl->accessControlPrivate());
    CHECK(cacheControl->proxyRevalidate() == false);
    CHECK(cacheControl->maxAgeExists() == true);
    CHECK(cacheControl->maxAgeValue() == 1000);
    CHECK(cacheControl->sharedMaxAgeExists() == true);
    CHECK(cacheControl->sharedMaxAgeValue() == 10);
  }
}
