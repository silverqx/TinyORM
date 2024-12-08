include_guard(GLOBAL)

# Configure a passed auto test
function(tiny_configure_test target)

    set(options
        DEPENDS_ON_UNITTESTS INCLUDE_MIGRATIONS INCLUDE_MODELS PROVIDES_PCH RUN_SERIAL
    )
    cmake_parse_arguments(PARSE_ARGV 1 TINY "${options}" "" "")

    if(DEFINED TINY_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} was passed extra arguments: \
${TINY_UNPARSED_ARGUMENTS}")
    endif()

    # Configure PCH for the given test case
    tiny_configure_test_pch(${target} ${TINY_PROVIDES_PCH})

    set_target_properties(${target}
        PROPERTIES
            C_VISIBILITY_PRESET "hidden"
            CXX_VISIBILITY_PRESET "hidden"
            VISIBILITY_INLINES_HIDDEN YES
            AUTOMOC ON
    )

    # These two settings allow to run tests in parallel using eg.: ctest --parallel 10
    # Parallel 30 saves ~12s on MSVC (from 42s to 30s)
    set_tests_properties(${target} PROPERTIES RUN_SERIAL ${TINY_RUN_SERIAL})

    # Primarily to depend all functional tests on unit tests
    if(TINY_DEPENDS_ON_UNITTESTS)
        set_tests_properties(${target}
            PROPERTIES
                DEPENDS "databaseconnection;mysql_querybuilder;postgresql_querybuilder;\
sqlite_querybuilder;blueprint;mysql_schemabuilder;postgresql_schemabuilder;\
sqlite_schemabuilder;mysql_tinybuilder"
        )
    endif()

    # Setup the correct PATH environment variable for the ctest command
    # Needed to find TinyOrm, TinyUtils, TinyDrivers, TinyMySql libraries in build tree
    if(TINY_TESTS_ENV_PATH)
        set_property(TEST ${target} APPEND PROPERTY
            ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:${TINY_TESTS_ENV_PATH}" # Quotes are needed because of \;
        )
    endif()

    if(MINGW)
        target_compile_options(${target}
            PRIVATE
                $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:
                    -Wa,-mbig-obj
                    # Avoid string table overflow
                    -O1>
        )
    endif()

    target_compile_definitions(${target}
        PRIVATE
            PROJECT_TINYORM_TEST
            TINYORM_TESTS_CODE
            # To disable #pragma system_header if compiling TinyORM project itself
            TINYORM_PRAGMA_SYSTEM_HEADER_OFF
            # Disable debug output in release mode
            $<$<NOT:$<CONFIG:Debug>>:QT_NO_DEBUG_OUTPUT>
    )

    # Migrations header files
    if(TINY_INCLUDE_MIGRATIONS)
        tiny_target_sources(${target} PRIVATE
            PREFIX _tests_migration
            BASE_DIR "${PROJECT_SOURCE_DIR}/tests/database"
        )
    endif()

    # Models header files
    if(TINY_INCLUDE_MODELS)
        tiny_target_sources(${target} PRIVATE
            PREFIX _models
            BASE_DIR "${PROJECT_SOURCE_DIR}/tests/models"
        )
    endif()

    if(NOT STRICT_MODE)
        target_link_libraries(${target} PRIVATE ${TinyOrm_ns}::${CommonConfig_target})
    endif()

    target_link_libraries(${target}
        PRIVATE
            Qt${QT_VERSION_MAJOR}::Test
            ${TinyOrm_ns}::${TinyUtils_target}
            ${TinyOrm_ns}::${TinyOrm_target}
    )

    # Windows resource and manifest files
    # ---

    # Find Windows manifest file for MinGW
    if(MINGW)
        tiny_rc_flags(
            APPEND "-I \"${PROJECT_SOURCE_DIR}/tests/${TINY_SOURCE_RESOURCES_DIR}\"")

        # I will not use output variables here, I like it this way 🤘
        set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS}" PARENT_SCOPE)
        set(TinyRcFlagsToRemove "${TinyRcFlagsToRemove}" PARENT_SCOPE)
    endif()

    tiny_resource_and_manifest(${target}
        TEST RESOURCES_DIR "${PROJECT_SOURCE_DIR}/tests/${TINY_SOURCE_RESOURCES_DIR}/"
    )

endfunction()

