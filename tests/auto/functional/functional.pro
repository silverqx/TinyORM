TEMPLATE = subdirs

subdirsList = \
    orm \

!disable_tom: \
    subdirsList += \
        tom \

SUBDIRS = $$sorted(subdirsList)

unset(subdirsList)
