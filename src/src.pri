include(../include/include.pri)

SOURCES += \
    $$PWD/orm/databaseconnection.cpp \
    $$PWD/orm/entitymanager.cpp \
    $$PWD/orm/expression.cpp \
    $$PWD/orm/grammar.cpp \
    $$PWD/orm/logquery.cpp \
    $$PWD/orm/ormtypes.cpp \
    $$PWD/orm/query/joinclause.cpp \
    $$PWD/orm/query/querybuilder.cpp \
    $$PWD/orm/repositoryfactory.cpp \
    $$PWD/orm/tiny/basemodel.cpp \
    $$PWD/orm/tiny/concerns/hasattributes.cpp \
    $$PWD/orm/tiny/concerns/hasrelationstore.cpp \
    $$PWD/orm/tiny/relations/belongsto.cpp \
    $$PWD/orm/tiny/relations/hasmany.cpp \
    $$PWD/orm/tiny/relations/hasone.cpp \
    $$PWD/orm/tiny/relations/hasoneormany.cpp \
    $$PWD/orm/tiny/relations/relation.cpp \
    $$PWD/orm/tiny/tinybuilder.cpp\
    $$PWD/orm/utils/attribute.cpp \
    $$PWD/orm/utils/string.cpp \
    $$PWD/setting.cpp \
    $$PWD/torrent.cpp \
    $$PWD/torrentpeer.cpp \
    $$PWD/torrentpreviewablefile.cpp \
    $$PWD/torrentpreviewablefileproperty.cpp \
