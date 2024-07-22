include(TinyResourceAndManifest)
include(TinySources)

# Configure a passed auto test
function(tiny_configure_test name)

    set(options
        DEPENDS_ON_UNITTESTS INCLUDE_MIGRATIONS INCLUDE_MODELS INCLUDE_PROJECT_SOURCE_DIR
        PROVIDES_PCH RUN_SERIAL
    )
    cmake_parse_arguments(PARSE_ARGV 1 TINY "${options}" "" "")

    if(DEFINED TINY_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} was passed extra arguments: \
${TINY_UNPARSED_ARGUMENTS}")
    endif()

    # Configure PCH for the given test case
    tiny_configure_test_pch(${name} ${TINY_PROVIDES_PCH})

    set_target_properties(${name}
        PROPERTIES
            C_VISIBILITY_PRESET "hidden"
            CXX_VISIBILITY_PRESET "hidden"
            VISIBILITY_INLINES_HIDDEN YES
            AUTOMOC ON
    )

    # These two settings allow to run tests in parallel using eg.: ctest --parallel 10
    # Parallel 30 saves ~12s on MSVC (from 42s to 30s)
    set_tests_properties(${name} PROPERTIES RUN_SERIAL ${TINY_RUN_SERIAL})

    # Primarily to depend all functional tests on unit tests
    if(TINY_DEPENDS_ON_UNITTESTS)
        set_tests_properties(${name}
            PROPERTIES
                DEPENDS "databaseconnection;mysql_querybuilder;postgresql_querybuilder;\
sqlite_querybuilder;blueprint;mysql_schemabuilder;postgresql_schemabuilder;\
sqlite_schemabuilder;mysql_tinybuilder"
        )
    endif()

    # Setup correct PATH env. variable, used by ctest command, needed to find TinyUtils
    # and TinyOrm libraries in the build tree
    set_property(TEST ${name} APPEND PROPERTY
        ENVIRONMENT "PATH=${TINY_TESTS_ENV}"
    )

    if(MINGW)
        target_compile_options(${name}
            PRIVATE
                $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:
                    -Wa,-mbig-obj
                    # Avoid string table overflow
                    -O1>
        )
    endif()

    target_compile_definitions(${name}
        PRIVATE
            PROJECT_TINYORM_TEST
            TINYORM_TESTS_CODE
            # To disable #pragma system_header if compiling TinyORM project itself
            TINYORM_PRAGMA_SYSTEM_HEADER_OFF
            # Disable debug output in release mode
            $<$<NOT:$<CONFIG:Debug>>:QT_NO_DEBUG_OUTPUT>
    )

    # Currently unused
    if(TINY_INCLUDE_PROJECT_SOURCE_DIR)
        target_include_directories(${name}
            PRIVATE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
        )
    endif()

    if(TINY_INCLUDE_MIGRATIONS)
        target_include_directories(${name}
            PRIVATE "$<BUILD_INTERFACE:${${TinyOrm_ns}_SOURCE_DIR}/tests/database>"
        )

        # Migrations header files
        tiny_tests_migration_sources(${name}_headers)
        target_sources(${name} PRIVATE
            ${${name}_headers}
        )
    endif()

    if(TINY_INCLUDE_MODELS)
        target_include_directories(${name}
            PRIVATE "$<BUILD_INTERFACE:${${TinyOrm_ns}_SOURCE_DIR}/tests/models>"
        )

        # Models header and source files
        tiny_model_sources(${name}_headers ${name}_sources)
        target_sources(${name} PRIVATE
            ${${name}_headers}
            ${${name}_sources}
        )
    endif()

    if(NOT STRICT_MODE)
        target_link_libraries(${name} PRIVATE ${TinyOrm_ns}::${CommonConfig_target})
    endif()

    target_link_libraries(${name}
        PRIVATE
            Qt${QT_VERSION_MAJOR}::Test
            ${TinyOrm_ns}::${TinyUtils_target}
            ${TinyOrm_ns}::${TinyOrm_target}
    )

    # Windows resource and manifest files
    # ---

    # Find Windows manifest file for MinGW
    if(MINGW)
        tiny_set_rc_flags("-I \"${${TinyOrm_ns}_SOURCE_DIR}/tests/resources\"")

        # I will not use output variables here, I like it this way 🤘
        set(CMAKE_RC_FLAGS ${CMAKE_RC_FLAGS} PARENT_SCOPE)
        set(TINY_RC_FLAGS_BACKUP ${TINY_RC_FLAGS_BACKUP} PARENT_SCOPE)
    endif()

    tiny_resource_and_manifest(${name}
        OUTPUT_DIR "${TINY_BUILD_GENDIR}/tmp/"
        TEST RESOURCES_DIR "${${TinyOrm_ns}_SOURCE_DIR}/tests/resources"
    )

endfunction()

# Configure PCH for the given test case
# The PROVIDES_PCH parameter from the tiny_configure_test() function tags a test case that
# will provide PCH for all other test cases, only one test case can be tagged with it and
# all other test cases will use this PCH without compilation.
function(tiny_configure_test_pch name provides_pch)

    # Qt <v6.8.0 breaks REUSE_FROM
    # See: https://bugreports.qt.io/projects/QTBUG/issues/QTBUG-126729
    if(NOT TINY_QT_VERSION VERSION_GREATER_EQUAL "6.8.0")
        target_precompile_headers(${name} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:"${${TinyOrm_ns}_SOURCE_DIR}/include/pch.h">
        )
        return()
    endif()

    if(provides_pch)
        # Throw an exception if CACHE{TINY_TESTS_PCH_REUSE_FROM} isn't equal to the name
        tiny_throw_if_wrong_reuse_from(${name})

        target_precompile_headers(${name} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:"${${TinyOrm_ns}_SOURCE_DIR}/include/pch.h">
        )

        set(TINY_TESTS_PCH_REUSE_FROM ${name} CACHE INTERNAL
            "The name of the first test case that provides PCH for all other test cases \
(used by REUSE_FROM).")

    else()
        # This should never happen :/
        # Throw an exception if the CACHE{TINY_TESTS_PCH_REUSE_FROM} isn't DEFINED
        tiny_throw_if_no_cache_reuse_from()

        # TODO REUSE_FROM will fail if NOT $<COMPILE_LANGUAGE:CXX> silverqx
        target_precompile_headers(${name} REUSE_FROM $CACHE{TINY_TESTS_PCH_REUSE_FROM})

        return()
    endif()

    if(NOT CMAKE_DISABLE_PRECOMPILE_HEADERS)
        target_compile_definitions(${name} PRIVATE TINYORM_USING_PCH)
    endif()

endfunction()

# Throw an exception if the CACHE{TINY_TESTS_PCH_REUSE_FROM} is not equal to the name
function(tiny_throw_if_wrong_reuse_from name)

    # Nothing to do
    if(NOT DEFINED CACHE{TINY_TESTS_PCH_REUSE_FROM} OR
            $CACHE{TINY_TESTS_PCH_REUSE_FROM} STREQUAL name
    )
        return()
    endif()

    message(FATAL_ERROR "The '${name}' test case can't set the PROVIDES_PCH because \
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
