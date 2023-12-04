TEMPLATE = subdirs

subdirsList = \
    databasemanager \
    postgresql_connection \
    query \
    schema \

!disable_orm: \
    subdirsList += tiny

SUBDIRS = $$sorted(subdirsList)

unset(subdirsList)
