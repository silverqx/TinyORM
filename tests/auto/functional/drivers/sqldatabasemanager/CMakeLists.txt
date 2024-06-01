project(sqldatabasemanager
    LANGUAGES CXX
)

add_executable(sqldatabasemanager
    tst_sqldatabasemanager.cpp
)

add_test(NAME sqldatabasemanager COMMAND sqldatabasemanager)

include(TinyTestCommon)
tiny_configure_test(sqldatabasemanager RUN_SERIAL)
