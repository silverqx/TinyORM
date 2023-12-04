TEMPLATE = subdirs

subdirsList = \
    databaseconnection \
    query \
    schema \

!disable_orm: \
    subdirsList += tiny

SUBDIRS = $$sorted(subdirsList)

unset(subdirsList)
