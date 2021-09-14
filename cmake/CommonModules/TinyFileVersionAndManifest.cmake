# Configure file version file and Windows resource and manifest files
function(tiny_file_version_and_manifest target version_header_path_in
        version_header_path_out)

    set(tiny_original_extension)
    set(tiny_original_filename)

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

    # Append a major version number for Windows shared libraries only
    if(${TinyOrm_target}_VERSION_EXT)
        set(tiny_original_filename
            "${target}${PROJECT_VERSION_MAJOR}${tiny_original_extension}")
    else()
        set(tiny_original_filename
            "${target}${tiny_original_extension}")
    endif()

    configure_file(
        "${version_header_path_in}version.hpp.in"
        "${version_header_path_out}version.hpp"
        @ONLY NEWLINE_STYLE LF
    )

    target_sources(${target} PRIVATE
        "${PROJECT_BINARY_DIR}/${version_header_path_out}version.hpp"
    )

    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_sources(${target} PRIVATE
            "resources/${target}.rc"
            "resources/${target}${tiny_original_extension}.manifest"
        )
    endif()

endfunction()
