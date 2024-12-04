add_executable(model_qdatetime)

target_sources(model_qdatetime PRIVATE tst_model_qdatetime.cpp)

add_test(NAME model_qdatetime COMMAND model_qdatetime)

tiny_configure_test(model_qdatetime DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)
