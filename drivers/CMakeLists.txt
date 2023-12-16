# TinyDrivers database drivers
# ---

add_subdirectory(common)

if(TINY_BUILD_LOADABLE_DRIVERS AND BUILD_MYSQL_DRIVER)
    add_subdirectory(mysql)
endif()
