QT *= core sql testlib
QT -= gui

TEMPLATE = app

# Common configuration
# ---

include(../config.pri)

# Tests specific configuration
# ---

CONFIG *= qt console testcase link_prl

# TinyOrm defines
# ---

DEFINES += PROJECT_TINYORM_TEST

# Use TinyOrm's library precompiled headers (PCH)
# ---

include(../include/pch.pri)

# TinyOrm library headers include path
# ---

include(../include/include.pri)

# Link against TinyOrm library
# ---

win32:CONFIG(release, debug|release) {
    LIBS += -L$$TINYORM_BUILD_TREE/src/release/ -lTinyOrm0
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$TINYORM_BUILD_TREE/src/debug/ -lTinyOrm0
}
else:unix {
    LIBS += -L$$TINYORM_BUILD_TREE/src/ -lTinyOrm0
}

target.CONFIG += no_default_install
