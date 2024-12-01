add_executable(mysql_schemabuilder
    tst_mysql_schemabuilder.cpp
)

add_test(NAME mysql_schemabuilder COMMAND mysql_schemabuilder)

if(ORM)
    tiny_configure_test(mysql_schemabuilder INCLUDE_MODELS)
else()
    tiny_configure_test(mysql_schemabuilder)
endif()
