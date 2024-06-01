project(model_qdatetime
    LANGUAGES CXX
)

add_executable(model_qdatetime
    tst_model_qdatetime.cpp
)

add_test(NAME model_qdatetime COMMAND model_qdatetime)

include(TinyTestCommon)
tiny_configure_test(model_qdatetime DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)
