# Create Package Config and Package Config Version files and install the TinyORM Project
function(tiny_install_tinyorm)

    include(GNUInstallDirs)

    # Install targets from the project and assign them to the export set
    install(
        TARGETS ${TinyOrm_target} ${CommonConfig_target}
        EXPORT TinyOrmTargets
        LIBRARY ARCHIVE RUNTIME
    )

    # Install all other files
    install(DIRECTORY "include/orm" TYPE INCLUDE FILES_MATCHING PATTERN "*.hpp")
    install(DIRECTORY "${PROJECT_BINARY_DIR}/include/orm" TYPE INCLUDE)
    file(GLOB tiny_docs "docs/*.mdx")
    install(FILES ${tiny_docs} DESTINATION "${CMAKE_INSTALL_DOCDIR}/mdx")
    install(FILES AUTHOR LICENSE TYPE DOC)
    install(FILES NOTES.txt TYPE DOC RENAME NOTES)
    install(FILES README.md TYPE DOC RENAME README)
    if(MSVC)
        install(FILES "$<TARGET_PDB_FILE:${TinyOrm_target}>" TYPE BIN OPTIONAL)
    endif()

    set(tiny_config_package_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${TinyOrm_target}")

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

    # Generate and install a code that can be used to import targets from the Install Tree
    install(
        EXPORT TinyOrmTargets
        NAMESPACE TinyOrm::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/TinyOrm"
    )

    # Install destination directories for the Install Tree
    set(BIN_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}/")
    set(CONFIG_INSTALL_DIR "${tiny_config_package_dir}/")
    set(DOC_INSTALL_DIR "${CMAKE_INSTALL_DOCDIR}/")
    set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/")
    set(LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/")

    # TODO stackoverflow, scope of includes in functions silverqx
    include(CMakePackageConfigHelpers)
    # Configure Package Config file for the Install Tree
    configure_package_config_file(
        "cmake/TinyOrmConfig.cmake.in"
        "cmake/TinyOrmConfig.cmake"
        INSTALL_DESTINATION "${tiny_config_package_dir}"
        PATH_VARS
            BIN_INSTALL_DIR CONFIG_INSTALL_DIR DOC_INSTALL_DIR INCLUDE_INSTALL_DIR
            LIB_INSTALL_DIR
    )

    # Compatible Interface Requirement for the project's major version
    set_property(
        TARGET ${TinyOrm_target}
        PROPERTY INTERFACE_TinyOrm_MAJOR_VERSION ${PROJECT_VERSION_MAJOR}
    )
    set_property(
        TARGET ${TinyOrm_target} APPEND PROPERTY
        COMPATIBLE_INTERFACE_STRING TinyOrm_MAJOR_VERSION
    )

    # Generate the Package Version file for the Package Config file for the Install Tree
    write_basic_package_version_file(
        "cmake/TinyOrmConfigVersion.cmake"
        COMPATIBILITY SameMajorVersion
    )

    # Install Package Config and Package Config Verion files
    install(
        FILES
            "${PROJECT_BINARY_DIR}/cmake/TinyOrmConfig.cmake"
            "${PROJECT_BINARY_DIR}/cmake/TinyOrmConfigVersion.cmake"
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

    # Export Targets from the Build Tree
    export(
        EXPORT TinyOrmTargets
        FILE "TinyOrmTargets.cmake"
        NAMESPACE TinyOrm::
    )

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
        "TinyOrmConfigVersion.cmake"
        COMPATIBILITY SameMajorVersion
    )

    # Store the current Build Tree in the CMake User Package Registry
    export(PACKAGE TinyOrm)

endfunction()
