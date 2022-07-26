#include <QCoreApplication>
#include <QtSql/QSqlDriver>
#include <QtTest>

#include <typeinfo>

#include "orm/db.hpp"
#include "orm/query/querybuilder.hpp"

#include "databases.hpp"

using Orm::Constants::ASTERISK;
using Orm::Constants::COMMA;
using Orm::Constants::GT;
using Orm::Constants::ID;
using Orm::Constants::LT;
using Orm::Constants::NAME;
using Orm::Constants::SIZE;

using Orm::DB;
using Orm::Query::Builder;

using QueryBuilder = Orm::Query::Builder;

using TestUtils::Databases;

class tst_QueryBuilder : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase_data() const;

    void find() const;

    void findOr() const;
    void findOr_WithReturnType() const;

    void pluck() const;
    void pluck_EmptyResult() const;
    void pluck_QualifiedColumnOrKey() const;

    void implode() const;
    void implode_EmptyResult() const;
    void implode_QualifiedColumnOrKey() const;

    void updateOrInsert() const;
    void updateOrInsert_EmptyValues() const;

    void count() const;
    void count_Distinct() const;
    void min_Aggregate() const;
    void sum_Aggregate() const;
    void sum_Aggregate_ShouldReturnZeroInsteadOfNull() const;

    void exists() const;
    void doesntExist() const;

    void existsOr() const;
    void doesntExistOr() const;
    void existsOr_WithReturnType() const;
    void doesntExistOr_WithReturnType() const;

    void limit() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Create QueryBuilder instance for the given connection. */
    [[nodiscard]] std::shared_ptr<QueryBuilder>
    createQuery(const QString &connection) const;
};

/* private slots */

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

void tst_QueryBuilder::findOr() const
{
    QFETCH_GLOBAL(QString, connection);

    // Callback invoked
    {
        auto builder = createQuery(connection);

        auto callbackInvoked = false;
        auto query = builder->from("torrents")
                     .findOr(100, {ASTERISK}, [&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(!query.isValid());
        QVERIFY(callbackInvoked);
    }
    // Callback invoked (second overload)
    {
        auto builder = createQuery(connection);

        auto callbackInvoked = false;
        auto query = builder->from("torrents").findOr(100, [&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(!query.isValid());
        QVERIFY(callbackInvoked);
    }
    // Callback not invoked
    {
        auto builder = createQuery(connection);

        auto callbackInvoked = false;
        auto query = builder->from("torrents").findOr(1, [&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(query.isValid());
        QVERIFY(!callbackInvoked);
    }
}

void tst_QueryBuilder::findOr_WithReturnType() const
{
    QFETCH_GLOBAL(QString, connection);

    // Callback invoked
    {
        auto builder = createQuery(connection);

        auto [query, result] = builder->from("torrents").findOr<int>(100, []()
        {
            return 1;
        });

        QVERIFY(!query.isValid());
        QCOMPARE(result, 1);
    }
    // Callback invoked (second overload)
    {
        auto builder = createQuery(connection);

        auto [query, result] = builder->from("torrents")
                               .findOr<int>(100, {ID, NAME}, []()
        {
            return 1;
        });

        QVERIFY(!query.isValid());
        QCOMPARE(result, 1);
    }
    // Callback not invoked
    {
        auto builder = createQuery(connection);

        auto [query, result] = builder->from("torrents").findOr<int>(1, []()
        {
            return 1;
        });

        QVERIFY(query.isValid());
        QCOMPARE(result, 0);
    }
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

        auto result = builder->from("torrents").pluck<QString>(SIZE, NAME);

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

void tst_QueryBuilder::implode() const
{
    QFETCH_GLOBAL(QString, connection);

    // Simple implode without the glue
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents").orderBy(NAME).implode(NAME);

        QCOMPARE(result, QString {"test1test2test3test4test5test6"});
    }
    // implode with the ', ' glue
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents").orderBy(NAME).implode(NAME, COMMA);

        QCOMPARE(result, QString {"test1, test2, test3, test4, test5, test6"});
    }
}

void tst_QueryBuilder::implode_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection);

    // Simple implode without the glue
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents")
                      .whereEq(NAME, "DUMMY_RECORD").orderBy(NAME)
                      .implode(NAME);

        QCOMPARE(result, QString());
    }
    // implode with the ', ' glue
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents")
                      .whereEq(NAME, "DUMMY_RECORD").orderBy(NAME)
                      .implode(NAME, COMMA);

        QCOMPARE(result, QString());
    }
}

void tst_QueryBuilder::implode_QualifiedColumnOrKey() const
{
    QFETCH_GLOBAL(QString, connection);

    // Simple implode without the glue
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents").orderBy(NAME).implode("torrents.name");

        QCOMPARE(result, QString {"test1test2test3test4test5test6"});
    }
    // implode with the ', ' glue
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents").orderBy(NAME)
                      .implode("torrents.name", COMMA);

        QCOMPARE(result, QString {"test1, test2, test3, test4, test5, test6"});
    }
}

