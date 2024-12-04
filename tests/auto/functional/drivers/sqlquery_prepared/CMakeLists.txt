add_executable(sqlquery_prepared)

target_sources(sqlquery_prepared PRIVATE tst_sqlquery_prepared.cpp)

add_test(NAME sqlquery_prepared COMMAND sqlquery_prepared)

tiny_configure_test(sqlquery_prepared RUN_SERIAL)
