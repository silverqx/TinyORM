include_guard(GLOBAL)

include(TinyDrivers)

# Initialize the default CMake variables on which CMake options depend
macro(tiny_init_cmake_variables_pre)

    # Set the CMAKE_EXPORT_PACKAGE_REGISTRY from the TINYORM_EXPORT_PACKAGE_REGISTRY
    # environment variable
    tiny_set_cache_bool_from_environment(CMAKE_EXPORT_PACKAGE_REGISTRY
        "Enables the export(PACKAGE) command, export packages to the user package \
registry."
        TINYORM_EXPORT_PACKAGE_REGISTRY
    )

endmacro()

# Initialize the default CMake variables
macro(tiny_init_cmake_variables)

    # Especially important for multi-config generators (a good practice for single-config
    # generators as well)
    set(CMAKE_DEBUG_POSTFIX d CACHE STRING
        "Default filename postfix for libraries for Debug configuration.")

    # find_package() related
    set(CMAKE_FIND_PACKAGE_SORT_ORDER NATURAL CACHE STRING
        "The default order for sorting packages found using find_package().")
    set(CMAKE_FIND_PACKAGE_SORT_DIRECTION DEC CACHE STRING
        "The sorting direction used by CMAKE_FIND_PACKAGE_SORT_ORDER.")

    # Others
    set(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION ON CACHE BOOL
        "Ask cmake_install.cmake script to warn each time a file with absolute INSTALL \
DESTINATION is encountered.")

    mark_as_advanced(
        CMAKE_DEBUG_POSTFIX
        CMAKE_FIND_PACKAGE_SORT_ORDER
        CMAKE_FIND_PACKAGE_SORT_DIRECTION
        CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION
    )

    # Allow selecting dynamic or static MSVC runtime, also the TINY_VCPKG check isn't
    # strictly needed as on vcpkg the MSVC_RUNTIME_DYNAMIC isn't defined/used but with
    # the TINY_VCPKG is clearly visible whats up.
    # Also see the comment for the MSVC_RUNTIME_DYNAMIC option.
    if(MSVC AND NOT TINY_VCPKG AND NOT DEFINED VCPKG_CRT_LINKAGE AND
            DEFINED MSVC_RUNTIME_DYNAMIC AND
            # Can't simply be: NOT MSVC_RUNTIME_DYNAMIC; as it would change the meaning,
            # and no need to test if ends with -NOTFOUND as this is our internal thing,
            # but I will test it this way because  it covers the edge case and can fail
            # when the value is eg. xyz-NOTFOUND:
            # NOT MSVC_RUNTIME_DYNAMIC STREQUAL "MSVC_RUNTIME_DYNAMIC-NOTFOUND"
            NOT MSVC_RUNTIME_DYNAMIC MATCHES "(-NOTFOUND)$"
    )
        if(MSVC_RUNTIME_DYNAMIC)
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        else()
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        endif()
    endif()

    # TODO test on unix silverqx
