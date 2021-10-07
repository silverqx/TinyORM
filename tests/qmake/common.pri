QT *= core sql testlib
QT -= gui

TEMPLATE = app

# Tests specific configuration
# ---

CONFIG *= qt console testcase link_prl

# Common configuration
# ---

include(../../qmake/common.pri)

# Compiler and Linker options
# ---

win32-g++ {
    QMAKE_CXXFLAGS_DEBUG += -Wa,-mbig-obj
    # Avoid string table overflow
    QMAKE_CXXFLAGS_DEBUG += -O1
}

# Tests defines
# ---

DEFINES += PROJECT_TINYORM_TEST

# TinyORM library defines
# ---

# Enable code needed by tests, eg connection overriding in the Model
DEFINES += TINYORM_TESTS_CODE

CONFIG(shared, dll|shared|static|staticlib) | \
CONFIG(dll, dll|shared|static|staticlib): \
    DEFINES += TINYORM_LINKING_SHARED

# Use TinyORM's library precompiled headers (PCH)
# ---

precompile_header: \
    include(../../include/pch.pri)

# TinyORM library headers include path
# ---

include(../../include/include.pri)

# Link against TinyORM library
# ---

LIBS += $$quote(-L$$TINYORM_BUILD_TREE/src$$TINY_RELEASE_TYPE/) -lTinyOrm

target.CONFIG += no_default_install

# User Configuration
# ---

exists(../conf.pri): \
    include(../conf.pri)

else: \
    error( "'tests/conf.pri' for 'tests' project does not exist. See an example\
            configuration in 'tests/conf.pri.example'." )
