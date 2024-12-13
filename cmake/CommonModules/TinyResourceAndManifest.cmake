include_guard(GLOBAL)

# Configure Windows resource and manifest files
#
# Synopsis:
# tiny_resource_and_manifest(<target> [TEST]
#   [OUTPUT_DIR [<dir>]]
#   [RESOURCES_DIR [<dir>]]
#   [RESOURCE_BASENAME [<resource-basename>]]
#   [MANIFEST_BASENAME [<manifest-basename>]]
# )
#
# <target> name to operate on.
# TEST to configure the test <target>, all tests use the same TinyTest.rc.in file.
# OUTPUT_DIR relative folder path for the configure_file() for generated content.
# The CMAKE_CURRENT_BINARY_DIR/.tiny will be used if is empty or undefined.
# RESOURCES_DIR relative or absolute folder path where the TinyXyz.rc.in file is located.
# The CMAKE_CURRENT_SOURCE_DIR/resources will be used if is empty or undefined.
# RESOURCE_BASENAME basename for the resource file, eg. TinyOrm will use TinyOrm.rc.in.
# The <target> name and for TEST the TinyTest will be used if is empty or undefined.
# MANIFEST_BASENAME basename for the manifest file, eg. TinyOrm library will use
# TinyOrm.dll.manifest, for executables it will be eg. tom.exe.manifest.
# The <resource-basename> will be used if empty or undefined.
function(tiny_resource_and_manifest target)

    # Arguments
    set(options TEST)
    set(oneValueArgs OUTPUT_DIR RESOURCES_DIR RESOURCE_BASENAME MANIFEST_BASENAME)
    cmake_parse_arguments(PARSE_ARGV 1 TINY ${options} "${oneValueArgs}" "")

    # Arguments checks
    if(DEFINED TINY_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "The ${CMAKE_CURRENT_FUNCTION}() was passed extra arguments: \
${TINY_UNPARSED_ARGUMENTS}")
    endif()

    # It must be inside the build tree and it's handled per-project below.
    # RESOURCES_DIR can theoretically be anywhere, so it doesn't have this limitation.
    # Don't use cmake_path(IS_ABSOLUTE), the if(IS_ABSOLUTE) is TRUE also for c:xyz or
    # /xyz on Windows, which by accident is what I want. 😵‍💫
    if(IS_ABSOLUTE "${TINY_OUTPUT_DIR}")
        message(FATAL_ERROR "The 'OUTPUT_DIR' single-value keyword cannot be an absolute \
path (must be relative to CMAKE_CURRENT_BINARY_DIR) in ${CMAKE_CURRENT_FUNCTION}().")
    endif()

    # Body
    # Include Windows RC and manifest file for a shared library or executable
    get_target_property(targetType ${target} TYPE)

    # Return if the target type isn't an executable (is static archive library)
    # The static archive doesn't need an RC or manifest file
    _tiny_resource_return_if_wrong_type() # return()

    # Initialize variables

    # file(REAL_PATH) can't be used here because it started throwing a warning since v3.31
    # if a folder doesn't exist. Another problem is that it resolves symlinks to actual
    # locations, and that's undesirable with QtCreator's build tree junctions feature.

    # TINY_OUTPUT_DIR as an absolute path, normalize it, and remove trailing slashes
    _tiny_resource_get_output_dir(TINY_OUTPUT_DIR)
    # TINY_RESOURCES_DIR as an absolute path, normalize it, and remove trailing slashes
    _tiny_resource_get_resources_dir(TINY_RESOURCES_DIR)

    # Prepare file basename for the TinyXyz.rc.in resource file
    _tiny_resource_prepare_resource_basename()
    # Prepare file basename for the TinyXyz.exe/dll.manifest file
    _tiny_resource_prepare_manifest_basename()

    # Modify the Tom_target variable for substitution (original exe and icon filename)
    # This is special logic because Tom_target is used in 3 CMake projects
    # Used only in the tom.rc.in, I'm not going to wrap it in the if()
    set(Tom_target ${target})
    # CMake doesn't have problem with UTF-8 encoded files
    set(pragma_codepage "65001")

    # Start configuring
    configure_file(
        "${TINY_RESOURCES_DIR}/${rcBasename}.rc.in"
        "${TINY_OUTPUT_DIR}/${rcBasename}_genexp.rc.in"
        @ONLY NEWLINE_STYLE LF
    )

    # To support an OriginalFilename in Windows RC file for multi-config generators
    file(GENERATE OUTPUT "${TINY_OUTPUT_DIR}/${rcBasename}-$<CONFIG>.rc"
        INPUT "${TINY_OUTPUT_DIR}/${rcBasename}_genexp.rc.in"
        NEWLINE_STYLE UNIX
    )

    # Needed in the RC file, MinGW does not define the _DEBUG macro
    if(MINGW)
        # TODO ask if is planned support for genex in the filepath silverqx
        set_source_files_properties("${TINY_OUTPUT_DIR}/${rcBasename}-Debug.rc"
            TARGET_DIRECTORY ${target}
            PROPERTIES COMPILE_DEFINITIONS $<$<CONFIG:Debug>:_DEBUG>
        )
    endif()

    # TODO multi-config and empty $<CONFIG>, why silverqx
    # Windows Resource file
    target_sources(${target} PRIVATE
        "${TINY_RESOURCES_DIR}/${rcBasename}.rc.in"
        "${TINY_OUTPUT_DIR}/${rcBasename}_genexp.rc.in"
        "$<$<BOOL:$<CONFIG>>:${TINY_OUTPUT_DIR}/${rcBasename}-$<CONFIG>.rc>"
    )

    # Manifest file (injected through the RC file on MinGW)
    if(NOT MINGW)
        # Obtain an executable extension by the target type (.exe or .dll)
        tiny_get_target_extension(originalExtension ${target})

        set(manifestFilename "${manifestBasename}${originalExtension}.manifest")

        target_sources(${target} PRIVATE "${TINY_RESOURCES_DIR}/${manifestFilename}")
    endif()

