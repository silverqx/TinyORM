project(mysql_qdatetime
    LANGUAGES CXX
)

add_executable(mysql_qdatetime
    tst_mysql_qdatetime.cpp
)

add_test(NAME mysql_qdatetime COMMAND mysql_qdatetime)

include(TinyTestCommon)
tiny_configure_test(mysql_qdatetime DEPENDS_ON_UNITTESTS RUN_SERIAL)
