# Common Configuration ( also for tests )
# ---

CONFIG *= c++2a strict_c++ warn_on utf8_source hide_symbols silent
CONFIG -= c++11 app_bundle

# Qt defines
# ---

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
# Disables all the APIs deprecated before Qt 6.0.0
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

#DEFINES += QT_NO_CAST_FROM_ASCII
#DEFINES += QT_RESTRICTED_CAST_FROM_ASCII
DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_CAST_FROM_BYTEARRAY
DEFINES += QT_USE_QSTRINGBUILDER
DEFINES += QT_STRICT_ITERATORS

# TinyORM defines
# ---

# Enable MySQL ping on Orm::MySqlConnection
mysql_ping {
    DEFINES += TINYORM_MYSQL_PING
}

# Platform specific configuration
# ---
win32: include(winconf.pri)
macx: include(macxconf.pri)
unix:!macx: include(unixconf.pri)
