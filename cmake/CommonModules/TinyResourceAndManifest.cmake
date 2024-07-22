# Configure Windows resource and manifest files
function(tiny_resource_and_manifest target)

    set(options TEST)
    set(oneValueArgs OUTPUT_DIR RESOURCES_DIR RESOURCE_BASENAME MANIFEST_BASENAME)
    cmake_parse_arguments(PARSE_ARGV 1 TINY ${options} "${oneValueArgs}" "")

    if(DEFINED TINY_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "The ${CMAKE_CURRENT_FUNCTION}() was passed extra arguments: \
${TINY_UNPARSED_ARGUMENTS}")
    endif()

    # Include Windows RC and manifest file for a shared library or executable
    get_target_property(target_type ${target} TYPE)

    # The static archive doesn't need an RC or manifest file
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows" OR
            NOT (target_type STREQUAL "EXECUTABLE" OR
                 target_type STREQUAL "SHARED_LIBRARY" OR
                 target_type STREQUAL "MODULE_LIBRARY")
    )
        return()
    endif()

    # Initialize variables
    set(tiny_original_extension)
    if(NOT TINY_OUTPUT_DIR)
        set(TINY_OUTPUT_DIR "tmp/")
    endif()

    if(NOT DEFINED TINY_RESOURCES_DIR)
        set(TINY_RESOURCES_DIR "resources")
    endif()

    # Absolute paths will not be touched
    file(REAL_PATH "${TINY_OUTPUT_DIR}" TINY_OUTPUT_DIR
        BASE_DIRECTORY "${PROJECT_BINARY_DIR}"
    )
    file(REAL_PATH "${TINY_RESOURCES_DIR}" TINY_RESOURCES_DIR
        BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
    )

    # Modify the Tom_target variable for substitution (original exe and icon filename)
    # This is special logic because Tom_target is used in 3 CMake projects
    # Used only in the tom.rc.in, I'm not going to wrap it in the if()
    set(Tom_target ${target})

    # Allow to pass a custom RC basename
    if(DEFINED TINY_RESOURCE_BASENAME)
        set(rcBasename ${TINY_RESOURCE_BASENAME})

    # All tests use the same TinyTest.rc.in file
    elseif(TINY_TEST)
        set(rcBasename TinyTest)

        # Used for icon basename
        set(TinyTest_icon ${rcBasename})
        # Test's RC file has a common substitution token for all tests
        set(TinyTest_target ${target})
    else()
        set(rcBasename ${target})
    endif()

    # Allow to pass a custom manifest basename
    if(DEFINED TINY_MANIFEST_BASENAME)
        set(tiny_manifest_basename ${TINY_MANIFEST_BASENAME})
        # For MinGW (used only in the tom.rc.in)
        set(Tom_manifest ${TINY_MANIFEST_BASENAME})
    else()
        set(tiny_manifest_basename ${rcBasename})
        # For MinGW (used only in the tom.rc.in)
        set(Tom_manifest ${rcBasename})
    endif()

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
        # Obtain extension by target type - .exe or .dll
        if(target_type STREQUAL "SHARED_LIBRARY" OR target_type STREQUAL "MODULE_LIBRARY")
            set(tiny_original_extension "${CMAKE_SHARED_LIBRARY_SUFFIX}")
        elseif(target_type STREQUAL "EXECUTABLE")
            set(tiny_original_extension "${CMAKE_EXECUTABLE_SUFFIX}")
        endif()

        target_sources(${target} PRIVATE
            "${TINY_RESOURCES_DIR}/${tiny_manifest_basename}${tiny_original_extension}.manifest"
        )
    endif()

endfunction()
