project(model_return_relation
    LANGUAGES CXX
)

add_executable(model_return_relation
    tst_model_return_relation.cpp
)

add_test(NAME model_return_relation COMMAND model_return_relation)

include(TinyTestCommon)
tiny_configure_test(model_return_relation DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)
