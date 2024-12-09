# Tom migrations for unit tests application
# ---

# Initialize Project Version
# ---

tiny_read_version(TINY_VERSION
    TINY_VERSION_MAJOR TINY_VERSION_MINOR TINY_VERSION_PATCH TINY_VERSION_TWEAK
    VERSION_HEADER "${PROJECT_SOURCE_DIR}/tom/include/tom/version.hpp"
    PREFIX TINYTOM
    HEADER_FOR "${TomTestData_ns}"
)

# Tom migrations for unit tests application
# ---

add_executable(${TomTestData_target})
add_executable(${TomTestData_ns}::${TomTestData_target} ALIAS ${TomTestData_target})

# Tom migrations for unit tests source files
# ---

target_sources(${TomTestData_target} PRIVATE main.cpp)

# Tom testdata migrations and seeder header files
# ---

tiny_target_sources(${TomTestData_target} PRIVATE
    PREFIX _tom_testdata_database
    BASE_DIR "database"
)

# Tom migrations for unit tests specific configuration
# ---

set_target_properties(${TomTestData_target}
    PROPERTIES
        C_VISIBILITY_PRESET "hidden"
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN YES
        VERSION ${TINY_VERSION}
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
    tiny_rc_flags(APPEND "-I \"${PROJECT_SOURCE_DIR}/tom/${TINY_SOURCE_RESOURCES_DIR}\"")
endif()

tiny_resource_and_manifest(${TomTestData_target}
    RESOURCES_DIR "${PROJECT_SOURCE_DIR}/tom/${TINY_SOURCE_RESOURCES_DIR}/"
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
