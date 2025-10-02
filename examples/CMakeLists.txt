cmake_minimum_required(VERSION 3.16)
project("ncdlgen-examples")

enable_language(CXX)

find_package(ncdlgen REQUIRED)

# Run generator to create test wrappers
# The 'generator' binary should be in PATH when compiling
add_custom_command(
                   OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/example_data.h
                   OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/example_data.cpp
                   COMMAND generator ${CMAKE_CURRENT_SOURCE_DIR}/example_data.cdl
                        --header
                        --interface_class_name example_data
                        --use_library_include
                        > ${CMAKE_CURRENT_SOURCE_DIR}/example_data.h
                   COMMAND generator ${CMAKE_CURRENT_SOURCE_DIR}/example_data.cdl
                        --source
                        --interface_class_name example_data
                        --use_library_include
                        > ${CMAKE_CURRENT_SOURCE_DIR}/example_data.cpp
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/example_data.cdl
                   VERBATIM
                   )
# Add dependency to generated code
add_custom_target(generated-test-code
                  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/example_data.h
                  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/example_data.cpp  )
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/example_data.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/example_data.cpp
                            PROPERTIES GENERATED TRUE)
set(GENERATED_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/example_data.cpp)


# Generate the example_data.nc from the example_data.cdl with ncgen
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/example.nc
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND ncgen -4 ${CMAKE_SOURCE_DIR}/example_data.cdl -o example.nc
)


add_executable(custom_parser custom_parser.cpp ${GENERATED_SOURCES} ${CMAKE_SOURCE_DIR}/example.nc)
target_link_libraries(custom_parser PRIVATE ncdlgen::ncdlgen)