


set(SOURCES
    tokeniser.cpp
    parser.cpp
    types.cpp
    syntax.cpp
    utils.cpp
    equality.cpp
    netcdf_interface.cpp
    wrappers/foo_wrapper.cpp
    generator/generator.cpp
)

add_library(ncdlgen ${SOURCES})

target_include_directories(ncdlgen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                          ${CMAKE_CURRENT_SOURCE_DIR}/wrappers
                                          ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
                                          ${CMAKE_CURRENT_SOURCE_DIR}/generator)
target_compile_features(ncdlgen PUBLIC cxx_std_17)

# NOTE: when installing fmt through conan, the target is 'fmt' instead of 'fmt::fmt'
target_link_libraries(ncdlgen PUBLIC ${CONAN_LIBS})

# Add parser executable
add_executable(parser main.cpp)
target_link_libraries(parser ncdlgen)

# Add generator executable
add_executable(generator generator/main.cpp)
target_link_libraries(generator PRIVATE ncdlgen)
