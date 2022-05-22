project(mysql_schemabuilder
    LANGUAGES CXX
)

add_executable(mysql_schemabuilder
    tst_postgresql_schemabuilder.cpp
)

add_test(NAME mysql_schemabuilder COMMAND mysql_schemabuilder)

include(TinyTestCommon)
if(ORM)
    tiny_configure_test(mysql_schemabuilder INCLUDE_MODELS)
else()
    tiny_configure_test(mysql_schemabuilder)
endif()
