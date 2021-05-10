TEMPLATE = subdirs

SUBDIRS = \
    functional \
    unit \
    utils \

functional.depends = utils
unit.depends = utils
