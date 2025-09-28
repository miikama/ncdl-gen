


set(SOURCES
    tokeniser.cpp
    parser.cpp
    types.cpp
    syntax.cpp
    utils.cpp
    equality.cpp
    interfaces/vector_interface.cpp
    generator/generator.cpp
)

# only include public headers here
set(HEADERS
    equality.h
    interfaces/interface.h
    parser.h
    types.h
    logging.h
    syntax.h
    utils.h
    tokeniser.h
    interfaces/vector_interface.h
    generator/generator.h
    )


# Build netcdf files optionally (default = ON)
set(BUILD_NETCDF ON CACHE BOOL "Whether to build netcdf dependencies (True) or not (False)")
set(NETCDF_TARGET "")
if(BUILD_NETCDF)
    find_package(netCDF REQUIRED)
    set(NETCDF_TARGET "netCDF::netcdf")

    set(SOURCES ${SOURCES}
        pipes/netcdf_pipe.cpp
        wrappers/foo_wrapper.cpp
        )

    set(HEADERS ${HEADERS}
        pipes/netcdf_pipe.h
        )
endif()

# Build zeromq files optionally (default = ON)
set(BUILD_ZEROMQ ON CACHE BOOL "Whether to build zeromq dependencies (True) or not (False)")
set(ZEROMQ_TARGET "")
if(BUILD_ZEROMQ)
    find_package(cppzmq REQUIRED)
    set(ZEROMQ_TARGET "cppzmq")

    set(SOURCES ${SOURCES}
        pipes/zeromq_pipe.cpp
        )

    set(HEADERS ${HEADERS}
        pipes/zeromq_pipe.h
        )
endif()

# Create the ncdlgen library
add_library(ncdlgen ${SOURCES})

# Add dependencies to the library
find_package(fmt REQUIRED)
find_package(CLI11 REQUIRED)

# during building, headers are in the original directories
# during installation, all headers are put into a single directory
target_include_directories(ncdlgen PUBLIC
                                        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wrappers>
                                        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interfaces>
                                        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generator>
                                        $<INSTALL_INTERFACE:include>)
target_compile_features(ncdlgen PUBLIC cxx_std_17)
target_link_libraries(ncdlgen PUBLIC fmt::fmt ${NETCDF_TARGET} ${ZEROMQ_TARGET})

# Add parser executable
add_executable(parser main.cpp)
target_link_libraries(parser ncdlgen)

# Add generator executable
add_executable(generator generator/main.cpp)
target_link_libraries(generator PRIVATE ncdlgen CLI11::CLI11)

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
