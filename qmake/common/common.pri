# Load .env and .env.$$QMAKE_PLATFORM files
# ---

load(tiny_dotenv)

# Common Configuration ( also for tests )
# ---

CONFIG *= qt c++20 strict_c++ warn_on utf8_source hide_symbols no_keywords no_include_pwd
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
