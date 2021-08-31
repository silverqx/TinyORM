project(mysql_querybuilder
    LANGUAGES CXX
)

add_executable(mysql_querybuilder
    tst_mysql_querybuilder.cpp
)

add_test(NAME mysql_querybuilder COMMAND mysql_querybuilder)

include(TinyTestCommon)
tiny_configure_test(mysql_querybuilder)
