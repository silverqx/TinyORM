# Invert a boolean variable value
function(tiny_invert_bool out_variable value)

    if(${value})
        set(${out_variable} FALSE PARENT_SCOPE)
    else()
        set(${out_variable} TRUE PARENT_SCOPE)
    endif()

endfunction()

# Convert to a boolean value
function(tiny_to_bool out_variable value)

    if(${value})
        set(${out_variable} TRUE PARENT_SCOPE)
    else()
        set(${out_variable} FALSE PARENT_SCOPE)
    endif()

endfunction()

# Make minimum toolchain version a requirement
function(tiny_toolchain_requirement)

    set(oneValueArgs MSVC GCC CLANG CLANG_CL)
    cmake_parse_arguments(PARSE_ARGV 0 TINY "" "${oneValueArgs}" "")

    if(DEFINED TINY_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} was passed extra arguments: \
${TINY_UNPARSED_ARGUMENTS}")
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS TINY_MSVC)
            message(FATAL_ERROR "Minimum required MSVC version was not satisfied, \
required version >=${TINY_MSVC}, your version is ${CMAKE_CXX_COMPILER_VERSION}, upgrade \
Visual Studio")
        endif()
    endif()

    # Clang-cl
    if(MSVC AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND
            CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC"
    )
        if(CMAKE_CXX_SIMULATE_VERSION VERSION_LESS TINY_MSVC)
            message(FATAL_ERROR "Minimum required MSVC version was not satisfied, \
required version >=${TINY_MSVC}, your version is ${CMAKE_CXX_SIMULATE_VERSION}, upgrade \
Visual Studio")
        endif()

        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS TINY_CLANG_CL)
            message(FATAL_ERROR "Minimum required Clang-cl version was not satisfied, \
required version >=${TINY_CLANG_CL}, your version is ${CMAKE_CXX_COMPILER_VERSION}, \
upgrade LLVM")
        endif()
    endif()

    if(NOT MSVC AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS TINY_GCC)
            message(STATUS "Minimum recommended GCC version was not satisfied, \
recommended version >=${TINY_GCC}, your version is ${CMAKE_CXX_COMPILER_VERSION}, \
upgrade the GCC compiler")
        endif()
    endif()

    if(NOT MSVC AND
            (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
                CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    )
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS TINY_CLANG)
            message(STATUS "Minimum recommended Clang version was not satisfied, \
recommended version >=${TINY_CLANG}, your version is ${CMAKE_CXX_COMPILER_VERSION}, \
upgrade Clang compiler")
        endif()
    endif()

endfunction()

# A helper macro that calls find_package() and appends the package (if found) to the
# TINY_PACKAGE_DEPENDENCIES list that will be used later to generate find_dependency()
# calls for the TinyORM package configuration file
macro(tiny_find_package package_name)

    find_package(${package_name} ${ARGN})

    if(${package_name}_FOUND)
        set(args "${package_name}")
        # These arguments will be forwarded to the find_package() by find_dependency()
        list(APPEND args "${ARGN}")
        # REQUIRED and QUIETLY arguments are handled by find_dependency() macro
        # find_dependency() forwards the correct parameters for QUIET and REQUIRED which
        # were passed to the original find_package() call
        list(REMOVE_ITEM args "REQUIRED" "QUIET")
        # Remove all empty items
        list(REMOVE_ITEM args "")
        # Convert to the string
        string(REPLACE ";" " " args "${args}")

        # Check if the given args are in the TINY_PACKAGE_DEPENDENCIES list
        get_property(packageDependencies GLOBAL PROPERTY TINY_PACKAGE_DEPENDENCIES)

        if(NOT args IN_LIST packageDependencies)
            set_property(GLOBAL APPEND PROPERTY TINY_PACKAGE_DEPENDENCIES "${args}")
        endif()
    endif()

    unset(args)

endmacro()