void tst_QueryBuilder::updateOrInsert() const
{
    QFETCH_GLOBAL(QString, connection);

    // update
    {
        auto [affected, query] = createQuery(connection)->from("user_phones").
                                 updateOrInsert(
                                     {{"user_id", 3}, {"number", "905111999"}},
                                     {{"number", "905333999"}});

        QVERIFY(query);
        QVERIFY(!query->isValid() && !query->isSelect() && query->isActive());
        QCOMPARE(affected, 1);

        // validate
        auto count = createQuery(connection)->from("user_phones")
                     .whereEq("user_id", 3)
                     .count();
        QCOMPARE(count, 1);
        auto number = createQuery(connection)->from("user_phones")
                      .whereEq("user_id", 3)
                      .value("number");
        QCOMPARE(number, QString("905333999"));
    }

    // remove
    {
        auto [affected, query] = createQuery(connection)->from("user_phones")
                                 .whereEq("user_id", 3)
                                 .remove();

        QVERIFY(!query.isValid() && !query.isSelect() && query.isActive());
        QCOMPARE(affected, 1);

        // validate
        QVERIFY(createQuery(connection)->from("user_phones")
                .whereEq("user_id", 3)
                .doesntExist());
    }

    // insert (also restore db)
    {
        auto [affected, query] = createQuery(connection)->from("user_phones").
                                 updateOrInsert(
                                     {{"user_id", 3}, {"number", "905000000"}},
                                     {{"number", "905111999"}});

        QVERIFY(query);
        QVERIFY(!query->isValid() && !query->isSelect() && query->isActive());
        QCOMPARE(affected, -1);

        // validate
        auto count = createQuery(connection)->from("user_phones")
                     .whereEq("user_id", 3)
                     .count();
        QCOMPARE(count, 1);
        auto number = createQuery(connection)->from("user_phones")
                      .whereEq("user_id", 3)
                      .value("number");
        QCOMPARE(number, QString("905111999"));
    }
}

