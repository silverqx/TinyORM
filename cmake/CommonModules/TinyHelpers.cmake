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

    # clang-cl
    if(MSVC AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND
        CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC"
    )
        if(CMAKE_CXX_SIMULATE_VERSION VERSION_LESS TINY_MSVC)
            message(FATAL_ERROR "Minimum required MSVC version was not satisfied, \
required version >=${TINY_MSVC}, your version is ${CMAKE_CXX_SIMULATE_VERSION}, upgrade \
Visual Studio")
        endif()

        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS TINY_CLANG_CL)
            message(FATAL_ERROR "Minimum required clang-cl version was not satisfied, \
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
# TINY_PACKAGE_DEPENDENCIES list, which can be used later to generate package config file
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

        list(APPEND tiny_package_dependencies "${args}")
    endif()

    unset(args)

endmacro()

# Generate find_dependency calls for the TinyORM package config file
function(tiny_generate_find_dependency_calls out_dependency_calls)

    set(find_dependency_calls)

    string(REGEX REPLACE "([^;]+)" "find_dependency(\\1)" find_dependency_calls
        "${tiny_package_dependencies}")

    string(REPLACE ";" "\n" find_dependency_calls "${find_dependency_calls}")

    set(${out_dependency_calls} ${find_dependency_calls} PARENT_SCOPE)

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

# Create an empty SQLite database file when does not exist
function(create_sqlite_db db_filepath)

    if(EXISTS ${db_filepath})
        return()
    endif()

    message(STATUS "Creating SQLite database at '${db_filepath}'")

    file(TOUCH "${db_filepath}")

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

    set(regexp ".+_MAJOR +([0-9]+);.+_MINOR +([0-9]+);.+_BUGFIX +([0-9]+);\
.+_BUILD +([0-9]+)")
    string(REGEX MATCHALL "${regexp}" match "${versionFileContent}")

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

    if(MINGW AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT BUILD_SHARED_LIBS)
        message(FATAL_ERROR "MinGW clang static build is not supported, it has problems \
with inline constants :/.")
    endif()

    if(MINGW AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND BUILD_SHARED_LIBS AND
        INLINE_CONSTANTS
    )
        message(FATAL_ERROR "MinGW clang shared build crashes with inline constants, \
don't enable the INLINE_CONSTANTS cmake option :/.")
    endif()

endfunction()

# Print a VERBOSE message against which library is project linking
function(tiny_print_linking_against target)

    if(TINY_IS_MULTI_CONFIG)
        return()
    endif()

    string(TOUPPER ${CMAKE_BUILD_TYPE} buildType)

    if(WIN32 AND BUILD_SHARED_LIBS)
        get_target_property(libraryFilepath ${target} IMPORTED_IMPLIB_${buildType})
    else()
        get_target_property(libraryFilepath ${target} IMPORTED_LOCATION_${buildType})
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

    # Target the msvc and clang-cl with msvc compilers on Windows
    if(NOT WIN32 OR NOT MSVC OR MINGW OR NOT DEFINED CMAKE_CXX_COMPILER_LAUNCHER)
        set(${out_variable} FALSE PARENT_SCOPE)
        return()
    endif()

    # Support the ccache and also sccache (I have tried and sccache doesn't work)
    set(isCcacheCompilerLauncher FALSE)
    tiny_is_ccache_compiler_launcher(isCcacheCompilerLauncher)

    set(${out_variable} ${isCcacheCompilerLauncher} PARENT_SCOPE)

endfunction()

# Disable the precompilation of header files
function(tiny_disable_precompile_headers)

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

# Helper function to replace /Zi by /Z7 in the CMAKE_<C|CXX>_FLAGS_<CONFIG> option
function(tiny_replace_Zi_by_Z7_for option help_string)

    if(DEFINED ${option} AND ${option} MATCHES "/Zi")
        string(REPLACE "/Zi" "/Z7" ${option} "${${option}}")

        get_property(help_string_property CACHE ${option} PROPERTY HELPSTRING)
        if(NOT help_string_property)
            set(help_string_property ${help_string})
        endif()

        set(${option} ${${option}} CACHE STRING ${help_string_property} FORCE)
    endif()

endfunction()

# Replace /Zi by /Z7 in the CMAKE_<C|CXX>_FLAGS_<CONFIG> option for the CMake <3.25
function(tiny_fix_ccache_msvc_324)

    # Replace /Zi by /Z7 by the build config type, for the CMake <=3.24
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        tiny_replace_Zi_by_Z7_for(CMAKE_CXX_FLAGS_DEBUG
            "Flags used by the CXX compiler during DEBUG builds.")
        tiny_replace_Zi_by_Z7_for(CMAKE_C_FLAGS_DEBUG
            "Flags used by the C compiler during DEBUG builds.")

    # This should never happen, but I leave it here because it won't hurt anything
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        tiny_replace_Zi_by_Z7_for(CMAKE_CXX_FLAGS_RELEASE
            "Flags used by the CXX compiler during RELEASE builds.")
        tiny_replace_Zi_by_Z7_for(CMAKE_C_FLAGS_RELEASE
            "Flags used by the C compiler during RELEASE builds.")

    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        tiny_replace_Zi_by_Z7_for(CMAKE_CXX_FLAGS_RELWITHDEBINFO
            "Flags used by the CXX compiler during RELWITHDEBINFO builds.")
        tiny_replace_Zi_by_Z7_for(CMAKE_C_FLAGS_RELWITHDEBINFO
            "Flags used by the C compiler during RELWITHDEBINFO builds.")
    endif()

endfunction()

# Fix CMake variables if CMAKE_CXX_COMPILER_LAUNCHER is ccache or sccache
# It applies fixes for MSVC compiler. It disables precompiled headers as they are not
# supported on Windows with ccache and changes the -Zi compiler option to the -Z7
# for debug builds as the -Zi compiler option is not supported for CMake <3.25 or
# set up the CMAKE_MSVC_DEBUG_INFORMATION_FORMAT for CMake >=3.25
# (https://github.com/ccache/ccache/issues/1040)
function(tiny_fix_ccache_msvc)

    # I will check only the CMAKE_CXX_COMPILER_LAUNCHER and not also the
    # CMAKE_C_COMPILER_LAUNCHER as this is a pure c++ project and c compiler is not used
    # anyway but I will replace the Zi to Z7 compiler option in both
    # CMAKE_<C|CXX>_FLAGS_<CONFIG> to be consistent 🤔

    tiny_disable_precompile_headers()

    # Fix the MSVC debug information format by the CMake version
    set(isNewMsvcDebugInformationFormat FALSE)
    tiny_is_new_msvc_debug_information_format_325(isNewMsvcDebugInformationFormat)

    if(isNewMsvcDebugInformationFormat)
        # Support the MSVC debug information format flags added in the CMake 3.25
        tiny_fix_ccache_msvc_325()
    else()
        # Replace /Zi by /Z7 in the CMAKE_<C|CXX>_FLAGS_<CONFIG> for the CMake <3.25
        tiny_fix_ccache_msvc_324()
    endif()

endfunction()

# Determine if the current platform needs fixes and the CMAKE_CXX_COMPILER_LAUNCHER
# contains ccache/sccache (fixes for Clang compilers)
function(tiny_should_fix_ccache_clang out_variable)

    # Target the Clang on Linux and MSYS2
    if(MSVC OR NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
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

    # Fixes for the MSVC compiler
    set(shouldFixCcacheMsvc FALSE)
    tiny_should_fix_ccache_msvc(shouldFixCcacheMsvc)

    if(shouldFixCcacheMsvc)
        tiny_fix_ccache_msvc()
    endif()

    # Fixes for the Clang compiler
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
