add_executable(mysql_querybuilder
    tst_mysql_querybuilder.cpp
)

add_test(NAME mysql_querybuilder COMMAND mysql_querybuilder)

tiny_configure_test(mysql_querybuilder)
