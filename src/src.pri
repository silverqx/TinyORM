INCLUDEPATH += $$PWD

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
    $$PWD/testorm.cpp \
    $$PWD/torrent.cpp \
    $$PWD/torrentpeer.cpp \
    $$PWD/torrentpreviewablefile.cpp \
    $$PWD/torrentpreviewablefileproperty.cpp \

HEADERS += \
    $$PWD/export.h \
    $$PWD/orm/databaseconnection.h \
    $$PWD/orm/entitymanager.h \
    $$PWD/orm/expression.h \
    $$PWD/orm/grammar.h \
    $$PWD/orm/logquery.h \
    $$PWD/orm/ormerror.h \
    $$PWD/orm/ormtypes.h \
    $$PWD/orm/query/joinclause.h \
    $$PWD/orm/query/querybuilder.h \
    $$PWD/orm/repositoryfactory.h \
    $$PWD/orm/tiny/basemodel.h \
    $$PWD/orm/tiny/concerns/hasattributes.h \
    $$PWD/orm/tiny/concerns/hasrelationstore.h \
    $$PWD/orm/tiny/relations/belongsto.h \
    $$PWD/orm/tiny/relations/hasmany.h \
    $$PWD/orm/tiny/relations/hasone.h \
    $$PWD/orm/tiny/relations/hasoneormany.h \
    $$PWD/orm/tiny/relations/relation.h \
    $$PWD/orm/tiny/tinybuilder.h \
    $$PWD/orm/utils/attribute.h \
    $$PWD/orm/utils/string.h \
    $$PWD/orm/utils/type.h \
    $$PWD/setting.h \
    $$PWD/testorm.h \
    $$PWD/torrent.h \
    $$PWD/torrentpeer.h \
    $$PWD/torrentpreviewablefile.h \
    $$PWD/torrentpreviewablefileproperty.h \

SUBDIRS += \
    $$PWD/src.pro
