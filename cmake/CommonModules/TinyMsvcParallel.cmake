# Allow per-translation-unit parallel builds when using MSVC
function(tiny_msvc_parallel desc)

    if(CMAKE_GENERATOR MATCHES "Visual Studio"
            AND (CMAKE_C_COMPILER_ID MATCHES "MSVC|Intel"
                OR CMAKE_CXX_COMPILER_ID MATCHES "MSVC|Intel")
    )
        set(MSVC_PARALLEL ON CACHE STRING "${desc}")

        if(MSVC_PARALLEL)
            if(MSVC_PARALLEL GREATER 0)
                string(APPEND CMAKE_C_FLAGS " /MP${CMake_MSVC_PARALLEL}")
                string(APPEND CMAKE_CXX_FLAGS " /MP${CMake_MSVC_PARALLEL}")
            else()
                string(APPEND CMAKE_C_FLAGS " /MP")
                string(APPEND CMAKE_CXX_FLAGS " /MP")
            endif()
        endif()
    endif()

endfunction()
