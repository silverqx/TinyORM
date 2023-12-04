# Unsupported build types
# ---
!build_pass:win32-clang-g++ {
    CONFIG(static, dll|shared|static|staticlib) | \
    CONFIG(staticlib, dll|shared|static|staticlib): \
        error( "MinGW clang static build is not supported, contains a problem with\
                duplicate symbols, you can try to fix it :)." )
}

# Load .env and .env.$$QMAKE_PLATFORM files
# ---

load(tiny_dotenv)

# Common Configuration ( also for tests )
# ---

CONFIG *= qt c++2a strict_c++ warn_on utf8_source hide_symbols no_keywords
CONFIG *= tiny_system_headers
CONFIG *= silent
CONFIG -= c++11 app_bundle

# Qt defines
# ---

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
# Disables all the APIs deprecated before Qt 6.0.0
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

# Use extern constants for shared build
# clang-cl notes:
# shared build crashes with extern constants, force to inline constants 😕🤔
# only one option with the clang-cl is inline constants for both shared/static builds
# Look at NOTES.txt[inline constants] how this funckin machinery works 😎
win32-clang-msvc: \
    CONFIG += inline_constants

else: \
CONFIG(shared, dll|shared|static|staticlib) | \
CONFIG(dll, dll|shared|static|staticlib) {
    # Support override because inline_constants can be used in the shared build too
    !inline_constants: \
        CONFIG *= extern_constants
}
# Archive library build (static build)
else: \
    CONFIG += inline_constants

# Platform specific configuration
# ---
win32: include($$PWD/winconf.pri)
macx:  include($$PWD/macxconf.pri)
mingw|if(unix:!macx): include($$PWD/unixconf.pri)

!if(win32|macx|if(mingw|if(unix:!macx))): \
    message( "Unsupported platform ($${QMAKE_PLATFORM}-$${QMAKE_COMPILER})." )
