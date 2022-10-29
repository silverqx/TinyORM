# Tom command-line application example
# ---

# Initialize Project Version
# ---

include(TinyHelpers)
tiny_read_version(TINY_VERSION
    TINY_VERSION_MAJOR TINY_VERSION_MINOR TINY_VERSION_PATCH TINY_VERSION_TWEAK
    VERSION_HEADER "${${TinyOrm_ns}_SOURCE_DIR}/tom/include/tom/version.hpp"
    PREFIX TINYTOM
    HEADER_FOR "${TomExample_ns}"
)

# Basic project
# ---

project(${TomExample_ns}
    DESCRIPTION "Tom console for TinyORM"
    HOMEPAGE_URL "https://www.tinyorm.org"
    LANGUAGES CXX
    VERSION ${TINY_VERSION}
)

# Tom command-line application
# ---

add_executable(${TomExample_target}
    main.cpp
)
add_executable(${TomExample_ns}::${TomExample_target} ALIAS ${TomExample_target})

# Tom example migrations and seeders header files
# ---

include(TinySources)
tiny_tom_example_database_sources(${TomExample_target}_headers)

target_sources(${TomExample_target} PRIVATE
    ${${TomExample_target}_headers}
)

# Tom command-line application specific configuration
# ---

set_target_properties(${TomExample_target}
    PROPERTIES
        C_VISIBILITY_PRESET "hidden"
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN YES
        VERSION ${PROJECT_VERSION}
)

target_include_directories(${TomExample_target} PRIVATE
    "${${TinyOrm_ns}_SOURCE_DIR}/tests/database"
)

# Tom command-line application defines
# ---

target_compile_definitions(${TomExample_target}
    PRIVATE
        PROJECT_TOMEXAMPLE
)

# Windows resource and manifest files
# ---

# Find icons, tom/version.hpp, and Windows manifest file for MinGW
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    tiny_set_rc_flags("-I \"${${TinyOrm_ns}_SOURCE_DIR}/tom/resources\"")
endif()

include(TinyResourceAndManifest)
tiny_resource_and_manifest(${TomExample_target}
    OUTPUT_DIR "${TINY_BUILD_GENDIR}/tmp/"
    RESOURCES_DIR "${${TinyOrm_ns}_SOURCE_DIR}/tom/resources"
)

# Resolve and link dependencies
# ---

# Unconditional dependencies
target_link_libraries(${TomExample_target} PRIVATE ${TinyOrm_ns}::${TinyOrm_target})