# Generate find_dependency() calls for the TinyORM package config file
function(tiny_generate_find_dependency_calls out_dependency_calls)

    set(findDependencyCalls)

    get_property(packageDependencies GLOBAL PROPERTY TINY_PACKAGE_DEPENDENCIES)

    # The ([^;]+) regex matches every list item excluding the ; character 😮
    string(REGEX REPLACE "([^;]+)" "find_dependency(\\1)" findDependencyCalls
        "${packageDependencies}")

    string(REPLACE ";" "\n" findDependencyCalls "${findDependencyCalls}")

    set(${out_dependency_calls} ${findDependencyCalls} PARENT_SCOPE)

endfunction()

# Add a simple build option which controls compile definition(s) for a target.
#
# Synopsis:
# target_optional_compile_definitions(<target> <scope> [FEATURE]
#   NAME <name> DESCRIPTION <description> DEFAULT <default_value>
#   [ENABLED [enabled_compile_definitions...]]
#   [DISABLED [disabled_compile_defnitions...]]
# )
#
# NAME, DESCRIPTION and DEFAULT are passed to option() command.
# If FEATURE is given, they are also passed to add_feature_info() command.
# <scope> determines the scope for the following compile definitions.
# ENABLED lists compile definitions that will be set on <target> when option is enabled,
# DISABLED lists definitions that will be set otherwise.
# ADVANCED calls mark_as_advanced(<NAME>) command.
function(target_optional_compile_definitions target scope)

    set(options ADVANCED FEATURE)
    set(oneValueArgs NAME DESCRIPTION DEFAULT)
    set(multiValueArgs ENABLED DISABLED)
    cmake_parse_arguments(PARSE_ARGV 2 TINY "${options}" "${oneValueArgs}"
        "${multiValueArgs}"
    )

    if(DEFINED TINY_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} was passed extra arguments: \
${TINY_UNPARSED_ARGUMENTS}")
    endif()

    option(${TINY_NAME} "${TINY_DESCRIPTION}" ${TINY_DEFAULT})

    if(${${TINY_NAME}})
        target_compile_definitions(${target} ${scope} ${TINY_ENABLED})
    else()
        target_compile_definitions(${target} ${scope} ${TINY_DISABLED})
    endif()

    if(TINY_FEATURE)
        add_feature_info(${TINY_NAME} ${TINY_NAME} "${TINY_DESCRIPTION}")
    endif()

    if(TINY_ADVANCED)
        mark_as_advanced(${TINY_NAME})
    endif()

endfunction()

# Set TINYORM_LTO macro based on the INTERPROCEDURAL_OPTIMIZATION target property
# Used by the tom about command to show if the LTO is enabled
function(tiny_set_lto_compile_definition target)

    get_target_property(tinyHasLTO ${target} INTERPROCEDURAL_OPTIMIZATION)

    target_compile_definitions(${target} PRIVATE -DTINYORM_LTO=${tinyHasLTO})

endfunction()

# Create an empty SQLite database file if it does not exist
function(tiny_create_sqlite_db db_filepath)

    if(EXISTS ${db_filepath})
        return()
    endif()

    message(STATUS "Creating SQLite database at '${db_filepath}'")

    file(TOUCH "${db_filepath}")

endfunction()

# Create an empty .build_tree file in the folder where the TinyDrivers shared library is
# located (inside the build tree)
function(tiny_create_buildtree_tagfiles filepaths)

    foreach(filepath ${filepaths})
        # Nothing to do, .build_tree tag already exists
        if(EXISTS ${filepath})
            continue()
        endif()

        message(VERBOSE "Creating .build_tree tag file at '${filepath}'")

        file(TOUCH "${filepath}")
    endforeach()

endfunction()

# Find version numbers in the version header file, search following tokens
# <PREFIX>_VERSION_<MAJOR,MINOR,BUGFIX,BUILD>
function(tiny_read_version out_version out_major out_minor out_patch out_tweak)

    # Arguments
    set(oneValueArgs VERSION_HEADER PREFIX HEADER_FOR)
    cmake_parse_arguments(PARSE_ARGV 5 TINY "" "${oneValueArgs}" "")

    if(DEFINED TINY_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} was passed extra arguments: \
