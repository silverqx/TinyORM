TEMPLATE = subdirs

load(tiny_drivers)

subdirsList = \
    orm \
    others \

tiny_is_building_drivers(): \
    subdirsList += drivers

!disable_tom: \
    subdirsList += tom

SUBDIRS = $$sorted(subdirsList)

unset(subdirsList)
