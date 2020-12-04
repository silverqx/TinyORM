QT -= gui
QT += sql

# Configuration
# ---
CONFIG += c++2a strict_c++ console silent
CONFIG -= c++11 app_bundle

# Some info output
# ---
CONFIG(debug, debug|release): message( "Project is built in DEBUG mode." )
CONFIG(release, debug|release): message( "Project is built in RELEASE mode." )

# Disable debug output in release mode
CONFIG(release, debug|release) {
    message( "Disabling debug output." )
    DEFINES += QT_NO_DEBUG_OUTPUT
}

# TinyOrm defines
# ---
DEFINES += PROJECT_TINYORM
# Log queries with time measurement
DEFINES += MANGO_DEBUG_SQL

# Qt defines
# ---

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#DEFINES += QT_NO_CAST_FROM_ASCII
#DEFINES += QT_RESTRICTED_CAST_FROM_ASCII
DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_CAST_FROM_BYTEARRAY
DEFINES += QT_USE_QSTRINGBUILDER
DEFINES += QT_STRICT_ITERATORS

# WinApi
# ---
# Windows 10 1903 "19H1" - 0x0A000007
DEFINES += NTDDI_VERSION=0x0A000007
# Windows 10 - 0x0A00
DEFINES += _WIN32_WINNT=0x0A00
DEFINES += _WIN32_IE=0x0A00
DEFINES += UNICODE
DEFINES += _UNICODE
DEFINES += WIN32
DEFINES += _WIN32
DEFINES += WIN32_LEAN_AND_MEAN
DEFINES += NOMINMAX

win32-msvc* {
    # I don't use -MP flag, because using jom
    QMAKE_CXXFLAGS += -guard:cf -permissive- -Zc:ternary
    QMAKE_LFLAGS += /guard:cf
    QMAKE_LFLAGS_RELEASE += /OPT:REF /OPT:ICF=5
}

win32-g++* {
}
else:win32-msvc* {
    # MySQL C library is used by ORM and it uses mysql_ping()
    INCLUDEPATH += $$quote(C:/Program Files/MySQL/MySQL Server 8.0/include)
    # range-v3
    INCLUDEPATH += $$quote(E:/c/qMedia/vcpkg/installed/x64-windows/include)
    # boost
    INCLUDEPATH += $$quote(E:/c_libs/boost/boost_latest)

    LIBS += $$quote(-LC:/Program Files/MySQL/MySQL Server 8.0/lib)

    LIBS += libmysql.lib
}

# File version and windows manifest
# ---
win32:VERSION = 0.1.0.0
else:VERSION = 0.1.0

win32-msvc* {
    QMAKE_TARGET_PRODUCT = TinyOrm
    QMAKE_TARGET_DESCRIPTION = TinyOrm user friendly ORM
    QMAKE_TARGET_COMPANY = Crystal Studio
    QMAKE_TARGET_COPYRIGHT = Copyright (©) 2020 Crystal Studio
#    RC_ICONS = images/qMedia.ico
    RC_LANG = 1033
}

# Use Precompiled headers (PCH)
# ---

INCLUDEPATH += $$PWD

PRECOMPILED_HEADER = pch.h

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

HEADERS += pch.h

# Application source files
# ---

SOURCES += main.cpp

include(src/src.pri)