${TINY_UNPARSED_ARGUMENTS}")
    endif()

    # Debug setup
    list(APPEND CMAKE_MESSAGE_CONTEXT VersionHeader)
    set(mainMessage "Reading Version Header for ${TINY_HEADER_FOR}")
    message(DEBUG ${mainMessage})
    list(APPEND CMAKE_MESSAGE_INDENT "  ")

    # ---

    file(STRINGS ${TINY_VERSION_HEADER} versionFileContent
        REGEX "^#define ${TINY_PREFIX}.*_VERSION_[A-Z]+ +[0-9]+"
    )

    message(DEBUG "Version file content - ${versionFileContent}")

    set(regex ".+_MAJOR +([0-9]+);.+_MINOR +([0-9]+);.+_BUGFIX +([0-9]+);\
.+_BUILD +([0-9]+)")
    string(REGEX MATCHALL "${regex}" match "${versionFileContent}")

    if(NOT match)
        message(FATAL_ERROR
            "Could not detect project version number from ${versionHeader}")
    endif()

    message(DEBUG "Matched version string - ${match}")

    set(version "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}.${CMAKE_MATCH_4}")

    # ---

    message(DEBUG "${out_version} - ${version}")
    message(DEBUG "${out_major} - ${CMAKE_MATCH_1}")
    message(DEBUG "${out_minor} - ${CMAKE_MATCH_2}")
    message(DEBUG "${out_patch} - ${CMAKE_MATCH_3}")
    message(DEBUG "${out_tweak} - ${CMAKE_MATCH_4}")

    # Debug finish
    list(POP_BACK CMAKE_MESSAGE_INDENT)
    message(DEBUG "${mainMessage} - done")
    list(POP_BACK CMAKE_MESSAGE_CONTEXT)

    # Return values
    set(${out_version} ${version} PARENT_SCOPE)
    set(${out_major} ${CMAKE_MATCH_1} PARENT_SCOPE)
    set(${out_minor} ${CMAKE_MATCH_2} PARENT_SCOPE)
    set(${out_patch} ${CMAKE_MATCH_3} PARENT_SCOPE)
    set(${out_tweak} ${CMAKE_MATCH_4} PARENT_SCOPE)

endfunction()

# Set CMAKE_RC_FLAGS, it saves and restores original content of the CMAKE_RC_FLAGS
# variable, so rc/windres compile commands will not be polluted with include paths from
# previous calls
macro(tiny_set_rc_flags)

    # Remove RC flags from the previous call
    if(NOT TINY_RC_FLAGS_BACKUP STREQUAL "")
        foreach(toRemove ${TINY_RC_FLAGS_BACKUP})
            string(REGEX REPLACE "${toRemove}" "" CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS}")
        endforeach()
        unset(toRemove)
    endif()

    list(APPEND CMAKE_RC_FLAGS " ${ARGN}")
    list(JOIN CMAKE_RC_FLAGS " " CMAKE_RC_FLAGS)

    # Remove redundant whitespaces
    string(REGEX REPLACE " +" " " CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS}")
    string(STRIP "${CMAKE_RC_FLAGS}" CMAKE_RC_FLAGS)

    # Will be removed from the CMAKE_RC_FLAGS in a future call
    set(TINY_RC_FLAGS_BACKUP "${ARGN}")

endmacro()

# Throw a fatal error for unsupported environments
function(tiny_check_unsupported_build)

    # Fixed in Clang v18 🎉
    # Related issue: https://github.com/llvm/llvm-project/issues/55938

    if(MINGW AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT BUILD_SHARED_LIBS AND
            CMAKE_CXX_COMPILER_VERSION VERSION_LESS "18"
    )
        message(FATAL_ERROR "MinGW Clang <18 static build is not supported, it has \
problems with inline constants :/.")
    endif()

    if(MINGW AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND BUILD_SHARED_LIBS AND
            INLINE_CONSTANTS AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "18"
    )
        message(FATAL_ERROR "MinGW Clang <18 shared build crashes with inline constants, \
don't enable the INLINE_CONSTANTS cmake option :/.")
    endif()

    if(TINY_VCPKG AND TINY_IS_MULTI_CONFIG)
        message(FATAL_ERROR "Multi-configuration generators are not supported in vcpkg \
ports.")
    endif()

    if(TINY_VCPKG AND TINY_BUILD_LOADABLE_DRIVERS)
        message(FATAL_ERROR "Loadable SQL drivers are not supported in vcpkg ports.")
    endif()

    if(BUILD_DRIVERS AND NOT BUILD_MYSQL_DRIVER)
        message(FATAL_ERROR "If the BUILD_DRIVERS option is enabled, at least one \
driver implementation must be enabled, please enable BUILD_MYSQL_DRIVER.")
    endif()

