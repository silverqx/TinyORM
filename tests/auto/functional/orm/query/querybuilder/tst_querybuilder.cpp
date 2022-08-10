#include <QCoreApplication>
#include <QtSql/QSqlDriver>
#include <QtTest>

#include <typeinfo>

#include "orm/db.hpp"
#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/query/querybuilder.hpp"

#include "databases.hpp"

using Orm::Constants::ASTERISK;
using Orm::Constants::COMMA;
using Orm::Constants::GT;
using Orm::Constants::ID;
using Orm::Constants::LT;
using Orm::Constants::NAME;
using Orm::Constants::OR;
using Orm::Constants::QMYSQL;
using Orm::Constants::SIZE;

using Orm::DB;
using Orm::Exceptions::InvalidArgumentError;
using Orm::Exceptions::RuntimeError;
using Orm::Query::Builder;

using QueryBuilder = Orm::Query::Builder;
using QueryUtils = Orm::Utils::Query;

using TestUtils::Databases;

class tst_QueryBuilder : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase_data() const;

    void find() const;

    void findOr() const;
    void findOr_WithReturnType() const;

    void first() const;

    void pluck() const;
    void pluck_EmptyResult() const;
    void pluck_QualifiedColumnOrKey() const;

    void implode() const;
    void implode_EmptyResult() const;
    void implode_QualifiedColumnOrKey() const;

    void updateOrInsert() const;
    void updateOrInsert_EmptyValues() const;

    void upsert() const;
    void upsert_EmptyValues() const;
    void upsert_EmptyUpdate() const;
    void upsert_WithoutUpdate_UpdateAll() const;

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

    /* Builds Queries */
    void chunk() const;
    void chunk_ReturnFalse() const;
    void chunk_EnforceOrderBy() const;
    void chunk_EmptyResult() const;

    void each() const;
    void each_ReturnFalse() const;
    void each_EnforceOrderBy() const;
    void each_EmptyResult() const;

    void chunkById() const;
    void chunkById_ReturnFalse() const;
    void chunkById_EmptyResult() const;

    void chunkById_WithAlias() const;
    void chunkById_ReturnFalse_WithAlias() const;
    void chunkById_EmptyResult_WithAlias() const;

    void eachById() const;
    void eachById_ReturnFalse() const;
    void eachById_EmptyResult() const;

    void eachById_WithAlias() const;
    void eachById_ReturnFalse_WithAlias() const;
    void eachById_EmptyResult_WithAlias() const;

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
        QCOMPARE(query.value(ID), QVariant(1));
        QCOMPARE(query.value(NAME), QVariant("test1"));
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
        QCOMPARE(query.value(ID), QVariant(1));
        QCOMPARE(query.value(NAME), QVariant("test1"));
    }
}

