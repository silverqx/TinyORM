include(TinyHelpers)

# Initialize CMake default variables by project options
function(tiny_init_cmake_variables)

    set(CMAKE_FIND_PACKAGE_SORT_ORDER NATURAL)
    set(CMAKE_FIND_PACKAGE_SORT_DIRECTION DEC)
    set(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION YES)

    tiny_invert_bool(PRECOMPILE_HEADERS ${PRECOMPILE_HEADERS})
    set(CMAKE_DISABLE_PRECOMPILE_HEADERS ${PRECOMPILE_HEADERS})

    if (EXPORT_PACKAGE_REGISTRY)
        set(CMAKE_EXPORT_PACKAGE_REGISTRY YES)
    endif()

    if(MSVC_RUNTIME_DYNAMIC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()

    # TODO test on unix silverqx
#    set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

    # Fix install prefix for the x64 toolchain
    if(WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT
            AND CMAKE_SIZEOF_VOID_P EQUAL 8
    )
        get_property(intall_prefix_docs CACHE CMAKE_INSTALL_PREFIX PROPERTY HELPSTRING)

        set(CMAKE_INSTALL_PREFIX "C:/Program Files/${PROJECT_NAME}"
            CACHE PATH "${intall_prefix_docs}" FORCE
        )
    endif()

endfunction()

# Initialize variable for append a major version number for Windows shared libraries
function(tiny_init_target_version_ext target)

    set(result OFF)
    get_target_property(target_type ${target} TYPE)

    if(WIN32 AND target_type STREQUAL "SHARED_LIBRARY")
        set(result ON)
    endif()

    set(${target}_VERSION_EXT ${result} PARENT_SCOPE)

endfunction()

# Initialize Tiny variables
macro(tiny_init_tiny_variables)

    # List of package dependencies for the package config
    set(tiny_package_dependencies)

endmacro()
