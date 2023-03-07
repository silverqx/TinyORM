# Merge release and debug CMake targets and configs to support multiconfig generators.
# 1. Fix ${_IMPORT_PREFIX} of IMPORTED_IMPLIB_DEBUG and IMPORTED_LOCATION_DEBUG
#    in auto-generated debug target
# 2. Move /debug/share/cmake/TinyOrm/*[Tt]argets-debug.cmake to /share/cmake/${PORT}/
function(tiny_cmake_config_fixup)

    set(TinyOrm_ns TinyOrm)
    set(debug_cmake "${CURRENT_PACKAGES_DIR}/debug/share/cmake/${TinyOrm_ns}")
    set(release_cmake "${CURRENT_PACKAGES_DIR}/share/cmake/${PORT}")

    file(GLOB debug_targets "${debug_cmake}/*[Tt]argets-debug.cmake")

    foreach(debug_target IN LISTS debug_targets)
        get_filename_component(debug_target_filename "${debug_target}" NAME)

        file(READ "${debug_target}" contents)

        string(REPLACE "\${_IMPORT_PREFIX}/lib" "\${_IMPORT_PREFIX}/debug/lib"
            contents "${contents}")
        string(REPLACE "\${_IMPORT_PREFIX}/bin" "\${_IMPORT_PREFIX}/debug/bin"
            contents "${contents}")

        file(WRITE "${release_cmake}/${debug_target_filename}" "${contents}")

        file(REMOVE "${debug_target}")
    endforeach()

endfunction()
