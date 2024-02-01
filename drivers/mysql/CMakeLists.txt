# TinyMySql database driver
# ---
# All include and link dependencies can be PRIVATE because the TinyMySql library will be
# only used as a loadable shared library (aka. add_library(MODULE)), so we don't need to
# propagate these dependencies anywhere

# Initialize Project Version
# ---

include(TinyHelpers)
tiny_read_version(TINY_VERSION
    TINY_VERSION_MAJOR TINY_VERSION_MINOR TINY_VERSION_PATCH TINY_VERSION_TWEAK
    VERSION_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/orm/drivers/mysql/version.hpp"
    PREFIX TINYMYSQL
    HEADER_FOR "${TinyMySql_ns}"
)

# Basic project
# ---

project(${TinyMySql_ns}
    DESCRIPTION "MySQL driver for TinyORM library"
    HOMEPAGE_URL "https://www.tinyorm.org"
    LANGUAGES CXX
    VERSION ${TINY_VERSION}
)

# TinyMySql library
# ---

add_library(${TinyMySql_target} MODULE)
add_library(${TinyMySql_ns}::${TinyMySql_target} ALIAS ${TinyMySql_target})

# TinyMySql library header and source files
# ---

include(TinySources)
tinymysqldriver_sources(
    ${TinyMySql_target}_headers_private
    ${TinyMySql_target}_headers
    ${TinyMySql_target}_sources
)

target_sources(${TinyMySql_target} PRIVATE
    ${${TinyMySql_target}_headers_private}
    ${${TinyMySql_target}_headers}
    ${${TinyMySql_target}_sources}
)

# Use Precompiled headers (PCH)
# ---

target_precompile_headers(${TinyMySql_target}
    PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"pch.h">
)

if(NOT CMAKE_DISABLE_PRECOMPILE_HEADERS)
    target_compile_definitions(${TinyMySql_target} PRIVATE TINYMYSQL_USING_PCH)
endif()

# TinyMySql library specific configuration
# ---

set_target_properties(${TinyMySql_target}
    PROPERTIES
        C_VISIBILITY_PRESET "hidden"
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN YES
        VERSION ${PROJECT_VERSION}
        # Needed by the Compatible Interface Requirement for the project's major version
        VERSION_MAJOR ${PROJECT_VERSION_MAJOR}
        SOVERSION 0
        EXPORT_NAME ${TinyMySql_ns}
)

# Append a major version number for shared or static library (Windows/MinGW only)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # TODO use a new CMAKE_DLL_NAME_WITH_SOVERSION in CMake v3.27 silverqx
    set_property(
        TARGET ${TinyMySql_target}
        PROPERTY OUTPUT_NAME "${TinyMySql_target}${PROJECT_VERSION_MAJOR}"
    )
endif()

target_include_directories(${TinyMySql_target}
    PRIVATE
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include_private>"
        "$<BUILD_INTERFACE:${${TinyOrm_ns}_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${${TinyOrm_ns}_SOURCE_DIR}/drivers/common/include_private>"
)

# TinyMySql defines
# ---

target_compile_definitions(${TinyMySql_target}
    PUBLIC
        PROJECT_TINYMYSQL
    PRIVATE
        # Release build
        $<$<NOT:$<CONFIG:Debug>>:TINYDRIVERS_NO_DEBUG>
        # Debug build
        $<$<CONFIG:Debug>:TINYDRIVERS_DEBUG>
        # TinyMySql support these strict Qt macros
        QT_ASCII_CAST_WARNINGS
        QT_NO_CAST_FROM_ASCII
        # TinyMySql defines
        TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
)

target_compile_definitions(${TinyMySql_target}
    PRIVATE
        # TODO cmake uses target_EXPORTS, use cmake convention instead silverqx
        TINYDRIVERS_BUILDING_SHARED
)

# Specifies which global constant types will be used
if(TINY_EXTERN_CONSTANTS)
    target_compile_definitions(${TinyMySql_target} PRIVATE TINYDRIVERS_EXTERN_CONSTANTS)
else()
    target_compile_definitions(${TinyMySql_target} PRIVATE TINYDRIVERS_INLINE_CONSTANTS)
endif()

# Enable code needed by tests (not used)
if(BUILD_TESTS)
    target_compile_definitions(${TinyMySql_target} PRIVATE TINYDRIVERS_TESTS_CODE)
endif()

# Windows resource and manifest files
# ---

# Find icons, orm/version.hpp, and Windows manifest file for MinGW
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    tiny_set_rc_flags("-I \"${PROJECT_SOURCE_DIR}/resources\"")
endif()

include(TinyResourceAndManifest)
tiny_resource_and_manifest(${TinyMySql_target}
    OUTPUT_DIR "${TINY_BUILD_GENDIR}/tmp/"
)

# Resolve and link dependencies
# ---

# Must be before the TinyCommon, to exclude WINVER for the MSYS2 Qt6 builds to avoid:
# 'WINVER' macro redefined [-Wmacro-redefined]
# Look also to the TinyCommon for conditional WINVER definition
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
# No need to call the tiny_find_package() here as the core TinyOrm always depends on it
find_package(Qt${QT_VERSION_MAJOR} ${minQtVersion} CONFIG
    REQUIRED COMPONENTS Core
)
tiny_find_package(MySQL REQUIRED)

# Unconditional dependencies
target_link_libraries(${TinyMySql_target}
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        MySQL::MySQL
        ${TinyOrm_ns}::${CommonConfig_target}
        ${TinyDrivers_ns}::${TinyDrivers_target}
)
