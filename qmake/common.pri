# Unsupported build types
# ---
!build_pass:win32-clang-g++ {
    CONFIG(static, dll|shared|static|staticlib) | \
    CONFIG(staticlib, dll|shared|static|staticlib): \
        error( "MinGW clang static build is not supported, contains a problem with\
                duplicit symbols, you can try to fix it :)." )
}

# Common Configuration ( also for tests )
# ---

CONFIG *= c++2a strict_c++ warn_on utf8_source hide_symbols tiny_system_headers
CONFIG *= silent
CONFIG -= c++11 app_bundle

# Qt defines
# ---

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
# Disables all the APIs deprecated before Qt 6.0.0
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

#DEFINES *= QT_ASCII_CAST_WARNINGS
#DEFINES *= QT_NO_CAST_FROM_ASCII
#DEFINES *= QT_RESTRICTED_CAST_FROM_ASCII
DEFINES *= QT_NO_CAST_TO_ASCII
DEFINES *= QT_NO_CAST_FROM_BYTEARRAY
DEFINES *= QT_USE_QSTRINGBUILDER
DEFINES *= QT_STRICT_ITERATORS

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
CONFIG(shared, dll|shared|static|staticlib | \
CONFIG(dll, dll|shared|static|staticlib)) {
    # Support override because inline_constants can be used in the shared build too
    !inline_constants: \
        CONFIG += extern_constants
}
# Archive library build (static build)
else: \
    CONFIG += inline_constants

# TinyORM defines
# ---

# Release build
CONFIG(release, debug|release): DEFINES += TINYORM_NO_DEBUG
# Debug build
CONFIG(debug, debug|release): DEFINES *= TINYORM_DEBUG

# Enable MySQL ping on Orm::MySqlConnection
mysql_ping: DEFINES *= TINYORM_MYSQL_PING

# Log queries with a time measurement
CONFIG(release, debug|release): DEFINES += TINYORM_NO_DEBUG_SQL
CONFIG(debug, debug|release): DEFINES *= TINYORM_DEBUG_SQL

# Enable code needed by tests, eg. connection overriding in the Model
!disable_orm:build_tests: \
    DEFINES *= TINYORM_TESTS_CODE

# TinyTom related defines
# ---

!disable_tom {
    # Release build
    CONFIG(release, debug|release): DEFINES += TINYTOM_NO_DEBUG
    # Debug build
    CONFIG(debug, debug|release): DEFINES *= TINYTOM_DEBUG

    # Enable code needed by tests (modify the migrate:status command for tests need)
    build_tests: \
        DEFINES *= TINYTOM_TESTS_CODE
}

# Platform specific configuration
# ---
win32: include(winconf.pri)
macx: include(macxconf.pri)
mingw|if(unix:!macx): include(unixconf.pri)

# Common Variables
# ---

# Folder by release type
debug_and_release {
    CONFIG(release, debug|release): \
        TINY_BUILD_SUBFOLDER = $$quote(/release)
    else:CONFIG(debug, debug|release): \
        TINY_BUILD_SUBFOLDER = $$quote(/debug)
}
else: TINY_BUILD_SUBFOLDER =
