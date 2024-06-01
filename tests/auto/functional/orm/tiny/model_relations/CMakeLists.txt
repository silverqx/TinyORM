project(model_relations
    LANGUAGES CXX
)

add_executable(model_relations
    tst_model_relations.cpp
)

add_test(NAME model_relations COMMAND model_relations)

include(TinyTestCommon)
tiny_configure_test(model_relations DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)
