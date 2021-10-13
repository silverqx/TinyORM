# Policies <= CMP0126 default to NEW
# 3.20 - because NEWLINE_STYLE is used in the file(GENERATE) command
cmake_minimum_required(VERSION 3.20...3.21 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CommonModules"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules"
)

# Initialize Project Version
# ---

include(TinyHelpers)
include(TinyInitDefaultVariables)

# Initialize Tiny variables, early init.
tiny_init_tiny_variables_pre()

tiny_read_version(TINY_VERSION
    TINY_VERSION_MAJOR TINY_VERSION_MINOR TINY_VERSION_PATCH TINY_VERSION_TWEAK
    VERSION_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/orm/version.hpp"
    PREFIX TINYORM
    HEADER_FOR "${TinyOrm_ns}"
)

# Basic project and cmake build setup
# ---

# TODO add support for POSITION_INDEPENDENT_CODE silverqx
project(${TinyOrm_ns}
    DESCRIPTION "c++ ORM library for Qt framework"
    HOMEPAGE_URL "https://silverqx.github.io/TinyORM/"
    LANGUAGES CXX
    VERSION ${TINY_VERSION}
)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)

# Version requirements
# ---
# Older vesions may work, but you are on your own
# Req - requirement, throws error
# Rec - recommended, shows message

# Compilers
# 16.10/16.11 (1929) to support pragma system_header
set(minReqMsvcVersion  19.29)
set(minRecGCCVersion   10.2)
set(minRecClangVersion 12)
# Packages
set(minQtVersion      5.15.2)
set(minRangeV3Version 0.11.0)

# Make minimum toolchain version a requirement
tiny_toolchain_requirement(
    MSVC ${minReqMsvcVersion}
    GCC ${minRecGCCVersion}
    CLANG ${minRecClangVersion}
)

# TinyORM build options
# ---

include(FeatureSummary)
include(TinyFeatureOptions)

# Initialize default CMake variables on which options depend
tiny_init_cmake_variables_pre()

feature_option(BUILD_SHARED_LIBS
    "Build using shared libraries" ON
)
feature_option(BUILD_TESTS
    "Build Qt tests" OFF
)
# TODO make it also dependend on NOT isMultiConfig silverqx
# Depends on tiny_init_cmake_variables_pre() call
feature_option_dependent(MATCH_EQUAL_EXPORTED_BUILDTREE
    "Exported package configuration from the build tree is considered to match only \
when the build type is equal" OFF
    CMAKE_EXPORT_PACKAGE_REGISTRY OFF
)
feature_option_dependent(MSVC_RUNTIME_DYNAMIC
    "Use MSVC dynamic runtime library (-MD) instead of static (-MT), also considers \
a Debug configuration (-MTd, -MDd)" ON
    "MSVC;NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY" MSVC_RUNTIME_DYNAMIC-NOTFOUND
)
feature_option(VERBOSE_CONFIGURE
    "Show information about PACKAGES_FOUND and PACKAGES_NOT_FOUND in the configure \
output" OFF
)
# MinGW clang shared build crashes with inline constants
if(NOT MINGW OR NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR NOT BUILD_SHARED_LIBS)
    feature_option_dependent(INLINE_CONSTANTS
        "Use inline constants instead of extern constants in the shared build. \
OFF is highly recommended for the shared build; is always ON for the static build" OFF
        "BUILD_SHARED_LIBS" ON
    )
endif()

mark_as_advanced(MATCH_EQUAL_EXPORTED_BUILDTREE)

# TODO so this doesn't work, anyway it has to be refactored, use target_compile_options() instead of string(APPEND CMAKE_C_FLAGS silverqx
# TODO make it dependend on cmake_disable_precompile_headers and MSVC silverqx
# MSVC_PARALLEL option
include(TinyMsvcParallel)
tiny_msvc_parallel("\
Enables /MP flag for parallel builds using MSVC. Specify an \
integer value to control the number of threads used (Only \
works on some older versions of Visual Studio). Setting to \
ON lets the toolchain decide how many threads to use. Set to \
OFF to disable /MP completely.")

# Initialize Tiny and default CMake variables
tiny_init_cmake_variables()
tiny_init_tiny_variables()

# Throw a fatal error for unsupported environments
tiny_check_unsupported_build()

# TinyORM library header and source files
# ---

include(TinySources)
tiny_sources(${TinyOrm_target}_headers ${TinyOrm_target}_sources)

add_library(${TinyOrm_target}
    ${${TinyOrm_target}_headers}
    ${${TinyOrm_target}_sources}
)
add_library(${TinyOrm_ns}::${TinyOrm_target} ALIAS ${TinyOrm_target})

# TinyORM build options - target is needed
# ---

target_optional_compile_definitions(${TinyOrm_target}
    PUBLIC
        FEATURE NAME MYSQL_PING DEFAULT OFF
        DESCRIPTION "Enable MySQL ping on Orm::MySqlConnection"
        ENABLED TINYORM_MYSQL_PING
)

# Use Precompiled headers (PCH)
# ---

