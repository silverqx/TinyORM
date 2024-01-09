# The common/TinyOrm.pri exists because the src.pro project (TinyOrm library) needs only
# this part.

# Common for the whole TinyORM library
# ---

include($$PWD/common.pri)

# TinyORM defines
# ---

# Release build
CONFIG(release, debug|release): DEFINES += TINYORM_NO_DEBUG
# Debug build
CONFIG(debug, debug|release): DEFINES *= TINYORM_DEBUG

# Enable MySQL ping on Orm::MySqlConnection
mysql_ping: DEFINES *= TINYORM_MYSQL_PING

# Log queries with a time measurement
CONFIG(release, debug|release): DEFINES += TINYORM_NO_DEBUG_SQL
CONFIG(debug, debug|release): DEFINES *= TINYORM_DEBUG_SQL

# Enable code needed by tests, eg. connection overriding in the Model or
# MySqlConnection::setConfigVersion().
build_tests: \
    DEFINES *= TINYORM_TESTS_CODE

# TinyTom related defines
# ---

!disable_tom {
    # Release build
    CONFIG(release, debug|release): DEFINES += TINYTOM_NO_DEBUG
    # Debug build
    CONFIG(debug, debug|release): DEFINES *= TINYTOM_DEBUG

    # Enable code needed by tests (modify the migrate:status command for tests need)
    build_tests: \
        DEFINES *= TINYTOM_TESTS_CODE
}

# Link against the TinyDrivers library (if enabled) or QtSql module
# ---

load(tiny_drivers)
tiny_link_drivers()
