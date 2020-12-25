QT *= core sql testlib
QT -= gui

TEMPLATE = app

# Common configuration
# ---

include(../config.pri)

# Tests specific configuration
# ---

CONFIG *= qt console testcase link_prl

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
