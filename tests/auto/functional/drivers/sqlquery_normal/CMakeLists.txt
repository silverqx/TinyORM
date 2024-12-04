add_executable(sqlquery_normal)

target_sources(sqlquery_normal PRIVATE tst_sqlquery_normal.cpp)

add_test(NAME sqlquery_normal COMMAND sqlquery_normal)

tiny_configure_test(sqlquery_normal RUN_SERIAL)
