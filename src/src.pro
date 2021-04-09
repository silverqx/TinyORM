QT *= core sql
QT -= gui

TEMPLATE = lib
TARGET = TinyOrm

# Configuration
# ---

include(../config.pri)

# TinyORM library specific configuration
# ---

CONFIG *= create_prl object_parallel_to_source
#CONFIG *= create_libtool create_pc

# Some info output
# ---

CONFIG(debug, debug|release): message( "Project is built in DEBUG mode." )
CONFIG(release, debug|release): message( "Project is built in RELEASE mode." )

# Disable debug output in release mode
CONFIG(release, debug|release) {
    message( "Disabling debug output." )
    DEFINES += QT_NO_DEBUG_OUTPUT
}

# TinyORM defines
# ---

DEFINES += PROJECT_TINYORM
# Log queries with time measurement
DEFINES += TINYORM_DEBUG_SQL

# Build as shared library
DEFINES += TINYORM_BUILDING_SHARED

# Enable code needed by tests, eg connection overriding in the BaseModel
build_tests {
    DEFINES += TINYORM_TESTS_CODE
}

# Dependencies include and library paths
# ---

win32-g++* {
}
else:win32-msvc* {
    # MySQL C library is used by ORM and it uses mysql_ping()
    INCLUDEPATH += $$quote(C:/Program Files/MySQL/MySQL Server 8.0/include)
    # range-v3
    INCLUDEPATH += $$quote(E:/c/qMedia/vcpkg/installed/x64-windows/include)
    # boost
#    INCLUDEPATH += $$quote(E:/c_libs/boost/boost_latest)

    LIBS += $$quote(-LC:/Program Files/MySQL/MySQL Server 8.0/lib)

    LIBS += -llibmysql
}

# File version and windows manifest
# ---

win32:VERSION = 0.1.0.0
else:VERSION = 0.1.0

win32-msvc* {
    QMAKE_TARGET_PRODUCT = TinyORM
    QMAKE_TARGET_DESCRIPTION = TinyORM user-friendly ORM
    QMAKE_TARGET_COMPANY = Crystal Studio
    QMAKE_TARGET_COPYRIGHT = Copyright (©) 2020 Crystal Studio
#    RC_ICONS = images/TinyOrm.ico
    RC_LANG = 1033
}

# Use Precompiled headers (PCH)
# ---

# TODO add possibility to control PCH by qmake CONFIG variable silverqx
include(../include/pch.pri)

# TinyORM library header and source files
# ---

include(src.pri)

# Default rules for deployment
# ---

release {
    win32-msvc*: target.path = C:/optx64/$${TARGET}
    !isEmpty(target.path): INSTALLS += target
}
