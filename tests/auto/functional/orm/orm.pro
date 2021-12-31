TEMPLATE = subdirs

subdirsList = \
    databasemanager \
    query \

!disable_orm: \
    subdirsList += \
        tiny \

SUBDIRS = $$sorted(subdirsList)

unset(subdirsList)
