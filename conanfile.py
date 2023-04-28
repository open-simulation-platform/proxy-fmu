import os

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import load


class ProxyFmuConan(ConanFile):
    name = "proxyfmu"
    author = "osp"
    license = "MIT"
    exports = "version.txt"
    exports_sources = "CMakeLists.txt", "version.txt", "src/*", "include/*", "cmake/*", "data*", "tests/*", "tool/*"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"
    requires = (
        "cli11/2.3.1",
        "fmilibrary/2.3",
        "thrift/0.13.0"
    )
    options = {
        "shared": [True, False]
    }
    default_options = {
        "shared": True
    }

    def set_version(self):
        self.version = load(self, os.path.join(self.recipe_folder, "version.txt")).strip()

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.cache_variables["BUILD_SHARED_LIBS"] = self.options.shared
        tc.cache_variables["CONAN_EXPORTED"] = True
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["proxyfmu-client"]
        self.cpp_info.defines = ["Boost_USE_STATIC_LIBS=ON"]
