project(databasemanager
    LANGUAGES CXX
)

add_executable(databasemanager
    tst_databasemanager.cpp
)

add_test(NAME databasemanager COMMAND databasemanager)

include(TinyTestCommon)
tiny_configure_test(databasemanager DEPENDS_ON_UNITTESTS PROVIDES_PCH)
