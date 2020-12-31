#include <QCoreApplication>
#include <QtTest>

#include "orm/databaseconnection.hpp"
#include "orm/grammar.hpp"
#include "orm/query/querybuilder.hpp"

#include "database.hpp"

using namespace Orm;

class tst_QueryBuilder : public QObject
{
    Q_OBJECT

public:
    tst_QueryBuilder();
    ~tst_QueryBuilder() = default;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void find() const;
    void limit() const;

private:
    inline QueryBuilder createQuery() const
    { return QueryBuilder(m_db, Grammar()); }

    DatabaseConnection &m_db;
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

void tst_QueryBuilder::find() const
{
    auto builder = createQuery();

    auto [ok, query] = builder.from("torrents").find(2);

    if (!ok)
        QFAIL("find() query failed.");

    QCOMPARE(query.value("id").toULongLong(), 2);
    QCOMPARE(query.value("name").toString(), QString("test2"));
}

void tst_QueryBuilder::limit() const
{
    auto builder = createQuery();

    {
        auto [ok, query] = builder.from("torrents").limit(1).get({"id"});

        if (!ok)
            QFAIL("limit(1) query failed.");

        QCOMPARE(query.size(), 1);
    }

    {
        auto [ok, query] = builder.from("torrents").limit(3).get({"id"});

        if (!ok)
            QFAIL("limit(3) query failed.");

        QCOMPARE(query.size(), 3);
    }

    {
        auto [ok, query] = builder.from("torrents").limit(4).get({"id"});

        if (!ok)
            QFAIL("limit(4) query failed.");

        QCOMPARE(query.size(), 4);
    }
}

QTEST_MAIN(tst_QueryBuilder)

#include "tst_querybuilder.moc"
