
cmake_minimum_required(VERSION 3.10)
project("ncdlgen" VERSION 0.3.1)

enable_language(CXX)

add_subdirectory(src)

if (BUILD_TESTING)

    enable_testing()
    add_subdirectory(test)

endif (BUILD_TESTING)

# Export ncdlgen::ncdlgen target used by downstream consumers
include(cmake/ncdlgenPackaging.cmake)
