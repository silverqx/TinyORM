include($$TINYORM_SOURCE_TREE/tests/qmake/common.pri)
include($$TINYORM_SOURCE_TREE/tests/qmake/TinyUtils.pri)
!disable_orm: \
    include($$TINYORM_SOURCE_TREE/tests/models/models.pri)

SOURCES = tst_postgresql_schemabuilder.cpp
