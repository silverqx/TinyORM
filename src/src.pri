include(../include/include.pri)

SOURCES += \
    $$PWD/orm/concerns/detectslostconnections.cpp \
    $$PWD/orm/concerns/hasconnectionresolver.cpp \
    $$PWD/orm/connectors/connectionfactory.cpp \
    $$PWD/orm/connectors/connector.cpp \
    $$PWD/orm/connectors/mysqlconnector.cpp \
    $$PWD/orm/connectors/sqliteconnector.cpp \
    $$PWD/orm/databaseconnection.cpp \
    $$PWD/orm/databasemanager.cpp \
    $$PWD/orm/db.cpp \
    $$PWD/orm/logquery.cpp \
    $$PWD/orm/mysqlconnection.cpp \
    $$PWD/orm/ormtypes.cpp \
    $$PWD/orm/query/expression.cpp \
    $$PWD/orm/query/grammars/grammar.cpp \
    $$PWD/orm/query/grammars/mysqlgrammar.cpp \
    $$PWD/orm/query/grammars/sqlitegrammar.cpp \
    $$PWD/orm/query/joinclause.cpp \
    $$PWD/orm/query/querybuilder.cpp \
    $$PWD/orm/queryerror.cpp \
    $$PWD/orm/sqlerror.cpp \
    $$PWD/orm/sqliteconnection.cpp \
    $$PWD/orm/support/configurationoptionsparser.cpp \
    $$PWD/orm/tiny/basemodel.cpp \
    $$PWD/orm/tiny/modelnotfounderror.cpp \
    $$PWD/orm/tiny/relationnotfounderror.cpp \
    $$PWD/orm/tiny/relationnotloadederror.cpp \
    $$PWD/orm/tiny/relations/relation.cpp \
    $$PWD/orm/utils/attribute.cpp \
    $$PWD/orm/utils/string.cpp \
