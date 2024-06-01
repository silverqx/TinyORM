project(model_connection_independent
    LANGUAGES CXX
)

add_executable(model_conn_indep
    tst_model_connection_independent.cpp
)

add_test(NAME model_conn_indep COMMAND model_conn_indep)

include(TinyTestCommon)
tiny_configure_test(model_conn_indep DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)
