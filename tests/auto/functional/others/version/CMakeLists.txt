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

if(TOM_EXAMPLE)
    target_compile_definitions(version PRIVATE TINYTOM_EXAMPLE)

    # To find tom/include/version.hpp
    target_include_directories(version PRIVATE "${CMAKE_SOURCE_DIR}/tom/include")
endif()

target_include_directories(version PRIVATE
    "${PROJECT_BINARY_DIR}/${TINY_BUILD_GENDIR}/include"
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
file(GENERATE OUTPUT "${TINY_BUILD_GENDIR}/include/versiondebug_cmake-$<CONFIG>.hpp"
    INPUT "${PROJECT_BINARY_DIR}/${TINY_BUILD_GENDIR}/tmp/versiondebug_cmake.hpp.genexp"
    NEWLINE_STYLE UNIX
)

# Following commented code throws warning with clang-cl:
# warning: definition of macro 'TINYTEST_VERSION_VERSIONDEBUG' does not match definition
# in precompiled header [-Wclang-cl-pch]

# Pass generated filename to the tst_version.cpp source file (used in #include directive)
#set_source_files_properties("tst_version.cpp"
#    TARGET_DIRECTORY version
#    PROPERTIES
#        COMPILE_DEFINITIONS
#            TINYTEST_VERSION_VERSIONDEBUG="versiondebug_cmake-$<CONFIG>.hpp"
#)

# This is ok
target_compile_definitions(version
    PRIVATE TINYTEST_VERSION_VERSIONDEBUG="versiondebug_cmake-$<CONFIG>.hpp"
)

target_sources(version PRIVATE
    "$<$<BOOL:$<CONFIG>>:${TINY_BUILD_GENDIR}/include/versiondebug_cmake-$<CONFIG>.hpp>"
)
