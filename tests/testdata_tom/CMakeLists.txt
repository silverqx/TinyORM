# Tom migrations for unit tests
# ---

# Initialize Project Version
# ---

include(TinyHelpers)
tiny_read_version(TINY_VERSION
    TINY_VERSION_MAJOR TINY_VERSION_MINOR TINY_VERSION_PATCH TINY_VERSION_TWEAK
    VERSION_HEADER "${CMAKE_SOURCE_DIR}/tom/include/tom/version.hpp"
    PREFIX TINYTOM
    HEADER_FOR "${TomTestData_ns}"
)

# Basic project
# ---

project(${TomTestData_ns}
    DESCRIPTION "Tom console for TinyORM"
    HOMEPAGE_URL "https://www.tinyorm.org"
    LANGUAGES CXX
    VERSION ${TINY_VERSION}
)

# Tom migrations for unit tests
# ---

add_executable(${TomTestData_target}
    main.cpp
)
add_executable(${TomTestData_ns}::${TomTestData_target} ALIAS ${TomTestData_target})

# Tom migrations for unit tests specific configuration
# ---

set_target_properties(${TomTestData_target}
    PROPERTIES
        C_VISIBILITY_PRESET "hidden"
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN YES
        VERSION ${PROJECT_VERSION}
)

target_include_directories(${TomTestData_target} PRIVATE "database")

# Tom migrations for unit tests defines
# ---

target_compile_definitions(${TomTestData_target}
    PRIVATE
        PROJECT_TOM_TESTDATA
)

# Windows resource and manifest files
# ---

# Find icons, tom/version.hpp, and Windows manifest file for MinGW
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    tiny_set_rc_flags("-I \"${CMAKE_SOURCE_DIR}/tom/resources\"")
endif()

include(TinyResourceAndManifest)
tiny_resource_and_manifest(${TomTestData_target}
    OUTPUT_DIR "${TINY_BUILD_GENDIR}/tmp/"
    RESOURCES_DIR "${CMAKE_SOURCE_DIR}/tom/resources"
    RESOURCE_BASENAME ${Tom_target}
    MANIFEST_BASENAME ${Tom_target}
)

# Resolve and link dependencies
# ---

# Unconditional dependencies
target_link_libraries(${TomTestData_target} PRIVATE ${TinyOrm_ns}::${TinyOrm_target})
