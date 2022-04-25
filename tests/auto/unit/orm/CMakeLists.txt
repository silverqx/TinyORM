add_subdirectory(databaseconnection)
add_subdirectory(query)
add_subdirectory(schema)

if(ORM)
    add_subdirectory(tiny)
endif()
