

# Locate GTest
find_package(GTest REQUIRED)

add_executable(test_cases test_main.cpp
                          test_tokeniser.cpp
                          test_parser.cpp
                          test_common.cpp)

target_include_directories(test_cases PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries(test_cases PRIVATE ncdlgen ${GTEST_LIBRARIES} pthread)


include(GoogleTest)
gtest_discover_tests(test_cases)