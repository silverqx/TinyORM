# TinyDrivers database drivers - core/common code
# ---

# Initialize Project Version
# ---

include(TinyHelpers)
tiny_read_version(TINY_VERSION
    TINY_VERSION_MAJOR TINY_VERSION_MINOR TINY_VERSION_PATCH TINY_VERSION_TWEAK
    VERSION_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/orm/drivers/version.hpp"
    PREFIX TINYDRIVERS
    HEADER_FOR "${TinyDrivers_ns}"
)

# Basic project
# ---

project(${TinyDrivers_ns}
    DESCRIPTION "Database drivers for TinyORM library"
    HOMEPAGE_URL "https://www.tinyorm.org"
    LANGUAGES CXX
    VERSION ${TINY_VERSION}
)

# TinyDrivers library
# ---

add_library(${TinyDrivers_target} ${TINY_DRIVERS_LIBRARY_TYPE})
add_library(${TinyDrivers_ns}::${TinyDrivers_target} ALIAS ${TinyDrivers_target})

# TinyDrivers library header and source files
# ---

include(TinySources)
tinydrivers_sources(
    ${TinyDrivers_target}_headers_private
    ${TinyDrivers_target}_headers
    ${TinyDrivers_target}_sources
)

target_sources(${TinyDrivers_target} PRIVATE
    ${${TinyDrivers_target}_headers_private}
    ${${TinyDrivers_target}_headers}
    ${${TinyDrivers_target}_sources}
)

# Using source files directly to avoid unnecessary static library build and linking
if(NOT TINY_BUILD_LOADABLE_DRIVERS AND BUILD_MYSQL_DRIVER)
    tinymysqldriver_sources(
        ${TinyMySql_target}_headers_private
        ${TinyMySql_target}_headers
        ${TinyMySql_target}_sources
    )

    target_sources(${TinyDrivers_target} PRIVATE
        ${${TinyMySql_target}_headers_private}
        ${${TinyMySql_target}_headers}
        ${${TinyMySql_target}_sources}
    )
endif()

# Use Precompiled headers (PCH)
# ---

target_precompile_headers(${TinyDrivers_target}
    PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"pch.h">
)

if(NOT CMAKE_DISABLE_PRECOMPILE_HEADERS)
    target_compile_definitions(${TinyDrivers_target} PRIVATE TINYDRIVERS_USING_PCH)
endif()

# TinyDrivers library specific configuration
# ---

set_target_properties(${TinyDrivers_target}
    PROPERTIES
        C_VISIBILITY_PRESET "hidden"
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN YES
        VERSION ${PROJECT_VERSION}
        SOVERSION 0
        EXPORT_NAME ${TinyDrivers_ns}
)

# Append a major version number for shared or static library (Windows/MinGW only)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # TODO use a new CMAKE_DLL_NAME_WITH_SOVERSION in CMake v3.27 silverqx
    set_property(
        TARGET ${TinyDrivers_target}
        PROPERTY OUTPUT_NAME "${TinyDrivers_target}${PROJECT_VERSION_MAJOR}"
    )
endif()

target_include_directories(${TinyDrivers_target}
    PUBLIC
        "$<BUILD_INTERFACE:${${TinyOrm_ns}_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    PRIVATE
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include_private>"
)

if(NOT TINY_BUILD_LOADABLE_DRIVERS AND BUILD_MYSQL_DRIVER)
    target_include_directories(${TinyDrivers_target}
        PRIVATE
            "$<BUILD_INTERFACE:${${TinyOrm_ns}_SOURCE_DIR}/drivers/mysql/include>"
            "$<BUILD_INTERFACE:${${TinyOrm_ns}_SOURCE_DIR}/drivers/mysql/include_private>"
    )
endif()

# TinyDrivers defines
# ---

# No need to set these _NO/_DEBUG macros as PUBLIC because they are used in cpp files only
target_compile_definitions(${TinyDrivers_target}
    PUBLIC
        PROJECT_TINYDRIVERS
    PRIVATE
        # Release build
        $<$<NOT:$<CONFIG:Debug>>:TINYDRIVERS_NO_DEBUG>
        # Debug build
        $<$<CONFIG:Debug>:TINYDRIVERS_DEBUG>
        # TinyDrivers support these strict Qt macros
        QT_ASCII_CAST_WARNINGS
        QT_NO_CAST_FROM_ASCII
)

if(TINY_BUILD_SHARED_DRIVERS OR TINY_BUILD_LOADABLE_DRIVERS)
    target_compile_definitions(${TinyDrivers_target}
        PRIVATE
            # TODO cmake uses target_EXPORTS, use cmake convention instead silverqx
            TINYDRIVERS_BUILDING_SHARED
        INTERFACE
            TINYDRIVERS_LINKING_SHARED
    )
endif()

# Specifies which global constant types will be used
if(TINY_EXTERN_CONSTANTS)
    target_compile_definitions(${TinyDrivers_target} PRIVATE TINYDRIVERS_EXTERN_CONSTANTS)
else()
    target_compile_definitions(${TinyDrivers_target} PRIVATE TINYDRIVERS_INLINE_CONSTANTS)
endif()

if(TINY_BUILD_LOADABLE_DRIVERS AND BUILD_MYSQL_DRIVER)
    target_compile_definitions(${TinyDrivers_target}
        PRIVATE
            TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
            # Don't user quotes around the path here
            TINYDRIVERS_MYSQL_PATH=${CMAKE_BINARY_DIR}/drivers/mysql/
    )
endif()

# Enable code needed by tests (not used)
if(BUILD_TESTS)
    target_compile_definitions(${TinyDrivers_target} PRIVATE TINYDRIVERS_TESTS_CODE)
endif()

# Windows resource and manifest files
# ---

# Find icons, orm/version.hpp, and Windows manifest file for MinGW
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    tiny_set_rc_flags("-I \"${PROJECT_SOURCE_DIR}/resources\"")
endif()

include(TinyResourceAndManifest)
tiny_resource_and_manifest(${TinyDrivers_target}
    OUTPUT_DIR "${TINY_BUILD_GENDIR}/tmp/"
)

# Resolve and link dependencies
# ---

# Must be before the TinyCommon, to exclude WINVER for the MSYS2 Qt6 builds to avoid:
# 'WINVER' macro redefined [-Wmacro-redefined]
# Look also to the TinyCommon for conditional WINVER definition
find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
tiny_find_package(Qt${QT_VERSION_MAJOR} ${minQtVersion} CONFIG
    REQUIRED COMPONENTS Core
)

# Unconditional dependencies
target_link_libraries(${TinyDrivers_target} PUBLIC Qt${QT_VERSION_MAJOR}::Core)

# Conditional dependencies
if(STRICT_MODE)
    target_link_libraries(${TinyDrivers_target}
        PUBLIC ${TinyOrm_ns}::${CommonConfig_target}
    )
else()
    target_link_libraries(${TinyDrivers_target}
        PRIVATE ${TinyOrm_ns}::${CommonConfig_target}
    )
endif()

# The PRIVATE MySQL dependency is propagated for TinyDrivers library static build
if(NOT TINY_BUILD_LOADABLE_DRIVERS AND BUILD_MYSQL_DRIVER)
    tiny_find_package(MySQL REQUIRED)
    target_link_libraries(${TinyDrivers_target} PRIVATE MySQL::MySQL)
endif()