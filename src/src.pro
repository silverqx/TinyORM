QT *= core sql
QT -= gui

TEMPLATE = lib
TARGET = TinyOrm

# TinyORM library specific configuration
# ---

CONFIG *= create_prl create_pc create_libtool

# Common Configuration
# ---

include(../qmake/common.pri)

# TinyORM defines
# ---

DEFINES += PROJECT_TINYORM
# Log queries with a time measurement
DEFINES += TINYORM_DEBUG_SQL

# Build as the shared library
CONFIG(shared, dll|shared|static|staticlib) | \
CONFIG(dll, dll|shared|static|staticlib): \
    DEFINES += TINYORM_BUILDING_SHARED

# Enable code needed by tests, eg. connection overriding in the Model
build_tests: \
    DEFINES += TINYORM_TESTS_CODE

# TinyORM library header and source files
# ---

# tiny_version_numbers() depends on HEADERS (version.hpp)
include(../include/include.pri)
include(src.pri)

# File version
# ---

# Find version numbers in the version header file and assign them to the
# <TARGET>_VERSION_<MAJOR,MINOR,PATCH,TWEAK> and also to the VERSION variable.
load(tiny_version_numbers)
tiny_version_numbers()

# Windows resource and manifest files
# ---

load(tiny_resource_and_manifest)
tiny_resource_and_manifest(                                                            \
    # RC_INCLUDEPATH - find icons, Windows manifest on MinGW and orm/version.hpp
    $$quote($$TINYORM_SOURCE_TREE/include/) $$quote($$TINYORM_SOURCE_TREE/resources/), \
    ../resources                                                                       \
)

# Use Precompiled headers (PCH)
# ---

precompile_header: \
    include(../include/pch.pri)

# Deployment
# ---

win32-msvc:CONFIG(debug, debug|release) {
    win32-msvc: target.path = C:/optx64/$${TARGET}
#    else: unix:!android: target.path = /opt/$${TARGET}/bin
    !isEmpty(target.path): INSTALLS += target
}

# Some info output
# ---

CONFIG(debug, debug|release):!build_pass: message( "Project is built in DEBUG mode." )
CONFIG(release, debug|release):!build_pass: message( "Project is built in RELEASE mode." )

# Disable debug output in release mode
CONFIG(release, debug|release) {
    !build_pass: message( "Disabling debug output." )
    DEFINES += QT_NO_DEBUG_OUTPUT
}

# User Configuration
# ---

exists(../conf.pri): \
    include(../conf.pri)

else:is_vcpkg_build: \
    include(../qmake/vcpkgconf.pri)

else: \
    error( "'conf.pri' for 'src' project does not exist. See an example configuration\
            in 'conf.pri.example' or call 'vcpkg install' in the project's root." )
