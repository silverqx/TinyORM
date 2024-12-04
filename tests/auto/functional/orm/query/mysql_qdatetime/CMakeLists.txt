add_executable(mysql_qdatetime)

target_sources(mysql_qdatetime PRIVATE tst_mysql_qdatetime.cpp)

add_test(NAME mysql_qdatetime COMMAND mysql_qdatetime)

tiny_configure_test(mysql_qdatetime DEPENDS_ON_UNITTESTS RUN_SERIAL)
