project(blueprint
    LANGUAGES CXX
)

add_executable(blueprint
    tst_blueprint.cpp
)

add_test(NAME blueprint COMMAND blueprint)

include(TinyTestCommon)
tiny_configure_test(blueprint)
