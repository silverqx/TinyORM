# Initialize Project Version
# ---

tiny_read_version(TINYUTILS_VERSION
    TINYUTILS_VERSION_MAJOR TINYUTILS_VERSION_MINOR TINYUTILS_VERSION_PATCH
    TINYUTILS_VERSION_TWEAK
    VERSION_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/version.hpp"
    PREFIX TINYUTILS
    HEADER_FOR "${TinyUtils_ns}"
)

# Basic project and CMake build setup
# ---

project(${TinyUtils_ns}
    DESCRIPTION "Utils library for TinyORM tests"
    LANGUAGES CXX
    VERSION ${TINYUTILS_VERSION}
)

# TinyUtils library header and source files
# ---

add_library(${TinyUtils_target}
    src/common/collection.hpp
    src/databases.hpp
    src/export.hpp
    src/fs.hpp
    src/macros.hpp
    src/version.hpp

    src/databases.cpp
    src/fs.cpp
)
add_library(${TinyOrm_ns}::${TinyUtils_target} ALIAS ${TinyUtils_target})

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

target_include_directories(${TinyUtils_target}
    PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)

# TinyUtils defines
# ---

target_compile_definitions(${TinyUtils_target}
    PRIVATE
        # Disable debug output in release mode
        $<$<NOT:$<CONFIG:Debug>>:QT_NO_DEBUG_OUTPUT>
        # To disable #pragma system_header if compiling TinyORM project itself
        TINYORM_PRAGMA_SYSTEM_HEADER_OFF
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${TinyUtils_target}
        PRIVATE
            TINYUTILS_BUILDING_SHARED
        INTERFACE
            TINYUTILS_LINKING_SHARED
    )
endif()

# Windows resource and manifest files
# ---

# Find Windows manifest file
if(MINGW)
    tiny_set_rc_flags("-I \"${PROJECT_SOURCE_DIR}/resources\"")

# To remove values from CMAKE_RC_FLAGS from the previous call
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    tiny_set_rc_flags()
endif()

tiny_resource_and_manifest(${TinyUtils_target}
    OUTPUT_DIR "${TINY_BUILD_GENDIR}/tmp/"
)

# Resolve and link dependencies
# ---

# Unconditional dependencies
target_link_libraries(${TinyUtils_target}
    PUBLIC
        # Never use versionless Qt targets
        Qt${QT_VERSION_MAJOR}::Core
        ${TinyOrm_ns}::${TinyOrm_target}
)

# Conditional dependencies
if(NOT STRICT_MODE)
    target_link_libraries(${TinyUtils_target}
        PRIVATE ${TinyOrm_ns}::${CommonConfig_target}
    )
endif()

if(BUILD_DRIVERS)
    target_link_libraries(${TinyUtils_target} PRIVATE range-v3::range-v3)
endif()

# Others
# ---

# Create an empty SQLite database file when does not exist
set(tiny_sqlite_db "${${TinyOrm_ns}_BINARY_DIR}/tests/tinyorm_test_1.sqlite3")
tiny_create_sqlite_db("${tiny_sqlite_db}")

# Add the SQLite database file file to the clean target (CMake doesn't have distclean)
set_property(TARGET ${TinyUtils_target}
    APPEND
        PROPERTY ADDITIONAL_CLEAN_FILES "${tiny_sqlite_db}"
)

target_compile_definitions(${TinyUtils_target}
    PRIVATE TINYORM_SQLITE_DATABASE="${tiny_sqlite_db}"
)
