QT *= core sql testlib
QT -= gui

TEMPLATE = app

# Common configuration
# ---

include(../config.pri)

# Tests specific configuration
# ---

CONFIG *= qt console testcase link_prl

# TinyORM defines
# ---

DEFINES += PROJECT_TINYORM_TEST

# Use TinyORM's library precompiled headers (PCH)
# ---

include(../include/pch.pri)

# TinyORM library headers include path
# ---

include(../include/include.pri)

# Link against TinyORM library
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
