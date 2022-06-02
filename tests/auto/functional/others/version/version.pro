include($$TINYORM_SOURCE_TREE/tests/qmake/common.pri)
include($$TINYORM_SOURCE_TREE/tests/qmake/TinyUtils.pri)

SOURCES = tst_version.cpp

# Used by checkFileVersion_*() tests
win32 {
    DEFINES += TINYTEST_VERSION_IS_QMAKE

    tom_example:!disable_tom: \
        DEFINES += TINYTOM_EXAMPLE

    CONFIG(shared, dll|shared|static|staticlib) | \
    CONFIG(dll, dll|shared|static|staticlib): \
        DEFINES += TINYTEST_VERSION_IS_SHARED_BUILD

    TINYTEST_VERSION_TINYORM_PATH = \
        $$quote($${TINYORM_BUILD_TREE}/src$${TINY_BUILD_SUBFOLDER}/TinyOrm0.dll)
    TINYTEST_VERSION_TINYUTILS_PATH = \
        $$quote($${TINYORM_BUILD_TREE}/tests/TinyUtils$${TINY_BUILD_SUBFOLDER}/TinyUtils0.dll)
    TINYTEST_VERSION_TOMEXAMPLE_PATH = \
        $$quote($${TINYORM_BUILD_TREE}/examples/tom$${TINY_BUILD_SUBFOLDER}/tom.exe)

    QMAKE_SUBSTITUTES += $$quote(include/versiondebug_qmake.hpp.in)

    HEADERS += $$OUT_PWD/include/versiondebug_qmake.hpp

    INCLUDEPATH += $$quote($$OUT_PWD/include/)

    # To find tom/include/version.hpp (don't need to include whole qmake/tom.pri)
    tom_example:!disable_tom: \
        INCLUDEPATH += $$quote($$TINYORM_SOURCE_TREE/tom/include/)

    LIBS += -lVersion
}
