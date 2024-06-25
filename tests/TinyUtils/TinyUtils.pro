QT -= gui

TEMPLATE = lib
TARGET = TinyUtils

# Common for static/shared libraries
# ---

include($$TINYORM_SOURCE_TREE/qmake/common/libraries.pri)

# Link against TinyORM library (also adds defines and include headers)
# ---

include($$TINYORM_SOURCE_TREE/qmake/TinyOrm.pri)

# TinyUtils library defines
# ---

DEFINES *= PROJECT_TINYUTILS

# Build as the shared library
CONFIG(shared, dll|shared|static|staticlib) | \
CONFIG(dll, dll|shared|static|staticlib): \
    DEFINES *= TINYUTILS_BUILDING_SHARED

# TinyUtils library header and source files
# ---

# tiny_version_numbers() depends on HEADERS (version.hpp)
include($$PWD/src/src.pri)

# File version
# ---

# Find version numbers in a version header file and assign them to the
# <TARGET>_VERSION_<MAJOR,MINOR,PATCH,TWEAK> and also to the VERSION variable
load(private/tiny_version_numbers)
tiny_version_numbers()

# Windows resource and manifest files
# ---

# To find stringify.hpp and version.hpp
tinyRcIncludepath = $$quote($$TINYORM_SOURCE_TREE/include/) \
                    $$quote($$TINYORM_SOURCE_TREE/tests/TinyUtils/src/)
# Find Windows manifest
mingw: tinyRcIncludepath += $$quote($$TINYORM_SOURCE_TREE/tests/TinyUtils/resources/)

load(private/tiny_resource_and_manifest)
tiny_resource_and_manifest($$tinyRcIncludepath)

unset(tinyRcIncludepath)

# Default rules for deployment
# ---

target.CONFIG += no_default_install

# Create the SQLite database
# ---

build_tests {
    # Default SQLite test database, can be overridden by DB_SQLITE_DATABASE env. variable
    TINYORM_SQLITE_DATABASE = $$quote($$TINYORM_BUILD_TREE/tests/tinyorm_test_1.sqlite3)

    sqlitedatabase.target = sqlitedatabase
    sqlitedatabase.dbname = $$shell_quote($$TINYORM_SQLITE_DATABASE)
    win32: sqlitedatabase.commands = type nul > $$sqlitedatabase.dbname
    unix: sqlitedatabase.commands = touch $$sqlitedatabase.dbname
    sqlitedatabase.depends = sqlitedatabase_message

    sqlitedatabase_message.commands = @echo Creating SQLite database at $$sqlitedatabase.dbname

    QMAKE_EXTRA_TARGETS *= sqlitedatabase sqlitedatabase_message

    !exists($$TINYORM_SQLITE_DATABASE): \
        POST_TARGETDEPS *= sqlitedatabase

    # Set path to the SQLite database
    contains(TEMPLATE, vc.*): \
        DEFINES += TINYORM_SQLITE_DATABASE=\"$$TINYORM_SQLITE_DATABASE\"
    else: \
        DEFINES += TINYORM_SQLITE_DATABASE=$$shell_quote(\"$$TINYORM_SQLITE_DATABASE\")
}

# Clean the SQLite database
# ---
# Don't add it to the QMAKE_CLEAN as the QMAKE_CLEAN doesn't delete executables

build_tests: \
    QMAKE_DISTCLEAN *= $$TINYORM_SQLITE_DATABASE

# User Configuration
# ---

exists($$TINYORM_SOURCE_TREE/tests/conf.pri): \
    include($$TINYORM_SOURCE_TREE/tests/conf.pri)

else:disable_autoconf: \
    error( "'tests/conf.pri' for 'tests/$${TARGET}' library does not exist.\
             See an example configuration in 'tests/conf.pri.example'." )
