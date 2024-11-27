# Configure Windows resource and manifest files
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
    # Don't use cmake_path(IS_ABSOLUTE), the if(IS_ABSOLUTE) covers also c:xyz or /xyz.
    if(IS_ABSOLUTE "${TINY_OUTPUT_DIR}")
        message(FATAL_ERROR "The 'OUTPUT_DIR' single-value keyword cannot be an absolute \
path (must be relative to PROJECT_BINARY_DIR) in ${CMAKE_CURRENT_FUNCTION}().")
    endif()

    if(("RESOURCE_BASENAME" IN_LIST TINY_KEYWORDS_MISSING_VALUES OR
            "MANIFEST_BASENAME" IN_LIST TINY_KEYWORDS_MISSING_VALUES) OR
        # This doesn't work with the CMP0174 set to OLD, there is no simple way to handle
        # this case with OLD other than parsing arguments manually and it's not worth
        # the effort (I shouldn't even write these checks as it's an internal thing 😵‍💫).
        ((DEFINED RESOURCE_BASENAME AND "${RESOURCE_BASENAME}" STREQUAL "") OR
            (DEFINED MANIFEST_BASENAME AND "${MANIFEST_BASENAME}" STREQUAL ""))
    )
        message(FATAL_ERROR "The ${CMAKE_CURRENT_FUNCTION}() is missing a value or \
the value is empty for some keywords: MANIFEST_BASENAME, RESOURCE_BASENAME")
    endif()

    # Body
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

    # file(REAL_PATH) can't be used here because it started throwing a warning since v3.31
    # if a folder doesn't exist. Another problem is that it resolves symlinks to actual
    # locations, and that's undesirable with QtCreator's build tree junctions feature.

    # TINY_OUTPUT_DIR (absolute path)
    if(NOT DEFINED TINY_OUTPUT_DIR OR "${TINY_OUTPUT_DIR}" STREQUAL "")
        set(TINY_OUTPUT_DIR "${PROJECT_BINARY_DIR}/tmp")
    else() # It's always relative, see check above
        string(PREPEND TINY_OUTPUT_DIR "${PROJECT_BINARY_DIR}/")
    endif()

    # TINY_RESOURCES_DIR (absolute path)
    if(NOT DEFINED TINY_RESOURCES_DIR OR "${TINY_RESOURCES_DIR}" STREQUAL "")
        set(TINY_RESOURCES_DIR "${PROJECT_SOURCE_DIR}/resources")
    elseif(NOT IS_ABSOLUTE "${TINY_RESOURCES_DIR}")
        string(PREPEND TINY_RESOURCES_DIR "${PROJECT_SOURCE_DIR}/")
    endif()

    file(TO_CMAKE_PATH ${TINY_OUTPUT_DIR} TINY_OUTPUT_DIR)
    file(TO_CMAKE_PATH ${TINY_RESOURCES_DIR} TINY_RESOURCES_DIR)

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
        set(tiny_original_extension "")

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
