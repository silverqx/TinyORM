# Configure Windows resource and manifest files
function(tiny_resource_and_manifest target tmp_folder_path)

    # Include Windows RC and manifest file for a shared library or executable
    get_target_property(target_type ${target} TYPE)

    # The static archive doesn't need an RC or manifest file
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows" OR
            (NOT target_type STREQUAL "EXECUTABLE" AND
                NOT target_type STREQUAL "SHARED_LIBRARY")
    )
        return()
    endif()

    set(tiny_original_extension)
    if(NOT tmp_folder_path)
        set(tmp_folder_path "tmp/")
    endif()

    # The path is relative to the build tree, an absolute path will not be touched
    file(REAL_PATH "${tmp_folder_path}" tmp_folder_path
        BASE_DIRECTORY "${CMAKE_BINARY_DIR}"
    )

    configure_file(
        "resources/${target}.rc.in"
        "${tmp_folder_path}/${target}_genexp.rc.in"
        @ONLY NEWLINE_STYLE LF
    )

    # To support an OriginalFilename in Windows RC file for multi-config generators
    file(GENERATE OUTPUT "${tmp_folder_path}/${target}.rc"
        INPUT "${tmp_folder_path}/${target}_genexp.rc.in"
        NEWLINE_STYLE UNIX
    )

    # Needed in the RC file, MinGW does not define the _DEBUG macro
    if(MINGW)
        set_source_files_properties("${tmp_folder_path}/${target}.rc"
            TARGET_DIRECTORY ${target}
            PROPERTIES COMPILE_DEFINITIONS $<$<CONFIG:Debug>:_DEBUG>
        )
    endif()

    # Windows Resource file
    target_sources(${target} PRIVATE
        "resources/${target}.rc.in"
        "${tmp_folder_path}/${target}_genexp.rc.in"
        "${tmp_folder_path}/${target}.rc"
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
            "resources/${target}${tiny_original_extension}.manifest"
        )
    endif()

endfunction()
