# Configuration
# ---

CONFIG *= c++2a strict_c++ silent warn_on
CONFIG -= c++11 app_bundle

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

# Compilers
# ---

win32-msvc* {
    # I don't use -MP flag, because using jom
    QMAKE_CXXFLAGS += -guard:cf -permissive- -Zc:ternary
    QMAKE_LFLAGS += /guard:cf
    QMAKE_LFLAGS_RELEASE += /OPT:REF /OPT:ICF=5
}
