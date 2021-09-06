# Policies <= CMP0126 default to NEW
cmake_minimum_required(VERSION 3.21...3.21 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CommonModules"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules"
)

# Basic project and cmake build setup
# ---

# TODO add support for POSITION_INDEPENDENT_CODE silverqx
project(TinyOrm
    DESCRIPTION "c++ ORM library for Qt framework"
    HOMEPAGE_URL "https://silverqx.github.io/TinyORM/"
    LANGUAGES CXX
    VERSION 0.1.0.0
)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)

# Version requirements - older vesions may work, but you are on your own
set(minMsvcVersion 19.28)
set(minQtVersion 5.15.2)
set(minRangeV3Version 0.11.0)

include(TinyHelpers)
# Make minimum toolchain version a requirement
tiny_toolchain_requirement(MSVC ${minMsvcVersion})

# TinyORM build options
# ---

include(FeatureSummary)
include(TinyFeatureOptions)

feature_option(BUILD_SHARED_LIBS
    "Build using shared libraries" ON)
feature_option(BUILD_TESTS
    "Build Qt tests" OFF)
#    "Build Qt tests" ON)
feature_option(EXPORT_PACKAGE_REGISTRY
    "Store the current build directory in the CMake User Package Registry" ON)
feature_option(MSVC_RUNTIME_DYNAMIC
    "Use MSVC dynamic runtime library (-MD) instead of static (-MT)" ON)
feature_option(PRECOMPILE_HEADERS
    "Enables support for the use of precompiled headers." ON)
feature_option(VERBOSE_CONFIGURE
    "Show information about PACKAGES_FOUND and PACKAGES_NOT_FOUND in the configure \
output" OFF)

mark_as_advanced(EXPORT_PACKAGE_REGISTRY)

# MSVC_PARALLEL option
include(TinyMsvcParallel)
tiny_msvc_parallel("\
Enables /MP flag for parallel builds using MSVC. Specify an \
integer value to control the number of threads used (Only \
works on some older versions of Visual Studio). Setting to \
ON lets the toolchain decide how many threads to use. Set to \
OFF to disable /MP completely.")

# Initialize Tiny and default cmake variables
include(TinyInitDefaultVariables)
tiny_init_cmake_variables()
tiny_init_tiny_variables()

# TinyORM library header and source files
# ---

include(TinySources)
tiny_sources()

set(TinyOrm_target TinyOrm)
add_library(${TinyOrm_target}
    ${headers}
    ${sources}
)
add_library(TinyOrm::TinyOrm ALIAS ${TinyOrm_target})

target_compile_features(${TinyOrm_target} PUBLIC cxx_std_20)

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

if(PRECOMPILE_HEADERS)
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
        EXPORT_NAME TinyOrm
)

# Append a major version number for Windows shared libraries only
tiny_init_target_version_ext(${TinyOrm_target})
if(${TinyOrm_target}_VERSION_EXT)
    set_property(
        TARGET ${TinyOrm_target}
        PROPERTY OUTPUT_NAME "${TinyOrm_target}${PROJECT_VERSION_MAJOR}"
    )
endif()

target_include_directories(${TinyOrm_target} PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>"
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
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

# Enable code needed by tests, eg. connection overriding in the Model
if(BUILD_TESTS)
    target_compile_definitions(${TinyOrm_target} PUBLIC TINYORM_TESTS_CODE)
endif()

# File version and Windows resource and manifest
# ---

include(TinyFileVersionAndManifest)
tiny_file_version_and_manifest(${TinyOrm_target} "include/orm/")

# Resolve and link dependencies
# ---

# Common configuration as interface library
include(TinyQtCommon)
set(CommonConfig_target CommonConfig)
tiny_qt_common(${CommonConfig_target}
    NAMESPACE TinyOrm
    EXPORT NAME CommonConfig
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
        TinyOrm::CommonConfig
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
tiny_generate_find_dependency_calls()

# Create Package Config and Config Package Version files and install the TinyORM Project
tiny_install_tinyorm()

# Create Package Config and Package Config Version files for the Build Tree and export it
tiny_export_build_tree()

# Generate pkg-config file
#if (NOT MSVC)
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
    # TODO test with CMAKE_CONFIGURATION_TYPES, multi config generators silverqx
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Disabling debug output")
    endif()

    feature_summary(WHAT ALL)
else()
    feature_summary(WHAT ENABLED_FEATURES DISABLED_FEATURES)
endif()
