# TinyUtils library headers include path
# ---

INCLUDEPATH += $$quote($$TINYORM_SOURCE_TREE/tests/auto/utils/src/)

# Link against tests's TinyUtils library
# ---

win32:CONFIG(release, debug|release) {
    LIBS += $$quote(-L$$TINYORM_BUILD_TREE/tests/auto/utils/release/) -lTinyUtils
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += $$quote(-L$$TINYORM_BUILD_TREE/tests/auto/utils/debug/) -lTinyUtils
}
else:unix {
    LIBS += $$quote(-L$$TINYORM_BUILD_TREE/tests/auto/utils/) -lTinyUtils
}
