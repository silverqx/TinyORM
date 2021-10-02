QT -= gui

TEMPLATE = lib
TARGET = TinyUtils

# Common configuration
# ---

include(../../qmake/common.pri)

# TinyUtils library specific configuration
# ---

CONFIG *= qt link_prl create_prl create_pc create_libtool

# TinyUtils library defines
# ---

DEFINES += PROJECT_TINYUTILS

# Build as the shared library
CONFIG(shared, dll|shared|static|staticlib) | CONFIG(dll, dll|shared|static|staticlib) {
    DEFINES += UTILS_BUILDING_SHARED
}

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
RC_INCLUDEPATH = $$quote($$TINYORM_SOURCE_TREE/tests/utils/src/)
# Find Windows manifest
mingw: RC_INCLUDEPATH += $$quote($$TINYORM_SOURCE_TREE/tests/utils/resources/)

load(tiny_resource_and_manifest)
tiny_resource_and_manifest()

# User Configuration
# ---

exists(../conf.pri) {
    include(../conf.pri)
}
else {
    error( "'tests/conf.pri' for 'tests/utils' library does not exist. See an example\
            configuration in 'tests/conf.pri.example'." )
}

# Use Precompiled headers (PCH)
# ---

precompile_header {
    include($$PWD/src/pch.pri)
}

# TinyORM library headers include path
# ---

include(../../include/include.pri)

# Link against TinyORM library
# ---

win32:CONFIG(release, debug|release) {
    LIBS += $$quote(-L$$TINYORM_BUILD_TREE/src/release/) -lTinyOrm
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += $$quote(-L$$TINYORM_BUILD_TREE/src/debug/) -lTinyOrm
}
else:unix {
    LIBS += $$quote(-L$$TINYORM_BUILD_TREE/src/) -lTinyOrm
}

# Default rules for deployment
# ---

CONFIG(release, debug|release) {
    target.CONFIG += no_default_install
}

# Create the SQLite database
# ---

build_tests {
    # Default SQLite test database, can be overriden by DB_SQLITE_DATABASE env. variable
    TINYORM_SQLITE_DATABASE = $$quote($$TINYORM_BUILD_TREE/tests/q_tinyorm_test_1.sqlite3)

    sqlitedatabase.target = sqlitedatabase
    sqlitedatabase.dbname = $$TINYORM_SQLITE_DATABASE
    win32: sqlitedatabase.commands = type nul >> $$sqlitedatabase.dbname
    unix: sqlitedatabase.commands = touch $$sqlitedatabase.dbname
    sqlitedatabase.depends = sqlitedatabase_message

    sqlitedatabase_message.commands = @echo Creating SQLite database at $$sqlitedatabase.dbname

    QMAKE_EXTRA_TARGETS += sqlitedatabase sqlitedatabase_message

    !exists($$TINYORM_SQLITE_DATABASE) {
        POST_TARGETDEPS += sqlitedatabase
    }

    # Set path to the SQLite database
    # ---
    contains(TEMPLATE, vc.*): DEFINES += TINYORM_SQLITE_DATABASE=\"$$TINYORM_SQLITE_DATABASE\"
    else: DEFINES += TINYORM_SQLITE_DATABASE=$$shell_quote(\"$$TINYORM_SQLITE_DATABASE\")
}

# Clean the SQLite database
# ---

build_tests {
    QMAKE_CLEAN = $$TINYORM_SQLITE_DATABASE
    QMAKE_DISTCLEAN = $$TINYORM_SQLITE_DATABASE
}
