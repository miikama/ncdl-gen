

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

# Make netcdf dependent test cases optional
if(BUILD_NETCDF)

    set(NETCDF_TESTS
        test_netcdf_pipe.cpp
        test_generator.cpp
        ${GENERATED_SOURCES}
        )
endif()

# Make zeromq dependent test cases optional
if(BUILD_ZEROMQ)

    set(ZEROMQ_TESTS
        test_zeromq_pipe.cpp
        ${GENERATED_SOURCES}
        )
endif()


# Locate GTest
find_package(GTest QUIET)
if (NOT GTest_FOUND)
    message(STATUS "Did not find GTest in Module mode, searching in config")
    find_package(GTest CONFIG REQUIRED)
endif()
message(STATUS "Found GTest ${GTest_FOUND}")

add_executable(test_cases test_main.cpp
               test_tokeniser.cpp
               test_parser.cpp
               test_common.cpp
               test_types.cpp
               test_vector_interface.cpp
               ${NETCDF_TESTS}
               ${ZEROMQ_TESTS}
               )
target_include_directories(test_cases PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


add_dependencies(test_cases generated-test-code)

target_link_libraries(test_cases PRIVATE ncdlgen GTest::gtest)

include(GoogleTest)
gtest_discover_tests(test_cases)