endfunction()

# Print a VERBOSE message against which library is project linking
function(tiny_print_linking_against target)

    # TINY_BUILD_TYPE_UPPER STREQUAL "" means that the CMAKE_BUILD_TYPE was not defined or is empty
    if(TINY_IS_MULTI_CONFIG OR TINY_BUILD_TYPE_UPPER STREQUAL "")
        return()
    endif()

    if(WIN32 AND BUILD_SHARED_LIBS)
        get_target_property(libraryFilepath ${target} IMPORTED_IMPLIB_${TINY_BUILD_TYPE_UPPER})
    else()
        get_target_property(libraryFilepath ${target} IMPORTED_LOCATION_${TINY_BUILD_TYPE_UPPER})
    endif()

    message(VERBOSE "Linking against ${target} at ${libraryFilepath}")

endfunction()

# Determine whether the CMAKE_CXX_COMPILER_LAUNCHER contains ccache/sccache
function(tiny_is_ccache_compiler_launcher out_variable)

    if(NOT DEFINED CMAKE_CXX_COMPILER_LAUNCHER)
        set(${out_variable} FALSE PARENT_SCOPE)
        return()
    endif()

    # Support the ccache and also sccache (I have tried and sccache doesn't work)
    cmake_path(GET CMAKE_CXX_COMPILER_LAUNCHER STEM ccacheStem)
    if(NOT ccacheStem STREQUAL "ccache" AND NOT ccacheStem STREQUAL "sccache")
        set(${out_variable} FALSE PARENT_SCOPE)
        return()
    endif()

    set(${out_variable} TRUE PARENT_SCOPE)

endfunction()

# Determine if the current platform needs fixes and the CMAKE_CXX_COMPILER_LAUNCHER
# contains ccache/sccache (fixes for MSVC compilers)
function(tiny_should_fix_ccache_msvc out_variable)

    # Target the MSVC and Clang-cl with MSVC compilers on Windows
    if(NOT WIN32 OR NOT MSVC OR MINGW OR NOT DEFINED CMAKE_CXX_COMPILER_LAUNCHER)
        set(${out_variable} FALSE PARENT_SCOPE)
        return()
    endif()

    # Support the ccache and also sccache (I have tried and sccache doesn't work)
    set(isCcacheCompilerLauncher FALSE)
    tiny_is_ccache_compiler_launcher(isCcacheCompilerLauncher)

    set(${out_variable} ${isCcacheCompilerLauncher} PARENT_SCOPE)

endfunction()

