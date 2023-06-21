project(model_serialization
    LANGUAGES CXX
)

add_executable(model_serialization
    tst_model_serialization.cpp
)

add_test(NAME model_serialization COMMAND model_serialization)

include(TinyTestCommon)
tiny_configure_test(model_serialization INCLUDE_MODELS)
