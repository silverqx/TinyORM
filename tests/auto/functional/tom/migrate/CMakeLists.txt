add_executable(migrate)

target_sources(migrate PRIVATE tst_migrate.cpp)

add_test(NAME migrate COMMAND migrate)

tiny_configure_test(migrate DEPENDS_ON_UNITTESTS INCLUDE_MIGRATIONS)