# Determine whether to disable PCH based on the ccache --print-version and set
# the internal cache variable TINY_CCACHE_VERSION (MSVC only)
# Precompiled headers are fully supported on MSVC for ccache >=4.10, so
# disable PCH for ccache <4.10 only.
# The git-ref is a special value, it means that the ccache was built manually from eg.
# master branch, in this case suppose the version is always >=4.10.
# If the ccache isn't on the system path or parsing the version failed set to 0.
function(tiny_should_disable_precompile_headers out_variable)

    # Nothing to do, ccache version was already populated (cache hit)
    if(DEFINED TINY_CCACHE_VERSION AND NOT TINY_CCACHE_VERSION STREQUAL "")
        if(TINY_CCACHE_VERSION VERSION_GREATER_EQUAL "4.10" OR
            TINY_CCACHE_VERSION STREQUAL "git-ref"
        )
            set(${out_variable} FALSE PARENT_SCOPE)
        else()
            set(${out_variable} TRUE PARENT_SCOPE)
        endif()

        return()
    endif()

    set(helpString "Ccache version used to determine whether to disable PCH (MSVC only).")

    execute_process(
        COMMAND "${CMAKE_CXX_COMPILER_LAUNCHER}" --print-version
        RESULT_VARIABLE exitCode
        OUTPUT_VARIABLE ccacheVersionRaw
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    # ccache can't be executed, in this case don't disable PCH and even don't cache
    # the TINY_CCACHE_VERSION because the build is gona to fail and we don't want to
    # cache wrong value
    if(exitCode STREQUAL "no such file or directory")
        set(${out_variable} FALSE PARENT_SCOPE)
        return()
    endif()

    # ccache <4.10 doesn't have the --print-version parameter, we can be pretty sure that
    # the version is <4.10 because we know the ccache is on the system path
    if(NOT exitCode EQUAL 0)
        set(TINY_CCACHE_VERSION "0" CACHE INTERNAL "${helpString}")
        set(${out_variable} TRUE PARENT_SCOPE)
        return()
    endif()

    # Detect a manual build version (git reference).
    # The git-ref is a special value, it means that the ccache was built manually from eg.
    # master branch, in this case set ccache version as the git-ref string. This version
    # will be supposed as the latest version and will be assumed it supports PCH.
    # CMake doesn't support {x,y}. 😮
    set(regexpGitRef
        "^.*\.[0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f]*$")

    if(ccacheVersionRaw MATCHES "${regexpGitRef}")
        set(TINY_CCACHE_VERSION "git-ref" CACHE INTERNAL "${helpString}")
        set(${out_variable} FALSE PARENT_SCOPE)
        return()
    endif()

    # Detect a normal tag version like eg. 4.10
    set(regexpVersion "^[0-9]+\.[0-9]+(\.[0-9]+)?(\.[0-9]+)?$")

    # This should never happen :/
    if(NOT ccacheVersionRaw MATCHES "${regexpVersion}")
        message(FATAL_ERROR "Parse of the 'ccache --print-version' failed \
in tiny_should_disable_precompile_headers().")
    endif()

    set(TINY_CCACHE_VERSION "${CMAKE_MATCH_0}" CACHE INTERNAL "${helpString}")

    if(TINY_CCACHE_VERSION VERSION_GREATER_EQUAL "4.10")
        set(${out_variable} FALSE PARENT_SCOPE)
    else()
        set(${out_variable} TRUE PARENT_SCOPE)
    endif()

endfunction()

# Disable the precompilation of header files
function(tiny_disable_precompile_headers)

    # Determine whether to disable PCH based on the ccache --print-version
    set(shouldDisablePCH FALSE)
    tiny_should_disable_precompile_headers(shouldDisablePCH)

    if(NOT shouldDisablePCH)
        return()
    endif()

    message(VERBOSE "Disabled PCH because ccache or sccache is used as compiler \
launcher for MSVC")

    get_property(help_string CACHE CMAKE_DISABLE_PRECOMPILE_HEADERS
        PROPERTY HELPSTRING
    )
    if(NOT help_string)
        set(help_string "Default value for DISABLE_PRECOMPILE_HEADERS of targets.")
    endif()

    set(CMAKE_DISABLE_PRECOMPILE_HEADERS ON CACHE BOOL ${help_string} FORCE)

endfunction()

# Determine whether the CMAKE_MSVC_DEBUG_INFORMATION_FORMAT, a new MSVC debug information
# format is in effect
function(tiny_is_new_msvc_debug_information_format_325 out_variable)

    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.25")

        cmake_policy(GET CMP0141 policy_cmp0141)
        if(policy_cmp0141 STREQUAL "NEW")
            set(${out_variable} TRUE PARENT_SCOPE)
            return()
        endif()

    endif()

    set(${out_variable} FALSE PARENT_SCOPE)

endfunction()

# Support the MSVC debug information format flags selected by an abstraction added
# in the CMake 3.25
function(tiny_fix_ccache_msvc_325)

    get_property(help_string CACHE CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        PROPERTY HELPSTRING
    )
    if(NOT help_string)
        set(help_string "Default value for MSVC_DEBUG_INFORMATION_FORMAT of targets.")
    endif()

    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>"
        CACHE BOOL ${help_string} FORCE
    )

    mark_as_advanced(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT)

