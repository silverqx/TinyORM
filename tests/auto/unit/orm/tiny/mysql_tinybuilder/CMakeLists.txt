project(mysql_tinybuilder
    LANGUAGES CXX
)

add_executable(mysql_tinybuilder
    tst_mysql_tinybuilder.cpp
)

add_test(NAME mysql_tinybuilder COMMAND mysql_tinybuilder)

include(TinyTestCommon)
tiny_configure_test(mysql_tinybuilder INCLUDE_MODELS)
