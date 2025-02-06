from os import path

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain
from conan.tools.files import load


class ProxyFmuConan(ConanFile):
    name = "proxyfmu"
    def set_version(self):
        self.version = load(self, path.join(self.recipe_folder, "version.txt")).strip()

    author = "osp"
    license = "MIT"
    url = "https://github.com/open-simulation-platform/proxy-fmu"

    package_type = "library"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False]
    }
    default_options = {
        "shared": True,
        "fPIC": True
    }

    generators = "CMakeDeps", "CMakeToolchain"
    exports = "version.txt"
    exports_sources = "*", "!cmake-build*", "!.venv", "!.git"

    def requirements(self):
        self.tool_requires("cmake/[>=3.15]")
        self.tool_requires("thrift/[~0.20]")
        self.requires("cli11/[~2.3]")
        self.requires("fmilibrary/[~2.3]")
        self.requires("boost/[~1.85]") # Required by Thrift
        self.requires("thrift/[~0.20]", transitive_headers=True)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["proxyfmu-client"]
        # Tell consumers to use "our" CMake package config file.
        self.cpp_info.builddirs.append(".")
        self.cpp_info.set_property("cmake_find_mode", "none")
