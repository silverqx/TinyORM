project(sqlite_querybuilder
    LANGUAGES CXX
)

add_executable(sqlite_querybuilder
    tst_sqlite_querybuilder.cpp
)

add_test(NAME sqlite_querybuilder COMMAND sqlite_querybuilder)

include(TinyTestCommon)
tiny_configure_test(sqlite_querybuilder)
