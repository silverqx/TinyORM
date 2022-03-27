QT *= core sql
QT -= gui

TEMPLATE = app
TARGET = tom

# TinyTom application specific configuration
# ---

CONFIG *= console

# Common Configuration
# ---

include($$TINYORM_SOURCE_TREE/qmake/common.pri)

# TinyTom defines
# ---

DEFINES += PROJECT_TINYTOM

# Release build
CONFIG(release, debug|release): DEFINES += TINYTOM_NO_DEBUG
# Debug build
CONFIG(debug, debug|release): DEFINES *= TINYTOM_DEBUG

# Enable code needed by tests
#build_tests: \
#    DEFINES *= TINYTOM_TESTS_CODE

# TinyTom library header and source files
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

# Find version.hpp
tinyRcIncludepath = $$quote($$PWD/../include/)
# Find Windows manifest
mingw: tinyRcIncludepath += $$quote($$PWD/../resources/)

load(tiny_resource_and_manifest)
tiny_resource_and_manifest($$tinyRcIncludepath, ../resources)

# Use Precompiled headers (PCH)
# ---

include(../include/pch.pri)

# Deployment
# ---

win32-msvc:CONFIG(debug, debug|release) {
    win32-msvc: target.path = C:/optx64/$${TARGET}
#    else: unix:!android: target.path = /opt/$${TARGET}/bin
    !isEmpty(target.path): INSTALLS += target
}

# Configure TinyORM library
# ---

include($$TINYORM_SOURCE_TREE/qmake/TinyOrm.pri)

# User Configuration
# ---

exists(../conf.pri): \
    include(../conf.pri)

#else:is_vcpkg_build: \
#    include(../qmake/vcpkgconf.pri)

else: \
    error( "'conf.pri' for 'tom' project does not exist. See an example configuration\
            in 'tom/conf.pri.example' or call 'vcpkg install' in the project's root." )

# Link against TinyORM library
# ---

INCLUDEPATH += $$quote($$TINYORM_SOURCE_TREE/include/)

LIBS += $$quote(-L$$TINYORM_BUILD_TREE/src$${TINY_RELEASE_TYPE}/)
LIBS += -lTinyOrm
