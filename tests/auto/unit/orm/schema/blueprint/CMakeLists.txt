add_executable(blueprint)

target_sources(blueprint PRIVATE tst_blueprint.cpp)

add_test(NAME blueprint COMMAND blueprint)

tiny_configure_test(blueprint)