# Configure PCH for the given test case
# The PROVIDES_PCH parameter from the tiny_configure_test() function tags a test case that
# will provide PCH for all other test cases, only one test case can be tagged with it and
# all other test cases will use this PCH without compilation.
function(tiny_configure_test_pch target provides_pch)

    # Set at the beginning as this function can early return
    if(NOT CMAKE_DISABLE_PRECOMPILE_HEADERS)
        target_compile_definitions(${target} PRIVATE TINYORM_USING_PCH)
    endif()

    # Qt <v6.9.0 breaks REUSE_FROM (will be fixed in Qt v6.9.0)
    # See https://bugreports.qt.io/projects/QTBUG/issues/QTBUG-126729
    # Also, I have patched the Qt6TestTargets.cmake so the REUSE_FROM work for me because
    # of this I need to skip this if() using the TINY_QT6_TEST_TARGET_PATCHED environment
    # variable, it also affects CI pipelines on GitHub self-hosted runners
    if(TINY_QT_VERSION VERSION_LESS "6.9.0" AND
            NOT (DEFINED ENV{TINY_QT6_TEST_TARGET_PATCHED} AND
                "$ENV{TINY_QT6_TEST_TARGET_PATCHED}") # Quotes needed to avoid fail if undefined as conditions don't short-circuit!
    )
        target_precompile_headers(${target} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:"${${TinyOrm_ns}_SOURCE_DIR}/include/pch.h">
        )
        return()
    endif()

    # Disable QT_TESTCASE_BUILDDIR/SOURCEDIR compile definitions for REUSE_FROM to work,
    # this property doesn't exist in my patched Qt6TestTargets.cmake
    if(TINY_QT_VERSION VERSION_GREATER_EQUAL "6.9.0")
        set_target_properties(${target} PROPERTIES QT_SKIP_DEFAULT_TESTCASE_DIRS YES)
    endif()

    # The <target> will provide PCH for all other auto tests
    if(provides_pch)
        # Throw an exception if CACHE{TINY_TESTS_PCH_REUSE_FROM} isn't equal to <target>
        tiny_throw_if_wrong_reuse_from(${target})

        target_precompile_headers(${target} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:"${${TinyOrm_ns}_SOURCE_DIR}/include/pch.h">
        )

        set(TINY_TESTS_PCH_REUSE_FROM ${target} CACHE INTERNAL
            "The <target> name of the first test case that provides PCH for all other \
test cases (used by REUSE_FROM).")

        return()
    endif()

    # Reuse PCH provided by the target stored in CACHE{TINY_TESTS_PCH_REUSE_FROM} variable
    # This should never happen :/
    # Throw an exception if the CACHE{TINY_TESTS_PCH_REUSE_FROM} isn't DEFINED
    tiny_throw_if_no_cache_reuse_from()

    # TODO REUSE_FROM will fail if NOT $<COMPILE_LANGUAGE:CXX> silverqx
    target_precompile_headers(${target} REUSE_FROM "$CACHE{TINY_TESTS_PCH_REUSE_FROM}")

endfunction()

# Throw an exception if the CACHE{TINY_TESTS_PCH_REUSE_FROM} is not equal to the <target>
function(tiny_throw_if_wrong_reuse_from target)

    # Nothing to do
    if(NOT DEFINED CACHE{TINY_TESTS_PCH_REUSE_FROM} OR
            "$CACHE{TINY_TESTS_PCH_REUSE_FROM}" STREQUAL target # Quotes needed to avoid fail if undefined as conditions don't short-circuit!
    )
        return()
    endif()

    message(FATAL_ERROR "The '${target}' test case can't set the PROVIDES_PCH because \
the \$CACHE{TINY_TESTS_PCH_REUSE_FROM} is already set \
for the '$CACHE{TINY_TESTS_PCH_REUSE_FROM}' test case, in ${CMAKE_CURRENT_FUNCTION}().")

endfunction()

# Throw an exception if the CACHE{TINY_TESTS_PCH_REUSE_FROM} isn't DEFINED
function(tiny_throw_if_no_cache_reuse_from)

    # Nothing to do
    if(DEFINED CACHE{TINY_TESTS_PCH_REUSE_FROM})
        return()
    endif()

    message(FATAL_ERROR "The \$CACHE{TINY_TESTS_PCH_REUSE_FROM} is NOT DEFINED, \
the first compiled test case must be tagged with the PROVIDES_PCH argument.")

endfunction()
