add_executable(databasemanager)

target_sources(databasemanager PRIVATE tst_databasemanager.cpp)

add_test(NAME databasemanager COMMAND databasemanager)

tiny_configure_test(databasemanager DEPENDS_ON_UNITTESTS PROVIDES_PCH)
