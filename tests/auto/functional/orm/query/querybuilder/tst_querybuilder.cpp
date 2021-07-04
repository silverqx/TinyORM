#include <QCoreApplication>
#include <QtSql/QSqlDriver>
#include <QtTest>

#include "orm/db.hpp"
#include "orm/query/querybuilder.hpp"

#include "databases.hpp"

using QueryBuilder = Orm::Query::Builder;

using TestUtils::Databases;

class tst_QueryBuilder : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase_data() const;

    void find() const;

    void count() const;
    void count_Distinct() const;
    void min_Aggregate() const;
    void sum_Aggregate() const;
    void sum_Aggregate_ShouldReturnZeroInsteadOfNull() const;

    void limit() const;

private:
    /*! Create QueryBuilder instance for the given connection. */
    inline QSharedPointer<QueryBuilder>
    createQuery(const QString &connection) const
    { return DB::connection(connection).query(); }
};

void tst_QueryBuilder::initTestCase_data() const
{
    const auto &connections = Databases::createConnections();

    if (connections.isEmpty())
        QSKIP(QStringLiteral("%1 autotest skipped, environment variables "
                             "for ANY connection have not been defined.")
              .arg("tst_QueryBuilder").toUtf8().constData(), );

    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : connections)
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

void tst_QueryBuilder::find() const
{
    QFETCH_GLOBAL(QString, connection);

    auto builder = createQuery(connection);

    auto query = builder->from("torrents").find(2);

    QCOMPARE(query.value("id"), QVariant(2));
    QCOMPARE(query.value("name"), QVariant("test2"));
}

void tst_QueryBuilder::count() const
{
    QFETCH_GLOBAL(QString, connection);

    {
        auto builder = createQuery(connection);

        auto count = builder->from("torrent_peers").count();

        QCOMPARE(typeid (quint64), typeid (count));
        QCOMPARE(count, static_cast<quint64>(4));
    }

    // Should exclude columns with NULL values
    {
        auto builder = createQuery(connection);

        auto count = builder->from("torrent_peers").count("seeds");

        QCOMPARE(typeid (quint64), typeid (count));
        QCOMPARE(count, static_cast<quint64>(3));
    }
}

void tst_QueryBuilder::count_Distinct() const
{
    QFETCH_GLOBAL(QString, connection);

    auto builder = createQuery(connection);

    auto count = builder->from("file_property_properties")
                 .distinct()
                 .count("file_property_id");

    QCOMPARE(typeid (quint64), typeid (count));
    QCOMPARE(count, static_cast<quint64>(5));
}

void tst_QueryBuilder::min_Aggregate() const
{
    QFETCH_GLOBAL(QString, connection);

    auto builder = createQuery(connection);

    auto count = builder->from("torrent_peers").min("total_seeds");

    QCOMPARE(count, QVariant(1));
}

void tst_QueryBuilder::sum_Aggregate() const
{
    QFETCH_GLOBAL(QString, connection);

    auto builder = createQuery(connection);

    auto sum = builder->from("torrent_peers").sum("total_seeds");

    QCOMPARE(sum, QVariant(10));
}

void tst_QueryBuilder::sum_Aggregate_ShouldReturnZeroInsteadOfNull() const
{
    QFETCH_GLOBAL(QString, connection);

    auto builder = createQuery(connection);

    auto sum = builder->from("torrent_peers").whereEq("id", 4).sum("seeds");

    QCOMPARE(sum, QVariant(0));
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
        auto query = builder->from("torrents").limit(1).get({"id"});

        QCOMPARE(query.size(), 1);
    }

    {
        auto query = builder->from("torrents").limit(3).get({"id"});

        QCOMPARE(query.size(), 3);
    }

    {
        auto query = builder->from("torrents").limit(4).get({"id"});

        QCOMPARE(query.size(), 4);
    }
}

QTEST_MAIN(tst_QueryBuilder)

#include "tst_querybuilder.moc"
