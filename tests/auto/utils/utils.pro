QT -= gui

TEMPLATE = lib

# Utils library specific configuration
# ---

CONFIG *= qt link_prl tinyorm_utils

# Common configuration
# ---

include(../../../qmake/common.pri)

# Utils library defines
# ---

# Build as shared library
DEFINES += UTILS_BUILDING_SHARED

# File version and windows manifest
# ---

win32:VERSION = 0.1.0.0
else:VERSION = 0.1.0

win32-msvc* {
    QMAKE_TARGET_PRODUCT = Utils
    QMAKE_TARGET_DESCRIPTION = Utils library for TinyORM tests
    QMAKE_TARGET_COMPANY = Crystal Studio
    QMAKE_TARGET_COPYRIGHT = Copyright (©) 2020 Crystal Studio
#    RC_ICONS = images/utils.ico
    RC_LANG = 1033
}

# User Configuration
# ---

exists(../../conf.pri) {
    include(../../conf.pri)
}
else {
    error( "'tests/conf.pri' for 'tests/auto/utils' library does not exist. See an example configuration in 'tests/conf.pri.example'." )
}

# Use Precompiled headers (PCH)
# ---

PRECOMPILED_HEADER = $$quote($$PWD/src/pch.h)

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

HEADERS += $$quote($$PWD/src/pch.h)

# TinyORM library headers include path
# ---

include(../../../include/include.pri)

# Link against TinyORM library
# ---

win32:CONFIG(release, debug|release) {
    LIBS += -L$$TINYORM_BUILD_TREE/src/release/ -lTinyOrm
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$TINYORM_BUILD_TREE/src/debug/ -lTinyOrm
}
else:unix {
    LIBS += -L$$TINYORM_BUILD_TREE/src/ -lTinyOrm
}

# Utils library header and source files
# ---

include(src/src.pri)

# Default rules for deployment
# ---

release {
    target.CONFIG += no_default_install
}

# Create the SQLite database
# ---

build_tests {
    # Default SQLite test database, can be overriden by DB_SQLITE_DATABASE env. variable
    TINYORM_SQLITE_DATABASE = $$quote($$TINYORM_BUILD_TREE/tests/q_tinyorm_test_1.sqlite3)

    sqlitedatabase.target = sqlitedatabase
    sqlitedatabase.dbname = $$TINYORM_SQLITE_DATABASE
    sqlitedatabase.commands = touch $$sqlitedatabase.dbname
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
