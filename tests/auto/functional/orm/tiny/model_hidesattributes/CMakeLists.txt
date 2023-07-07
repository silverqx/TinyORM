project(model_hidesattributes
    LANGUAGES CXX
)

add_executable(model_hidesattributes
    tst_model_hidesattributes.cpp
)

add_test(NAME model_hidesattributes COMMAND model_hidesattributes)

include(TinyTestCommon)
tiny_configure_test(model_hidesattributes INCLUDE_MODELS)
