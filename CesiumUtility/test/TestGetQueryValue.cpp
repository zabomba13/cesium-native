#include "CesiumUtility/Uri.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace CesiumUtility;

TEST_CASE("Uri::getQueryValue") {
  std::string url = "https://example.com/?name=John&age=25";
  CHECK(Uri::getQueryValue(url, "name") == "John");
  CHECK(Uri::getQueryValue(url, "age") == std::to_string(25));
  CHECK(Uri::getQueryValue(url, "gender").empty());
  CHECK(Uri::getQueryValue(url, "").empty());
  CHECK(Uri::getQueryValue("", "name").empty());
  CHECK(
      Uri::getQueryValue(
          "https://example.com/?name=John&name=Jane&age=25",
          "name") == "John");
  CHECK(
      Uri::getQueryValue(
          "https://example.com/?name=John%20Doe&age=25",
          "name") == "John Doe");
  CHECK(Uri::getQueryValue("//example.com?value=1", "value") == "1");
}
