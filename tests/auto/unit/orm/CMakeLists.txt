add_subdirectory(databaseconnection)
add_subdirectory(query)

if(ORM)
    add_subdirectory(tiny)
endif()

add_subdirectory(version)
