add_executable(blueprint
    tst_blueprint.cpp
)

add_test(NAME blueprint COMMAND blueprint)

tiny_configure_test(blueprint)