endfunction()

# Helper function to replace /Zi and /ZI by /Z7 in the CMAKE_<C|CXX>_FLAGS_<CONFIG> option
function(tiny_replace_Zi_by_Z7_for option help_string)

    if(DEFINED ${option} AND ${option} MATCHES "(/|-)(Zi|ZI)")
        string(REGEX REPLACE "(/|-)(Zi|ZI)" "/Z7" ${option} "${${option}}")

        get_property(help_string_property CACHE ${option} PROPERTY HELPSTRING)
        if(NOT help_string_property)
            set(help_string_property ${help_string})
        endif()

        set(${option} ${${option}} CACHE STRING ${help_string_property} FORCE)
    endif()

endfunction()

# Replace /Zi or /ZI by /Z7 in the CMAKE_<C|CXX>_FLAGS_<CONFIG> option for the CMake <3.25
function(tiny_fix_ccache_msvc_324)

    # Nothing to do, multi-configuration generators are not supported
    if(TINY_IS_MULTI_CONFIG OR TINY_BUILD_TYPE_LOWER STREQUAL "")
        message(STATUS "The ccache compiler launcher is not supported for multi-configuration \
generators or with undefined CMAKE_BUILD_TYPE on CMake <3.25")
        return()
    endif()

    # Replace /Zi and /ZI by /Z7 by the build config type, for the CMake <=3.24
    if(TINY_BUILD_TYPE_LOWER STREQUAL "debug")
        tiny_replace_Zi_by_Z7_for(CMAKE_CXX_FLAGS_DEBUG
            "Flags used by the CXX compiler during DEBUG builds.")
        tiny_replace_Zi_by_Z7_for(CMAKE_C_FLAGS_DEBUG
            "Flags used by the C compiler during DEBUG builds.")

    # This should never happen, but I leave it here because it won't hurt anything
    elseif(TINY_BUILD_TYPE_LOWER STREQUAL "release")
        tiny_replace_Zi_by_Z7_for(CMAKE_CXX_FLAGS_RELEASE
            "Flags used by the CXX compiler during RELEASE builds.")
        tiny_replace_Zi_by_Z7_for(CMAKE_C_FLAGS_RELEASE
            "Flags used by the C compiler during RELEASE builds.")

    elseif(TINY_BUILD_TYPE_LOWER STREQUAL "relwithdebinfo")
        tiny_replace_Zi_by_Z7_for(CMAKE_CXX_FLAGS_RELWITHDEBINFO
            "Flags used by the CXX compiler during RELWITHDEBINFO builds.")
        tiny_replace_Zi_by_Z7_for(CMAKE_C_FLAGS_RELWITHDEBINFO
            "Flags used by the C compiler during RELWITHDEBINFO builds.")
    endif()

endfunction()

# Fix CMake variables if CMAKE_CXX_COMPILER_LAUNCHER is ccache or sccache
# It applies fixes for MSVC compiler. It disables precompiled headers as they are not
# supported on Windows with ccache and changes the -Zi and -ZI compiler options to the -Z7
# for debug builds for CMake <3.25 as the -Zi and -ZI compiler options is not supported or
# set up the CMAKE_MSVC_DEBUG_INFORMATION_FORMAT for CMake >=3.25
# (https://github.com/ccache/ccache/issues/1040)
function(tiny_fix_ccache_msvc)

    # I will check only the CMAKE_CXX_COMPILER_LAUNCHER and not also the
    # CMAKE_C_COMPILER_LAUNCHER as this is a pure c++ project and c compiler is not used
    # anyway but I will replace the Zi to Z7 compiler options in both
    # CMAKE_<C|CXX>_FLAGS_<CONFIG> to be consistent 🤔

    # Fix the MSVC debug information format by the CMake version
    set(isNewMsvcDebugInformationFormat FALSE)
    tiny_is_new_msvc_debug_information_format_325(isNewMsvcDebugInformationFormat)

    if(isNewMsvcDebugInformationFormat)
        # Support the MSVC debug information format flags added in the CMake 3.25
        tiny_fix_ccache_msvc_325()
    else()
        # Replace /Zi and /ZI by /Z7 in the CMAKE_<C|CXX>_FLAGS_<CONFIG>
        # for the CMake <3.25
        tiny_fix_ccache_msvc_324()
    endif()

    # Don't disable PCH if no fixes were applied
    # The new MSVC debug information format flags method also supports multi-config generators
    # The old replace (/Zi|/ZI) by /Z7 method doesn't support multi-config generators
    # Don't remove this isNewMsvcDebugInformationFormat check, is correct!
    if(isNewMsvcDebugInformationFormat OR NOT TINY_IS_MULTI_CONFIG)
        tiny_disable_precompile_headers()
    endif()

