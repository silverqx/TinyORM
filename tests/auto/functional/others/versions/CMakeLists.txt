add_executable(versions)

target_sources(versions PRIVATE
    tst_versions.cpp
    include/versionsdebug_cmake.hpp.in
)

add_test(NAME versions COMMAND versions)

tiny_configure_test(versions)

# For checkFileVersion_*() tests
# ---

target_compile_definitions(versions PRIVATE TINYTEST_VERSIONS_IS_CMAKE)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(versions PRIVATE TINYTEST_VERSIONS_IS_SHARED_BUILD)
endif()

if(TOM_EXAMPLE)
    target_compile_definitions(versions PRIVATE TINYTOM_EXAMPLE)
endif()

target_include_directories(versions
    PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/${TINY_BUILD_INCLUDE_DIR}>"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(versions PRIVATE Version.lib)
endif()

# Support a real TinyOrm/TinyUtils library filenames
configure_file(
    "include/versionsdebug_cmake.hpp.in"
    "${TINY_BUILD_TMP_DIR}/versionsdebug_cmake.hpp.genexp"
    @ONLY NEWLINE_STYLE LF
)
file(GENERATE OUTPUT "${TINY_BUILD_INCLUDE_DIR}/versionsdebug_cmake-$<CONFIG>.hpp"
    INPUT "${CMAKE_CURRENT_BINARY_DIR}/${TINY_BUILD_TMP_DIR}/versionsdebug_cmake.hpp.genexp"
    NEWLINE_STYLE UNIX
)

# Following commented code throws warning with Clang-cl:
# warning: definition of macro 'TINYTEST_VERSIONS_VERSIONSDEBUG' does not match definition
# in precompiled header [-Wclang-cl-pch]

# Pass generated filename to the tst_versions.cpp source file (used in #include directive)
#set_source_files_properties("tst_versions.cpp"
#    TARGET_DIRECTORY versions
#    PROPERTIES
#        COMPILE_DEFINITIONS
#            TINYTEST_VERSIONS_VERSIONSDEBUG="versionsdebug_cmake-$<CONFIG>.hpp"
#)

# This is ok
target_compile_definitions(versions
    PRIVATE TINYTEST_VERSIONS_VERSIONSDEBUG="versionsdebug_cmake-$<CONFIG>.hpp"
)

target_sources(versions PRIVATE
    "$<$<BOOL:$<CONFIG>>:${TINY_BUILD_INCLUDE_DIR}/versionsdebug_cmake-$<CONFIG>.hpp>"
)
