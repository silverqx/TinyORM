project(castattributes
    LANGUAGES CXX
)

add_executable(castattributes
    tst_castattributes.cpp
)

add_test(NAME castattributes COMMAND castattributes)

include(TinyTestCommon)
tiny_configure_test(castattributes DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)