endfunction()

# Determine if the current platform needs fixes and the CMAKE_CXX_COMPILER_LAUNCHER
# contains ccache/sccache (fixes for Clang compilers)
function(tiny_should_fix_ccache_clang out_variable)

    # Target the Clang on Linux, MSYS2, and also Clang-cl with MSVC
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(${out_variable} FALSE PARENT_SCOPE)
        return()
    endif()

    # Support the ccache and also sccache (I have tried and sccache doesn't work)
    set(isCcacheCompilerLauncher FALSE)
    tiny_is_ccache_compiler_launcher(isCcacheCompilerLauncher)

    set(${out_variable} ${isCcacheCompilerLauncher} PARENT_SCOPE)

endfunction()

# Fix CMake variables if CMAKE_CXX_COMPILER_LAUNCHER is ccache or sccache
# This is a general function that applies fixes for MSVC and Clang compilers, it checks
# the current platform and based on it applies correct fixes
function(tiny_fix_ccache)

    # MSYS2 g++ or clang++ work well with the precompiled headers but the msvc doesn't

    # Fixes for the MSVC compiler (including the Clang-cl with MSVC)
    set(shouldFixCcacheMsvc FALSE)
    tiny_should_fix_ccache_msvc(shouldFixCcacheMsvc)

    if(shouldFixCcacheMsvc)
        tiny_fix_ccache_msvc()
    endif()

    # Fixes for the Clang compiler on Linux, MSYS2, and also Clang-cl with MSVC
    # Ignore PCH timestamps if the ccache is used (recommended in ccache docs)
    set(shouldFixCcacheClang FALSE)
    tiny_should_fix_ccache_clang(shouldFixCcacheClang)

    if(shouldFixCcacheClang)
        list(APPEND CMAKE_CXX_COMPILE_OPTIONS_CREATE_PCH -Xclang -fno-pch-timestamp)

        set(CMAKE_CXX_COMPILE_OPTIONS_CREATE_PCH
            "${CMAKE_CXX_COMPILE_OPTIONS_CREATE_PCH}" PARENT_SCOPE
        )
    endif()

endfunction()

# Set the Compatible Interface Requirement for the project's major version using
# the given target
function(tiny_set_compatible_interface_string target)

    # Arguments
    set(multiValueArgs PROPERTIES)
    cmake_parse_arguments(PARSE_ARGV 1 TINY "" "" "${multiValueArgs}")

    if(DEFINED TINY_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} was passed extra arguments: \
${TINY_UNPARSED_ARGUMENTS}")
    endif()

    # Body
    foreach(property ${TINY_PROPERTIES})
        # Skip the TinyOrm_VERSION_MAJOR as it's already defined in the main/parent
        # CMakeLists.txt file
        if(NOT (target STREQUAL TinyOrm_target AND property STREQUAL "VERSION_MAJOR"))
            get_target_property(${target}_${property} ${target} ${property})
        endif()

        set_property(
            TARGET ${target}
            PROPERTY INTERFACE_${target}_${property} ${${target}_${property}}
        )

        set_property(
            TARGET ${target}
            APPEND PROPERTY COMPATIBLE_INTERFACE_STRING ${target}_${property}
        )
    endforeach()

