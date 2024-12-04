add_executable(model)

target_sources(model PRIVATE tst_model.cpp)

add_test(NAME model COMMAND model)

tiny_configure_test(model DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)
