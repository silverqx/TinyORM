QT *= core sql testlib
QT -= gui

TEMPLATE = app

# Common configuration
# ---

include(../../qmake/common.pri)

# Tests specific configuration
# ---

CONFIG *= qt console testcase link_prl

# Compiler and Linker options
# ---

win32-g++ {
    QMAKE_CXXFLAGS_DEBUG += -Wa,-mbig-obj
    # Avoid string table overflow
    QMAKE_CXXFLAGS_DEBUG += -O1
}

# TinyORM defines
# ---

DEFINES += PROJECT_TINYORM_TEST

# Enable code needed by tests, eg connection overriding in the Model
DEFINES += TINYORM_TESTS_CODE

# Other libraries defines
# ---

DEFINES += TINYORM_LINKING_SHARED

# User Configuration
# ---

exists(../conf.pri) {
    include(../conf.pri)
}
else {
    error( "'tests/conf.pri' for 'tests' project does not exist. See an example\
            configuration in 'tests/conf.pri.example'." )
}

# Use TinyORM's library precompiled headers (PCH)
# ---

include(../../include/pch.pri)

# TinyORM library headers include path
# ---

include(../../include/include.pri)

# Link against TinyORM library
# ---

win32:CONFIG(release, debug|release) {
    LIBS += -L$$TINYORM_BUILD_TREE/src/release/ -lTinyOrm
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$TINYORM_BUILD_TREE/src/debug/ -lTinyOrm
}
else:unix {
    LIBS += -L$$TINYORM_BUILD_TREE/src/ -lTinyOrm
}

target.CONFIG += no_default_install
