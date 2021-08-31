project(model_connection_independent
    LANGUAGES CXX
)

add_executable(model_connection_independent
    tst_model_connection_independent.cpp
)

add_test(NAME model_connection_independent COMMAND model_connection_independent)

include(TinyTestCommon)
tiny_configure_test(model_connection_independent INCLUDE_MODELS)
