include($$TINYORM_SOURCE_TREE/tests/qmake/common.pri)
include($$TINYORM_SOURCE_TREE/tests/qmake/TinyUtils.pri)

HEADERS += $$PWD/include/versiondebug_qmake.hpp.in

SOURCES = tst_version.cpp

# Used by checkFileVersion_*() tests
win32 {
    DEFINES += TINYTEST_VERSION_IS_QMAKE

    CONFIG(shared, dll|shared|static|staticlib) | \
    CONFIG(dll, dll|shared|static|staticlib): \
        DEFINES += TINYTEST_VERSION_IS_SHARED_BUILD

    TINYTEST_VERSION_TINYORM_PATH = \
        $$quote($${TINYORM_BUILD_TREE}/src$${TINY_RELEASE_TYPE}/TinyOrm0.dll)
    TINYTEST_VERSION_TINYUTILS_PATH = \
        $$quote($${TINYORM_BUILD_TREE}/tests/utils$${TINY_RELEASE_TYPE}/TinyUtils0.dll)

    QMAKE_SUBSTITUTES += $$quote(include/versiondebug_qmake.hpp.in)

    HEADERS += $$OUT_PWD/include/versiondebug_qmake.hpp

    INCLUDEPATH += $$quote($$OUT_PWD/include/)

    LIBS += -lVersion
}