#    set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

    # Fix install prefix for the MinGW and x64 toolchain
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND
            CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT
    )
        get_property(help_string CACHE CMAKE_INSTALL_PREFIX PROPERTY HELPSTRING)
        if(NOT help_string)
            set(help_string "Install path prefix, prepended onto install directories.")
        endif()

        if(MINGW)
            set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "${help_string}" FORCE)

        elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(CMAKE_INSTALL_PREFIX "C:/Program Files/${PROJECT_NAME}"
                CACHE PATH "${help_string}" FORCE
            )
        endif()
    endif()

    # Avoid linking a release build types against debug builds
    set(helpStringTemplate
        "Map from <CONFIG> project configuration to an imported target's configuration.")

    string(REPLACE "<CONFIG>" "Release" release_helpString ${helpStringTemplate})
    string(REPLACE "<CONFIG>" "RelWithDebInfo" relWithDebInfo_helpString
        ${helpStringTemplate})
    string(REPLACE "<CONFIG>" "MinSizeRel" minSizeRel_helpString ${helpStringTemplate})
    string(REPLACE "<CONFIG>" "Debug" debug_helpString ${helpStringTemplate})

    set(CMAKE_MAP_IMPORTED_CONFIG_RELEASE Release RelWithDebInfo MinSizeRel ""
        CACHE STRING ${release_helpString})
    set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO RelWithDebInfo Release MinSizeRel ""
        CACHE STRING ${relWithDebInfo_helpString})
    set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL MinSizeRel RelWithDebInfo Release ""
        CACHE STRING ${minSizeRel_helpString})

    # MSVC runtime library crashes if you do not link a debug build against a debug build
    if(MSVC)
        set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG Debug "" CACHE STRING ${debug_helpString})
    else()
        set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG Debug RelWithDebInfo Release MinSizeRel ""
            CACHE STRING ${debug_helpString})
    endif()

    mark_as_advanced(
        CMAKE_MAP_IMPORTED_CONFIG_RELEASE
        CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO
        CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL
        CMAKE_MAP_IMPORTED_CONFIG_DEBUG
    )

    unset(debug_helpString)
    unset(minSizeRel_helpString)
    unset(relWithDebInfo_helpString)
    unset(release_helpString)
    unset(helpStringTemplate)

    if(VERBOSE_CONFIGURE)
        message(STATUS "${TinyOrm_ns}: Set up defaults for \
CMAKE_MAP_IMPORTED_CONFIG_<CONFIG> to avoid linking a release build types against debug \
builds

 * CMAKE_MAP_IMPORTED_CONFIG_RELEASE        = ${CMAKE_MAP_IMPORTED_CONFIG_RELEASE}
 * CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO = ${CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO}
 * CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL     = ${CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL}
 * CMAKE_MAP_IMPORTED_CONFIG_DEBUG          = ${CMAKE_MAP_IMPORTED_CONFIG_DEBUG}
")
    endif()

    # Remove the lib prefix for shared libraries
    if(MINGW)
        set(CMAKE_SHARED_LIBRARY_PREFIX "")
    endif()

    # Used to save and restore original content of the CMAKE_RC_FLAGS variable
    set(TINY_RC_FLAGS_BACKUP "")

    # Add -nologo to the CMAKE_RC_FLAGS if it does not already contain it
    if(MSVC AND NOT CMAKE_RC_FLAGS MATCHES " *[-/]nologo *")
        get_property(help_string CACHE CMAKE_RC_FLAGS PROPERTY HELPSTRING)
        if(NOT help_string)
            set(help_string "Flags for Windows Resource Compiler during all build types.")
        endif()

        set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} -nologo" CACHE STRING ${help_string} FORCE)
    endif()

    unset(help_string)

    # Fix CMake variables if CMAKE_CXX_COMPILER_LAUNCHER is ccache or sccache
    tiny_fix_ccache()

    # To avoid vcpkg warning: D9025 : overriding '/W3' with '/W4'
    # I also though about the following RegEx but it seems to aggresive:
    # " *(\/|-)(W[0-4]|nologo|EHsc|utf-8) *"
    # CMAKE_CXX_FLAGS can also be used somewhere else like in features compile tests or
    # similar things and it could change their behavior.
    if(MSVC AND TINY_VCPKG)
        string(REGEX REPLACE " *(\/|-)W[0-4] *" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()

endmacro()

# Initialize Tiny variables, early initialization
macro(tiny_init_tiny_variables_pre)

    # Top level project name, used for alias namespaces, CMAKE_MESSAGE_CONTEXT, or as
    # the main package name
    set(TinyDrivers_ns TinyDrivers)
    set(TinyMySql_ns TinyMySql)
    set(TinyOrm_ns TinyOrm)
    set(TinyUtils_ns TinyUtils)
    set(TomExample_ns tom)
    set(TomTestData_ns tom_testdata)
    # Target names
    set(CommonConfig_target CommonConfig)
    set(TinyDrivers_target TinyDrivers)
    set(TinyMySql_target TinyMySql)
    set(TinyOrm_target TinyOrm)
    set(TinyUtils_target TinyUtils)
    set(TomExample_target tom)
    set(TomTestData_target tom_testdata)
    # User should see Tom_target in the tom.rc.in not TomExample_target that is
    # the reason of this mapping, also applies in the version/CMakeLists.txt
    set(Tom_target ${TomExample_target})
    # Targets' folders
    set(TomExample_folder examples/tom)
    # Tom migrations folder for the make:migration command
    set(TomMigrations_folder database/migrations)
    # Tom models folder for the make:model command
    set(TomModels_folder database/models)
    # Tom seeders folder for the make:seeder command
    set(TomSeeders_folder database/seeders)

    # Used in STREQUAL comparisons
    string(TOLOWER "${CMAKE_BUILD_TYPE}" TINY_BUILD_TYPE_LOWER)
    string(TOUPPER "${CMAKE_BUILD_TYPE}" TINY_BUILD_TYPE_UPPER)

    get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    set(TINY_IS_MULTI_CONFIG "${isMultiConfig}" CACHE INTERNAL
        "True when using a multi-configuration generator.")
    unset(isMultiConfig)

    # Provide the default value if not set
    if(NOT TINY_VCPKG)
        set(TINY_VCPKG FALSE)
        set(TINY_PORT TINY_PORT-NOTFOUND)
    endif()

    # Auto-detect the CMAKE_TOOLCHAIN_FILE from the VCPKG_ROOT environment variable
    if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Path to the toolchain file supplied to CMake.")
    endif()
    # GitHub Actions defines the VCPKG_INSTALLATION_ROOT instead of VCPKG_ROOT
    if(DEFINED ENV{VCPKG_INSTALLATION_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE
            "$ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Path to the toolchain file supplied to CMake.")
    endif()

    # Vcpkg CMake integration ignores VCPKG_DEFAULT_TRIPLET env. variable but accepts
    # the VCPKG_TARGET_TRIPLET command-line option
    if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
        set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING
            "Change the default triplet for CMake Integration.")
    endif()

    # Initialize TinyDrivers CMake internal cache variables, early initialization
    tiny_init_driver_types_pre()

endmacro()

# Initialize Tiny variables
macro(tiny_init_tiny_variables)

    # List of package dependencies for the package config
    # Can't be a simple variable because add_subdirectory() creates a new scope and
    # creates a copy of all variables, so changes will be discarded when scope ends.
    define_property(GLOBAL PROPERTY TINY_PACKAGE_DEPENDENCIES
        BRIEF_DOCS "Recorded arguments from find_package() calls."
        FULL_DOCS "Recorded arguments from find_package() calls that will be used \
to generate find_dependency() calls for the TinyORM package configuration file."
    )

    # Qt Required components for find_package() command
    set(TinyQtComponentsRequired Core)
    if(NOT BUILD_DRIVERS)
        list(APPEND TinyQtComponentsRequired Sql)
    endif()

    # Specifies which TinyDrivers build type is currently being built (for nicer if()-s)
    tiny_init_driver_types()

    # Setup the correct PATH environment variable for the ctest command
    set(TINY_TESTS_ENV_PATH TINY_TESTS_ENV_PATH-NOTFOUND) # Always use if() before using the variable
    tiny_init_ctest_path_win32()

    # Build Tree folders for generated content
    # For better naming and no need to be INTERNAL
    set(TINY_BUILD_GENERATED_DIR   ".${TinyOrm_ns}")
    set(TINY_BUILD_BUILDTREE_DIR   "${TINY_BUILD_GENERATED_DIR}/buildtree")
    set(TINY_BUILD_INSTALLTREE_DIR "${TINY_BUILD_GENERATED_DIR}/installtree")
    set(TINY_BUILD_INCLUDE_DIR     "${TINY_BUILD_GENERATED_DIR}/include")
    set(TINY_BUILD_TMP_DIR         "${TINY_BUILD_GENERATED_DIR}/tmp")

    # Provide the default value if not set
    if(NOT TINY_VCPKG)
        set(TINY_VCPKG FALSE)
        set(TINY_PORT TINY_PORT-NOTFOUND)
    endif()

    # Specifies which global constant types will be used
    if(BUILD_SHARED_LIBS AND NOT INLINE_CONSTANTS)
        set(tinyExternConstants ON)
        message(VERBOSE "Using extern constants")
    else()
        set(tinyExternConstants OFF)
        message(VERBOSE "Using inline constants")
    endif()
    set(TINY_EXTERN_CONSTANTS ${tinyExternConstants} CACHE INTERNAL
        "Determine whether ${TinyOrm_target} library will be built with extern or inline \
constants.")
    unset(tinyExternConstants)

    # To evaluate only once (used in tiny_common())
    if(NOT TINY_VCPKG AND (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24" AND
                              NOT DEFINED CMAKE_COMPILE_WARNING_AS_ERROR)
    )
        set(TinyCompileWarningAsError TRUE)
    else()
        set(TinyCompileWarningAsError FALSE)
    endif()

endmacro()

# Initialize the default database paths for the make:migration/model/seeder commands
macro(tiny_init_tom_database_dirs)

    if(TOM_EXAMPLE)
        # Provide the default migrations path for the make:migration command
        if(NOT DEFINED TOM_MIGRATIONS_DIR)
            # Relative path to the pwd
            set(TOM_MIGRATIONS_DIR ${TomMigrations_folder})
        endif()

        # Provide the default models path for the make:models command
        if(NOT DEFINED TOM_MODELS_DIR)
            # Relative path to the pwd
            set(TOM_MODELS_DIR ${TomSeeders_folder})
        endif()

        # Provide the default seeders path for the make:seeders command
        if(NOT DEFINED TOM_SEEDERS_DIR)
            # Relative path to the pwd
            set(TOM_SEEDERS_DIR ${TomSeeders_folder})
        endif()

        # Set path from the -D options or from the above default values
        set(TOM_MIGRATIONS_DIR "${TOM_MIGRATIONS_DIR}" CACHE PATH
            "Default migrations path for the make:migration command.")

        set(TOM_MODELS_DIR "${TOM_MODELS_DIR}" CACHE PATH
            "Default models path for the make:model command.")

        set(TOM_SEEDERS_DIR "${TOM_SEEDERS_DIR}" CACHE PATH
            "Default seeders path for the make:seeder command.")

        mark_as_advanced(TOM_MIGRATIONS_DIR TOM_MODELS_DIR TOM_SEEDERS_DIR)
    endif()

endmacro()

# Setup the correct PATH environment variable for the ctest command
# To debug these paths on the PATH environment variable run ctest --debug (outside
# of QtCreator because it handles invoking of unit tests without the ctest command).
function(tiny_init_ctest_path_win32)

    # Nothing to do
    if(NOT WIN32 OR NOT BUILD_TESTS)
        return()
    endif()

    set(tinyEnvPath "")

    # Prepend TinyOrm, TinyUtils, TinyDrivers, TinyMySql library folders
    # Handles both Single/Multi-config generators (thanks to $<TARGET_FILE_DIR>)
    # The order of these paths is correct (revisited)
    # Testing for target eg. if(TARGET ${${TinyMySql_target}) would be redundant here
    if(TINY_BUILD_LOADABLE_DRIVERS AND BUILD_MYSQL_DRIVER)
        list(PREPEND tinyEnvPath
            "$<SHELL_PATH:$<TARGET_FILE_DIR:${TinyMySql_target}>>"
        )
    endif()

    if(BUILD_DRIVERS)
        list(PREPEND tinyEnvPath
            "$<SHELL_PATH:$<TARGET_FILE_DIR:${TinyDrivers_target}>>"
        )
    endif()

    list(PREPEND tinyEnvPath
        "$<SHELL_PATH:$<TARGET_FILE_DIR:${TinyOrm_target}>>"
        "$<SHELL_PATH:$<TARGET_FILE_DIR:${TinyUtils_target}>>"
    )

    # Escaping is needed for the ENVIRONMENT_MODIFICATION path_list_prepend
    list(JOIN tinyEnvPath "\;" tinyEnvPath)

    set(TINY_TESTS_ENV_PATH "${tinyEnvPath}" PARENT_SCOPE) # Quotes for tinyEnvPath are needed because of escaping

endfunction()
