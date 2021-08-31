project(mysql_querybuilder
    LANGUAGES CXX
)

add_executable(mysql_querybuilder
    tst_mysql_querybuilder.cpp
)

add_test(NAME mysql_querybuilder COMMAND mysql_querybuilder)

include(TinyTestCommon)
tiny_configure_test(mysql_querybuilder)

#set(test_env_path "${CMAKE_BINARY_DIR};${CMAKE_BINARY_DIR}/tests/auto/utils;$ENV{PATH}")
##message(STATUS "env_path : ${test_env_path}")
#string(REPLACE ";" "\;" test_env_path "${test_env_path}")
#message(STATUS "env_path : ${test_env_path}")
#set_property(TEST mysql_querybuilder APPEND PROPERTY
#    ENVIRONMENT "PATH=${test_env_path}"
#)
