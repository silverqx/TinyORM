#.rst:
# FindMySQL
# ---------
#
# Try to locate the mysql client library.
# If found, this will define the following variables:
#
# ``MySQL_FOUND``
#     True if the mysql library is available
# ``MySQL_INCLUDE_DIRS``
#     The mysql include directories
# ``MySQL_LIBRARIES``
#     The mysql libraries for linking
#
# If ``MySQL_FOUND`` is TRUE, it will also define the following
# imported target:
#
# ``MySQL::MySQL``
#     The mysql client library

find_package(PkgConfig QUIET)

# TODO future, why it finds 'C:/Program Files/MySQL/MySQL Server 8.0/lib/libmysql.lib' on mingw and doesn't search mingw dirs first? silverqx
# This hack fixes bug on the MinGW described above
set(tmpCheckModules_mysql mysqlclient)
set(tmpCheckModules_maria libmariadb)
set(tmpFindPathSuffixes_mysql mysql)
set(tmpFindPathSuffixes_maria mariadb)
set(tmpFindLibraryNames_mysql libmysql mysql mysqlclient)
set(tmpFindLibraryNames_maria libmariadb mariadb)

# Prefer mariadb on MinGW as there is no mysqlclient library
if(MINGW)
    set(tmpCheckModules ${tmpCheckModules_maria})
    set(tmpFindPathSuffixes ${tmpFindPathSuffixes_maria})
    set(tmpFindLibraryNames ${tmpFindLibraryNames_maria})
else()
    set(tmpCheckModules ${tmpCheckModules_mysql} ${tmpCheckModules_maria})
    set(tmpFindPathSuffixes ${tmpFindPathSuffixes_mysql} ${tmpFindPathSuffixes_maria})
    set(tmpFindLibraryNames ${tmpFindLibraryNames_mysql} ${tmpFindLibraryNames_maria})
endif()

pkg_check_modules(PC_MySQL QUIET ${tmpCheckModules})

find_path(MySQL_INCLUDE_DIR
    NAMES mysql.h
    HINTS ${PC_MySQL_INCLUDEDIR}
    PATH_SUFFIXES ${tmpFindPathSuffixes}
)

find_library(MySQL_LIBRARY
    NAMES ${tmpFindLibraryNames}
    HINTS ${PC_MySQL_LIBDIR}
)

unset(tmpFindLibraryNames)
unset(tmpFindPathSuffixes)
unset(tmpCheckModules)
unset(tmpFindLibraryNames_maria)
unset(tmpFindLibraryNames_mysql)
unset(tmpFindPathSuffixes_maria)
unset(tmpFindPathSuffixes_mysql)
unset(tmpCheckModules_maria)
unset(tmpCheckModules_mysql)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MySQL DEFAULT_MSG MySQL_LIBRARY MySQL_INCLUDE_DIR)

if(MySQL_FOUND)
    set(MySQL_INCLUDE_DIRS "${MySQL_INCLUDE_DIR}")
    set(MySQL_LIBRARIES "${MySQL_LIBRARY}")

    if(NOT TARGET MySQL::MySQL)
        add_library(MySQL::MySQL UNKNOWN IMPORTED)
        set_target_properties(MySQL::MySQL
            PROPERTIES
                IMPORTED_LOCATION "${MySQL_LIBRARIES}"
                INTERFACE_INCLUDE_DIRECTORIES "${MySQL_INCLUDE_DIRS}"
        )
    endif()
endif()

mark_as_advanced(MySQL_INCLUDE_DIR MySQL_LIBRARY)

include(FeatureSummary)
set_package_properties(MySQL
    PROPERTIES
        URL "https://www.mysql.com"
        DESCRIPTION "MySQL client library"
)