endfunction()

# Return if the target type isn't an executable (is static archive library)
macro(_tiny_resource_return_if_wrong_type)

    # The static archive doesn't need an RC or manifest file
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows" OR
            NOT (targetType MATCHES "^(EXECUTABLE|SHARED_LIBRARY|MODULE_LIBRARY)$")
    )
        return() # Applies for the caller, not here
    endif()

endmacro()

# Get TINY_OUTPUT_DIR as an absolute path, normalize it, and remove trailing slashes
function(_tiny_resource_get_output_dir output_dir_variable)

    # Provide the default value if empty, undefined, or missing value
    if("${${output_dir_variable}}" STREQUAL "")
        set(${output_dir_variable}
            "${CMAKE_CURRENT_BINARY_DIR}/${TINY_BUILD_TMP_DIR}" PARENT_SCOPE
        )
        return()
    endif()

    # It's always relative, see the check above (so no if() checks needed)
    cmake_path(ABSOLUTE_PATH ${output_dir_variable}
        BASE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        NORMALIZE OUTPUT_VARIABLE result
    )

    # Remove trailing slashes
    tiny_path_remove_slashes(result)

    set(${output_dir_variable} "${result}" PARENT_SCOPE)

endfunction()

# Get TINY_RESOURCES_DIR as an absolute path, normalize it, and remove trailing slashes
function(_tiny_resource_get_resources_dir resources_dir_variable)

    # For a shorter variable name
    set(result "${${resources_dir_variable}}")

    # Provide the default value if empty, undefined, or missing value
    if("${result}" STREQUAL "")
        set(${resources_dir_variable}
            "${CMAKE_CURRENT_SOURCE_DIR}/${TINY_SOURCE_RESOURCES_DIR}" PARENT_SCOPE
        )
        return()
    endif()

    # Absolute paths stay untouched
    cmake_path(ABSOLUTE_PATH result NORMALIZE OUTPUT_VARIABLE result) # Relative to CMAKE_CURRENT_SOURCE_DIR

    # Remove trailing slashes
    tiny_path_remove_slashes(result)

    set(${resources_dir_variable} "${result}" PARENT_SCOPE)

endfunction()

# Prepare file basename for the TinyXyz.rc.in resource file
macro(_tiny_resource_prepare_resource_basename)

    # if()-s order is important in this macro

    # Set what was passed
    if(NOT "${TINY_RESOURCE_BASENAME}" STREQUAL "")
        set(rcBasename ${TINY_RESOURCE_BASENAME})

    # All tests use the same TinyTest.rc.in file
    elseif(TINY_TEST)
        set(rcBasename "TinyTest")

        # Used for icon basename
        set(TinyTest_icon ${rcBasename})
        # Test's RC file has a common substitution token for all tests
        set(TinyTest_target ${target})

    # Provide the default value if empty, undefined, or missing value
    else()
        set(rcBasename ${target})
    endif()

endmacro()

# Prepare file basename for the TinyXyz.exe/dll.manifest file
macro(_tiny_resource_prepare_manifest_basename)

    # Provide the default value if empty, undefined, or missing value
    if("${TINY_MANIFEST_BASENAME}" STREQUAL "")
        set(manifestBasename ${rcBasename})
        # For MinGW (used only in the tom.rc.in)
        set(Tom_manifest ${rcBasename})

    # Otherwise set what was passed
    else()
        set(manifestBasename ${TINY_MANIFEST_BASENAME})
        # For MinGW (used only in the tom.rc.in)
        set(Tom_manifest ${TINY_MANIFEST_BASENAME})
    endif()

endmacro()
