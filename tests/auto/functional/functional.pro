TEMPLATE = subdirs

subdirsList = \
    drivers \
    orm \
    others \

!disable_tom: \
    subdirsList += \
        tom \

SUBDIRS = $$sorted(subdirsList)

unset(subdirsList)
