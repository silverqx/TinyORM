project(sqlquery_prepared
    LANGUAGES CXX
)

add_executable(sqlquery_prepared
    tst_sqlquery_prepared.cpp
)

add_test(NAME sqlquery_prepared COMMAND sqlquery_prepared)

tiny_configure_test(sqlquery_prepared RUN_SERIAL)
