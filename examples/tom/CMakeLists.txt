# Tom command-line example application
# ---

# Initialize Project Version
# ---

tiny_read_version(TINY_VERSION
    TINY_VERSION_MAJOR TINY_VERSION_MINOR TINY_VERSION_PATCH TINY_VERSION_TWEAK
    VERSION_HEADER "${${TinyOrm_ns}_SOURCE_DIR}/tom/include/tom/version.hpp"
    PREFIX TINYTOM
    HEADER_FOR "${TomExample_ns}"
)

# Tom command-line application
# ---

add_executable(${TomExample_target}
    main.cpp
)
add_executable(${TomExample_ns}::${TomExample_target} ALIAS ${TomExample_target})

# Tom example migrations and seeders header files
# ---

tiny_target_sources(${TomExample_target} PRIVATE
    PREFIX _tom_example_database
    BASE_DIR "${PROJECT_SOURCE_DIR}/tests/database"
)

# Tom command-line application specific configuration
# ---

set_target_properties(${TomExample_target}
    PROPERTIES
        C_VISIBILITY_PRESET "hidden"
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN YES
)

# Disable executable versioning for the vcpkg package manager on Linux
# The vcpkg doesn't handle executable symlinks correctly on Linux, it uses the file(COPY)
# without the FOLLOW_SYMLINK_CHAIN option so the package's bin/ folder ends up
# with the tom-0.6.0.0 file and the tom symlink is copied to the tools/tinyorm/ during
# the vcpkg_copy_tools()
# https://github.com/microsoft/vcpkg/issues/33551
# TODO vcpkg_copy_tools() doesn't handle executable symlinks correctly (mainly Linux), it uses file(COPY) without the FOLLOW_SYMLINK_CHAIN so the packages bin/ folder ends up with the tom-0.6.0.0 file and the tom symlink is copied to the tools/tinyorm/; so currently disabled for Linux platform silverqx
if(WIN32 OR NOT TINY_VCPKG)
    set_target_properties(${TomExample_target}
        PROPERTIES
            VERSION ${TINY_VERSION}
    )
endif()

# Tom command-line application defines
# ---

target_compile_definitions(${TomExample_target}
    PRIVATE
        PROJECT_TOM_EXAMPLE
        # To disable #pragma system_header if compiling TinyORM project itself
        TINYORM_PRAGMA_SYSTEM_HEADER_OFF
)

# Windows resource and manifest files
# ---

# Find icons, tom/version.hpp, and Windows manifest file for MinGW
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    tiny_set_rc_flags("-I \"${PROJECT_SOURCE_DIR}/tom/resources\"")
endif()

tiny_resource_and_manifest(${TomExample_target}
    OUTPUT_DIR "${TINY_BUILD_TMP_DIR}/"
    RESOURCES_DIR "${PROJECT_SOURCE_DIR}/tom/resources/"
)

# Resolve and link dependencies
# ---

if(NOT STRICT_MODE)
    target_link_libraries(${TomExample_target}
        PRIVATE ${TinyOrm_ns}::${CommonConfig_target}
    )
endif()

target_link_libraries(${TomExample_target} PRIVATE ${TinyOrm_ns}::${TinyOrm_target})

# Conditional dependencies
# vcpkg workaround bugfix for QtSql module and static builds (needed on Windows and Linux too, MSYS2
# doesn't support compiling libmysql).
# Source of this problem starts in the QtSql6 QMYSQL driver package config file at:
# packages/qtbase_x64-windows-static/share/Qt6Sql/Qt6QMYSQLDriverPluginTargets.cmake
# QMYSQL driver uses FindMySQL.cmake find module instead of the unofficial-libmysql vcpkg port.
# The libmysql provided by vcpkg is de-vendored so it doesn't contain OpenSSL, zstd, zlib inside.
# See https://github.com/microsoft/vcpkg/issues/38194#issuecomment-2059143576
if(TINY_VCPKG_QT_WITH_MYSQL)
    tiny_find_and_link_mysql(${TomExample_target})
endif()
