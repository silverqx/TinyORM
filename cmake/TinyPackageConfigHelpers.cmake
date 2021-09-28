# Build configuration type requirements for an Install Tree
# Build configurations are considered to match if both are "Debug" or neither
# is "Debug", otherwise all other configurations can be matched freely.
function(tiny_build_type_requirements_install_tree
        out_package_version out_package_version_unsuitable
        cvf_is_multi_config cvf_config_build_type)

    message(DEBUG "cvf_is_multi_config = ${cvf_is_multi_config}")

    if(NOT cvf_is_multi_config)
        get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

        message(DEBUG "isMultiConfig = ${isMultiConfig}")
        message(DEBUG "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
        message(DEBUG "cvf_config_build_type = ${cvf_config_build_type}")
        message(DEBUG "MSVC = ${MSVC}")
        message(DEBUG "CMAKE_CURRENT_LIST_FILE = ${CMAKE_CURRENT_LIST_FILE}")

        if(isMultiConfig)
            set(${out_package_version} "${${out_package_version}} single-config"
                PARENT_SCOPE)
            set(${out_package_version_unsuitable} TRUE PARENT_SCOPE)
            return()

        elseif(MSVC AND ((CMAKE_BUILD_TYPE STREQUAL "Debug"
                AND NOT cvf_config_build_type STREQUAL "Debug")
                OR (NOT CMAKE_BUILD_TYPE STREQUAL "Debug"
                    AND cvf_config_build_type STREQUAL "Debug"))
        )
            set(${out_package_version}
                "${${out_package_version}} single-config CMAKE_BUILD_TYPE=${cvf_config_build_type}"
                PARENT_SCOPE)
            set(${out_package_version_unsuitable} TRUE PARENT_SCOPE)
            return()
        endif()
    endif()

endfunction()

# Build configuration type requirements for a Build Tree
# Build configurations are considered to match only when they are equal if
# the MATCH_EQUAL_EXPORTED_BUILDTREE option is enabled or for MSVC, build
# configurations are considered to match if both are "Debug" or neither
# is "Debug", otherwise all other configurations can be matched freely.
function(tiny_build_type_requirements_build_tree
        out_package_version out_package_version_unsuitable
        cvf_is_multi_config cvf_config_build_type cvf_match_buildtree)

    message(DEBUG "cvf_is_multi_config = ${cvf_is_multi_config}")

    if(NOT cvf_is_multi_config)
        get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

        message(DEBUG "isMultiConfig = ${isMultiConfig}")
        message(DEBUG "cvf_match_buildtree = ${cvf_match_buildtree}")
        message(DEBUG "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
        message(DEBUG "cvf_config_build_type = ${cvf_config_build_type}")
        message(DEBUG "MSVC = ${MSVC}")
        message(DEBUG "CMAKE_CURRENT_LIST_FILE = ${CMAKE_CURRENT_LIST_FILE}")

        # TODO forbid MINGW64 vs UCRT64 vs MSVC mismatch on windows silverqx

        if(isMultiConfig)
            set(${out_package_version} "${${out_package_version}} single-config"
                PARENT_SCOPE)
            set(${out_package_version_unsuitable} TRUE PARENT_SCOPE)
            return()

        elseif((cvf_match_buildtree
                AND NOT CMAKE_BUILD_TYPE STREQUAL cvf_config_build_type)
                OR (MSVC AND ((CMAKE_BUILD_TYPE STREQUAL "Debug"
                    AND NOT cvf_config_build_type STREQUAL "Debug")
                    OR (NOT CMAKE_BUILD_TYPE STREQUAL "Debug"
                        AND cvf_config_build_type STREQUAL "Debug")))
        )
            set(${out_package_version}
                "${${out_package_version}} single-config CMAKE_BUILD_TYPE=${cvf_config_build_type}"
                PARENT_SCOPE)
            set(${out_package_version_unsuitable} TRUE PARENT_SCOPE)
            return()
        endif()
    endif()

endfunction()
