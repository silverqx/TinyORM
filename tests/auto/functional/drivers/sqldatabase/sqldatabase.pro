# Add the TinyDrivers include path as a non-system include path
TINY_DRIVERS_INCLUDE_NONSYSTEM = true

include($$TINYORM_SOURCE_TREE/tests/qmake/common.pri)
include($$TINYORM_SOURCE_TREE/tests/qmake/TinyUtils.pri)

SOURCES += tst_sqldatabase.cpp
