QT *= core
QT -= gui

TEMPLATE = lib
TARGET = TinyDrivers

# Common for static/shared libraries
# ---

include($$TINYORM_SOURCE_TREE/qmake/common/libraries.pri)

# Qt defines
# ---

DEFINES *= QT_ASCII_CAST_WARNINGS
DEFINES *= QT_NO_CAST_FROM_ASCII

# Variables to target the correct build folder
# ---

include($$TINYORM_SOURCE_TREE/qmake/support/variables.pri)

# TinyDrivers configuration
# ---
# It must be before the common/common.pri because of constants resolving

build_static_drivers: \
    CONFIG *= staticlib

# Common Configuration
# ---

include($$TINYORM_SOURCE_TREE/qmake/common/common.pri)

# TinyDrivers defines
# ---

DEFINES *= PROJECT_TINYDRIVERS
# Private defines
DEFINES *= TINY_QMAKE_BUILD_PRIVATE

# Build as the shared library
CONFIG(shared, dll|shared|static|staticlib) | \
CONFIG(dll, dll|shared|static|staticlib): \
    DEFINES *= TINYDRIVERS_BUILDING_SHARED

# Release build
CONFIG(release, debug|release): DEFINES += TINYDRIVERS_NO_DEBUG
# Debug build
CONFIG(debug, debug|release): DEFINES *= TINYDRIVERS_DEBUG

load(private/tiny_drivers)
# Cache the result to the isBuildingMySqlDriver variable
tiny_is_building_driver(mysql, isBuildingMySqlDriver)

$$isBuildingMySqlDriver: \
    DEFINES *= TINYDRIVERS_MYSQL_DRIVER

build_loadable_drivers: \
$$isBuildingMySqlDriver {
    DEFINES *= TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
    DEFINES *= TINYDRIVERS_MYSQL_PATH=$$quote($$shell_quote(\
               $$clean_path($$TINYORM_BUILD_TREE)/drivers/mysql$${TINY_BUILD_SUBFOLDER}/))
}

# Enable code needed by tests (not used)
build_tests: \
    DEFINES *= TINYDRIVERS_TESTS_CODE

# TinyDrivers library header and source files
# ---

# tiny_version_numbers() depends on HEADERS (version.hpp)
include($$PWD/include_private/include_private.pri)
include($$PWD/include/include.pri)
include($$PWD/src/src.pri)

!build_loadable_drivers: \
$$isBuildingMySqlDriver {
    include($$TINYORM_SOURCE_TREE/drivers/mysql/include_private/include_private.pri)
    include($$TINYORM_SOURCE_TREE/drivers/mysql/include/include.pri)
    include($$TINYORM_SOURCE_TREE/drivers/mysql/src/src.pri)
}

# File version
# ---

# Find version numbers in the version header file and assign them to the
# <TARGET>_VERSION_<MAJOR,MINOR,PATCH,TWEAK> and also to the VERSION variable.
load(private/tiny_version_numbers)
tiny_version_numbers()

# Windows resource and manifest files
# ---

# To find stringify.hpp and version.hpp
tinyRcIncludepath = $$quote($$TINYORM_SOURCE_TREE/include/) \
                    $$quote($$TINYORM_SOURCE_TREE/drivers/common/include/)
# To find Windows manifest
mingw: tinyRcIncludepath += $$quote($$TINYORM_SOURCE_TREE/drivers/common/resources/)

load(private/tiny_resource_and_manifest)
tiny_resource_and_manifest(                                                             \
    $$tinyRcIncludepath, $$quote($$TINYORM_SOURCE_TREE/drivers/common/resources)        \
)

# Use Precompiled headers (PCH)
# ---

include($$PWD/include/pch.pri)

# Some info output
# ---

!build_pass: \
    message( "Build TinyDrivers library." )

# TinyDrivers library dependencies
# ---
# To include commonnamespace.hpp, export_common.hpp, likely.hpp, and systemheader.hpp
# Helps to avoid dependency on the TinyCommon (no need because of 4 simple files)

load(private/tiny_system_includepath)
tiny_add_system_includepath($$quote($$TINYORM_SOURCE_TREE/include/))

# This path is already added on the INCLUDEPATH in the mysql/include/include.pri file
# for shared and static builds
build_loadable_drivers: \
$$isBuildingMySqlDriver: \
    tiny_add_system_includepath($$quote($$TINYORM_SOURCE_TREE/drivers/mysql/include/))

# Auto-configuration
# ---

!build_loadable_drivers: \
!disable_autoconf {
    load(private/tiny_find_packages)

    # Find the MySQL and add it on the system include path and library path
    $$isBuildingMySqlDriver: \
        tiny_find_mysql()
}

# User Configuration
# ---

exists($$TINYORM_SOURCE_TREE/drivers/conf.pri): \
    include($$TINYORM_SOURCE_TREE/drivers/conf.pri)

else:disable_autoconf: \
    error( "'conf.pri' for '$${TARGET}' project does not exist.\
            See an example configuration in 'drivers/conf.pri.example'." )

# Create the .build_tree tag file
# ---
# Create an empty .build_tree file in the folder where the TinyDrivers shared library is
# located, in the build tree. This file will be checked in the SqlDriverFactoryPrivate
# while loading the runtime shared library, eg. TinyMySql. If SqlDriverFactoryPrivate
# finds this file and cannot load TinyMySql from standard locations then it will try
# to load TinyMySql from the build tree. This means that it only attempts to load
# TinyMySql from the build tree if the TinyDrivers library itself is in the build tree.
# This ensures that it will not be loaded from the build tree, e.g. after installation
# because it is non-standard behavior.

build_loadable_drivers {
    TINYDRIVERS_BUILDTREE_TAG_PATH = $$quote($$TINYORM_BUILD_TREE/drivers/common$${TINY_BUILD_SUBFOLDER}/.build_tree)

    buildtreetagfile.target = buildtreetagfile
    buildtreetagfile.tagFilepath = $$shell_quote($$TINYDRIVERS_BUILDTREE_TAG_PATH)
    win32: buildtreetagfile.commands = type nul > $$buildtreetagfile.tagFilepath
    unix: buildtreetagfile.commands = touch $$buildtreetagfile.tagFilepath
    buildtreetagfile.depends = buildtreetagfile_message

    buildtreetagfile_message.commands = @echo Creating .build_tree tag file at $$buildtreetagfile.tagFilepath

    QMAKE_EXTRA_TARGETS *= buildtreetagfile buildtreetagfile_message

    !exists($$TINYDRIVERS_BUILDTREE_TAG_PATH): \
        POST_TARGETDEPS *= buildtreetagfile
}

# Clean the .build_tree tag file
# ---
# Don't add it to the QMAKE_CLEAN as the QMAKE_CLEAN doesn't delete executables

build_loadable_drivers: \
    QMAKE_DISTCLEAN *= $$TINYDRIVERS_BUILDTREE_TAG_PATH
