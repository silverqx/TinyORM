project(model
    LANGUAGES CXX
)

add_executable(model
    tst_model.cpp
)

add_test(NAME model COMMAND model)

include(TinyTestCommon)
tiny_configure_test(model DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)
