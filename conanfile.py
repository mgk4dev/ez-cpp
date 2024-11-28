from conan import ConanFile
from conan.tools.cmake import cmake_layout


class EzRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("benchmark/1.9.0")
        self.requires("boost/1.86.0")
        self.requires("gtest/1.15.0")
        self.requires("protobuf/5.27.0")
        self.requires("grpc/1.67.1")
        self.requires("qt/6.7.3")
        self.requires("libmount/[2.39.2]", override=True)


    def build_requirements(self):
        self.tool_requires("cmake/[>=3.29]")

    def layout(self):
        cmake_layout(self)
