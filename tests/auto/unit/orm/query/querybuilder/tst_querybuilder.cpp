#include <QtTest>

#include "orm/databaseconnection.hpp"
#include "orm/grammar.hpp"
#include "orm/query/querybuilder.hpp"

class tst_QueryBuilder : public QObject
{
    Q_OBJECT

public:
    tst_QueryBuilder();
    ~tst_QueryBuilder();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void setDistinct();
    void setTable();
    void setFrom();
};

tst_QueryBuilder::tst_QueryBuilder()
{

}

tst_QueryBuilder::~tst_QueryBuilder()
{

}

void tst_QueryBuilder::initTestCase()
{

}

void tst_QueryBuilder::cleanupTestCase()
{

}

void tst_QueryBuilder::setDistinct()
{
    Orm::QueryBuilder builder(Orm::DatabaseConnection::instance(),
                              Orm::Grammar());

    auto distinct = builder.getDistinct();
    QCOMPARE(distinct, false);

    builder.distinct();
    distinct = builder.getDistinct();

    QCOMPARE(distinct, true);
}

void tst_QueryBuilder::setTable()
{
    Orm::QueryBuilder builder(Orm::DatabaseConnection::instance(),
                              Orm::Grammar());

    auto table = builder.getTable();
    auto tableFrom = builder.getFrom();
    const auto &expected = QStringLiteral("");
    QCOMPARE(table, expected);
    QCOMPARE(tableFrom, expected);

    const auto &newTableName = QStringLiteral("table_name");
    builder.table(newTableName);

    table = builder.getTable();
    tableFrom = builder.getFrom();

    QCOMPARE(table, newTableName);
    QCOMPARE(tableFrom, newTableName);
}

void tst_QueryBuilder::setFrom()
{
    Orm::QueryBuilder builder(Orm::DatabaseConnection::instance(),
                              Orm::Grammar());

    auto table = builder.getTable();
    auto tableFrom = builder.getFrom();
    const auto &expected = QStringLiteral("");
    QCOMPARE(table, expected);
    QCOMPARE(tableFrom, expected);

    const auto &newTableName = QStringLiteral("table_name");
    builder.from(newTableName);

    table = builder.getTable();
    tableFrom = builder.getFrom();

    QCOMPARE(table, newTableName);
    QCOMPARE(tableFrom, newTableName);
}

QTEST_APPLESS_MAIN(tst_QueryBuilder)

#include "tst_querybuilder.moc"
