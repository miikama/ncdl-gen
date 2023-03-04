


set(SOURCES
    tokeniser.cpp
    parser.cpp
    types.cpp
    syntax.cpp
    utils.cpp
    equality.cpp
    wrappers/foo_wrapper.cpp
)

add_library(ncdlgen ${SOURCES})

target_include_directories(ncdlgen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(ncdlgen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/wrappers)
target_compile_features(ncdlgen PUBLIC cxx_std_17)

# NOTE: when installing fmt through conan, the target is 'fmt' instead of 'fmt::fmt'
target_link_libraries(ncdlgen PRIVATE fmt)

add_executable(parser main.cpp)

target_link_libraries(parser ncdlgen)

