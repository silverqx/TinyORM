#include <QCoreApplication>
#include <QtSql/QSqlDriver>
#include <QtTest>

#include "orm/db.hpp"
#include "orm/query/querybuilder.hpp"

#include "databases.hpp"

using namespace Orm::Constants;

using QueryBuilder = Orm::Query::Builder;

using TestUtils::Databases;

class tst_QueryBuilder : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase_data() const;

    void find() const;

    void pluck() const;
    void pluck_EmptyResult() const;
    void pluck_QualifiedColumnOrKey() const;

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

    QCOMPARE(query.value(ID), QVariant(2));
    QCOMPARE(query.value(NAME), QVariant("test2"));
}

void tst_QueryBuilder::pluck() const
{
    QFETCH_GLOBAL(QString, connection);

    // Simple pluck without keying
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents").orderBy(NAME).pluck(NAME);

        QVector<QVariant> expected {
            "test1", "test2", "test3", "test4", "test5", "test6",
        };
        QCOMPARE(result, expected);
    }
    // Templated pluck keyed by id<quint64>
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents").pluck<quint64>(NAME, ID);

        std::map<quint64, QVariant> expected {
            {1, "test1"}, {2, "test2"}, {3, "test3"}, {4, "test4"},
            {5, "test5"}, {6, "test6"},
        };
        QCOMPARE(result, expected);
    }
    // Templated pluck keyed by name<QString>
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents").pluck<QString>("size", NAME);

        std::map<QString, QVariant> expected {
            {"test1", 11}, {"test2", 12}, {"test3", 13}, {"test4", 14},
            {"test5", 15}, {"test6", 16},
        };
        QCOMPARE(result, expected);
    }
    // Templated pluck keyed by file_index<bool>, bool type is used intentionally 😎
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrent_previewable_files").orderBy(ID)
                      .pluck<bool>("filepath", "file_index");

        std::map<bool, QVariant> expected {
            {false, "test1_file1.mkv"}, {true, "test2_file2.mkv"},
        };
        QCOMPARE(result, expected);
    }
}

void tst_QueryBuilder::pluck_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection);

    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents")
                      .whereEq(NAME, "DUMMY_RECORD").pluck(NAME);

        QCOMPARE(result, QVector<QVariant>());
    }
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents")
                      .whereEq(NAME, "DUMMY_RECORD").pluck<quint64>(NAME, ID);

        std::map<quint64, QVariant> expected;
        QCOMPARE(result, expected);
    }
}

void tst_QueryBuilder::pluck_QualifiedColumnOrKey() const
{
    QFETCH_GLOBAL(QString, connection);

    // Strip table name
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents").orderBy(NAME).pluck("torrents.name");

        QVector<QVariant> expected {
            "test1", "test2", "test3", "test4", "test5", "test6",
        };
        QCOMPARE(result, expected);
    }
    // Strip table name
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents").pluck<quint64>("torrents.name",
                                                               "torrents.id");

        std::map<quint64, QVariant> expected {
            {1, "test1"}, {2, "test2"}, {3, "test3"}, {4, "test4"},
            {5, "test5"}, {6, "test6"},
        };
        QCOMPARE(result, expected);
    }
    // Strip column alias
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents")
                      .orderBy("name_alt").pluck("name as name_alt");

        QVector<QVariant> expected {
            "test1", "test2", "test3", "test4", "test5", "test6",
        };
        QCOMPARE(result, expected);
    }
    // Strip column alias
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents")
                      .pluck<quint64>("name as name_alt", "id as id_alt");

        std::map<quint64, QVariant> expected {
            {1, "test1"}, {2, "test2"}, {3, "test3"}, {4, "test4"},
            {5, "test5"}, {6, "test6"},
        };
        QCOMPARE(result, expected);
    }
    // Strip column alias and table name
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents")
                      .pluck<quint64>("torrents.name", "id as id_alt");

        std::map<quint64, QVariant> expected {
            {1, "test1"}, {2, "test2"}, {3, "test3"}, {4, "test4"},
            {5, "test5"}, {6, "test6"},
        };
        QCOMPARE(result, expected);
    }
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

    auto sum = builder->from("torrent_peers").whereEq(ID, 4).sum("seeds");

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
        auto query = builder->from("torrents").limit(1).get({ID});

        QCOMPARE(query.size(), 1);
    }

    {
        auto query = builder->from("torrents").limit(3).get({ID});

        QCOMPARE(query.size(), 3);
    }

    {
        auto query = builder->from("torrents").limit(4).get({ID});

        QCOMPARE(query.size(), 4);
    }
}

QTEST_MAIN(tst_QueryBuilder)

#include "tst_querybuilder.moc"
