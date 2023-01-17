include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(TinyHelpers)

set(TINY_BUILD_BUILDTREEDIR "${TINY_BUILD_GENDIR}/buildtree" CACHE INTERNAL
    "Generated content in the build tree for the build tree")
set(TINY_BUILD_INSTALLTREEDIR "${TINY_BUILD_GENDIR}/installtree" CACHE INTERNAL
    "Generated content in the build tree for the install tree")

# Create Package Config and Package Config Version files and install the TinyORM Project
function(tiny_install_tinyorm)

    # Install targets from the project and assign them to the export set
    install(
        TARGETS ${TinyOrm_target} ${CommonConfig_target}
        EXPORT TinyOrmTargets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        LIBRARY ARCHIVE RUNTIME
        # TODO test NAMELINK_ on unix silverqx
    )

    if(TINY_VCPKG)
        set(tiny_config_package_dir "${CMAKE_INSTALL_DATADIR}/cmake/${TinyOrm_ns}")
    else()
        set(tiny_config_package_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${TinyOrm_ns}")
    endif()

    # Generate and install a code that can be used to import targets from the Install Tree
    install(
        EXPORT TinyOrmTargets
        NAMESPACE ${TinyOrm_ns}::
        DESTINATION "${tiny_config_package_dir}"
    )

    if(MSVC AND BUILD_SHARED_LIBS)
        install(FILES "$<TARGET_PDB_FILE:${TinyOrm_target}>" TYPE BIN OPTIONAL)
    endif()

    # Do not install Package config, config version, header, doc. and CMake helper files
    # when installing for VCPKG Debug configuration
    if(TINY_VCPKG AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        return()
    endif()

    # Install all other files
    install(DIRECTORY "include/orm" TYPE INCLUDE FILES_MATCHING PATTERN "*.hpp")
    file(GLOB tiny_docs "docs/*.mdx")
    install(FILES ${tiny_docs} DESTINATION "${CMAKE_INSTALL_DOCDIR}/mdx")
    install(FILES AUTHOR LICENSE TYPE DOC)
    install(FILES NOTES.txt TYPE DOC RENAME NOTES)
    install(FILES README.md TYPE DOC RENAME README)

    # TinyORM's package config needs the FindMySQL package module when the MYSQL_PING
    # is enabled
    set(tiny_cmake_module_path)
    if(MYSQL_PING)
        install(FILES "cmake/Modules/FindMySQL.cmake"
            DESTINATION "${tiny_config_package_dir}/Modules"
        )

        set(tiny_cmake_module_path "\
# To find the FindMySQL package module
list(APPEND CMAKE_MODULE_PATH \"\${CMAKE_CURRENT_LIST_DIR}/Modules\")")
    endif()

    # Helpers to match build configuration types for an Install Tree
    install(FILES "cmake/TinyPackageConfigHelpers.cmake"
        DESTINATION "${tiny_config_package_dir}/Modules"
    )

    # Used in the Package Config and Config Version files
    get_property(cvf_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    tiny_to_bool(cvf_is_multi_config ${cvf_is_multi_config})

    tiny_to_bool(cvf_is_vcpkg ${TINY_VCPKG})

    # Install destination directories for the Install Tree
    set(BIN_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}/")
    set(CONFIG_INSTALL_DIR "${tiny_config_package_dir}/")
    set(DOC_INSTALL_DIR "${CMAKE_INSTALL_DOCDIR}/")
    set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/")
    set(LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/")

    # Configure Package Config file for the Install Tree
    configure_package_config_file(
        "cmake/TinyOrmConfig.cmake.in"
        "${TINY_BUILD_INSTALLTREEDIR}/TinyOrmConfig.cmake"
        INSTALL_DESTINATION "${tiny_config_package_dir}"
        # TODO future, do this like Qt is doing it, I'm missing Qt5Core_COMPILE_DEFINITIONS, Qt5Core_DEFINITIONS, Qt5Core_INCLUDE_DIRS, Qt5Core_LIBRARIES, also do the same for the build tree silverqx
        PATH_VARS
            BIN_INSTALL_DIR CONFIG_INSTALL_DIR DOC_INSTALL_DIR INCLUDE_INSTALL_DIR
            LIB_INSTALL_DIR
    )

    # Compatible Interface Requirement for the project's major version
    set_property(
        TARGET ${TinyOrm_target}
        PROPERTY INTERFACE_${TinyOrm_target}_MAJOR_VERSION ${PROJECT_VERSION_MAJOR}
    )
    set_property(
        TARGET ${TinyOrm_target} APPEND PROPERTY
        COMPATIBLE_INTERFACE_STRING ${TinyOrm_target}_MAJOR_VERSION
    )

    # Generate the Package Version file for the Package Config file for the Install Tree
    write_basic_package_version_file(
        "${TINY_BUILD_INSTALLTREEDIR}/TinyOrmConfigVersion.cmake.in"
        COMPATIBILITY SameMajorVersion
    )

    # Append build type requirements at the end of a generated package version file
    file(READ "cmake/TinyConfigVersionBuildTypeReq.cmake.in" buildTypeReqTempalate)
    file(APPEND
        "${PROJECT_BINARY_DIR}/${TINY_BUILD_INSTALLTREEDIR}/TinyOrmConfigVersion.cmake.in"
        "\n${buildTypeReqTempalate}"
    )

    configure_file(
        "${PROJECT_BINARY_DIR}/${TINY_BUILD_INSTALLTREEDIR}/TinyOrmConfigVersion.cmake.in"
        "${TINY_BUILD_INSTALLTREEDIR}/TinyOrmConfigVersion.cmake"
        @ONLY NEWLINE_STYLE LF
    )

    # Install Package Config and Package Config Verion files
    install(
        FILES
            "${PROJECT_BINARY_DIR}/${TINY_BUILD_INSTALLTREEDIR}/TinyOrmConfig.cmake"
            "${PROJECT_BINARY_DIR}/${TINY_BUILD_INSTALLTREEDIR}/TinyOrmConfigVersion.cmake"
        DESTINATION "${tiny_config_package_dir}"
    )

endfunction()

# Create Package Config and Package Config Version files for the Build Tree and export it
function(tiny_export_build_tree)

    # TinyORM's package config needs the FindMySQL package module when the MYSQL_PING
    # is enabled
    set(tiny_cmake_module_path)
    if(MYSQL_PING)
        file(COPY "cmake/Modules/FindMySQL.cmake" DESTINATION "cmake/Modules")

        set(tiny_cmake_module_path "\
# To find the FindMySQL package module
list(APPEND CMAKE_MODULE_PATH \"\${CMAKE_CURRENT_LIST_DIR}/cmake/Modules\")")
    endif()

    # Helpers to match build configuration types for a Build Tree
    file(COPY "cmake/TinyPackageConfigHelpers.cmake" DESTINATION "cmake/Modules")

    # Export Targets from the Build Tree
    export(
        EXPORT TinyOrmTargets
        FILE "TinyOrmTargets.cmake"
        NAMESPACE ${TinyOrm_ns}::
    )

    get_property(cvf_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    tiny_to_bool(cvf_is_multi_config ${cvf_is_multi_config})

    # Configure Package Config file for the Build Tree
    configure_package_config_file(
        "cmake/TinyOrmBuildTreeConfig.cmake.in"
        "TinyOrmConfig.cmake"
        # TODO check on unix if "/" is absolute path silverqx
        # TODO also test how file(RELATIVE_PATH) behaves on unix with "/" and "./" silverqx
        INSTALL_DESTINATION "./"
        INSTALL_PREFIX "${PROJECT_BINARY_DIR}"
        NO_SET_AND_CHECK_MACRO
    )

    # Generate the Package Version file for the Package Config file for the Build Tree
    write_basic_package_version_file(
        "${TINY_BUILD_BUILDTREEDIR}/TinyOrmConfigVersion.cmake.in"
        COMPATIBILITY SameMajorVersion
    )

    # Append build type requirements at the end of a generated package version file
    file(READ "cmake/TinyBuildTreeConfigVersionBuildTypeReq.cmake.in"
        buildTypeReqTempalate
    )
    file(APPEND
        "${PROJECT_BINARY_DIR}/${TINY_BUILD_BUILDTREEDIR}/TinyOrmConfigVersion.cmake.in"
        "\n${buildTypeReqTempalate}"
    )

    configure_file(
        "${PROJECT_BINARY_DIR}/${TINY_BUILD_BUILDTREEDIR}/TinyOrmConfigVersion.cmake.in"
        "TinyOrmConfigVersion.cmake"
        @ONLY NEWLINE_STYLE LF
    )

    # Store the current Build Tree in the CMake User Package Registry
    export(PACKAGE ${TinyOrm_ns})

endfunction()
