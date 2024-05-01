# Tom migrations for unit tests
# ---

# Initialize Project Version
# ---

include(TinyHelpers)
tiny_read_version(TINY_VERSION
    TINY_VERSION_MAJOR TINY_VERSION_MINOR TINY_VERSION_PATCH TINY_VERSION_TWEAK
    VERSION_HEADER "${${TinyOrm_ns}_SOURCE_DIR}/tom/include/tom/version.hpp"
    PREFIX TINYTOM
    HEADER_FOR "${TomTestData_ns}"
)

# Basic project
# ---

project(${TomTestData_ns}
    DESCRIPTION "Tom console application for TinyORM C++ library"
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

# Tom testdata migrations and seeder header files
# ---

include(TinySources)
tiny_tom_testdata_database_sources(${TomTestData_target}_headers)

target_sources(${TomTestData_target} PRIVATE
    ${${TomTestData_target}_headers}
)

# Tom migrations for unit tests specific configuration
# ---

set_target_properties(${TomTestData_target}
    PROPERTIES
        C_VISIBILITY_PRESET "hidden"
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN YES
        VERSION ${PROJECT_VERSION}
)

target_include_directories(${TomTestData_target}
    PRIVATE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/database>"
)

# Tom migrations for unit tests defines
# ---

target_compile_definitions(${TomTestData_target}
    PRIVATE
        PROJECT_TOM_TESTDATA
        # To disable #pragma system_header if compiling TinyORM project itself
        TINYORM_PRAGMA_SYSTEM_HEADER_OFF
)

# Windows resource and manifest files
# ---

# Find icons, tom/version.hpp, and Windows manifest file for MinGW
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    tiny_set_rc_flags("-I \"${${TinyOrm_ns}_SOURCE_DIR}/tom/resources\"")
endif()

include(TinyResourceAndManifest)
tiny_resource_and_manifest(${TomTestData_target}
    OUTPUT_DIR "${TINY_BUILD_GENDIR}/tmp/"
    RESOURCES_DIR "${${TinyOrm_ns}_SOURCE_DIR}/tom/resources"
    RESOURCE_BASENAME ${Tom_target}
    MANIFEST_BASENAME ${Tom_target}
)

# Resolve and link dependencies
# ---

if(NOT STRICT_MODE)
    target_link_libraries(${TomTestData_target}
        PRIVATE ${TinyOrm_ns}::${CommonConfig_target}
    )
endif()

target_link_libraries(${TomTestData_target} PRIVATE ${TinyOrm_ns}::${TinyOrm_target})
