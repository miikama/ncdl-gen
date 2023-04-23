

# Run generator to create test wrappers
add_custom_command(
                   OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/generated_simple.h
                   OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/generated_simple.cpp
                   COMMAND generator ${CMAKE_SOURCE_DIR}/data/simple.cdl --header > ${CMAKE_CURRENT_SOURCE_DIR}/generated_simple.h
                   COMMAND generator ${CMAKE_SOURCE_DIR}/data/simple.cdl --source > ${CMAKE_CURRENT_SOURCE_DIR}/generated_simple.cpp
                   DEPENDS generator
                   DEPENDS ${CMAKE_SOURCE_DIR}/data/simple.cdl
                   VERBATIM                   
                   )
# Add dependency to generated code
add_custom_target(generated-test-code
                  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generated_simple.h
                  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generated_simple.cpp  )
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/generated_simple.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/generated_simple.cpp
                            PROPERTIES GENERATED TRUE)
set(GENERATED_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/generated_simple.cpp)

# Locate GTest
find_package(GTest REQUIRED)

add_executable(test_cases test_main.cpp
               test_tokeniser.cpp
               test_parser.cpp
               test_common.cpp
               test_types.cpp
               test_netcdf_interface.cpp
               test_generator.cpp
               ${GENERATED_SOURCES})
target_include_directories(test_cases PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


add_dependencies(test_cases generated-test-code)

target_link_libraries(test_cases PRIVATE ncdlgen GTest::GTest)

include(GoogleTest)
gtest_discover_tests(test_cases)