project(model_appends
    LANGUAGES CXX
)

add_executable(model_appends
    tst_model_appends.cpp
)

add_test(NAME model_appends COMMAND model_appends)

include(TinyTestCommon)
tiny_configure_test(model_appends DEPENDS_ON_UNITTESTS INCLUDE_MODELS)
