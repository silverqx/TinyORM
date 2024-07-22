# Policies <= CMP0128 default to NEW
# 3.20 - because is used NEWLINE_STYLE in the file(GENERATE) command
# 3.22 - because is used CMP0127: cmake_dependent_option() supports full Condition Syntax
# Maximum version allowed for the minimum version is 3.22 because of Ubuntu 22.04 LTS so
# currently can't use the target_sources(FILE_SET) 😔
# Minimum version must always correspond to the version that is in the latest Ubuntu LTS
# TODO use LINKER_TYPE target property when min. version will be CMake v3.29 (Noble has 3.28.3 😭) silverqx
# TODO use CMAKE_COMPILE_WARNING_AS_ERROR when Ubuntu 24.04.1 will be out, in increases minimum version to CMake v3.24 silverqx
cmake_minimum_required(VERSION 3.22...3.30 FATAL_ERROR)

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

# Basic project and CMake build setup
# ---

# TODO add support for POSITION_INDEPENDENT_CODE silverqx
project(${TinyOrm_ns}
    DESCRIPTION "Modern C++ ORM library for Qt framework"
    HOMEPAGE_URL "https://www.tinyorm.org"
    LANGUAGES CXX
    VERSION ${TINY_VERSION}
)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)
# Set the AUTOMOC property explicitly only when needed (eg. unit tests need AUTOMOC)
set(CMAKE_AUTOMOC OFF)

# Version requirements
# ---
# Older versions may work, but you are on your own
# Req - requirement, throws error
# Rec - recommended, shows info message

# Compilers
# 16.10/16.11 (1929) to support pragma system_header
set(minReqMsvcVersion    19.29)
set(minRecGCCVersion     10.2)
set(minRecClangVersion   12)
set(minReqClangClVersion 14.0.3)
# Packages
set(minReqQtVersion      6.2)
# Unused
# As the range-v3 uses exact version policy in the package config file so passing version
# makes real problems on CI pipelines where different OS images can have installed
# different versions of range-v3 (doesn't matter if it's vcpkg or some package manager)
#set(minReqRangeV3Version  0.12.0)
# tabulate doesn't provide Package Version File
#set(minReqTabulateVersion 1.5.0)

# Make minimum toolchain version a requirement
include(TinyToolchainRequirement)
tiny_toolchain_requirement(
    MSVC     ${minReqMsvcVersion}
    CLANG_CL ${minReqClangClVersion}
    GCC      ${minRecGCCVersion}
    CLANG    ${minRecClangVersion}
    QT       ${minReqQtVersion}
)

# TinyORM build options
# ---

include(FeatureSummary)
include(TinyFeatureOptions)
include(TinyOptions)

# Initialize default CMake variables on which CMake options depend
tiny_init_cmake_variables_pre()

feature_option(BUILD_SHARED_LIBS
    "Build using shared libraries" ON
)
feature_option(BUILD_TESTS
    "Build TinyORM unit tests" OFF
)
feature_option(BUILD_TREE_DEPLOY
    "Copy TinyDrivers and TinyMySql libraries to the root of the build tree" ON
)

# TinyDrivers options
feature_option(BUILD_DRIVERS
    "Build TinyDrivers SQL database drivers (core/common code; replaces QtSql module)" OFF
)
feature_string_option_dependent(DRIVERS_TYPE
    "Shared;Loadable;Static"
    "How to build and link against TinyDrivers SQL database drivers (supported values: \
Shared, Loadable, and Static)" Shared
    "BUILD_DRIVERS AND BUILD_SHARED_LIBS" ${TINY_DRIVERS_TYPE_FORCE}
)
feature_option_dependent(BUILD_MYSQL_DRIVER
    "Build TinyDrivers MySQL database driver" ON
    "BUILD_DRIVERS" OFF
)

# Depends on tiny_init_cmake_variables_pre() call
feature_option_dependent(MATCH_EQUAL_EXPORTED_BUILDTREE
    "Exported package configuration from the build tree is considered to match only \
when the build type is equal" OFF
    "CMAKE_EXPORT_PACKAGE_REGISTRY AND NOT TINY_IS_MULTI_CONFIG" OFF
)

