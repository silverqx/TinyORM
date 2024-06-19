# Load .env and .env.$$QMAKE_PLATFORM files
# ---

load(tiny_dotenv)

# Common Configuration ( also for tests )
# ---

# TODO qt5 remove silverqx
versionAtLeast(QT_VERSION, 6.2.4): \
    CONFIG *= c++20
else: \
    CONFIG *= c++2a

CONFIG *= qt strict_c++ warn_on utf8_source hide_symbols no_keywords no_include_pwd
CONFIG *= tiny_system_headers
CONFIG *= silent
CONFIG -= c++11 app_bundle

# Qt defines
# ---

# Disable deprecated APIs up to the given Qt version
# Disable all the APIs deprecated up to Qt 6.7.0
# TODO qt5 remove silverqx
versionAtLeast(QT_VERSION, 6): \
    DEFINES *= QT_DISABLE_DEPRECATED_UP_TO=0x060800
# Disable all the APIs deprecated up to Qt 6.0.0
else: \
    DEFINES *= QT_DISABLE_DEPRECATED_BEFORE=0x060000

#DEFINES *= QT_ASCII_CAST_WARNINGS
#DEFINES *= QT_NO_CAST_FROM_ASCII
#DEFINES *= QT_RESTRICTED_CAST_FROM_ASCII
DEFINES *= QT_NO_CAST_TO_ASCII
DEFINES *= QT_NO_CAST_FROM_BYTEARRAY
# Don't define globally, it's better for compatibility, instead use % when needed
#DEFINES *= QT_USE_QSTRINGBUILDER
DEFINES *= QT_STRICT_ITERATORS

# Disable debug output in release mode
CONFIG(release, debug|release): \
    DEFINES *= QT_NO_DEBUG_OUTPUT

# TinyORM configuration
# ---

# Use extern constants by default
# Look at NOTES.txt[inline constants] how this funckin machinery works 😎
!extern_constants: \
!inline_constants: \
    CONFIG += extern_constants

# Platform specific configuration
# ---
win32: include($$PWD/winconf.pri)
macx:  include($$PWD/macxconf.pri)
mingw|if(unix:!macx): include($$PWD/unixconf.pri)

!if(win32|macx|if(mingw|if(unix:!macx))): \
    message( "Unsupported platform ($${QMAKE_PLATFORM}-$${QMAKE_COMPILER})." )

# Enable colors and ANSI escape codes in diagnostics (works also with clang-cl)
llvm: QMAKE_CXXFLAGS *= -fansi-escape-codes -fcolor-diagnostics