endfunction()

# Get target includes that contain IMPORTED targets for the TinyORM package config file
function(tiny_generate_target_includes out_variable)

    set(includeTmpl "include(\"\${CMAKE_CURRENT_LIST_DIR}/@target@.cmake\")")
    set(includeReplaced)
    set(result)

    # The order is important here, the TinyDriversTargets must be included before
    # the TinyOrmTargets because of the checks whether the exported targets exist
    # at the end of the TinyOrmTargets.cmake file

    # TinyDriversTargets
    if(BUILD_DRIVERS)
        string(REPLACE "@target@" "TinyDriversTargets" includeReplaced "${includeTmpl}")
        list(APPEND result ${includeReplaced})
    endif()

    # TinyOrmTargets
    string(REPLACE "@target@" "TinyOrmTargets" includeReplaced "${includeTmpl}")
    list(APPEND result ${includeReplaced})

    # No need to escape the ; character as include() statement can't contain it
    list(JOIN result "\n" result)

    set(${out_variable} ${result} PARENT_SCOPE)

endfunction()

# Set up package properties using the set_package_properties()
macro(set_packages_properties)

    set_package_properties(QT
        PROPERTIES
            URL "https://doc.qt.io/qt-${QT_VERSION_MAJOR}/"
            DESCRIPTION "Qt is a full development framework"
            TYPE REQUIRED
            PURPOSE "Provides SQL database layer by the QtSql module, QVariant, and QString"
    )
    set_package_properties(Qt${QT_VERSION_MAJOR}
        PROPERTIES
            URL "https://doc.qt.io/qt-${QT_VERSION_MAJOR}/"
            DESCRIPTION "Qt is a full development framework"
            TYPE REQUIRED
            PURPOSE "Provides SQL database layer by the QtSql module, QVariant, and QString"
    )
    set_package_properties(Qt${QT_VERSION_MAJOR}Core
        PROPERTIES
            URL "https://doc.qt.io/qt-${QT_VERSION_MAJOR}/qtcore-index.html"
            DESCRIPTION "Core non-graphical classes used by other modules"
            TYPE REQUIRED
            PURPOSE "Provides QVariant, QString, and Qt containers"
    )
    if(NOT BUILD_DRIVERS)
        set_package_properties(Qt${QT_VERSION_MAJOR}Sql
            PROPERTIES
                URL "https://doc.qt.io/qt-${QT_VERSION_MAJOR}/qtsql-index.html"
                DESCRIPTION "Classes for database integration using SQL"
                TYPE REQUIRED
                PURPOSE "Provides SQL database layer"
        )
    endif()
    set_package_properties(range-v3
        PROPERTIES
            URL "https://ericniebler.github.io/range-v3/"
            DESCRIPTION "Range algorithms, views, and actions for STL"
            TYPE REQUIRED
            PURPOSE "Used to have a nice and clear code"
    )
    if(MYSQL_PING OR BUILD_MYSQL_DRIVER)
        if(TINY_VCPKG)
            set_package_properties(unofficial-libmysql
                PROPERTIES
                    URL "https://www.mysql.com"
                    DESCRIPTION "MySQL client library (vcpkg unofficial)"
                    TYPE REQUIRED
                    PURPOSE "Provides low-level access to the MySQL client/server \
protocol (used by mysql-ping or build-mysql-driver vcpkg features)"
            )
        else()
            set_package_properties(MySQL
                PROPERTIES
                    # URL and DESCRIPTION are already set by Find-module Package (FindMySQL.cmake)
                    TYPE REQUIRED
                    PURPOSE "Provides low-level access to the MySQL client/server \
protocol (used by MySqlConnection::pingDatabase() or if BUILD_MYSQL_DRIVER is enabled)"
            )
        endif()
    endif()
    if(TOM)
        set_package_properties(tabulate
            PROPERTIES
                URL "https://github.com/p-ranav/tabulate"
                DESCRIPTION "Table Maker for Modern C++"
                TYPE REQUIRED
                PURPOSE "Used by the Tom in the migrate:status command"
        )
    endif()

endmacro()
