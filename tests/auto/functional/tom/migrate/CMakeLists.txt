project(${migrate_ns}
    LANGUAGES CXX
)

add_executable(migrate
    tst_migrate.cpp
)

add_test(NAME migrate COMMAND migrate)

tiny_configure_test(migrate DEPENDS_ON_UNITTESTS INCLUDE_MIGRATIONS)
