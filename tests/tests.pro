TEMPLATE = subdirs

SUBDIRS = \
    auto \
    TinyUtils \

!disable_tom: \
    SUBDIRS += testdata_tom

auto.depends = TinyUtils
