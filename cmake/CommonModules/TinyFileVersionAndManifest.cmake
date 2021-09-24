# Configure file version file and Windows resource and manifest files
function(tiny_file_version_and_manifest target version_header_path_in
        version_header_path_out tmp_folder_path)

    set(tiny_original_extension)
    set(tiny_original_filename)
    if(NOT tmp_folder_path)
        set(tmp_folder_path "tmp/")
    endif()

    # The path is relative to the build tree, an absolute path will not be touched
    file(REAL_PATH "${tmp_folder_path}" tmp_folder_path
        BASE_DIRECTORY "${CMAKE_BINARY_DIR}"
    )

    get_target_property(target_type ${target} TYPE)

    if(target_type STREQUAL "SHARED_LIBRARY")
        if(BUILD_SHARED_LIBS)
            set(tiny_original_extension "${CMAKE_SHARED_LIBRARY_SUFFIX}")
        else()
            set(tiny_original_extension "${CMAKE_STATIC_LIBRARY_SUFFIX}")
        endif()
    elseif(target_type STREQUAL "EXECUTABLE")
        set(tiny_original_extension "${CMAKE_EXECUTABLE_SUFFIX}")
    endif()

    # OriginalFilename behavior:
    # - append a major version number for Windows shared libraries only
    # - append d for Debug configuration for shared and static library

    configure_file(
        "${version_header_path_in}version.hpp.in"
        "${tmp_folder_path}version.hpp.in"
        @ONLY NEWLINE_STYLE LF
    )

    # To support an OriginalFilename in Windows RC file for multi-config generators
    file(GENERATE OUTPUT "${version_header_path_out}version.hpp"
        INPUT "${CMAKE_BINARY_DIR}/${tmp_folder_path}version.hpp.in"
        NEWLINE_STYLE UNIX
    )

    target_sources(${target} PRIVATE
        "${PROJECT_BINARY_DIR}/${version_header_path_out}version.hpp"
    )

    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        # Version Info Resource file
        target_sources(${target} PRIVATE "resources/${target}.rc")

        # Manifest (injected through the RC file on MinGW)
        if(MINGW)
            target_sources(${target} PRIVATE "resources/${target}_mingw.rc")
        else()
            target_sources(${target} PRIVATE
                "resources/${target}${tiny_original_extension}.manifest"
            )
        endif()
    endif()

endfunction()