# The condition allows to use both mechanism to control dynamic/static MSVC CRT linkage,
# the CMAKE_MSVC_RUNTIME_LIBRARY or MSVC_RUNTIME_DYNAMIC
# Also this isn't allowed with vcpkg as it controls this using triplet (VCPKG_CRT_LINKAGE)
feature_option_dependent(MSVC_RUNTIME_DYNAMIC
    "Use MSVC dynamic runtime library (-MD) instead of static (-MT), also considers \
a Debug configuration (-MTd, -MDd)" ON
    "MSVC AND NOT TINY_VCPKG AND NOT DEFINED VCPKG_CRT_LINKAGE AND \
NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY"
    MSVC_RUNTIME_DYNAMIC-NOTFOUND
)

feature_option_environment(STRICT_MODE
    "Propagate strict compiler/linker options and Qt definitions using \
the ${TinyOrm_ns}::${CommonConfig_target} interface library to the user code \
(highly recommended; can also be set with the TINYORM_STRICT_MODE environment variable)"
    TINYORM_STRICT_MODE OFF
)

feature_option(VERBOSE_CONFIGURE
    "Show information about PACKAGES_FOUND and PACKAGES_NOT_FOUND in the configure \
output" OFF
)

# Initialize INLINE_CONSTANTS CMake feature dependent option
tiny_initialize_inline_constants_option()

mark_as_advanced(MATCH_EQUAL_EXPORTED_BUILDTREE)

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

# TinyORM library
# ---

add_library(${TinyOrm_target})
add_library(${TinyOrm_ns}::${TinyOrm_target} ALIAS ${TinyOrm_target})

# TinyORM build options - target is needed
# ---

target_optional_compile_definitions(${TinyOrm_target}
    PUBLIC
        FEATURE NAME MYSQL_PING
        DEFAULT OFF
        DESCRIPTION "Enable MySQL ping on Orm::MySqlConnection"
        ENABLED TINYORM_MYSQL_PING
)

target_optional_compile_definitions(${TinyOrm_target}
    PUBLIC
        ADVANCED FEATURE NAME DISABLE_THREAD_LOCAL
        DEFAULT OFF
        DESCRIPTION "Remove all the thread_local storage duration specifiers \
(disables multi-threading support)"
        ENABLED TINYORM_DISABLE_THREAD_LOCAL
)

target_optional_compile_definitions(${TinyOrm_target}
    PUBLIC
        FEATURE NAME ORM
        DEFAULT ON
        DESCRIPTION "Controls the compilation of all ORM-related source code, when this \
option is disabled, then only the query builder without ORM is compiled"
        DISABLED TINYORM_DISABLE_ORM
)

target_optional_compile_definitions(${TinyOrm_target}
    PUBLIC
        FEATURE NAME TOM
        DEFAULT ON
        DESCRIPTION "Controls the compilation of all Tom-related source code \
(command-line interface)"
        DISABLED TINYORM_DISABLE_TOM
)

# Depends on the TOM build option
feature_option_dependent(TOM_EXAMPLE
    "Build the Tom command-line application example" OFF
    "TOM" TOM_EXAMPLE-NOTFOUND
)

# Depends on the TOM_EXAMPLE build option
tiny_init_tom_database_dirs()

# TinyORM library header and source files
# ---

include(TinySources)
tinyorm_sources(${TinyOrm_target}_headers ${TinyOrm_target}_sources)

target_sources(${TinyOrm_target} PRIVATE
    ${${TinyOrm_target}_headers}
    ${${TinyOrm_target}_sources}
)

# Use Precompiled headers (PCH)
# ---

