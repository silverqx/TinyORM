# Configure Windows resource and manifest files
function(tiny_resource_and_manifest target)

    set(options TEST)
    set(oneValueArgs OUTPUT_DIR RESOURCES_DIR)
    cmake_parse_arguments(PARSE_ARGV 1 TINY ${options} "${oneValueArgs}" "")

    # Include Windows RC and manifest file for a shared library or executable
    get_target_property(target_type ${target} TYPE)

    # The static archive doesn't need an RC or manifest file
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows" OR
            (NOT target_type STREQUAL "EXECUTABLE" AND
                NOT target_type STREQUAL "SHARED_LIBRARY")
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

    # All tests use the same TinyTest.rc.in file
    if(TINY_TEST)
        set(rcBasename TinyTest)

        # Used for icon basename
        set(TinyTest_icon ${rcBasename})
        # Test's RC file has a common substitution token for all tests
        set(TinyTest_target ${target})
    else()
        set(rcBasename ${target})
    endif()

    set(tiny_manifest_basename ${rcBasename})

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
        if(target_type STREQUAL "SHARED_LIBRARY")
            set(tiny_original_extension "${CMAKE_SHARED_LIBRARY_SUFFIX}")
        elseif(target_type STREQUAL "EXECUTABLE")
            set(tiny_original_extension "${CMAKE_EXECUTABLE_SUFFIX}")
        endif()

        target_sources(${target} PRIVATE
            "${TINY_RESOURCES_DIR}/${tiny_manifest_basename}${tiny_original_extension}.manifest"
        )
    endif()

endfunction()
