TEMPLATE = subdirs

subdirsList = \
    databasemanager \
    query \
    schema \

!disable_orm: \
    subdirsList += \
        tiny \

SUBDIRS = $$sorted(subdirsList)

unset(subdirsList)
