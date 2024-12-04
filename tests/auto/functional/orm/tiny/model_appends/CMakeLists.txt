add_executable(model_appends)

target_sources(model_appends PRIVATE tst_model_appends.cpp)

add_test(NAME model_appends COMMAND model_appends)

tiny_configure_test(model_appends DEPENDS_ON_UNITTESTS INCLUDE_MODELS)
