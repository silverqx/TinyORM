# utils library headers include path
# ---

INCLUDEPATH += $$quote($$PWD/utils/src)

# Link against tests's utils library
# ---

win32:CONFIG(release, debug|release) {
    LIBS += -L$$TINYORM_BUILD_TREE/tests/auto/utils/release/ -lutils0
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$TINYORM_BUILD_TREE/tests/auto/utils/debug/ -lutils0
}
else:unix {
    LIBS += -L$$TINYORM_BUILD_TREE/tests/auto/utils/ -lutils
}
