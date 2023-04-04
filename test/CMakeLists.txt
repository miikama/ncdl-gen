

# Run generator to create test wrappers
add_custom_command(
                   OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/generated_simple.h
                   COMMAND generator ${CMAKE_SOURCE_DIR}/cdls/simple.cdl > ${CMAKE_CURRENT_SOURCE_DIR}/generated_simple.h
                   DEPENDS generator
                   DEPENDS ${CMAKE_SOURCE_DIR}/cdls/simple.cdl
                   VERBATIM                   
                   )
# Add dependency to generated code
add_custom_target(generated-test-code
                  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generated_simple.h  )
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/generated_simple.h PROPERTIES GENERATED TRUE)

# Locate GTest
find_package(GTest REQUIRED)

add_executable(test_cases test_main.cpp
               test_tokeniser.cpp
               test_parser.cpp
               test_common.cpp
               test_types.cpp
               test_netcdf_interface.cpp
               test_generator.cpp)
target_include_directories(test_cases PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


add_dependencies(test_cases generated-test-code)

target_link_libraries(test_cases PRIVATE ncdlgen ${CONAN_LIBS})

include(GoogleTest)
gtest_discover_tests(test_cases)