target_precompile_headers(${TinyOrm_target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"pch.h">)

if(NOT CMAKE_DISABLE_PRECOMPILE_HEADERS)
    target_compile_definitions(${TinyOrm_target} PRIVATE TINYORM_USING_PCH)
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

# Append a major version number for shared or static library (Windows/MinGW only)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # TODO use a new CMAKE_DLL_NAME_WITH_SOVERSION in CMake v3.27 silverqx
    set_property(
        TARGET ${TinyOrm_target}
        PROPERTY OUTPUT_NAME "${TinyOrm_target}${PROJECT_VERSION_MAJOR}"
    )
endif()

# Order of #include-s is very important because of pch.h as its name is the same for all
# projects, the ${PROJECT_SOURCE_DIR}/include must be as the first #include!
target_include_directories(${TinyOrm_target}
    PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

# TinyORM defines
# ---

target_compile_definitions(${TinyOrm_target}
    PUBLIC
        PROJECT_TINYORM
        # Release build
        $<$<NOT:$<CONFIG:Debug>>:TINYORM_NO_DEBUG>
        # Do not log queries
        $<$<NOT:$<CONFIG:Debug>>:TINYORM_NO_DEBUG_SQL>
        # Debug build
        $<$<CONFIG:Debug>:TINYORM_DEBUG>
        # Log queries with time measurements
        $<$<CONFIG:Debug>:TINYORM_DEBUG_SQL>
    PRIVATE
        # To disable #pragma system_header if compiling TinyORM project itself
        TINYORM_PRAGMA_SYSTEM_HEADER_OFF
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

# Using the TinyDrivers instead of QtSql drivers
if(BUILD_DRIVERS)
    target_compile_definitions(${TinyOrm_target} PUBLIC TINYORM_USING_TINYDRIVERS)
else()
    target_compile_definitions(${TinyOrm_target} PUBLIC TINYORM_USING_QTSQLDRIVERS)
endif()

# Enable code needed by tests, eg. connection overriding in the Model or
# MySqlConnection::setConfigVersion()
if(BUILD_TESTS)
    target_compile_definitions(${TinyOrm_target} PUBLIC TINYORM_TESTS_CODE)
endif()

# Tom related header and source files
# ---

if(TOM)
    tinytom_sources(${TomExample_target}_headers ${TomExample_target}_sources)

    target_sources(${TinyOrm_target} PRIVATE
        ${${TomExample_target}_headers}
        ${${TomExample_target}_sources}
    )
endif()

# Tom related specific configuration
# ---

if(TOM)
    target_include_directories(${TinyOrm_target}
        PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/tom/include>"
    )
endif()

# Tom related defines
# ---

if(TOM)
    target_compile_definitions(${TinyOrm_target}
        PUBLIC
            # Release build
            $<$<NOT:$<CONFIG:Debug>>:TINYTOM_NO_DEBUG>
            # Debug build
            $<$<CONFIG:Debug>:TINYTOM_DEBUG>
        PRIVATE
            # For the tom about command
            # TINYORM_ is correct, it can be potentially used somewhere else
            TINYORM_MSVC_RUNTIME_DYNAMIC=${MSVC_RUNTIME_DYNAMIC}
            # TINYTOM_ is correct, it's strictly for the tom about command only
            TINYTOM_CMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
            TINYORM_STRICT_MODE=${STRICT_MODE}
    )

    # Enable code needed by tests (modify the migrate:status command for tests need)
    if(BUILD_TESTS)
        target_compile_definitions(${TinyOrm_target} PUBLIC TINYTOM_TESTS_CODE)
    endif()

    # Set TINYORM_LTO macro based on the INTERPROCEDURAL_OPTIMIZATION target property
    tiny_set_lto_compile_definition(${TinyOrm_target})
endif()

if(TOM_EXAMPLE)
    target_compile_definitions(${TinyOrm_target}
        PUBLIC
            # Will be stringified in the tom/application.cpp
            TINYTOM_MIGRATIONS_DIR=${TOM_MIGRATIONS_DIR}
            # Will be stringified in the tom/application.cpp
            TINYTOM_MODELS_DIR=${TOM_MODELS_DIR}
            # Will be stringified in the tom/application.cpp
            TINYTOM_SEEDERS_DIR=${TOM_SEEDERS_DIR}
        PRIVATE
            # For the tom about command
            TINYORM_TOM_EXAMPLE
    )
endif()

# Windows resource and manifest files
# ---

# Find icons, orm/version.hpp, and Windows manifest file for MinGW
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    tiny_set_rc_flags("-I \"${PROJECT_SOURCE_DIR}/resources\"")
endif()

include(TinyResourceAndManifest)
tiny_resource_and_manifest(${TinyOrm_target}
    OUTPUT_DIR "${TINY_BUILD_GENDIR}/tmp/"
)

# Resolve and link dependencies
# ---

# Must be before the TinyCommon, to exclude WINVER for the MSYS2 Qt6 builds to avoid:
# 'WINVER' macro redefined [-Wmacro-redefined]
# Look also to the TinyCommon for conditional WINVER definition
# TinyQtComponentsRequired: Core and Sql (if NOT BUILD_DRIVERS)
# Leaving this find_package(QT NAMES) search pattern even if the Qt v5 support was removed
# because I like it and can be used with future Qt version
find_package(QT NAMES Qt6 REQUIRED COMPONENTS ${TinyQtComponentsRequired})
tiny_find_package(Qt${QT_VERSION_MAJOR} ${minReqQtVersion} CONFIG
    REQUIRED COMPONENTS ${TinyQtComponentsRequired}
)
tiny_find_package(range-v3 CONFIG REQUIRED)

# Common configuration as interface library
include(TinyCommon)
tiny_common(${CommonConfig_target}
    NAMESPACE ${TinyOrm_ns}
    EXPORT NAME ${CommonConfig_target}
)

# Unconditional dependencies
target_link_libraries(${TinyOrm_target}
    PUBLIC
        # Never use versionless Qt targets (as QT_NO_CREATE_VERSIONLESS_TARGETS might be
        # defined)
        Qt${QT_VERSION_MAJOR}::Core
        range-v3::range-v3
)

# Conditional dependencies
if(NOT BUILD_DRIVERS)
    target_link_libraries(${TinyOrm_target} PUBLIC Qt${QT_VERSION_MAJOR}::Sql)
endif()

if(STRICT_MODE)
    target_link_libraries(${TinyOrm_target}
        PUBLIC ${TinyOrm_ns}::${CommonConfig_target}
    )
else()
    target_link_libraries(${TinyOrm_target}
        PRIVATE ${TinyOrm_ns}::${CommonConfig_target}
    )
endif()

if(MYSQL_PING)
    tiny_find_and_link_mysql(${TinyOrm_target})
endif()

if(TOM)
    # tabulate doesn't provide Package Version File
    tiny_find_package(tabulate CONFIG REQUIRED)
    target_link_libraries(${TinyOrm_target} PUBLIC tabulate::tabulate)
endif()

# Build TinyDrivers
# ---

if(BUILD_DRIVERS)
    add_subdirectory(drivers)

    target_link_libraries(${TinyOrm_target}
        PUBLIC ${TinyDrivers_ns}::${TinyDrivers_target}
    )
endif()

# Build auto tests
# ---

if(BUILD_TESTS)
    enable_testing()
    find_package(Qt${QT_VERSION_MAJOR} ${minReqQtVersion} REQUIRED COMPONENTS Test)

    add_subdirectory(tests)
endif()

# Build examples
# ---

if(TOM_EXAMPLE)
    add_subdirectory(examples)
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

# Copy TinyDrivers and TinyMySql libraries to the root of the build tree
if(BUILD_TREE_DEPLOY)
    tiny_build_tree_deployment()
endif()

# Generate pkg-config file
#if(NOT MSVC)
#	generate_and_install_pkg_config_file(torrent-rasterbar libtorrent-rasterbar)
#endif()

# Some info output
# ---

# Set up package properties using the set_package_properties()
set_packages_properties()

if(VERBOSE_CONFIGURE)
    if(NOT TINY_IS_MULTI_CONFIG AND NOT TINY_BUILD_TYPE_LOWER STREQUAL "debug")
        message(STATUS "Disabled debug output and asserts")
    endif()

    feature_summary(WHAT ALL)
else()
    feature_summary(WHAT ENABLED_FEATURES DISABLED_FEATURES)
endif()
