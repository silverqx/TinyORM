#include <QCoreApplication>
#include <QtTest>

#include "orm/databaseconnection.hpp"

#include "database.hpp"

class tst_QueryBuilder : public QObject
{
    Q_OBJECT

public:
    tst_QueryBuilder();
    ~tst_QueryBuilder() = default;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void test_case1();

private:
    Orm::DatabaseConnection &m_db;
};

tst_QueryBuilder::tst_QueryBuilder()
    : m_db(Utils::Database::createConnection())
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

QTEST_MAIN(tst_QueryBuilder)

#include "tst_querybuilder.moc"
