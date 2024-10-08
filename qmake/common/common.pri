# Load .env and .env.$$QMAKE_PLATFORM files
# ---

load(private/tiny_dotenv)

# Common Configuration ( also for tests )
# ---

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
# Disable all the APIs deprecated up to Qt v6.9.0 (including)
DEFINES *= QT_DISABLE_DEPRECATED_UP_TO=0x060900

#DEFINES *= QT_NO_DEPRECATED_WARNINGS
#DEFINES *= QT_WARN_DEPRECATED_UP_TO=0x060900

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

# Look at NOTES.txt[inline constants] how this funckin machinery works 😎
!extern_constants: \
!inline_constants {
    # Use extern constants by default for shared builds
    CONFIG(shared, dll|shared|static|staticlib) | \
    CONFIG(dll, dll|shared|static|staticlib): \
        CONFIG += extern_constants

    # Archive library build (static linkage)
    else: \
        CONFIG += inline_constants
}

# Platform specific configuration
# ---
win32: include($$PWD/winconf.pri)
macx:  include($$PWD/macxconf.pri)
mingw|if(unix:!macx): include($$PWD/unixconf.pri)

!if(win32|macx|if(mingw|if(unix:!macx))): \
    message( "Unsupported platform ($${QMAKE_PLATFORM}-$${QMAKE_COMPILER})." )

# Enable colors and ANSI escape codes in diagnostics (works also with Clang-cl)
llvm: QMAKE_CXXFLAGS *= -fansi-escape-codes -fcolor-diagnostics
