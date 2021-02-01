include(../include/include.pri)

SOURCES += \
    $$PWD/orm/concerns/detectslostconnections.cpp \
    $$PWD/orm/concerns/hasconnectionresolver.cpp \
    $$PWD/orm/connectors/connectionfactory.cpp \
    $$PWD/orm/connectors/connector.cpp \
    $$PWD/orm/connectors/mysqlconnector.cpp \
    $$PWD/orm/databaseconnection.cpp \
    $$PWD/orm/databasemanager.cpp \
    $$PWD/orm/grammar.cpp \
    $$PWD/orm/logquery.cpp \
    $$PWD/orm/mysqlconnection.cpp \
    $$PWD/orm/ormtypes.cpp \
    $$PWD/orm/query/expression.cpp \
    $$PWD/orm/query/joinclause.cpp \
    $$PWD/orm/query/querybuilder.cpp \
    $$PWD/orm/queryerror.cpp \
    $$PWD/orm/sqlerror.cpp \
    $$PWD/orm/support/configurationoptionsparser.cpp \
    $$PWD/orm/tiny/basemodel.cpp \
    $$PWD/orm/tiny/concerns/hasattributes.cpp \
    $$PWD/orm/tiny/concerns/hasrelationstore.cpp \
    $$PWD/orm/tiny/modelnotfounderror.cpp \
    $$PWD/orm/tiny/relations/belongsto.cpp \
    $$PWD/orm/tiny/relations/hasmany.cpp \
    $$PWD/orm/tiny/relations/hasone.cpp \
    $$PWD/orm/tiny/relations/hasoneormany.cpp \
    $$PWD/orm/tiny/relations/relation.cpp \
    $$PWD/orm/tiny/tinybuilder.cpp\
    $$PWD/orm/utils/attribute.cpp \
    $$PWD/orm/utils/string.cpp \
