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

    void test_case1();
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

void tst_QueryBuilder::test_case1()
{
}

QTEST_APPLESS_MAIN(tst_QueryBuilder)

#include "tst_querybuilder.moc"
