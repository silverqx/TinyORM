# migrate auto test
# ---

set(migrate_ns migrate)
set(migrate_target ${migrate_ns})

project(${migrate_ns}
    LANGUAGES CXX
)

add_executable(${migrate_target}
    tst_migrate.cpp
)

add_test(NAME ${migrate_target} COMMAND ${migrate_target})

include(TinyTestCommon)
tiny_configure_test(${migrate_target} DEPENDS_ON_UNITTESTS INCLUDE_MIGRATIONS)
