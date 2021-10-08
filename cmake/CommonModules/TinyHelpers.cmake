# Invert a boolean variable value
function(tiny_invert_bool out_variable value)

    if(${value})
        set(${out_variable} FALSE PARENT_SCOPE)
    else()
        set(${out_variable} TRUE PARENT_SCOPE)
    endif()

endfunction()

# Make minimum toolchain version a requirement
function(tiny_toolchain_requirement)

    set(oneValueArgs MSVC)
    cmake_parse_arguments(PARSE_ARGV 0 TINY "" "${oneValueArgs}" "")

    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS TINY_MSVC)
            message(FATAL_ERROR "Minimum required MSVC version was not satisfied, \
required version >=${TINY_MSVC}, your version is ${CMAKE_CXX_COMPILER_VERSION}, upgrade \
Visual Studio")
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
function(target_optional_compile_definitions target scope)

    set(options FEATURE)
    set(oneValueArgs NAME DESCRIPTION DEFAULT)
    set(multiValueArgs ENABLED DISABLED)
    cmake_parse_arguments(PARSE_ARGV 2 TINY ${options} "${oneValueArgs}"
        "${multiValueArgs}"
    )

    option(${TINY_NAME} "${TINY_DESCRIPTION}" ${TINY_DEFAULT})

    if(${${TINY_NAME}})
        target_compile_definitions(${target} ${scope} ${TINY_ENABLED})
    else()
        target_compile_definitions(${target} ${scope} ${TINY_DISABLED})
    endif()

    if(TINY_FEATURE)
        add_feature_info(${TINY_NAME} ${TINY_NAME} "${TINY_DESCRIPTION}")
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
    cmake_parse_arguments(PARSE_ARGV 4 TINY "" "${oneValueArgs}" "")

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
        message(FATAL_ERROR "MinGW clang static build is not supported, problem with \
inline constants :/.")
    endif()

endfunction()
