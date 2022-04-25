TEMPLATE = subdirs

subdirsList = \
    orm \
    others \

!disable_tom: \
    subdirsList += \
        tom \

SUBDIRS = $$sorted(subdirsList)

unset(subdirsList)