void tst_QueryBuilder::first() const
{
    QFETCH_GLOBAL(QString, connection);

    auto builder = createQuery(connection);

    auto query = builder->from("torrents").whereEq(ID, 2).first();

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
                      .whereEq(NAME, "dummy-NON_EXISTENT").pluck(NAME);

        QCOMPARE(result, QVector<QVariant>());
    }
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents")
                      .whereEq(NAME, "dummy-NON_EXISTENT").pluck<quint64>(NAME, ID);

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
                      .whereEq(NAME, "dummy-NON_EXISTENT").orderBy(NAME)
                      .implode(NAME);

        QCOMPARE(result, QString());
    }
    // implode with the ', ' glue
    {
        auto builder = createQuery(connection);

        auto result = builder->from("torrents")
                      .whereEq(NAME, "dummy-NON_EXISTENT").orderBy(NAME)
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
        auto [affected, query] = createQuery(connection)->from("user_phones")
                                 .updateOrInsert(
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

    // remove before insert test
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
        auto [affected, query] = createQuery(connection)->from("user_phones")
                                 .updateOrInsert(
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
    auto [affected, query] = createQuery(connection)->from("user_phones")
                             .updateOrInsert(
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

void tst_QueryBuilder::upsert() const
{
    QFETCH_GLOBAL(QString, connection);

    // Should update one row (color column) and insert one row
    {
        auto [affected, query] =
                createQuery(connection)->from("tag_properties")
                .upsert({{{"tag_id", 1}, {"color", "pink"},   {"position", 0}},
                         {{"tag_id", 1}, {"color", "purple"}, {"position", 4}}},
                        {"position"},
                        {"color"});

        QVERIFY(query);
        QVERIFY(!query->isValid() && !query->isSelect() && query->isActive());
        if (const auto driverName = DB::driverName(connection);
            driverName == QMYSQL
        )
            /* For MySQL the affected-rows value per row is 1 if the row is inserted
               as a new row, 2 if an existing row is updated, and 0 if an existing row
               is set to its current values. */
            QCOMPARE(affected, 3);
        else
            QCOMPARE(affected, 2);

        // Validate one update and one insert
        auto validateQuery = createQuery(connection)->from("tag_properties")
                             .whereEq("tag_id", 1)
                             .orderBy("position")
                             .get();

        QVERIFY(validateQuery.isSelect() && validateQuery.isActive());
        auto validateQuerySize = QueryUtils::queryResultSize(validateQuery);
        QCOMPARE(validateQuerySize, 2);

        QVector<QVector<QVariant>> result;
        result.reserve(validateQuerySize);

        while (validateQuery.next()) {
            QVERIFY(validateQuery.isValid());
            result.append({validateQuery.value("color"),
                           validateQuery.value("position"),
                           validateQuery.value("tag_id")});
        }

        QVector<QVector<QVariant>> expextedResult {
            {"pink",   0, 1},
            {"purple", 4, 1},
        };

        QCOMPARE(result, expextedResult);
    }

    // Restore db
    {
        auto [affected, query] = createQuery(connection)->from("tag_properties")
                                 .whereEq("position", 4)
                                 .remove();

        QVERIFY(!query.isValid() && !query.isSelect() && query.isActive());
        QCOMPARE(affected, 1);
    }
    {
        auto [affected, query] = createQuery(connection)->from("tag_properties")
                                 .whereEq("id", 1)
                                 .update({{"color", "white"}});

        QVERIFY(!query.isValid() && !query.isSelect() && query.isActive());
        QCOMPARE(affected, 1);
    }

    // Validate restored db
    {
        auto validateQuery = createQuery(connection)->from("tag_properties")
                             .whereEq("tag_id", 1)
                             .orderBy("position")
                             .get();

        QVERIFY(validateQuery.isSelect() && validateQuery.isActive());
        QCOMPARE(QueryUtils::queryResultSize(validateQuery), 1);
        QVERIFY(validateQuery.first());
        QVERIFY(validateQuery.isValid());
        QCOMPARE(validateQuery.value("color"), QVariant(QString("white")));
        QCOMPARE(validateQuery.value("position").value<int>(), 0);
    }
}

void tst_QueryBuilder::upsert_EmptyValues() const
{
    QFETCH_GLOBAL(QString, connection);

    // Should just return with no DB operations
    {
        auto [affected, query] =
                createQuery(connection)->from("tag_properties")
                .upsert({}, {"position"}, {"color"});

        QVERIFY(!query);
        QCOMPARE(affected, 0);
    }
}

void tst_QueryBuilder::upsert_EmptyUpdate() const
{
    QFETCH_GLOBAL(QString, connection);

    QVERIFY_EXCEPTION_THROWN(
                createQuery(connection)->from("tag_properties")
                .upsert({{{"tag_id", 1}, {"color", "pink"},   {"position", 0}},
                         {{"tag_id", 1}, {"color", "purple"}, {"position", 4}}},
                        {"position"}, {}),
                InvalidArgumentError);
}

void tst_QueryBuilder::upsert_WithoutUpdate_UpdateAll() const
{
    QFETCH_GLOBAL(QString, connection);

    // Should update one row (all columns) and insert one row
    {
        auto [affected, query] =
                createQuery(connection)->from("tag_properties")
                .upsert({{{"tag_id", 2}, {"color", "pink"},   {"position", 0}},
                         {{"tag_id", 1}, {"color", "purple"}, {"position", 4}}},
                        {"position"});

        QVERIFY(query);
        QVERIFY(!query->isValid() && !query->isSelect() && query->isActive());
        if (const auto driverName = DB::driverName(connection);
            driverName == QMYSQL
        )
            /* For MySQL the affected-rows value per row is 1 if the row is inserted
               as a new row, 2 if an existing row is updated, and 0 if an existing row
               is set to its current values. */
            QCOMPARE(affected, 3);
        else
            QCOMPARE(affected, 2);

        // Validate one update and one insert
        auto validateQuery = createQuery(connection)->from("tag_properties")
                             .where({{"position", 0}, {"position", 4}}, OR)
                             .orderBy("position")
                             .get();

        QVERIFY(validateQuery.isSelect() && validateQuery.isActive());
        auto validateQuerySize = QueryUtils::queryResultSize(validateQuery);
        QCOMPARE(validateQuerySize, 2);

        QVector<QVector<QVariant>> result;
        result.reserve(validateQuerySize);

        while (validateQuery.next()) {
            QVERIFY(validateQuery.isValid());
            result.append({validateQuery.value("color"),
                           validateQuery.value("position"),
                           validateQuery.value("tag_id")});
        }

        QVector<QVector<QVariant>> expextedResult {
            {"pink",   0, 2},
            {"purple", 4, 1},
        };

        QCOMPARE(result, expextedResult);
    }

    // Restore db
    {
        auto [affected, query] = createQuery(connection)->from("tag_properties")
                                 .whereEq("position", 4)
                                 .remove();

        QVERIFY(!query.isValid() && !query.isSelect() && query.isActive());
        QCOMPARE(affected, 1);
    }
    {
        auto [affected, query] = createQuery(connection)->from("tag_properties")
                                 .whereEq("position", 0)
                                 .update({{"color", "white"}, {"tag_id", 1}});

        QVERIFY(!query.isValid() && !query.isSelect() && query.isActive());
        QCOMPARE(affected, 1);
    }

    // Validate restored db
    {
        auto validateQuery = createQuery(connection)->from("tag_properties")
                             .whereEq("tag_id", 1)
                             .orderBy("position")
                             .get();

        QVERIFY(validateQuery.isSelect() && validateQuery.isActive());
        QCOMPARE(QueryUtils::queryResultSize(validateQuery), 1);
        QVERIFY(validateQuery.first());
        QVERIFY(validateQuery.isValid());
        QCOMPARE(validateQuery.value("color"), QVariant(QString("white")));
        QCOMPARE(validateQuery.value("position").value<int>(), 0);
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

    auto builder = createQuery(connection);

    {
        auto query = builder->from("torrents").limit(1).get({ID});

        QCOMPARE(QueryUtils::queryResultSize(query), 1);
    }

    {
        auto query = builder->from("torrents").limit(3).get({ID});

        QCOMPARE(QueryUtils::queryResultSize(query), 3);
    }

    {
        auto query = builder->from("torrents").limit(4).get({ID});

        QCOMPARE(QueryUtils::queryResultSize(query), 4);
    }
}

/* Builds Queries */

void tst_QueryBuilder::chunk() const
{
    QFETCH_GLOBAL(QString, connection);

    // <page, chunk_rowsCount>
    const std::unordered_map<int, int> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows](QSqlQuery &query, const int page)
    {
        QCOMPARE(QueryUtils::queryResultSize(query), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = createQuery(connection)->from("file_property_properties")
                  .orderBy(ID)
                  .chunk(3, [&compareResultSize, &ids](QSqlQuery &query, const int page)
    {
        compareResultSize(query, page);

        while (query.next())
            ids.emplace_back(query.value(ID).value<quint64>());

        return true;
    });

    QVERIFY(result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_QueryBuilder::chunk_ReturnFalse() const
{
    QFETCH_GLOBAL(QString, connection);

    // <page, chunk_rowsCount> (I leave it here also in this test, doesn't matter much
    const std::unordered_map<int, int> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows](QSqlQuery &query, const int page)
    {
        QCOMPARE(QueryUtils::queryResultSize(query), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = createQuery(connection)->from("file_property_properties")
                  .orderBy(ID)
                  .chunk(3, [&compareResultSize, &ids](QSqlQuery &query, const int page)
    {
        compareResultSize(query, page);

        while (query.next()) {
            auto id = query.value(ID).value<quint64>();
            ids.emplace_back(id);

            // Interrupt chunk-ing
            if (id == 5)
                return false;
        }

        return true;
    });

    QVERIFY(!result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_QueryBuilder::chunk_EnforceOrderBy() const
{
    QFETCH_GLOBAL(QString, connection);

    QVERIFY_EXCEPTION_THROWN(createQuery(connection)->from("file_property_properties")
                             .chunk(3, [](QSqlQuery &/*unused*/, const int /*unused*/)
    {
        return true;
    }),
            RuntimeError);
}

void tst_QueryBuilder::chunk_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection);

    auto callbackInvoked = false;

    auto result = createQuery(connection)->from("file_property_properties")
                  .whereEq(NAME, QStringLiteral("dummy-NON_EXISTENT"))
                  .orderBy(ID)
                  .chunk(3, [&callbackInvoked]
                            (QSqlQuery &/*unused*/, const int /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_QueryBuilder::each() const
{
    QFETCH_GLOBAL(QString, connection);

    std::vector<int> indexes;
    indexes.reserve(8);
    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = createQuery(connection)->from("file_property_properties")
                  .orderBy(ID)
                  .each([&indexes, &ids](QSqlQuery &query, const int index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(query.value(ID).value<quint64>());

        return true;
    });

    QVERIFY(result);

    std::vector<int> expectedIndexes {0, 1, 2, 3, 4, 5, 6, 7};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_QueryBuilder::each_ReturnFalse() const
{
    QFETCH_GLOBAL(QString, connection);

    std::vector<int> indexes;
    indexes.reserve(5);
    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = createQuery(connection)->from("file_property_properties")
                  .orderBy(ID)
                  .each([&indexes, &ids](QSqlQuery &query, const int index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(query.value(ID).value<quint64>());

        return index != 4; // false/interrupt on 4
    });

    QVERIFY(!result);

    std::vector<int> expectedIndexes {0, 1, 2, 3, 4};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_QueryBuilder::each_EnforceOrderBy() const
{
    QFETCH_GLOBAL(QString, connection);

    QVERIFY_EXCEPTION_THROWN(createQuery(connection)->from("file_property_properties")
                             .each([](QSqlQuery &/*unused*/, const int /*unused*/)
    {
        return true;
    }),
            RuntimeError);
}

void tst_QueryBuilder::each_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection);

    auto callbackInvoked = false;

    auto result = createQuery(connection)->from("file_property_properties")
                  .whereEq(NAME, QStringLiteral("dummy-NON_EXISTENT"))
                  .orderBy(ID)
                  .each([&callbackInvoked]
                        (QSqlQuery &/*unused*/, const int /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_QueryBuilder::chunkById() const
{
    QFETCH_GLOBAL(QString, connection);

    // <page, chunk_rowsCount>
    const std::unordered_map<int, int> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows](QSqlQuery &query, const int page)
    {
        QCOMPARE(QueryUtils::queryResultSize(query), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = createQuery(connection)->from("file_property_properties")
                  .orderBy(ID)
                  .chunkById(3, [&compareResultSize, &ids]
                                (QSqlQuery &query, const int page)
    {
        compareResultSize(query, page);

        while (query.next())
            ids.emplace_back(query.value(ID).value<quint64>());

        return true;
    });

    QVERIFY(result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_QueryBuilder::chunkById_ReturnFalse() const
{
    QFETCH_GLOBAL(QString, connection);

    // <page, chunk_rowsCount> (I leave it here also in this test, doesn't matter much
    const std::unordered_map<int, int> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows](QSqlQuery &query, const int page)
    {
        QCOMPARE(QueryUtils::queryResultSize(query), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = createQuery(connection)->from("file_property_properties")
                  .orderBy(ID)
                  .chunkById(3, [&compareResultSize, &ids]
                                (QSqlQuery &query, const int page)
    {
        compareResultSize(query, page);

        while (query.next()) {
            auto id = query.value(ID).value<quint64>();
            ids.emplace_back(id);

            // Interrupt chunk-ing
            if (id == 5)
                return false;
        }

        return true;
    });

    QVERIFY(!result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_QueryBuilder::chunkById_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection);

    auto callbackInvoked = false;

    auto result = createQuery(connection)->from("file_property_properties")
                  .whereEq(NAME, QStringLiteral("dummy-NON_EXISTENT"))
                  .orderBy(ID)
                  .chunkById(3, [&callbackInvoked]
                                (QSqlQuery &/*unused*/, const int /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_QueryBuilder::chunkById_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection);

    // <page, chunk_rowsCount>
    const std::unordered_map<int, int> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows](QSqlQuery &query, const int page)
    {
        QCOMPARE(QueryUtils::queryResultSize(query), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = createQuery(connection)->from("file_property_properties")
                  .select({ASTERISK, "id as id_as"})
                  .orderBy(ID)
                  .chunkById(3, [&compareResultSize, &ids]
                                (QSqlQuery &query, const int page)
    {
        compareResultSize(query, page);

        while (query.next())
            ids.emplace_back(query.value(ID).value<quint64>());

        return true;
    },
            ID, "id_as");

    QVERIFY(result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_QueryBuilder::chunkById_ReturnFalse_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection);

    // <page, chunk_rowsCount> (I leave it here also in this test, doesn't matter much
    const std::unordered_map<int, int> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows](QSqlQuery &query, const int page)
    {
        QCOMPARE(QueryUtils::queryResultSize(query), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = createQuery(connection)->from("file_property_properties")
                  .select({ASTERISK, "id as id_as"})
                  .orderBy(ID)
                  .chunkById(3, [&compareResultSize, &ids]
                                (QSqlQuery &query, const int page)
    {
        compareResultSize(query, page);

        while (query.next()) {
            auto id = query.value(ID).value<quint64>();
            ids.emplace_back(id);

            // Interrupt chunk-ing
            if (id == 5)
                return false;
        }

        return true;
    },
            ID, "id_as");

    QVERIFY(!result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_QueryBuilder::chunkById_EmptyResult_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection);

    auto callbackInvoked = false;

    auto result = createQuery(connection)->from("file_property_properties")
                  .select({ASTERISK, "id as id_as"})
                  .whereEq(NAME, QStringLiteral("dummy-NON_EXISTENT"))
                  .orderBy(ID)
                  .chunkById(3, [&callbackInvoked]
                                (QSqlQuery &/*unused*/, const int /*unused*/)
    {
        callbackInvoked = true;

        return true;
    },
            ID, "id_as");

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_QueryBuilder::eachById() const
{
    QFETCH_GLOBAL(QString, connection);

    std::vector<int> indexes;
    indexes.reserve(8);
    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = createQuery(connection)->from("file_property_properties")
                  .orderBy(ID)
                  .eachById([&indexes, &ids](QSqlQuery &query, const int index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(query.value(ID).value<quint64>());

        return true;
    });

    QVERIFY(result);

    std::vector<int> expectedIndexes {0, 1, 2, 3, 4, 5, 6, 7};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_QueryBuilder::eachById_ReturnFalse() const
{
    QFETCH_GLOBAL(QString, connection);

    std::vector<int> indexes;
    indexes.reserve(5);
    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = createQuery(connection)->from("file_property_properties")
                  .orderBy(ID)
                  .eachById([&indexes, &ids](QSqlQuery &query, const int index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(query.value(ID).value<quint64>());

        return index != 4; // false/interrupt on 4
    });

    QVERIFY(!result);

    std::vector<int> expectedIndexes {0, 1, 2, 3, 4};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_QueryBuilder::eachById_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection);

    auto callbackInvoked = false;

    auto result = createQuery(connection)->from("file_property_properties")
                  .whereEq(NAME, QStringLiteral("dummy-NON_EXISTENT"))
                  .orderBy(ID)
                  .eachById([&callbackInvoked]
                            (QSqlQuery &/*unused*/, const int /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_QueryBuilder::eachById_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection);

    std::vector<int> indexes;
    indexes.reserve(8);
    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = createQuery(connection)->from("file_property_properties")
                  .select({ASTERISK, "id as id_as"})
                  .orderBy(ID)
                  .eachById([&indexes, &ids](QSqlQuery &query, const int index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(query.value(ID).value<quint64>());

        return true;
    },
            1000, ID, "id_as");

    QVERIFY(result);

    std::vector<int> expectedIndexes {0, 1, 2, 3, 4, 5, 6, 7};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_QueryBuilder::eachById_ReturnFalse_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection);

    std::vector<int> indexes;
    indexes.reserve(5);
    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = createQuery(connection)->from("file_property_properties")
                  .select({ASTERISK, "id as id_as"})
                  .orderBy(ID)
                  .eachById([&indexes, &ids](QSqlQuery &query, const int index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(query.value(ID).value<quint64>());

        return index != 4; // false/interrupt on 4
    },
            1000, ID, "id_as");

    QVERIFY(!result);

    std::vector<int> expectedIndexes {0, 1, 2, 3, 4};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_QueryBuilder::eachById_EmptyResult_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection);

    auto callbackInvoked = false;

    auto result = createQuery(connection)->from("file_property_properties")
                  .select({ASTERISK, "id as id_as"})
                  .whereEq(NAME, QStringLiteral("dummy-NON_EXISTENT"))
                  .orderBy(ID)
                  .eachById([&callbackInvoked]
                            (QSqlQuery &/*unused*/, const int /*unused*/)
    {
        callbackInvoked = true;

        return true;
    },
            1000, ID, "id_as");

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

/* private */

std::shared_ptr<QueryBuilder>
tst_QueryBuilder::createQuery(const QString &connection) const
{
    return DB::connection(connection).query();
}

QTEST_MAIN(tst_QueryBuilder)

#include "tst_querybuilder.moc"
