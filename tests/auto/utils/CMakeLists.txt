# Initialize Project Version
# ---

tiny_read_version(TINYUTILS_VERSION
    TINYUTILS_VERSION_MAJOR TINYUTILS_VERSION_MINOR TINYUTILS_VERSION_PATCH
    TINYUTILS_VERSION_TWEAK
    VERSION_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/version.hpp"
    PREFIX TINYUTILS
    HEADER_FOR "${TinyUtils_ns}"
)

# Basic project and cmake build setup
# ---

project(${TinyUtils_ns}
    DESCRIPTION "Utils library for TinyORM tests"
    LANGUAGES CXX
    VERSION ${TINYUTILS_VERSION}
)

# TinyUtils library header and source files
# ---

add_library(${TinyUtils_target}
    src/databases.hpp
    src/fs.hpp
    src/utils_global.hpp
    src/version.hpp

    src/databases.cpp
    src/fs.cpp
)
add_library(${TinyOrm_ns}::${TinyUtils_target} ALIAS ${TinyUtils_target})

# Use Precompiled headers (PCH)
# ---

target_precompile_headers(${TinyUtils_target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"pch.h">)

if(NOT CMAKE_DISABLE_PRECOMPILE_HEADERS)
    target_compile_definitions(${TinyUtils_target} PRIVATE USING_PCH)
endif()

# TinyORM library specific configuration
# ---

set_target_properties(${TinyUtils_target}
    PROPERTIES
        C_VISIBILITY_PRESET "hidden"
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN YES
        VERSION ${PROJECT_VERSION}
        SOVERSION 0
)

# Append a major version number for shared or static library on Windows/MinGW only
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_property(
        TARGET ${TinyUtils_target}
        PROPERTY OUTPUT_NAME "${TinyUtils_target}${PROJECT_VERSION_MAJOR}"
    )
endif()

target_include_directories(${TinyUtils_target} PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)

# TinyUtils defines
# ---

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${TinyUtils_target} PRIVATE UTILS_BUILDING_SHARED)
endif()

# File version and Windows resource and manifest
# ---

tiny_resource_and_manifest(${TinyUtils_target} "${TINY_BUILD_GENDIR}/tmp/")

# Resolve and link dependencies
# ---

target_link_libraries(${TinyUtils_target}
    PUBLIC
        ${TinyOrm_ns}::${TinyOrm_target}
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
)

# Others
# ---

# Create an empty SQLite database file when does not exist
set(tiny_sqlite_db "${CMAKE_BINARY_DIR}/tests/q_tinyorm_test_1.sqlite3")
create_sqlite_db("${tiny_sqlite_db}")

target_compile_definitions(${TinyUtils_target}
    PRIVATE TINYORM_SQLITE_DATABASE="${tiny_sqlite_db}"
)
