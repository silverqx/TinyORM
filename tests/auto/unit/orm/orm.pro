TEMPLATE = subdirs

subdirsList = \
    databaseconnection \
    query \
    schema \
    version \

!disable_orm: \
    subdirsList += \
        tiny \

SUBDIRS = $$sorted(subdirsList)

unset(subdirsList)
