from conans import ConanFile, CMake, tools
from os import path


class ProxyFmuConan(ConanFile):
    name = "proxyfmu"
    author = "osp"
    license = "MIT"
    exports = "version.txt"
    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto"
    }
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    requires = (
        "boost/1.71.0",
        "fmilibrary/2.0.3",
        "thrift/0.13.0"
    )

    default_options = {
        "boost:shared": True
    }

    def set_version(self):
        self.version = tools.load(path.join(self.recipe_folder, "version.txt")).strip()

    def imports(self):
        self.copy("boost_unit_test_framework.dll", dst="bin", src="lib", keep_path=False)

    def configure_cmake(self):
        cmake = CMake(self)
        cmake.configure()
        return cmake

    def build(self):
        cmake = self.configure_cmake()
        cmake.build()

    def package(self):
        cmake = self.configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["proxyfmu-client"]
