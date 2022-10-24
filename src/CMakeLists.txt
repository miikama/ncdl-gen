


set(SOURCES parser.cpp)

add_library(ncdlgen ${SOURCES})

target_include_directories(ncdlgen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(ncdlgen PUBLIC cxx_std_17)


add_executable(parser main.cpp)

target_link_libraries(parser ncdlgen)

