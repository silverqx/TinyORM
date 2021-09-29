include($$TINYORM_SOURCE_TREE/tests/qmake/common.pri)
include($$TINYORM_SOURCE_TREE/tests/auto/utils.pri)

HEADERS += \
    $$PWD/include/versiondebug_qmake.hpp.in \
    $$OUT_PWD/include/versiondebug_qmake.hpp \

SOURCES = tst_version.cpp

# Used by checkFileVersion_*() tests
win32-msvc {
    DEFINES += TINYTEST_VERSION_IS_QMAKE
    CONFIG(shared, static|shared) {
        DEFINES += TINYTEST_VERSION_IS_SHARED_BUILD
    }

    CONFIG(release, debug|release) {
        TINY_RELEASE_TYPE = release
    }
    else:CONFIG(debug, debug|release) {
        TINY_RELEASE_TYPE = debug
    }

    TINYTEST_VERSION_TINYORM_PATH = \
        $$quote($${TINYORM_BUILD_TREE}/src/$${TINY_RELEASE_TYPE}/TinyOrm0.dll)
    TINYTEST_VERSION_TINYUTILS_PATH = \
        $$quote($${TINYORM_BUILD_TREE}/tests/auto/utils/$${TINY_RELEASE_TYPE}/TinyUtils0.dll)

    QMAKE_SUBSTITUTES += $$quote(include/versiondebug_qmake.hpp.in)

    INCLUDEPATH += $$quote($$OUT_PWD/include/)

    LIBS += -lVersion
}
