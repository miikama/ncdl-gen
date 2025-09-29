import os
from conan import ConanFile
from conan.tools.files import copy
from conan.tools.cmake import cmake_layout, CMakeToolchain, CMake, CMakeDeps


class NcdlGenConan(ConanFile):
    name = "ncdlgen"
    version = "0.3.0"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "with_testing": [True, False],
        "with_netcdf": [True, False],
        "with_zeromq": [True, False],
    }
    default_options = {
        "with_testing": False,
        "with_netcdf": True,
        "with_zeromq": True,
    }

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = (
        "CMakeLists.txt",
        "src/*",
        "include/*",
        "cmake/*",
    )

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("fmt/9.1.0")
        self.requires("cli11/2.4.2")

        if self.options.with_testing:
            self.requires("gtest/1.15.0")

        if self.options.with_netcdf:
            self.requires("netcdf/4.8.1")
        if self.options.with_zeromq:
            self.requires("cppzmq/4.10.0")

    def configure(self):
        if self.options.with_testing:
            self.options["gtest"].shared = True

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        tc = CMakeToolchain(self)
        tc.variables["BUILD_TESTING"] = self.options.with_testing
        tc.variables["BUILD_NETCDF"] = self.options.with_netcdf
        tc.variables["BUILD_ZEROMQ"] = self.options.with_zeromq
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(
            self,
            "LICENSE",
            dst=os.path.join(self.package_folder, "licenses"),
            src=self.source_folder,
        )
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["ncdlgen"]
        self.cpp_info.set_property("cmake_file_name", "ncdlgen")
        self.cpp_info.set_property("cmake_target_name", "ncdlgen::ncdlgen")
        self.cpp_info.set_property("pkg_config_name", "ncdlgen")