void tst_QueryBuilder::updateOrInsert_EmptyValues() const
{
    QFETCH_GLOBAL(QString, connection);

    // validate
    auto count = createQuery(connection)->from("user_phones")
                 .whereEq("user_id", 3)
                 .count();
    QCOMPARE(count, 1);

    // Get a current ID, it can not change
    auto expectedId = createQuery(connection)->from("user_phones")
                      .whereEq("user_id", 3)
                      .value(ID).value<quint64>();
    QVERIFY(expectedId >= 3);

    // main operation
    auto [affected, query] = createQuery(connection)->from("user_phones").
                             updateOrInsert(
                                 {{"user_id", 3}, {"number", "905111999"}},
                                 {});

    QVERIFY(!query);
    QCOMPARE(affected, 0);

    // validate
    count = createQuery(connection)->from("user_phones")
            .whereEq("user_id", 3)
            .count();
    QCOMPARE(count, 1);

    // ID is still the same
    auto id = createQuery(connection)->from("user_phones")
              .whereEq("user_id", 3)
              .value(ID).value<quint64>();
    QCOMPARE(id, expectedId);
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

void tst_QueryBuilder::exists() const
{
    QFETCH_GLOBAL(QString, connection);

    auto builder = createQuery(connection);

    auto result = builder->select("*").from("torrents").where(ID, LT, 5).exists();

    QVERIFY(result);
}

void tst_QueryBuilder::doesntExist() const
{
    QFETCH_GLOBAL(QString, connection);

    auto builder = createQuery(connection);

    auto result = builder->select("*").from("torrents").where(ID, GT, 100).doesntExist();

    QVERIFY(result);
}

void tst_QueryBuilder::existsOr() const
{
    QFETCH_GLOBAL(QString, connection);

    // Callback invoked
    {
        auto builder = createQuery(connection);

        auto callbackInvoked = false;
        auto result = builder->select("*").from("torrents").where(ID, GT, 100)
                      .existsOr([&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(!result);
    }
    // Callback not invoked
    {
        auto builder = createQuery(connection);

        auto callbackInvoked = false;
        auto result = builder->select("*").from("torrents").where(ID, LT, 5)
                      .existsOr([&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(result);
    }
}

void tst_QueryBuilder::doesntExistOr() const
{
    QFETCH_GLOBAL(QString, connection);

    // Callback invoked
    {
        auto builder = createQuery(connection);

        auto callbackInvoked = false;
        auto result = builder->select("*").from("torrents").where(ID, LT, 5)
                      .doesntExistOr([&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(!result);
    }
    // Callback not invoked
    {
        auto builder = createQuery(connection);

        auto callbackInvoked = false;
        auto result = builder->select("*").from("torrents").where(ID, GT, 100)
                      .doesntExistOr([&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(result);
    }
}

void tst_QueryBuilder::existsOr_WithReturnType() const
{
    QFETCH_GLOBAL(QString, connection);

    // Callback invoked
    {
        auto builder = createQuery(connection);

        auto [result, returnValue] = builder->select("*").from("torrents")
                                     .where(ID, GT, 100)
                                     .existsOr<int>([]()
        {
            return 1;
        });

        QVERIFY(!result);
        QCOMPARE(returnValue, 1);
    }
    // Callback not invoked
    {
        auto builder = createQuery(connection);

        auto [result, returnValue] = builder->select("*").from("torrents")
                                     .where(ID, LT, 5)
                                     .existsOr<int>([]()
        {
            return 1;
        });

        QVERIFY(result);
        QCOMPARE(returnValue, 0);
    }
}

void tst_QueryBuilder::doesntExistOr_WithReturnType() const
{
    QFETCH_GLOBAL(QString, connection);

    // Callback invoked
    {
        auto builder = createQuery(connection);

        auto [result, returnValue] = builder->select("*").from("torrents")
                                     .where(ID, LT, 5)
                                     .doesntExistOr<int>([]()
        {
            return 1;
        });

        QVERIFY(!result);
        QCOMPARE(returnValue, 1);
    }
    // Callback not invoked
    {
        auto builder = createQuery(connection);

        auto [result, returnValue] = builder->select("*").from("torrents")
                                     .where(ID, GT, 100)
                                     .doesntExistOr<int>([]()
        {
            return 1;
        });

        QVERIFY(result);
        QCOMPARE(returnValue, 0);
    }
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

/* private */

std::shared_ptr<QueryBuilder>
tst_QueryBuilder::createQuery(const QString &connection) const
{
    return DB::connection(connection).query();
}

QTEST_MAIN(tst_QueryBuilder)

#include "tst_querybuilder.moc"
