project(version
    LANGUAGES CXX
)

add_executable(version
    include/versiondebug_cmake.hpp.in
    tst_version.cpp
)

add_test(NAME version COMMAND version)

include(TinyTestCommon)
tiny_configure_test(version)

# For checkFileVersion_*() tests
# ---

target_compile_definitions(version PRIVATE TINYTEST_VERSION_IS_CMAKE)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(version PRIVATE TINYTEST_VERSION_IS_SHARED_BUILD)
endif()

target_include_directories(version PRIVATE
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/${TINY_BUILD_GENDIR}/include>"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(version PRIVATE Version.lib)
endif()

# Support a real TinyOrm/TinyUtils library filenames
configure_file(
    "include/versiondebug_cmake.hpp.in"
    "${TINY_BUILD_GENDIR}/tmp/versiondebug_cmake.hpp.genexp"
    @ONLY NEWLINE_STYLE LF
)
file(GENERATE OUTPUT "${TINY_BUILD_GENDIR}/include/versiondebug_cmake.hpp"
    INPUT "${PROJECT_BINARY_DIR}/${TINY_BUILD_GENDIR}/tmp/versiondebug_cmake.hpp.genexp"
    NEWLINE_STYLE UNIX
)

target_sources(version PRIVATE
    "${TINY_BUILD_GENDIR}/include/versiondebug_cmake.hpp"
)
