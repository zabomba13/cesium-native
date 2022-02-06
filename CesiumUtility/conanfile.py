# Do not edit this file! It is created by running "tools/automate.py create-recipes"

from conan import ConanFile
from conans import tools
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps

class CesiumUtilityConan(ConanFile):
    name = "CesiumUtility"
    version = "0.12.0"
    user = "kring"
    channel = "dev"
    license = "Apache-2.0"
    author = "CesiumGS, Inc. and Contributors"
    url = "https://github.com/CesiumGS/cesium-native"
    description = "Utility functions for JSON parsing, URI processing, etc."
    topics = () # TODO
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    requires = [
      "ms-gsl/4.0.0",
      "glm/0.9.9.8",
      "uriparser/0.9.6",
      "rapidjson/cci.20211112",
    ]

    exports_sources = [
      "include/*",
      "generated/*",
      "src/*",
      "test/*",
      "CMakeLists.txt",
      "../tools/cmake/cesium.cmake"
    ]

    def config_options(self):
      if self.settings.os == "Windows":
          del self.options.fPIC

    def generate(self):
      tc = CMakeToolchain(self)
      tc.variables["CESIUM_USE_CONAN_PACKAGES"] = True
      tc.variables["CESIUM_TESTS_ENABLED"] = False
      tc.generate()

      deps = CMakeDeps(self)
      deps.generate()

    def build(self):
      cmake = CMake(self)
      cmake.configure()
      cmake.build()
      # if self.develop:
      #   cmake.test()

    def package(self):
      cmake = CMake(self)
      cmake.install()

    def package_info(self):
      self.cpp_info.libs = tools.collect_libs(self)