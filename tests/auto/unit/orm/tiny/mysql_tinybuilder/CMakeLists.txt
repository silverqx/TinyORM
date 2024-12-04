add_executable(mysql_tinybuilder)

target_sources(mysql_tinybuilder PRIVATE tst_mysql_tinybuilder.cpp)

add_test(NAME mysql_tinybuilder COMMAND mysql_tinybuilder)

tiny_configure_test(mysql_tinybuilder INCLUDE_MODELS)
