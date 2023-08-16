QT -= gui

TEMPLATE = lib
TARGET = TinyUtils

# TinyUtils library specific configuration
# ---

CONFIG *= qt link_prl create_prl create_pc create_libtool

# Common configuration
# ---

include(../../qmake/common.pri)

# TinyUtils library defines
# ---

DEFINES += PROJECT_TINYUTILS

# Build as the shared library
CONFIG(shared, dll|shared|static|staticlib) | \
CONFIG(dll, dll|shared|static|staticlib): \
    DEFINES += TINYUTILS_BUILDING_SHARED

# Disable debug output in release mode
CONFIG(release, debug|release): \
    DEFINES *= QT_NO_DEBUG_OUTPUT

# TinyUtils library header and source files
# ---

# tiny_version_numbers() depends on HEADERS (version.hpp)
include(src/src.pri)

# File version
# ---

# Find version numbers in a version header file and assign them to the
# <TARGET>_VERSION_<MAJOR,MINOR,PATCH,TWEAK> and also to the VERSION variable
load(tiny_version_numbers)
tiny_version_numbers()

# Windows resource and manifest files
# ---

# Find version.hpp
tinyRcIncludepath = $$quote($$TINYORM_SOURCE_TREE/tests/TinyUtils/src/)
# Find Windows manifest
mingw: tinyRcIncludepath += $$quote($$TINYORM_SOURCE_TREE/tests/TinyUtils/resources/)

load(tiny_resource_and_manifest)
tiny_resource_and_manifest($$tinyRcIncludepath)

unset(tinyRcIncludepath)

# Use Precompiled headers (PCH)
# ---

include($$PWD/src/pch.pri)

# Link against TinyORM library (also adds defines and include headers)
# ---

include($$TINYORM_SOURCE_TREE/tests/qmake/TinyOrm.pri)

# Default rules for deployment
# ---

target.CONFIG += no_default_install

# Create the SQLite database
# ---

build_tests {
    # Default SQLite test database, can be overriden by DB_SQLITE_DATABASE env. variable
    TINYORM_SQLITE_DATABASE = $$quote($$TINYORM_BUILD_TREE/tests/tinyorm_test_1.sqlite3)

    sqlitedatabase.target = sqlitedatabase
    sqlitedatabase.dbname = $$TINYORM_SQLITE_DATABASE
    win32: sqlitedatabase.commands = type nul >> $$sqlitedatabase.dbname
    unix: sqlitedatabase.commands = touch $$sqlitedatabase.dbname
    sqlitedatabase.depends = sqlitedatabase_message

    sqlitedatabase_message.commands = @echo Creating SQLite database at $$sqlitedatabase.dbname

    QMAKE_EXTRA_TARGETS += sqlitedatabase sqlitedatabase_message

    !exists($$TINYORM_SQLITE_DATABASE): \
        POST_TARGETDEPS += sqlitedatabase

    # Set path to the SQLite database
    contains(TEMPLATE, vc.*): \
        DEFINES += TINYORM_SQLITE_DATABASE=\"$$TINYORM_SQLITE_DATABASE\"
    else: \
        DEFINES += TINYORM_SQLITE_DATABASE=$$shell_quote(\"$$TINYORM_SQLITE_DATABASE\")
}

# Clean the SQLite database
# ---

build_tests {
    QMAKE_CLEAN += $$TINYORM_SQLITE_DATABASE
    QMAKE_DISTCLEAN += $$TINYORM_SQLITE_DATABASE
}

# CONFIG tiny_autoconf
# ---

tiny_autoconf {
    load(tiny_find_packages)

    # Find the vcpkg and add the vcpkg/<triplet>/include/ on the system include path
    tiny_find_vcpkg()
}

# User Configuration
# ---

exists(../conf.pri): \
    include(../conf.pri)

else:!tiny_autoconf: \
    error( "'tests/conf.pri' for 'tests/TinyUtils' library does not exist. See an example\
            configuration in 'tests/conf.pri.example'." )
