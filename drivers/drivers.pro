TEMPLATE = subdirs

load(tiny_drivers)

SUBDIRS = common

build_loadable_drivers: \
tiny_is_building_driver(mysql) {
    SUBDIRS += mysql
    mysql.depends = common
}
