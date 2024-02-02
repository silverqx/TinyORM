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
    # TinyOrm library
    install(
        TARGETS ${TinyOrm_target} ${CommonConfig_target}
        EXPORT TinyOrmTargets
        # The $<INSTALL_INTERFACE:xyz> in the target_include_directories() doesn't
        # need to be defined because of this line
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        LIBRARY ARCHIVE RUNTIME
        # TODO test NAMELINK_ on unix silverqx
    )
    # TinyDrivers library
    if(BUILD_DRIVERS)
        install(
            TARGETS ${TinyDrivers_target} ${CommonConfig_target}
            EXPORT TinyDriversTargets
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            LIBRARY ARCHIVE RUNTIME
        )
    endif()
    # TinyMySql library (MODULE, loaded at runtime)
    # The MODULE libraries are installed to the bin/ folder on Linux and
    # they don't have namelink-s
    if(TINY_BUILD_LOADABLE_DRIVERS AND BUILD_MYSQL_DRIVER)
        install(
            TARGETS ${TinyMySql_target}
            EXPORT TinyDriversTargets
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            # The LIBRARY destination must always be provided for MODULE libraries
            LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()

    if(TOM_EXAMPLE AND
            # Don't install for vcpkg debug build type
            NOT (TINY_VCPKG AND TINY_BUILD_TYPE_LOWER STREQUAL "debug")
    )
        install(TARGETS ${TomExample_target} EXPORT TinyOrmTargets RUNTIME)
    endif()

    if(TINY_VCPKG)
        set(tiny_config_package_dir "${CMAKE_INSTALL_DATADIR}/${TINY_PORT}")
    else()
        set(tiny_config_package_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${TinyOrm_ns}")
    endif()

    # Generate and install a code that can be used to import targets from the Install Tree
    install(
        EXPORT TinyOrmTargets
        NAMESPACE ${TinyOrm_ns}::
        DESTINATION "${tiny_config_package_dir}"
    )
    if(BUILD_DRIVERS)
        install(
            EXPORT TinyDriversTargets
            NAMESPACE ${TinyDrivers_ns}::
            DESTINATION "${tiny_config_package_dir}"
        )
    endif()

    # Install PDB files
    if(MSVC AND BUILD_SHARED_LIBS)
        install(FILES "$<TARGET_PDB_FILE:${TinyOrm_target}>" TYPE BIN OPTIONAL)

        if(TOM_EXAMPLE AND
                # Don't install for vcpkg debug build type
                NOT (TINY_VCPKG AND TINY_BUILD_TYPE_LOWER STREQUAL "debug")
        )
            install(FILES "$<TARGET_PDB_FILE:${TomExample_target}>" TYPE BIN OPTIONAL)
        endif()

        if(TINY_BUILD_LOADABLE_DRIVERS OR TINY_BUILD_SHARED_DRIVERS)
            install(FILES "$<TARGET_PDB_FILE:${TinyDrivers_target}>" TYPE BIN OPTIONAL)
        endif()

        if(TINY_BUILD_LOADABLE_DRIVERS AND BUILD_MYSQL_DRIVER)
            install(FILES "$<TARGET_PDB_FILE:${TinyMySql_target}>" TYPE BIN OPTIONAL)
        endif()
    endif()

    # Do not install Package config, config version, header, doc. and CMake helper files
    # when installing for VCPKG Debug configuration
    if(TINY_VCPKG AND TINY_BUILD_TYPE_LOWER STREQUAL "debug")
        return()
    endif()

    # Install header files
    install(DIRECTORY "include/orm"
        TYPE INCLUDE
        FILES_MATCHING PATTERN "*.hpp"
        PATTERN "include/orm/tiny" EXCLUDE
    )
    if(ORM)
        install(DIRECTORY "include/orm/tiny"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/orm"
            FILES_MATCHING PATTERN "*.hpp"
        )
    endif()
    if(TOM)
        # The trailing / is important here (skip creating of the include/ folder
        # inside the INCLUDE folder and copy all content directly)
        install(DIRECTORY "tom/include/" TYPE INCLUDE FILES_MATCHING PATTERN "*.hpp")
    endif()
    # TinyDrivers
    if(BUILD_DRIVERS)
        install(DIRECTORY "drivers/common/include/orm"
            TYPE INCLUDE
            FILES_MATCHING PATTERN "*.hpp"
        )
    endif()
    # TinyMySql
    if(TINY_BUILD_LOADABLE_DRIVERS AND BUILD_MYSQL_DRIVER)
        install(DIRECTORY "drivers/mysql/include/orm"
            TYPE INCLUDE
            FILES_MATCHING PATTERN "*.hpp"
        )
    endif()

    # Install all other files
    # TODO needs more work for the vcpkg port because eg. Qt6 is installing weird docs to doc/Qt6/ silverqx
    if(NOT TINY_VCPKG)
        # The trailing / is important here
        install(DIRECTORY "docs/" DESTINATION "${CMAKE_INSTALL_DOCDIR}/mdx")
        install(FILES AUTHOR LICENSE TYPE DOC)
        install(FILES NOTES.txt TYPE DOC RENAME NOTES)
        install(FILES README.md TYPE DOC RENAME README)

    # Install the TinyORM Project for the vcpkg port (custom vcpkg logic)
    else()
        tiny_install_tinyorm_vcpkg()
    endif()

    # TinyORM's package config needs the FindMySQL package module when the MYSQL_PING
    # is enabled
    set(tiny_cmake_module_path)
    if(MYSQL_PING OR BUILD_MYSQL_DRIVER)
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

    # Generate target includes for the TinyORM package config file
    set(tiny_target_includes)
    tiny_generate_target_includes(tiny_target_includes)

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
    tiny_set_compatible_interface_string(${TinyOrm_target}
        PROPERTIES VERSION_MAJOR SOVERSION
    )
    if(BUILD_DRIVERS)
        tiny_set_compatible_interface_string(${TinyDrivers_target}
            PROPERTIES VERSION_MAJOR SOVERSION
        )
    endif()
    if(TINY_BUILD_LOADABLE_DRIVERS AND BUILD_MYSQL_DRIVER)
        tiny_set_compatible_interface_string(${TinyMySql_target}
            PROPERTIES VERSION_MAJOR SOVERSION
        )
    endif()

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

# Install the TinyORM Project for the vcpkg port (custom vcpkg logic)
function(tiny_install_tinyorm_vcpkg)

    # Install license
    install(FILES LICENSE
        DESTINATION "${CMAKE_INSTALL_DATADIR}/${TINY_PORT}"
        RENAME copyright
    )

    # Configure and install the usage file
    set(tinyOrmPackageVersion
        "${TINY_VERSION_MAJOR}.${TINY_VERSION_MINOR}.${TINY_VERSION_PATCH}"
    )
    configure_file("cmake/vcpkg/usage.in"
        "${TINY_BUILD_INSTALLTREEDIR}/usage"
        @ONLY NEWLINE_STYLE LF
    )

    install(FILES "${PROJECT_BINARY_DIR}/${TINY_BUILD_INSTALLTREEDIR}/usage"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/${TINY_PORT}"
    )

endfunction()

# Create Package Config and Package Config Version files for the Build Tree and export it
function(tiny_export_build_tree)

    # TinyORM's package config needs the FindMySQL package module when the MYSQL_PING
    # is enabled
    set(tiny_cmake_module_path)
    if(MYSQL_PING OR BUILD_MYSQL_DRIVER)
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
    if(BUILD_DRIVERS)
        export(
            EXPORT TinyDriversTargets
            FILE "TinyDriversTargets.cmake"
            NAMESPACE ${TinyDrivers_ns}::
        )
    endif()

    get_property(cvf_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    tiny_to_bool(cvf_is_multi_config ${cvf_is_multi_config})

    # Generate target includes for the TinyORM package config file
    set(tiny_target_includes)
    tiny_generate_target_includes(tiny_target_includes)

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

# Copy TinyDrivers and TinyMySql libraries to the root of the build tree
function(tiny_build_tree_deployment)

    # Nothing to do
    if(TINY_VCPKG OR NOT MSVC OR NOT BUILD_SHARED_LIBS)
        return()
    endif()

    set(filesToDeploy)

    # All generator expressions below will be expanded during the add_custom_target() call
    if(TINY_BUILD_LOADABLE_DRIVERS OR TINY_BUILD_SHARED_DRIVERS)
        list(APPEND filesToDeploy
            $<TARGET_FILE:${TinyDrivers_target}>
            $<$<CONFIG:Debug,RelWithDebInfo>:$<TARGET_PDB_FILE:${TinyDrivers_target}>>
        )
    endif()

    if(TINY_BUILD_LOADABLE_DRIVERS)
        list(APPEND filesToDeploy
            $<TARGET_FILE:${TinyMySql_target}>
            $<$<CONFIG:Debug,RelWithDebInfo>:$<TARGET_PDB_FILE:${TinyMySql_target}>>
        )
    endif()

    list(LENGTH filesToDeploy filesToDeployCount)

    if(filesToDeployCount GREATER 0)
        add_custom_target(TinyBuildTreeDeployTarget
            ALL ${CMAKE_COMMAND} -E copy -t ${CMAKE_BINARY_DIR} ${filesToDeploy}
            VERBATIM
            COMMENT
                "Copying ${TinyDrivers_target} libraries to the root of the build tree..."
        )
    endif()

endfunction()
