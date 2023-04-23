# Installation configuration
# This creates ncdlgen::ncdlgen target used by downstream consumers
install(
    EXPORT ncdlgenTargets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ncdlgen
    NAMESPACE ncdlgen::
    FILE ncdlgenTargets.cmake
    )

# This creates ncdlgenConfig.cmake, used by downstream consumers
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/ncdlgenConfig.cmake.in
    "ncdlgenConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ncdlgen
    PATH_VARS
    CMAKE_INSTALL_LIBDIR
    )
        
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ncdlgenConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/ncdlgenConfig.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/ncdlgenConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/ncdlgen"
)