target_precompile_headers(${TinyOrm_target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"pch.h">)

if(NOT CMAKE_DISABLE_PRECOMPILE_HEADERS)
    target_compile_definitions(${TinyOrm_target} PRIVATE USING_PCH)
endif()

# TinyORM library specific configuration
# ---

set_target_properties(${TinyOrm_target}
    PROPERTIES
        C_VISIBILITY_PRESET "hidden"
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN YES
        VERSION ${PROJECT_VERSION}
        SOVERSION 0
        EXPORT_NAME ${TinyOrm_ns}
)

# Append a major version number for shared or static library on Windows/MinGW only
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_property(
        TARGET ${TinyOrm_target}
        PROPERTY OUTPUT_NAME "${TinyOrm_target}${PROJECT_VERSION_MAJOR}"
    )
endif()

target_include_directories(${TinyOrm_target} PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

# TinyORM defines
# ---

target_compile_definitions(${TinyOrm_target}
    PUBLIC
        PROJECT_TINYORM
        # Log queries with a time measurement
        TINYORM_DEBUG_SQL
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${TinyOrm_target}
        PRIVATE
            # TODO cmake uses target_EXPORTS, use cmake convention instead silverqx
            TINYORM_BUILDING_SHARED
        INTERFACE
            TINYORM_LINKING_SHARED
    )
endif()

# Specifies which global constant types will be used
if(TINY_EXTERN_CONSTANTS)
    target_compile_definitions(${TinyOrm_target} PUBLIC TINYORM_EXTERN_CONSTANTS)
else()
    target_compile_definitions(${TinyOrm_target} PUBLIC TINYORM_INLINE_CONSTANTS)
endif()

# Enable code needed by tests, eg. connection overriding in the Model
if(BUILD_TESTS)
    target_compile_definitions(${TinyOrm_target} PUBLIC TINYORM_TESTS_CODE)
endif()

# Windows resource and manifest files
# ---

# Find icons, orm/version.hpp, and Windows manifest file for MinGW
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    tiny_set_rc_flags("-I \"${CMAKE_CURRENT_SOURCE_DIR}/resources\"")
endif()

include(TinyResourceAndManifest)
tiny_resource_and_manifest(${TinyOrm_target}
    OUTPUT_DIR "${TINY_BUILD_GENDIR}/tmp/"
)

# Resolve and link dependencies
# ---

# Common configuration as interface library
include(TinyQtCommon)
tiny_qt_common(${CommonConfig_target}
    NAMESPACE ${TinyOrm_ns}
    EXPORT NAME ${CommonConfig_target}
)

# Unconditional dependencies
find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core Sql)
tiny_find_package(Qt${QT_VERSION_MAJOR} ${minQtVersion} CONFIG
    REQUIRED COMPONENTS Core Sql
)
tiny_find_package(range-v3 ${minRangeV3Version} CONFIG REQUIRED)

target_link_libraries(${TinyOrm_target}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Sql
        range-v3::range-v3
        ${TinyOrm_ns}::${CommonConfig_target}
)

# Conditional dependencies
if(MYSQL_PING)
    tiny_find_package(MySQL REQUIRED)
    target_link_libraries(${TinyOrm_target} PRIVATE MySQL::MySQL)
endif()

# Build auto tests
# ---

if(BUILD_TESTS)
    enable_testing()
    find_package(Qt${QT_VERSION_MAJOR} ${minQtVersion} REQUIRED COMPONENTS Test)

    add_subdirectory(tests)
endif()

# Deployment
# ---

include(TinyDeployment)

# Generate find_dependency calls for the TinyORM package config file
tiny_generate_find_dependency_calls(tiny_find_dependency_calls)

# Create Package Config and Config Package Version files and install the TinyORM Project
tiny_install_tinyorm()

# Create Package Config and Package Config Version files for the Build Tree and export it
tiny_export_build_tree()

# Generate pkg-config file
#if(NOT MSVC)
#	generate_and_install_pkg_config_file(torrent-rasterbar libtorrent-rasterbar)
#endif()

# Some info output
# ---

set_package_properties(QT
    PROPERTIES
        URL "https://doc.qt.io/qt-${QT_VERSION_MAJOR}/"
        DESCRIPTION "Qt is a full development framework"
        TYPE REQUIRED
        PURPOSE "Provides SQL database layer by the QtSql module, QVariant, and QString"
)
set_package_properties(Qt${QT_VERSION_MAJOR}
    PROPERTIES
        URL "https://doc.qt.io/qt-${QT_VERSION_MAJOR}/"
        DESCRIPTION "Qt is a full development framework"
        TYPE REQUIRED
        PURPOSE "Provides SQL database layer by the QtSql module, QVariant, and QString"
)
set_package_properties(Qt${QT_VERSION_MAJOR}Core
    PROPERTIES
        URL "https://doc.qt.io/qt-${QT_VERSION_MAJOR}/qtcore-index.html"
        DESCRIPTION "Core non-graphical classes used by other modules"
        TYPE REQUIRED
        PURPOSE "Provides QVariant, QString, and Qt containers"
)
set_package_properties(Qt${QT_VERSION_MAJOR}Sql
    PROPERTIES
        URL "https://doc.qt.io/qt-${QT_VERSION_MAJOR}/qtsql-index.html"
        DESCRIPTION "Classes for database integration using SQL"
        TYPE REQUIRED
        PURPOSE "Provides SQL database layer"
)
set_package_properties(range-v3
    PROPERTIES
        URL "https://ericniebler.github.io/range-v3/"
        DESCRIPTION "Range algorithms, views, and actions for STL"
        TYPE REQUIRED
        PURPOSE "Used to have a nice and clear code"
)
if(MYSQL_PING)
    set_package_properties(MySQL
        PROPERTIES
            TYPE REQUIRED
            PURPOSE "Provides MySQL ping, enables MySqlConnection::pingDatabase()"
    )
endif()

if(VERBOSE_CONFIGURE)
    if(NOT TINY_IS_MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Disabling debug output")
    endif()

    feature_summary(WHAT ALL)
else()
    feature_summary(WHAT ENABLED_FEATURES DISABLED_FEATURES)
endif()
