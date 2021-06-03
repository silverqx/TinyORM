#include <QCoreApplication>
#include <QtSql/QSqlDriver>
#include <QtTest>

#include "orm/db.hpp"
#include "orm/query/querybuilder.hpp"

#include "database.hpp"

using QueryBuilder = Orm::Query::Builder;

using TestUtils::Database;

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
    const auto &connections = Database::createConnections();

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
