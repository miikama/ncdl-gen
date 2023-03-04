

# Locate GTest
find_package(GTest REQUIRED)

add_executable(test_cases test_main.cpp
                          test_tokeniser.cpp
                          test_parser.cpp
                          test_common.cpp
                          test_types.cpp
                          test_netcdf_interface.cpp)

target_link_libraries(test_cases PRIVATE ncdlgen ${CONAN_LIBS})

include(GoogleTest)
gtest_discover_tests(test_cases)