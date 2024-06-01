project(sqlquery_normal
    LANGUAGES CXX
)

add_executable(sqlquery_normal
    tst_sqlquery_normal.cpp
)

add_test(NAME sqlquery_normal COMMAND sqlquery_normal)

include(TinyTestCommon)
tiny_configure_test(sqlquery_normal RUN_SERIAL)
