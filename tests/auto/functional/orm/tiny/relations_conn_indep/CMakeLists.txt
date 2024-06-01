project(relations_connection_independent
    LANGUAGES CXX
)

add_executable(relations_conn_indep
    tst_relations_connection_independent.cpp
)

add_test(NAME relations_conn_indep COMMAND relations_conn_indep)

include(TinyTestCommon)
tiny_configure_test(relations_conn_indep DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)
