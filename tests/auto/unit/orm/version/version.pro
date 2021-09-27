include($$TINYORM_SOURCE_TREE/tests/qmake/common.pri)
include($$TINYORM_SOURCE_TREE/tests/auto/utils.pri)

HEADERS += \
    $$PWD/include/versiondebug_qmake.hpp.in \
    $$OUT_PWD/include/versiondebug_qmake.hpp \

SOURCES = tst_version.cpp

QMAKE_SUBSTITUTES += $$quote(include/versiondebug_qmake.hpp.in)

INCLUDEPATH += $$quote($$OUT_PWD/include)

LIBS += -lVersion
