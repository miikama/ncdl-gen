


set(SOURCES
    tokeniser.cpp
    parser.cpp
    types.cpp
    syntax.cpp
    utils.cpp
    equality.cpp
    generator/generator.cpp
)

# only include public headers here
set(HEADERS
    equality.h
    interface.h
    interface_support.h
    parser.h
    types.h
    logging.h
    syntax.h
    utils.h
    tokeniser.h
    interfaces/vector_interface.h
    generator/generator.h
    )


add_library(ncdlgen ${SOURCES})

# dependencies
find_package(fmt REQUIRED)


# Build netcdf files optionally (default = ON)
set(BUILD_NETCDF ON CACHE BOOL "Whether to build netcdf dependencies (True) or not (False)")
set(NETCDF_TARGET "")
if(BUILD_NETCDF)
    find_package(netCDF REQUIRED)
    set(NETCDF_TARGET "netCDF::netcdf")

    set(SOURCES ${SOURCES}
        netcdf_interface.cpp
        wrappers/foo_wrapper.cpp
        )

    set(HEADERS ${HEADERS}
        netcdf_interface.h
        )
endif()


# during building, headers are in the original directories
# during installation, all headers are put into a single directory
target_include_directories(ncdlgen PUBLIC
                                        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wrappers>
                                        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interfaces>
                                        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generator>
                                        $<INSTALL_INTERFACE:include>)
target_compile_features(ncdlgen PUBLIC cxx_std_17)
target_link_libraries(ncdlgen PUBLIC fmt::fmt ${NETCDF_TARGET})

# Add parser executable
add_executable(parser main.cpp)
target_link_libraries(parser ncdlgen)

# Add generator executable
add_executable(generator generator/main.cpp)
target_link_libraries(generator PRIVATE ncdlgen)

# Add install target for parser and generator
include(GNUInstallDirs)
install(
    TARGETS parser generator
    EXPORT ncdlgenTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_BINDIR})

# install the ncdlgen library
install(
    TARGETS ncdlgen
    EXPORT ncdlgenTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

# install the ncdlgen headers
install(
    FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ncdlgen)
