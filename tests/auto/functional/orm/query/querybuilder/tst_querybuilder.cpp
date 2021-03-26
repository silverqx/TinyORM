#include <QCoreApplication>
#include <QtSql/QSqlDriver>
#include <QtTest>

#include "orm/db.hpp"
#include "orm/query/querybuilder.hpp"

#include "database.hpp"

using QueryBuilder = Orm::Query::Builder;

class tst_QueryBuilder : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase_data() const;

    void find() const;
    void limit() const;

private:
    /*! Create QueryBuilder instance for the given connection. */
    inline QSharedPointer<QueryBuilder>
    createQuery(const QString &connection) const
    { return DB::connection(connection).query(); }
};

void tst_QueryBuilder::initTestCase_data() const
{
    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : TestUtils::Database::createConnections())
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

void tst_QueryBuilder::find() const
{
    QFETCH_GLOBAL(QString, connection);

    auto builder = createQuery(connection);

    auto [ok, query] = builder->from("torrents").find(2);

    if (!ok)
        QFAIL("find() query failed.");

    QCOMPARE(query.value("id"), QVariant(2));
    QCOMPARE(query.value("name"), QVariant("test2"));
}

void tst_QueryBuilder::limit() const
{
    QFETCH_GLOBAL(QString, connection);

    if (const auto qtConnection = QSqlDatabase::database(connection);
        !qtConnection.driver()->hasFeature(QSqlDriver::QuerySize)
    )
        // ", " to prevent warning about variadic macro
        QSKIP(QStringLiteral("'%1' driver doesn't support reporting the size "
                             "of a query.")
              .arg(qtConnection.driverName()).toUtf8().constData(), );

    auto builder = createQuery(connection);

    {
        auto [ok, query] = builder->from("torrents").limit(1).get({"id"});

        if (!ok)
            QFAIL("limit(1) query failed.");

        QCOMPARE(query.size(), 1);
    }

    {
        auto [ok, query] = builder->from("torrents").limit(3).get({"id"});

        if (!ok)
            QFAIL("limit(3) query failed.");

        QCOMPARE(query.size(), 3);
    }

    {
        auto [ok, query] = builder->from("torrents").limit(4).get({"id"});

        if (!ok)
            QFAIL("limit(4) query failed.");

        QCOMPARE(query.size(), 4);
    }
}

QTEST_MAIN(tst_QueryBuilder)

#include "tst_querybuilder.moc"
