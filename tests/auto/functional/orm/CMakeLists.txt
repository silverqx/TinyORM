add_subdirectory(databasemanager)
add_subdirectory(query)

if(ORM)
    add_subdirectory(tiny)
endif()
