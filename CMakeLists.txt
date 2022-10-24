
cmake_minimum_required(VERSION 3.16)
project("ncdl-gen")

enable_language(CXX)

add_subdirectory(src)


if (BUILD_TESTING)

    enable_testing()
    add_subdirectory(test)

endif (BUILD_TESTING)
