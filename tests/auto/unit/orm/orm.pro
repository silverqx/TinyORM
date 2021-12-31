TEMPLATE = subdirs

subdirsList = \
    databaseconnection \
    query \
    version \

!disable_orm: \
    subdirsList += \
        tiny \

SUBDIRS = $$sorted(subdirsList)

unset(subdirsList)
