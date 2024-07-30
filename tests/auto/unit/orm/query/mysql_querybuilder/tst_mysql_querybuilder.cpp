#include <QCoreApplication>
#include <QTest>

#include "orm/db.hpp"
#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/mysqlconnection.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"
#include "macros.hpp"

using Orm::Constants::AND;
using Orm::Constants::ASC;
using Orm::Constants::DESC;
using Orm::Constants::CREATED_AT;
using Orm::Constants::EQ;
using Orm::Constants::GE;
using Orm::Constants::GT;
using Orm::Constants::ID;
using Orm::Constants::LEFT;
using Orm::Constants::LIKE;
using Orm::Constants::LT;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::OR;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;
using Orm::Constants::dummy_NONEXISTENT;

using Orm::DB;
using Orm::Exceptions::InvalidArgumentError;
using Orm::MySqlConnection;
using Orm::Query::Builder;
using Orm::Query::Expression;

using QueryBuilder = Orm::Query::Builder;
using Raw = Orm::Query::Expression;
using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

/*
   Is not needed to duplicate these tests to tst_PostgreSQL_QueryBuilder and
   tst_SQLite_QueryBuilder because at the end of the day when there is no custom
   processing branch by DB, then I'm only testing quotation of columns that is only
   difference.
   It only makes sense only when there is a custom processing branch by DB.
   But I will not delete already written tests, more tests is better always 😁 right?
*/

/*
   Only the DatabaseConnection instance will be created during connection initialization
   in these query/tiny builder unit tests and a physical connection to the database
   will not be made because the DatabaseConnection class makes a connection lazily,
   until it is really needed.
   And because of these lazy connections database credentials for these tests are not
   needed.
   I wrote it like a hungarian, but whatever, it serves a purpose. 😁
*/

class tst_MySql_QueryBuilder : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void version() const;
    void version_InPretend() const;
    void version_InPretend_DefaultValue() const;

    void isMaria() const;
    void isMaria_InPretend() const;
    void isMaria_InPretend_DefaultValue() const;

    void get() const;
    void get_ColumnExpression() const;

    void find() const;
    void find_ColumnAndValueExpression() const;

    void first() const;
    void first_ColumnExpression() const;

    void value() const;
    void value_ColumnExpression() const;

    void pluck() const;
    void pluck_ColumnExpression() const;

    void pluck_Map() const;
    void pluck_Map_ColumnExpression() const;

    void count() const;
    void count_Distinct() const;
    void min_Aggregate() const;
    void min_Aggregate_ColumnExpression() const;
    void max_Aggregate() const;
    void sum_Aggregate() const;
    void average_Aggregate() const;

    void exists() const;
    void doesntExist() const;

    void select() const;
    void select_ColumnExpression() const;
    void select_ColumnAlias() const;

    void addSelect() const;
    void addSelect_ColumnExpression() const;
    void addSelect_ColumnAlias() const;

    void selectRaw() const;
    void selectRaw_WithBindings_WithWhere() const;

    void selectSub_QStringOverload() const;
    void selectSub_QueryBuilderOverload_WithWhere() const;
    void selectSub_CallbackOverload() const;

    void distinct() const;

    void from() const;
    void from_TableWrappingQuotationMarks() const;
    void from_WithPrefix() const;
    void from_AliasWithPrefix() const;

    void fromRaw() const;
    void fromRaw_WithWhere() const;
    void fromRaw_WithBindings_WithWhere() const;

    void fromSub_QStringOverload() const;
    void fromSub_QueryBuilderOverload_WithWhere() const;
    void fromSub_CallbackOverload() const;

    void joinSub_QStringOverload() const;
    void joinSub_QueryBuilderOverload_WithWhere() const;
    void joinSub_CallbackOverload() const;

    void where() const;
    void where_WithVectorValue() const;
    void where_WithVectorValue_DefaultCondition() const;
    void where_WithVectorValue_Condition_Override() const;
    void where_QueryableValue() const;
    void where_QueryableColumn() const;
    void where_ColumnExpression() const;
    void where_ValueExpression() const;

    void whereNot() const;
    void whereNot_WithVectorValue() const;
    void whereNot_WithVectorValue_DefaultCondition() const;
    void whereNot_QueryableValue() const;
    void whereNot_QueryableColumn() const;

    void orWhere() const;
    void orWhere_ColumnExpression() const;
    void orWhere_WithVectorValue() const;
    void orWhere_WithVectorValue_DefaultCondition() const;
    void orWhere_WithVectorValue_ColumnExpression() const;
    void orWhereEq_QueryableValue() const;
    void orWhereEq_QueryableColumn() const;
    void orWhereEq_QueryableColumnAndValue() const;

    void orWhereNot() const;
    void orWhereNot_ColumnExpression() const;
    void orWhereNot_WithVectorValue() const;
    void orWhereNot_WithVectorValue_DefaultCondition() const;
    void orWhereNot_WithVectorValue_ColumnExpression() const;
    void orWhereNotEq_QueryableValue() const;
    void orWhereNotEq_QueryableColumn() const;
    void orWhereNotEq_QueryableColumnAndValue() const;

    void whereColumn() const;
    void orWhereColumn() const;
    void orWhereColumn_ColumnExpression() const;
    void whereColumn_WithVectorValue() const;
    void orWhereColumn_WithVectorValue() const;
    void orWhereColumn_WithVectorValue_ColumnExpression() const;

    void whereIn() const;
    void whereNotIn() const;
    void whereNotIn_ColumnExpression() const;
    void whereIn_Empty() const;
    void whereNotIn_Empty() const;
    void whereIn_ValueExpression() const;

    void whereNull() const;
    void whereNotNull() const;
    void whereNotNull_ColumnExpression() const;
    void whereNull_WithVectorValue() const;
    void whereNotNull_WithVectorValue() const;

    void whereBetween() const;
    void whereNotBetween() const;
    void whereBetween_ColumnExpression() const;

    void whereBetweenColumns() const;
    void whereNotBetweenColumns() const;
    void whereBetweenColumns_ColumnExpression() const;

    void whereExists() const;
    void whereNotExists() const;
    void orWhereExists() const;
    void orWhereNotExists() const;

    void whereRowValues() const;
    void whereRowValues_Empty() const;
    void whereRowValues_ColumnExpression() const;
    void whereRowValues_ValueExpression() const;

    /* where dates */
    void whereDate();
    void whereTime();
    void whereDay();
    void whereMonth();
    void whereYear();

    void orderBy() const;
    void latestOldest() const;
    void inRandomOrder() const;

    void limitOffset() const;
    void takeSkip() const;
    void forPage() const;

    void lock() const;

    void insert() const;
    void insert_WithExpression() const;

    void update() const;
    void update_WithExpression() const;

    void upsert_UseUpsertAlias() const;
    void upsert_UseUpsertAlias_Disabled() const;
    void upsert_UseUpsertAlias_DefaultValue() const;
    void upsert_UseUpsertAlias_Maria() const;
    void upsert_WithoutUpdate_UpdateAll_UseUpsertAlias() const;
    void upsert_WithoutUpdate_UpdateAll_UseUpsertAlias_Disabled() const;
    void upsert_WithoutUpdate_UpdateAll_UseUpsertAlias_Maria() const;

    void remove() const;
    void remove_WithExpression() const;

    /* Builds Queries */
    void tap() const;

    void sole() const;
    void soleValue() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Create QueryBuilder instance for the given connection. */
    [[nodiscard]] std::shared_ptr<QueryBuilder> createQuery() const;

    /*! Connection name used in this test case. */
    QString m_connection;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_MySql_QueryBuilder::initTestCase()
{
    /* No need to test with the MariaDB (MARIADB connection) because it will produce
       the same output in 99% cases, one exception is eg. the upsert() or
       DatabaseConnection::version(). */
    m_connection = Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );
}

void tst_MySql_QueryBuilder::version() const
{
    auto version = sl("10.8.3-MariaDB");

    auto &mysqlConnection = dynamic_cast<MySqlConnection &>(DB::connection(m_connection));
    mysqlConnection.setConfigVersion(version);

    QCOMPARE(mysqlConnection.version(), version);
}

void tst_MySql_QueryBuilder::version_InPretend() const
{
    auto version = sl("10.8.3-MariaDB");

    // Need to be set before pretending
    auto &mysqlConnection = dynamic_cast<MySqlConnection &>(DB::connection(m_connection));
    mysqlConnection.setConfigVersion(version);

    auto log = mysqlConnection.pretend([&mysqlConnection, &version]
    {
        QCOMPARE(mysqlConnection.version(), version);
    });

    QVERIFY(log.isEmpty());
}

void tst_MySql_QueryBuilder::version_InPretend_DefaultValue() const
{
    // Need to be set before pretending
    auto &mysqlConnection = dynamic_cast<MySqlConnection &>(DB::connection(m_connection));
    mysqlConnection.setConfigVersion({});

    auto log = mysqlConnection.pretend([&mysqlConnection]
    {
        // No version set so it should return std::nullopt
        QVERIFY(!mysqlConnection.version());
    });

    QVERIFY(log.isEmpty());
}

void tst_MySql_QueryBuilder::isMaria() const
{
    auto &mysqlConnection = dynamic_cast<MySqlConnection &>(DB::connection(m_connection));
    mysqlConnection.setConfigVersion("10.4.7-MariaDB");

    QVERIFY(mysqlConnection.isMaria());
}

void tst_MySql_QueryBuilder::isMaria_InPretend() const
{
    // Need to be set before pretending
    auto &mysqlConnection = dynamic_cast<MySqlConnection &>(DB::connection(m_connection));
    mysqlConnection.setConfigVersion("10.4.7-MariaDB");

    auto log = mysqlConnection.pretend([&mysqlConnection]
    {
        QVERIFY(mysqlConnection.isMaria());
    });

    QVERIFY(log.isEmpty());
}

void tst_MySql_QueryBuilder::isMaria_InPretend_DefaultValue() const
{
    // Need to be set before pretending
    auto &mysqlConnection = dynamic_cast<MySqlConnection &>(DB::connection(m_connection));
    mysqlConnection.setConfigVersion({});

    auto log = mysqlConnection.pretend([&mysqlConnection]
    {
        // No version set so it the default value is false
        QVERIFY(!mysqlConnection.isMaria());
    });

    QVERIFY(log.isEmpty());
}

void tst_MySql_QueryBuilder::get() const
{
    {
        auto log = DB::connection(m_connection).pretend([](auto &connection)
        {
            connection.query()->from("torrents").get({ID, NAME});
        });

        QVERIFY(!log.isEmpty());
        const auto &firstLog = log.first();

        QCOMPARE(log.size(), 1);
        QCOMPARE(firstLog.query,
                 "select `id`, `name` from `torrents`");
        QVERIFY(firstLog.boundValues.isEmpty());
    }

    {
        auto log = DB::connection(m_connection).pretend([](auto &connection)
        {
            connection.query()->from("torrents").get();
        });

        QVERIFY(!log.isEmpty());
        const auto &firstLog = log.first();

        QCOMPARE(log.size(), 1);
        QCOMPARE(firstLog.query,
                 "select * from `torrents`");
        QVERIFY(firstLog.boundValues.isEmpty());
    }
}

void tst_MySql_QueryBuilder::get_ColumnExpression() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").get({Raw(ID), NAME});
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select id, `name` from `torrents`");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_QueryBuilder::find() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").find(3, {ID, NAME});
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select `id`, `name` from `torrents` where `id` = ? limit 1");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(3)}));
}

void tst_MySql_QueryBuilder::find_ColumnAndValueExpression() const
{
    {
        auto log = DB::connection(m_connection).pretend([](auto &connection)
        {
            connection.query()->from("torrents").find(3, {ID, Raw(NAME)});
        });

        QVERIFY(!log.isEmpty());
        const auto &firstLog = log.first();

        QCOMPARE(log.size(), 1);
        QCOMPARE(firstLog.query,
                 "select `id`, name from `torrents` where `id` = ? limit 1");
        QCOMPARE(firstLog.boundValues,
                 QList<QVariant>({QVariant(3)}));
    }

    {
        auto log = DB::connection(m_connection).pretend([](auto &connection)
        {
            connection.query()->from("torrents").find(Raw("1 + 3"), {ID, Raw(NAME)});
        });

        QVERIFY(!log.isEmpty());
        const auto &firstLog = log.first();

        QCOMPARE(log.size(), 1);
        QCOMPARE(firstLog.query,
                 "select `id`, name from `torrents` where `id` = 1 + 3 limit 1");
        QVERIFY(firstLog.boundValues.isEmpty());
    }
}

void tst_MySql_QueryBuilder::first() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").first({ID, NAME});
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select `id`, `name` from `torrents` limit 1");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_QueryBuilder::first_ColumnExpression() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").first({ID, Raw(NAME)});
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select `id`, name from `torrents` limit 1");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_QueryBuilder::value() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").value(NAME);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select `name` from `torrents` limit 1");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_QueryBuilder::value_ColumnExpression() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").value(Raw(NAME));
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select name from `torrents` limit 1");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_QueryBuilder::pluck() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").pluck(NAME);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select `name` from `torrents`");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_QueryBuilder::pluck_ColumnExpression() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").pluck(Raw(NAME));
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select name from `torrents`");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_QueryBuilder::pluck_Map() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").template pluck<quint64>(NAME, ID);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select `name`, `id` from `torrents`");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_QueryBuilder::pluck_Map_ColumnExpression() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").template pluck<quint64>(Raw(NAME), Raw(ID));
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select name, id from `torrents`");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_QueryBuilder::count() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").count();
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select count(*) as `aggregate` from `torrents`");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_QueryBuilder::count_Distinct() const
{
    {
        auto log = DB::connection(m_connection).pretend([](auto &connection)
        {
            connection.query()->from("torrents").distinct().count(SIZE_);
        });

        QVERIFY(!log.isEmpty());
        const auto &firstLog = log.first();

        QCOMPARE(log.size(), 1);
        QCOMPARE(firstLog.query,
                 "select count(distinct `size`) as `aggregate` from `torrents`");
        QVERIFY(firstLog.boundValues.isEmpty());
    }

    // MySQL only, it allows more columns
    {
        auto log = DB::connection(m_connection).pretend([](auto &connection)
        {
            connection.query()->from("torrents").distinct().count({SIZE_, NOTE});
        });

        QVERIFY(!log.isEmpty());
        const auto &firstLog = log.first();

        QCOMPARE(log.size(), 1);
        QCOMPARE(firstLog.query,
                 "select count(distinct `size`, `note`) as `aggregate` from `torrents`");
        QVERIFY(firstLog.boundValues.isEmpty());
    }

    // MySQL only, it allows more columns, columns defined in distinct()
    {
        auto log = DB::connection(m_connection).pretend([](auto &connection)
        {
            connection.query()->from("torrents").distinct({SIZE_, NOTE}).count();
        });

        QVERIFY(!log.isEmpty());
        const auto &firstLog = log.first();

        QCOMPARE(log.size(), 1);
        QCOMPARE(firstLog.query,
                 "select count(distinct `size`, `note`) as `aggregate` from `torrents`");
        QVERIFY(firstLog.boundValues.isEmpty());
    }
}

void tst_MySql_QueryBuilder::min_Aggregate() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").min(SIZE_);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select min(`size`) as `aggregate` from `torrents`");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_QueryBuilder::min_Aggregate_ColumnExpression() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").min(Raw(SIZE_));
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select min(size) as `aggregate` from `torrents`");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_QueryBuilder::max_Aggregate() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").max(SIZE_);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select max(`size`) as `aggregate` from `torrents`");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_QueryBuilder::sum_Aggregate() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").sum(SIZE_);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select sum(`size`) as `aggregate` from `torrents`");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_QueryBuilder::average_Aggregate() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").avg(SIZE_);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select avg(`size`) as `aggregate` from `torrents`");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_QueryBuilder::exists() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()
                ->select("*").from("torrent_peers").where(ID, LT, 7).exists();
    });

    QCOMPARE(log.size(), 1);
    const auto &firstLog = log.first();

    QCOMPARE(firstLog.query,
             "select exists(select * from `torrent_peers` where `id` < ?) as `exists`");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(7)}));
}

void tst_MySql_QueryBuilder::doesntExist() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()
                ->select("*").from("torrent_peers").where(ID, LT, 7).doesntExist();
    });

    QCOMPARE(log.size(), 1);
    const auto &firstLog = log.first();

    QCOMPARE(firstLog.query,
             "select exists(select * from `torrent_peers` where `id` < ?) as `exists`");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(7)}));
}

void tst_MySql_QueryBuilder::select() const
{
    auto builder = createQuery();

    builder->from("torrents");

    builder->select({ID, NAME});
    QCOMPARE(builder->toSql(),
             "select `id`, `name` from `torrents`");

    builder->select();
    QCOMPARE(builder->toSql(),
             "select * from `torrents`");

    builder->select(ID);
    QCOMPARE(builder->toSql(),
             "select `id` from `torrents`");
}

void tst_MySql_QueryBuilder::select_ColumnExpression() const
{
    auto builder = createQuery();

    builder->from("torrents");

    builder->select(Raw(NAME));
    QCOMPARE(builder->toSql(),
             "select name from `torrents`");

    builder->select({ID, Raw(NAME)});
    QCOMPARE(builder->toSql(),
             "select `id`, name from `torrents`");

    builder->select(DB::raw("count(*) as user_count, status"));
    QCOMPARE(builder->toSql(),
             "select count(*) as user_count, status from `torrents`");
}

void tst_MySql_QueryBuilder::select_ColumnAlias() const
{
    auto builder = createQuery();

    builder->from("torrents");

    builder->select({ID, "name as username"});
    QCOMPARE(builder->toSql(),
             "select `id`, `name` as `username` from `torrents`");

    builder->select({ID, "name    as   username"});
    QCOMPARE(builder->toSql(),
             "select `id`, `name` as `username` from `torrents`");

    builder->select({ID, "name AS username"});
    QCOMPARE(builder->toSql(),
             "select `id`, `name` as `username` from `torrents`");

    builder->select({ID, "name    AS   username"});
    QCOMPARE(builder->toSql(),
             "select `id`, `name` as `username` from `torrents`");
}

void tst_MySql_QueryBuilder::addSelect() const
{
    auto builder = createQuery();

    builder->from("torrents");

    builder->addSelect({ID, NAME});
    QCOMPARE(builder->toSql(),
             "select `id`, `name` from `torrents`");

    builder->addSelect(SIZE_);
    QCOMPARE(builder->toSql(),
             "select `id`, `name`, `size` from `torrents`");

    builder->addSelect("*");
    QCOMPARE(builder->toSql(),
             "select `id`, `name`, `size`, * from `torrents`");
}

void tst_MySql_QueryBuilder::addSelect_ColumnExpression() const
{
    auto builder = createQuery();

    builder->from("torrents");

    builder->addSelect(Raw(NAME));
    QCOMPARE(builder->toSql(),
             "select name from `torrents`");

    builder->addSelect({ID, Raw("happiness")});
    QCOMPARE(builder->toSql(),
             "select name, `id`, happiness from `torrents`");

    builder->addSelect(DB::raw("count(*) as user_count, status"));
    QCOMPARE(builder->toSql(),
             "select name, `id`, happiness, count(*) as user_count, status "
             "from `torrents`");
}

void tst_MySql_QueryBuilder::addSelect_ColumnAlias() const
{
    auto builder = createQuery();

    builder->from("torrents");

    builder->addSelect("name as username");
    QCOMPARE(builder->toSql(),
             "select `name` as `username` from `torrents`");

    builder->addSelect({ID, "name1    as   username1"});
    QCOMPARE(builder->toSql(),
             "select `name` as `username`, `id`, `name1` as `username1` "
               "from `torrents`");

    builder->addSelect("name2 AS username2");
    QCOMPARE(builder->toSql(),
             "select `name` as `username`, `id`, `name1` as `username1`, "
               "`name2` as `username2` "
               "from `torrents`");

    builder->addSelect({"note", "name3    AS   username3"});
    QCOMPARE(builder->toSql(),
             "select `name` as `username`, `id`, `name1` as `username1`, "
               "`name2` as `username2`, `note`, `name3` as `username3` "
               "from `torrents`");
}

void tst_MySql_QueryBuilder::selectRaw() const
{
    auto builder = createQuery();

    const auto &columns = builder->getColumns();

    // selectRaw() with bindings and with where on main query
    builder->selectRaw("id, filepath as `path`")
            .from("torrents");

    QCOMPARE(columns.size(), 1);
    QVERIFY(std::holds_alternative<Expression>(columns.first()));
    QCOMPARE(builder->toSql(),
             "select id, filepath as `path` from `torrents`");
    QVERIFY(builder->getBindings().isEmpty());
}

void tst_MySql_QueryBuilder::selectRaw_WithBindings_WithWhere() const
{
    auto builder = createQuery();

    const auto &columns = builder->getColumns();

    // selectRaw() with bindings and with where on main query
    builder->selectRaw("(select max(last_seen_at) as last_seen_at "
                       "from `sessions` where id < ?) as `last_seen_at`",
                       {10})
            .from("torrents")
            .where("last_seen_at", ">", 1520652582);

    QCOMPARE(columns.size(), 1);
    QVERIFY(std::holds_alternative<Expression>(columns.first()));
    QCOMPARE(builder->toSql(),
             "select (select max(last_seen_at) as last_seen_at "
                     "from `sessions` where id < ?) as `last_seen_at` "
             "from `torrents` "
             "where `last_seen_at` > ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(10), QVariant(1520652582)}));
}

void tst_MySql_QueryBuilder::selectSub_QStringOverload() const
{
    auto builder = createQuery();

    builder->selectSub("select max(size) from `torrents`",
                       "max_size")
            .addSelect({ID, NAME})
            .from("torrents");

    const auto &columns = builder->getColumns();

    QCOMPARE(columns.size(), 3);
    QVERIFY(std::holds_alternative<Expression>(columns.first()));
    QCOMPARE(builder->toSql(),
             "select (select max(size) from `torrents`) as `max_size`, `id`, `name` "
             "from `torrents`");
}

void tst_MySql_QueryBuilder::selectSub_QueryBuilderOverload_WithWhere() const
{
    auto builder = createQuery();

    // Ownership of the std::shared_ptr<QueryBuilder>
    auto subQuery = createQuery();
    subQuery->from("torrents")
            .select(Raw("max(size)"))
            .where(ID, "<", 5);

    builder->selectSub(*subQuery, "max_size")
            .addSelect({ID, NAME})
            .from("torrents")
            .whereEq(NAME, "xyz");

    const auto &columns = builder->getColumns();

    QCOMPARE(columns.size(), 3);
    QVERIFY(std::holds_alternative<Expression>(columns.first()));
    QCOMPARE(builder->toSql(),
             "select (select max(size) from `torrents` where `id` < ?) as `max_size`, "
               "`id`, `name` "
             "from `torrents` "
             "where `name` = ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(5), QVariant("xyz")}));
}

void tst_MySql_QueryBuilder::selectSub_CallbackOverload() const
{
    auto builder = createQuery();

    builder->selectSub([](auto &query)
    {
        query.select(Raw("max(size)"))
                .from("torrents")
                .where(ID, "<", 5);
    }, "max_size")
            .addSelect({ID, NAME})
            .from("torrents")
            .whereEq(NAME, "xyz");

    const auto &columns = builder->getColumns();

    QCOMPARE(columns.size(), 3);
    QVERIFY(std::holds_alternative<Expression>(columns.first()));
    QCOMPARE(builder->toSql(),
             "select (select max(size) from `torrents` where `id` < ?) as `max_size`, "
               "`id`, `name` "
             "from `torrents` "
             "where `name` = ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(5), QVariant("xyz")}));
}

void tst_MySql_QueryBuilder::distinct() const
{
    auto builder = createQuery();

    builder->from("torrents");

    auto distinct = std::get<bool>(builder->getDistinct());
    QCOMPARE(distinct, false);

    builder->distinct();
    distinct = std::get<bool>(builder->getDistinct());
    QCOMPARE(distinct, true);
    QCOMPARE(builder->toSql(),
             "select distinct * from `torrents`");

    builder->select({NAME, SIZE_});
    QCOMPARE(builder->toSql(),
             "select distinct `name`, `size` from `torrents`");
}

void tst_MySql_QueryBuilder::from() const
{
    auto builder = createQuery();

    const auto &from = builder->getFrom();

    QVERIFY(std::holds_alternative<std::monostate>(from));

    const auto tableTorrents = sl("torrents");
    builder->from(tableTorrents);

    QVERIFY(std::holds_alternative<QString>(from));
    QCOMPARE(std::get<QString>(from), tableTorrents);
    QCOMPARE(builder->toSql(),
             "select * from `torrents`");

    const auto tableTorrentPeers = sl("torrent_peers");
    builder->from(tableTorrentPeers);

    QVERIFY(std::holds_alternative<QString>(from));
    QCOMPARE(std::get<QString>(from), tableTorrentPeers);
    QCOMPARE(builder->toSql(),
             "select * from `torrent_peers`");
}

void tst_MySql_QueryBuilder::from_TableWrappingQuotationMarks() const
{
    auto builder = createQuery();

    const auto &from = builder->getFrom();

    // Protects quotation marks
    {
        const auto table = sl("some`table");
        builder->from(table);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from), table);
        QCOMPARE(builder->toSql(),
                 "select * from `some``table`");
    }
    {
        const auto table = sl("some\"table");
        builder->from(table);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from), table);
        QCOMPARE(builder->toSql(),
                 "select * from `some\"table`");
    }
    {
        const auto table = sl("some'table");
        builder->from(table);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from), table);
        QCOMPARE(builder->toSql(),
                 "select * from `some'table`");
    }
    // Wrapping as whole constant
    {
        const auto table = sl("baz");
        builder->select("x.y as foo.bar").from(table);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from), table);
        QCOMPARE(builder->toSql(),
                 "select `x`.`y` as `foo.bar` from `baz`");
    }
    // Wrapping with space in database name
    {
        const auto table = sl("baz");
        builder->select("w x.y.z as foo.bar").from(table);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from), table);
        QCOMPARE(builder->toSql(),
                 "select `w x`.`y`.`z` as `foo.bar` from `baz`");
    }
    // Wrapping with as
    {
        const auto table = sl("table as alias");
        builder->select("*").from(table);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from), table);
        QCOMPARE(builder->toSql(),
                 "select * from `table` as `alias`");
    }
    // Wrapping with as
    {
        const auto table = sl("table");
        const auto alias = sl("alias");
        builder->from(table, alias);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from), sl("%1 as %2").arg(table, alias));
        QCOMPARE(builder->toSql(),
                 "select * from `table` as `alias`");
    }
}

void tst_MySql_QueryBuilder::from_WithPrefix() const
{
    auto builder = createQuery();

    const auto prefix = sl("xyz_");
    const auto table = sl("table");
    builder->from(table);

    builder->getConnection().setTablePrefix(prefix);

    const auto &from = builder->getFrom();

    QVERIFY(std::holds_alternative<QString>(from));
    QCOMPARE(std::get<QString>(from), table);
    QCOMPARE(builder->toSql(),
             "select * from `xyz_table`");

    // Restore
    builder->getConnection().setTablePrefix("");
}

void tst_MySql_QueryBuilder::from_AliasWithPrefix() const
{
    auto builder = createQuery();

    const auto &from = builder->getFrom();

    const auto prefix = sl("xyz_");
    builder->getConnection().setTablePrefix(prefix);

    {
        const auto table = sl("table");
        const auto alias = sl("alias");
        builder->from(table, alias);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from), sl("%1 as %2").arg(table, alias));
        QCOMPARE(builder->toSql(),
                 "select * from `xyz_table` as `xyz_alias`");
    }

    {
        const auto table = sl("table as alias");
        builder->from(table);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from), table);
        QCOMPARE(builder->toSql(),
                 "select * from `xyz_table` as `xyz_alias`");
    }

    // Restore
    builder->getConnection().setTablePrefix("");
}

void tst_MySql_QueryBuilder::fromRaw() const
{
    auto builder = createQuery();

    const auto &from = builder->getFrom();

    builder->fromRaw("(select max(last_seen_at) as last_seen_at "
                     "from `user_sessions`) as `sessions`");

    QVERIFY(std::holds_alternative<Expression>(from));
    QCOMPARE(builder->toSql(),
             "select * from (select max(last_seen_at) as last_seen_at "
             "from `user_sessions`) as `sessions`");
}

void tst_MySql_QueryBuilder::fromRaw_WithWhere() const
{
    auto builder = createQuery();

    const auto &from = builder->getFrom();

    // fromRaw() with where on main query
    // this query is nonsense, but it doesn't matter
    builder->fromRaw("(select max(last_seen_at) as last_seen_at "
                     "from `sessions`) as `last_seen_at`")
            .where("last_seen_at", ">", 1520652582);

    QVERIFY(std::holds_alternative<Expression>(from));
    QCOMPARE(builder->toSql(),
             "select * from (select max(last_seen_at) as last_seen_at "
             "from `sessions`) as `last_seen_at` where `last_seen_at` > ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(1520652582)}));
}

void tst_MySql_QueryBuilder::fromRaw_WithBindings_WithWhere() const
{
    auto builder = createQuery();

    const auto &from = builder->getFrom();

    // fromRaw() with bindings and with where on main query
    // this query is nonsense, but it doesn't matter
    builder->fromRaw("(select max(last_seen_at) as last_seen_at "
                     "from `sessions` where id < ?) as `last_seen_at`",
                     {10})
            .where("last_seen_at", ">", 1520652582);

    QVERIFY(std::holds_alternative<Expression>(from));
    QCOMPARE(builder->toSql(),
             "select * from (select max(last_seen_at) as last_seen_at "
             "from `sessions` where id < ?) as `last_seen_at` "
             "where `last_seen_at` > ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(10), QVariant(1520652582)}));
}

void tst_MySql_QueryBuilder::fromSub_QStringOverload() const
{
    auto builder = createQuery();

    builder->fromSub("select max(last_seen_at) as last_seen_at from `user_sessions`",
                     "sessions");

    QVERIFY(std::holds_alternative<Expression>(builder->getFrom()));
    QCOMPARE(builder->toSql(),
             "select * from (select max(last_seen_at) as last_seen_at "
             "from `user_sessions`) as `sessions`");
}

void tst_MySql_QueryBuilder::fromSub_QueryBuilderOverload_WithWhere() const
{
    auto builder = createQuery();

    // Ownership of the std::shared_ptr<QueryBuilder>
    auto subQuery = createQuery();
    subQuery->from("user_sessions")
            .select({ID, NAME})
            .where(ID, "<", 5);

    builder->fromSub(*subQuery, "sessions")
            .whereEq(NAME, "xyz");

    QVERIFY(std::holds_alternative<Expression>(builder->getFrom()));
    QCOMPARE(builder->toSql(),
             "select * from (select `id`, `name` "
             "from `user_sessions` where `id` < ?) as `sessions` "
             "where `name` = ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(5), QVariant("xyz")}));
}

void tst_MySql_QueryBuilder::fromSub_CallbackOverload() const
{
    auto builder = createQuery();

    builder->fromSub([](auto &query)
    {
        query.from("user_sessions")
             .select({ID, NAME})
             .where(ID, "<", 5);
    }, "sessions").whereEq(NAME, "xyz");

    QVERIFY(std::holds_alternative<Expression>(builder->getFrom()));
    QCOMPARE(builder->toSql(),
             "select * from (select `id`, `name` "
             "from `user_sessions` where `id` < ?) as `sessions` "
             "where `name` = ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(5), QVariant("xyz")}));
}

void tst_MySql_QueryBuilder::joinSub_QStringOverload() const
{
    auto builder = createQuery();

    builder->from("users")
            .joinSub("select id as files_id, `user_id`, `name` from `user_sessions`",
                     "sessions", "users.id", "=", "sessions.user_id", LEFT);

    QCOMPARE(builder->toSql(),
             "select * from `users` "
             "left join (select id as files_id, `user_id`, `name` "
             "from `user_sessions`) as `sessions` "
             "on `users`.`id` = `sessions`.`user_id`");
}

void tst_MySql_QueryBuilder::joinSub_QueryBuilderOverload_WithWhere() const
{
    auto builder = createQuery();

    // Ownership of the std::shared_ptr<QueryBuilder>
    auto subQuery = createQuery();
    subQuery->from("user_sessions")
            .select({"id as files_id", "user_id", NAME})
            .where("user_id", "<", 5);

    builder->from("users")
            .joinSub(*subQuery, "sessions", "users.id", "=", "sessions.user_id")
            .whereEq(NAME, "xyz");

    QCOMPARE(builder->toSql(),
             "select * from `users` "
             "inner join (select `id` as `files_id`, `user_id`, `name` "
                 "from `user_sessions` where `user_id` < ?) as `sessions` "
             "on `users`.`id` = `sessions`.`user_id` "
             "where `name` = ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(5), QVariant("xyz")}));
}

void tst_MySql_QueryBuilder::joinSub_CallbackOverload() const
{
    auto builder = createQuery();

    builder->from("users")
            .joinSub([](auto &query)
    {
        query.from("user_sessions")
                .select({"id as files_id", "user_id", NAME})
                .where("user_id", "<", 5);
    }, "sessions", "users.id", "=", "sessions.user_id", LEFT)

            .whereEq(NAME, "xyz");

    QCOMPARE(builder->toSql(),
             "select * from `users` "
             "left join (select `id` as `files_id`, `user_id`, `name` "
                 "from `user_sessions` where `user_id` < ?) as `sessions` "
             "on `users`.`id` = `sessions`.`user_id` "
             "where `name` = ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(5), QVariant("xyz")}));
}

void tst_MySql_QueryBuilder::where() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, "=", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereEq(ID, 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereEq(ID, 3)
                .whereEq(NAME, "test3");
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? and `name` = ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(3), QVariant("test3")}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, "!=", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` != ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, "<>", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` <> ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, ">", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, ">", 3)
                .where(NAME, LIKE, "test%");
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? and `name` like ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(3), QVariant("test%")}));
    }
}

void tst_MySql_QueryBuilder::where_WithVectorValue() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where({{ID, 3}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where (`id` = ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where({{ID, 3}, {SIZE_, 10, ">"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where (`id` = ? and `size` > ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(3), QVariant(10)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where({{ID, 3}, {SIZE_, 10, ">"}})
                .where({{Progress, 100, ">="}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where (`id` = ? and `size` > ?) "
                 "and (`progress` >= ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(3), QVariant(10), QVariant(100)}));
    }
}

void tst_MySql_QueryBuilder::where_WithVectorValue_DefaultCondition() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents")
            .where({{Progress, 100, ">="}})
            .where({{ID, 3}, {SIZE_, 10, ">"}}, AND, OR);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where (`progress` >= ?) and "
             "(`id` = ? or `size` > ?)");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(100), QVariant(3), QVariant(10)}));
}

void tst_MySql_QueryBuilder::where_WithVectorValue_Condition_Override() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents")
            .where({{Progress, 100, ">="}})
            .where({{ID, 3}, {SIZE_, 10, ">"}, {NAME, "xyz", EQ, AND}}, AND, OR);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where (`progress` >= ?) and "
             "(`id` = ? or `size` > ? and `name` = ?)");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(100), QVariant(3), QVariant(10),
                              QVariant("xyz")}));
}

void tst_MySql_QueryBuilder::where_QueryableValue() const
{
    // With lambda expression
    {
        auto builder = createQuery();

        builder->from("torrents").where(ID, ">", [](auto &query)
        {
            query.from("torrents", "t").selectRaw("avg(t.size)");
        });
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where `id` > (select avg(t.size) from `torrents` as `t`)");
        QVERIFY(builder->getBindings().isEmpty());
    }
    // With QueryBuilder
    {
        auto builder = createQuery();

        builder->from("torrents")
                .where(ID, ">",
                       createQuery()->from("torrents", "t")
                                     .selectRaw("avg(t.size)"));
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where `id` > (select avg(t.size) from `torrents` as `t`)");
        QVERIFY(builder->getBindings().isEmpty());
    }
}

void tst_MySql_QueryBuilder::where_QueryableColumn() const
{
    // With lambda expression
    {
        auto builder = createQuery();

        builder->from("torrents").where([](auto &query)
        {
            query.from("torrents", "t").selectRaw("avg(t.size)");
        }, ">", 13);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where (select avg(t.size) from `torrents` as `t`) > ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(13)}));
    }
    // With QueryBuilder
    {
        auto builder = createQuery();

        builder->from("torrents")
                .where(createQuery()->from("torrents", "t")
                                     .selectRaw("avg(t.size)"),
                       ">", 13);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where (select avg(t.size) from `torrents` as `t`) > ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(13)}));
    }
}

void tst_MySql_QueryBuilder::where_ColumnExpression() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents").where(DB::raw(ID), "=", 3);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where id = ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant> {QVariant(3)});
}

void tst_MySql_QueryBuilder::where_ValueExpression() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents").where(ID, "=", DB::raw(3))
            .orWhereEq(DB::raw(NAME), DB::raw("'test3'"));
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where `id` = 3 or name = 'test3'");
    QVERIFY(builder->getBindings().isEmpty());
}

void tst_MySql_QueryBuilder::whereNot() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereNot(ID, "=", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where not `id` = ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereNotEq(ID, 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where not `id` = ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereNotEq(ID, 3)
                .whereEq(NAME, "test3");
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where not `id` = ? and `name` = ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(3), QVariant("test3")}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereNot(ID, "!=", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where not `id` != ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereNot(ID, "<>", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where not `id` <> ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereNot(ID, ">", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where not `id` > ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereNot(ID, ">", 3)
                .whereNot(NAME, LIKE, "test%");
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where not `id` > ? and not `name` like ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(3), QVariant("test%")}));
    }
}

void tst_MySql_QueryBuilder::whereNot_WithVectorValue() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereNot({{ID, 3}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where not (`id` = ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereNot({{ID, 3}, {SIZE_, 10, ">"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where not (`id` = ? and `size` > ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(3), QVariant(10)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereNot({{ID, 3}, {SIZE_, 10, ">"}})
                .whereNot({{Progress, 100, ">="}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where not (`id` = ? and `size` > ?) "
                 "and not (`progress` >= ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(3), QVariant(10), QVariant(100)}));
    }
}

void tst_MySql_QueryBuilder::whereNot_WithVectorValue_DefaultCondition() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents")
            .whereNot({{Progress, 100, ">="}})
            .whereNot({{ID, 3}, {SIZE_, 10, ">"}}, AND, OR);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where not (`progress` >= ?) and "
             "not (`id` = ? or `size` > ?)");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(100), QVariant(3), QVariant(10)}));
}

void tst_MySql_QueryBuilder::whereNot_QueryableValue() const
{
    // With lambda expression
    {
        auto builder = createQuery();

        builder->from("torrents").whereNot(ID, ">", [](auto &query)
        {
            query.from("torrents", "t").selectRaw("avg(t.size)");
        });
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where not `id` > (select avg(t.size) from `torrents` as `t`)");
        QVERIFY(builder->getBindings().isEmpty());
    }
    // With QueryBuilder
    {
        auto builder = createQuery();

        builder->from("torrents")
                .whereNot(ID, ">",
                          createQuery()->from("torrents", "t")
                                        .selectRaw("avg(t.size)"));
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where not `id` > (select avg(t.size) from `torrents` as `t`)");
        QVERIFY(builder->getBindings().isEmpty());
    }
}

void tst_MySql_QueryBuilder::whereNot_QueryableColumn() const
{
    // With lambda expression
    {
        auto builder = createQuery();

        builder->from("torrents").whereNot([](auto &query)
        {
            query.from("torrents", "t").selectRaw("avg(t.size)");
        }, ">", 13);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where not (select avg(t.size) from `torrents` as `t`) > ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(13)}));
    }
    // With QueryBuilder
    {
        auto builder = createQuery();

        builder->from("torrents")
                .whereNot(createQuery()->from("torrents", "t")
                                        .selectRaw("avg(t.size)"),
                          ">", 13);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where not (select avg(t.size) from `torrents` as `t`) > ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(13)}));
    }
}

void tst_MySql_QueryBuilder::orWhere() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, ">", 4)
                .orWhere(Progress, ">=", 300);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? or `progress` >= ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(4), QVariant(300)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, ">", 4)
                .orWhereEq(NAME, "test3");
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? or `name` = ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(4), QVariant("test3")}));
    }
}

void tst_MySql_QueryBuilder::orWhere_ColumnExpression() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents").where(Raw(ID), ">", 4)
            .orWhereEq(Raw("`name`"), "test3");
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where id > ? or `name` = ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(4), QVariant("test3")}));
}

void tst_MySql_QueryBuilder::orWhere_WithVectorValue() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents").where({{ID, 3}, {SIZE_, 10, ">"}})
            .orWhere({{Progress, 100, ">="}});
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where (`id` = ? and `size` > ?) or "
             "(`progress` >= ?)");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(3), QVariant(10), QVariant(100)}));
}

void tst_MySql_QueryBuilder::orWhere_WithVectorValue_DefaultCondition() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents").where({{Progress, 100, ">="}})
            .orWhere({{ID, 3}, {SIZE_, 10, ">"}}, AND);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where (`progress` >= ?) or "
             "(`id` = ? and `size` > ?)");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(100), QVariant(3), QVariant(10)}));
}

void tst_MySql_QueryBuilder::orWhere_WithVectorValue_ColumnExpression() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents")
            .where({{Raw(ID), 3}, {Raw("`size`"), 10, ">"}})
            .orWhere({{Raw(Progress), 100, ">="}});
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where (id = ? and `size` > ?) or "
             "(progress >= ?)");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(3), QVariant(10), QVariant(100)}));
}

void tst_MySql_QueryBuilder::orWhereEq_QueryableValue() const
{
    // With lambda expression
    {
        auto builder = createQuery();

        builder->from("torrents")
                .whereEq(ID, 2)
                .orWhereEq(ID, [](auto &query)
        {
            query.from("torrents", "t").selectRaw("avg(t.size)");
        });
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where `id` = ? or `id` = (select avg(t.size) from `torrents` as `t`)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }
    // With QueryBuilder
    {
        auto builder = createQuery();

        builder->from("torrents")
                .whereEq(ID, 2)
                .orWhereEq(ID,
                           createQuery()->from("torrents", "t")
                                         .selectRaw("avg(t.size)"));
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where `id` = ? or `id` = (select avg(t.size) from `torrents` as `t`)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }
}

void tst_MySql_QueryBuilder::orWhereEq_QueryableColumn() const
{
    // With lambda expression
    {
        auto builder = createQuery();

        builder->from("torrents")
                .whereEq(ID, 2)
                .orWhereEq([](auto &query)
        {
            query.from("torrents", "t").selectRaw("avg(t.size)");
        }, 13);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where `id` = ? or (select avg(t.size) from `torrents` as `t`) = ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2), QVariant(13)}));
    }
    // With QueryBuilder
    {
        auto builder = createQuery();

        builder->from("torrents")
                .whereEq(ID, 2)
                .orWhereEq(createQuery()->from("torrents", "t")
                                         .selectRaw("avg(t.size)"), 13);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where `id` = ? or (select avg(t.size) from `torrents` as `t`) = ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2), QVariant(13)}));
    }
}

void tst_MySql_QueryBuilder::orWhereEq_QueryableColumnAndValue() const
{
    // Following is extreme case, but it should work
    {
        auto builder = createQuery();

        builder->from("torrents")
                .whereEq(ID, 2)
                .orWhereEq([](auto &query)
        {
            query.from("torrents", "t").selectRaw("avg(t.size)");
        }, createQuery()->from("torrents", "t")
                         .selectRaw("avg(t.size)"));
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where `id` = ? or (select avg(t.size) from `torrents` as `t`) = "
                 "(select avg(t.size) from `torrents` as `t`)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }
    {
        auto builder = createQuery();

        builder->from("torrents")
                .whereEq(ID, 2)
                .orWhereEq(createQuery()->from("torrents", "t")
                                         .selectRaw("avg(t.size)"), [](auto &query)
        {
            query.from("torrents", "t").selectRaw("avg(t.size)");
        });
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where `id` = ? or (select avg(t.size) from `torrents` as `t`) = "
                 "(select avg(t.size) from `torrents` as `t`)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }
}

void tst_MySql_QueryBuilder::orWhereNot() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereNot(ID, ">", 4)
                .orWhereNot(Progress, ">=", 300);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where not `id` > ? or not `progress` >= ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(4), QVariant(300)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereNot(ID, ">", 4)
                .orWhereNotEq(NAME, "test3");
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where not `id` > ? or not `name` = ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(4), QVariant("test3")}));
    }
}

void tst_MySql_QueryBuilder::orWhereNot_ColumnExpression() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents").whereNot(Raw(ID), ">", 4)
            .orWhereNotEq(Raw("`name`"), "test3");
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where not id > ? or not `name` = ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(4), QVariant("test3")}));
}

void tst_MySql_QueryBuilder::orWhereNot_WithVectorValue() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents").whereNot({{ID, 3}, {SIZE_, 10, ">"}})
            .orWhereNot({{Progress, 100, ">="}});
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where not (`id` = ? and `size` > ?) or "
             "not (`progress` >= ?)");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(3), QVariant(10), QVariant(100)}));
}

void tst_MySql_QueryBuilder::orWhereNot_WithVectorValue_DefaultCondition() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents").whereNot({{Progress, 100, ">="}})
            .orWhereNot({{ID, 3}, {SIZE_, 10, ">"}}, AND);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where not (`progress` >= ?) or "
             "not (`id` = ? and `size` > ?)");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(100), QVariant(3), QVariant(10)}));
}

void tst_MySql_QueryBuilder::orWhereNot_WithVectorValue_ColumnExpression() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents")
            .whereNot({{Raw(ID), 3}, {Raw("`size`"), 10, ">"}})
            .orWhereNot({{Raw(Progress), 100, ">="}});
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where not (id = ? and `size` > ?) or "
             "not (progress >= ?)");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(3), QVariant(10), QVariant(100)}));
}

void tst_MySql_QueryBuilder::orWhereNotEq_QueryableValue() const
{
    // With lambda expression
    {
        auto builder = createQuery();

        builder->from("torrents")
                .whereNotEq(ID, 2)
                .orWhereNotEq(ID, [](auto &query)
        {
            query.from("torrents", "t").selectRaw("avg(t.size)");
        });
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where not `id` = ? or "
                 "not `id` = (select avg(t.size) from `torrents` as `t`)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }
    // With QueryBuilder
    {
        auto builder = createQuery();

        builder->from("torrents")
                .whereNotEq(ID, 2)
                .orWhereNotEq(ID,
                              createQuery()->from("torrents", "t")
                                            .selectRaw("avg(t.size)"));
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where not `id` = ? or "
                 "not `id` = (select avg(t.size) from `torrents` as `t`)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }
}

void tst_MySql_QueryBuilder::orWhereNotEq_QueryableColumn() const
{
    // With lambda expression
    {
        auto builder = createQuery();

        builder->from("torrents")
                .whereNotEq(ID, 2)
                .orWhereNotEq([](auto &query)
        {
            query.from("torrents", "t").selectRaw("avg(t.size)");
        }, 13);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where not `id` = ? or "
                 "not (select avg(t.size) from `torrents` as `t`) = ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2), QVariant(13)}));
    }
    // With QueryBuilder
    {
        auto builder = createQuery();

        builder->from("torrents")
                .whereNotEq(ID, 2)
                .orWhereNotEq(createQuery()->from("torrents", "t")
                                            .selectRaw("avg(t.size)"), 13);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where not `id` = ? or "
                 "not (select avg(t.size) from `torrents` as `t`) = ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2), QVariant(13)}));
    }
}

void tst_MySql_QueryBuilder::orWhereNotEq_QueryableColumnAndValue() const
{
    // Following is extreme case, but it should work
    {
        auto builder = createQuery();

        builder->from("torrents")
                .whereNotEq(ID, 2)
                .orWhereNotEq([](auto &query)
        {
            query.from("torrents", "t").selectRaw("avg(t.size)");
        }, createQuery()->from("torrents", "t")
                         .selectRaw("avg(t.size)"));
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where not `id` = ? or "
                 "not (select avg(t.size) from `torrents` as `t`) = "
                 "(select avg(t.size) from `torrents` as `t`)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }
    {
        auto builder = createQuery();

        builder->from("torrents")
                .whereNotEq(ID, 2)
                .orWhereNotEq(createQuery()->from("torrents", "t")
                                            .selectRaw("avg(t.size)"), [](auto &query)
        {
            query.from("torrents", "t").selectRaw("avg(t.size)");
        });
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` "
                 "where not `id` = ? or "
                 "not (select avg(t.size) from `torrents` as `t`) = "
                 "(select avg(t.size) from `torrents` as `t`)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }
}

void tst_MySql_QueryBuilder::whereColumn() const
{
    auto builder = createQuery();

    builder->select("*").from("torrent_previewable_files")
            .whereColumn("filepath", "=", NOTE)
            .whereColumn(SIZE_, ">=", Progress);
    QCOMPARE(builder->toSql(),
             "select * from `torrent_previewable_files` where `filepath` = `note` "
             "and `size` >= `progress`");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>());
}

void tst_MySql_QueryBuilder::orWhereColumn() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_previewable_files")
                .whereColumnEq("filepath", NOTE)
                .orWhereColumnEq(SIZE_, Progress);
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` where `filepath` = `note` "
                 "or `size` = `progress`");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_previewable_files")
                .whereColumnEq("filepath", NOTE)
                .orWhereColumn(SIZE_, ">", Progress);
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` where `filepath` = `note` "
                 "or `size` > `progress`");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>());
    }
}

void tst_MySql_QueryBuilder::orWhereColumn_ColumnExpression() const
{
    auto builder = createQuery();

    builder->select("*").from("torrent_previewable_files")
            .whereColumnEq(Raw("filepath"), Raw("`note`"))
            .orWhereColumn(Raw(SIZE_), ">", Raw(Progress));
    QCOMPARE(builder->toSql(),
             "select * from `torrent_previewable_files` where filepath = `note` "
             "or size > progress");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>());
}

void tst_MySql_QueryBuilder::whereColumn_WithVectorValue() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_previewable_files")
                .whereColumn({{"filepath", NOTE},
                              {SIZE_, Progress, ">"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` where (`filepath` = `note` "
                 "and `size` > `progress`)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_previewable_files")
                .whereColumn({{"filepath", NOTE},
                              {SIZE_, Progress, ">", "or"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` where (`filepath` = `note` "
                 "or `size` > `progress`)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>());
    }
}

void tst_MySql_QueryBuilder::orWhereColumn_WithVectorValue() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_previewable_files").whereEq(ID, 2)
                .orWhereColumn({{"filepath", NOTE},
                                {SIZE_, Progress, ">"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` "
                 "where `id` = ? or (`filepath` = `note` or `size` > `progress`)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_previewable_files").whereEq(ID, 2)
                .orWhereColumn({{"filepath", NOTE},
                                {SIZE_, Progress, ">", "and"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` "
                 "where `id` = ? or (`filepath` = `note` and `size` > `progress`)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_previewable_files").whereEq(ID, 2)
                .orWhereColumn({{"filepath", NOTE},
                                {SIZE_, Progress, ">", "or"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` "
                 "where `id` = ? or (`filepath` = `note` or `size` > `progress`)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }
}

void tst_MySql_QueryBuilder::orWhereColumn_WithVectorValue_ColumnExpression() const
{
    auto builder = createQuery();

    builder->select("*").from("torrent_previewable_files").whereEq(ID, 2)
            .orWhereColumn({{Raw("filepath"), Raw("`note`")},
                            {SIZE_, Raw(Progress), ">"}});
    QCOMPARE(builder->toSql(),
             "select * from `torrent_previewable_files` "
             "where `id` = ? or (filepath = `note` or `size` > progress)");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(2)}));
}

void tst_MySql_QueryBuilder::whereIn() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereIn(ID, {2, 3, 4});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` in (?, ?, ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2), QVariant(3), QVariant(4)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, "=", 1)
                .orWhereIn(ID, {2, 3, 4});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? or `id` in (?, ?, ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(1), QVariant(2),
                                  QVariant(3), QVariant(4)}));
    }
}

void tst_MySql_QueryBuilder::whereNotIn() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereNotIn(ID, {2, 3, 4});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` not in (?, ?, ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2), QVariant(3), QVariant(4)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, "=", 1)
                .orWhereNotIn(ID, {2, 3, 4});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? or `id` not in (?, ?, ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(1), QVariant(2),
                                  QVariant(3), QVariant(4)}));
    }
}

void tst_MySql_QueryBuilder::whereNotIn_ColumnExpression() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents").where(ID, "=", 1)
            .orWhereNotIn(Raw(ID), {2, 3, 4});
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where `id` = ? or id not in (?, ?, ?)");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(1), QVariant(2), QVariant(3), QVariant(4)}));
}

void tst_MySql_QueryBuilder::whereIn_Empty() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereIn(ID, {});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where 0 = 1");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, "=", 1)
                .orWhereIn(ID, {});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? or 0 = 1");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(1)}));
    }
}

void tst_MySql_QueryBuilder::whereNotIn_Empty() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereNotIn(ID, {});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where 1 = 1");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, "=", 1)
                .orWhereNotIn(ID, {});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? or 1 = 1");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(1)}));
    }
}

void tst_MySql_QueryBuilder::whereIn_ValueExpression() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereIn(ID, {Raw(3)});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` in (3)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereEq(ID, 2)
                .orWhereIn(ID, {Raw(3)});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? or `id` in (3)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereIn(NAME, {Raw("'xyz'")});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `name` in ('xyz')");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>());
    }
}

void tst_MySql_QueryBuilder::whereNull() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereNull("seeds");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `seeds` is null");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4)
                .whereNull("seeds");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? and `seeds` is null");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(4)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 3)
                .orWhereNull("seeds");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? or `seeds` is null");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(3)}));
    }
}

void tst_MySql_QueryBuilder::whereNotNull() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereNotNull("seeds");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `seeds` is not null");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4)
                .whereNotNull("seeds");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? and `seeds` is not null");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(4)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 3)
                .orWhereNotNull("seeds");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? or `seeds` is not null");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(3)}));
    }
}

void tst_MySql_QueryBuilder::whereNotNull_ColumnExpression() const
{
    auto builder = createQuery();

    builder->select("*").from("torrent_peers").whereEq(ID, 3)
            .orWhereNotNull(Raw("seeds"));
    QCOMPARE(builder->toSql(),
             "select * from `torrent_peers` where `id` = ? or seeds is not null");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(3)}));
}

void tst_MySql_QueryBuilder::whereNull_WithVectorValue() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereNull({"seeds", "total_seeds"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `seeds` is null "
                 "and `total_seeds` is null");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4)
                .whereNull({"seeds", "total_seeds"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? and `seeds` is null "
                 "and `total_seeds` is null");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(4)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 3)
                .orWhereNull({"seeds", "total_seeds"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? or `seeds` is null "
                 "or `total_seeds` is null");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(3)}));
    }
}

void tst_MySql_QueryBuilder::whereNotNull_WithVectorValue() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereNotNull({"seeds",
                                                                 "total_seeds"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `seeds` is not null "
                 "and `total_seeds` is not null");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4)
                .whereNotNull({"seeds", "total_seeds"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? and `seeds` is not null "
                 "and `total_seeds` is not null");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(4)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 3)
                .orWhereNotNull({"seeds", "total_seeds"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? or `seeds` is not null "
                 "or `total_seeds` is not null");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(3)}));
    }
}

void tst_MySql_QueryBuilder::whereBetween() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereBetween("size", {12, 14});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `size` between ? and ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(12), QVariant(14)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, GT, 2)
                .orWhereBetween("size", {12, 14});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? or `size` between ? and ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2), QVariant(12), QVariant(14)}));
    }
}

void tst_MySql_QueryBuilder::whereNotBetween() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereNotBetween("size", {12, 14});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `size` not between ? and ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(12), QVariant(14)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, GT, 2)
                .orWhereNotBetween("size", {12, 14});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? or `size` not between ? and ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2), QVariant(12), QVariant(14)}));
    }
}

void tst_MySql_QueryBuilder::whereBetween_ColumnExpression() const
{
    // min. value as expression
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, GT, 2)
                .orWhereBetween("size", {DB::raw(12), 14});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? or `size` between 12 and ?");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2), QVariant(14)}));
    }
    // max. value as expression
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, GT, 2)
                .orWhereBetween("size", {12, DB::raw(14)});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? or `size` between ? and 14");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2), QVariant(12)}));
    }
    // Both min. and max. values as expressions
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, GT, 2)
                .orWhereBetween("size", {DB::raw(12), DB::raw(14)});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? or `size` between 12 and 14");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }
}

void tst_MySql_QueryBuilder::whereBetweenColumns() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents")
                .whereBetweenColumns("size", {"min", "max"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `size` between `min` and `max`");
        QVERIFY(builder->getBindings().isEmpty());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, GT, 2)
                .orWhereBetweenColumns("size", {"min", "max"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? or "
                 "`size` between `min` and `max`");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }
}

void tst_MySql_QueryBuilder::whereNotBetweenColumns() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents")
                .whereNotBetweenColumns("size", {"min", "max"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `size` not between `min` and `max`");
        QVERIFY(builder->getBindings().isEmpty());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, GT, 2)
                .orWhereNotBetweenColumns("size", {"min", "max"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? or "
                 "`size` not between `min` and `max`");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }
}

void tst_MySql_QueryBuilder::whereBetweenColumns_ColumnExpression() const
{
    // min. column name as expression
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, GT, 2)
                .orWhereBetweenColumns("size", {DB::raw("min"), "max"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? or "
                 "`size` between min and `max`");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }
    // max. column name as expression
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, GT, 2)
                .orWhereBetweenColumns("size", {"min", DB::raw("max")});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? or "
                 "`size` between `min` and max");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }
    // Both min. and max. column names as expressions
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, GT, 2)
                .orWhereBetweenColumns("size", {DB::raw("min"), DB::raw("max")});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? or "
                 "`size` between min and max");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(2)}));
    }
}

void tst_MySql_QueryBuilder::whereExists() const
{
    // With lambda expression
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").where(ID, LT, 7)
                .whereExists([](Builder &query)
        {
            query.from("torrents").where(SIZE_, LT, 15);
        });

        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` < ? and "
                 "exists (select * from `torrents` where `size` < ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(7), QVariant(15)}));
    }
    // With QueryBuilder &
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").where(ID, LT, 7)
                .whereExists(createQuery()->from("torrents")
                                           .where(SIZE_, LT, 15));

        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` < ? and "
                 "exists (select * from `torrents` where `size` < ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(7), QVariant(15)}));
    }
    // With std::shared_ptr<QueryBuilder>
    {
        auto builder = createQuery();

        // Ownership of the std::shared_ptr<QueryBuilder>
        const auto builderForExists = createQuery();
        builderForExists->from("torrents").where(SIZE_, LT, 15);

        builder->select("*").from("torrent_peers").where(ID, LT, 7)
                .whereExists(builderForExists);

        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` < ? and "
                 "exists (select * from `torrents` where `size` < ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(7), QVariant(15)}));
    }
}

void tst_MySql_QueryBuilder::whereNotExists() const
{
    // With lambda expression
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").where(ID, LT, 7)
                .whereNotExists([](Builder &query)
        {
            query.from("torrents").where(SIZE_, LT, 15);
        });

        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` < ? and "
                 "not exists (select * from `torrents` where `size` < ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(7), QVariant(15)}));
    }
    // With QueryBuilder &
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").where(ID, LT, 7)
                .whereNotExists(createQuery()->from("torrents")
                                              .where(SIZE_, LT, 15));

        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` < ? and "
                 "not exists (select * from `torrents` where `size` < ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(7), QVariant(15)}));
    }
    // With std::shared_ptr<QueryBuilder>
    {
        auto builder = createQuery();

        // Ownership of the std::shared_ptr<QueryBuilder>
        const auto builderForExists = createQuery();
        builderForExists->from("torrents").where(SIZE_, LT, 15);

        builder->select("*").from("torrent_peers").where(ID, LT, 7)
                .whereNotExists(builderForExists);

        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` < ? and "
                 "not exists (select * from `torrents` where `size` < ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(7), QVariant(15)}));
    }
}

void tst_MySql_QueryBuilder::orWhereExists() const
{
    // With lambda expression
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").where(ID, LT, 7)
                .orWhereExists([](Builder &query)
        {
            query.from("torrents").where(SIZE_, LT, 15);
        });

        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` < ? or "
                 "exists (select * from `torrents` where `size` < ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(7), QVariant(15)}));
    }
    // With QueryBuilder &
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").where(ID, LT, 7)
                .orWhereExists(createQuery()->from("torrents")
                                             .where(SIZE_, LT, 15));

        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` < ? or "
                 "exists (select * from `torrents` where `size` < ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(7), QVariant(15)}));
    }
    // With std::shared_ptr<QueryBuilder>
    {
        auto builder = createQuery();

        // Ownership of the std::shared_ptr<QueryBuilder>
        const auto builderForExists = createQuery();
        builderForExists->from("torrents").where(SIZE_, LT, 15);

        builder->select("*").from("torrent_peers").where(ID, LT, 7)
                .orWhereExists(builderForExists);

        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` < ? or "
                 "exists (select * from `torrents` where `size` < ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(7), QVariant(15)}));
    }
}

void tst_MySql_QueryBuilder::orWhereNotExists() const
{
    // With lambda expression
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").where(ID, LT, 7)
                .orWhereNotExists([](Builder &query)
        {
            query.from("torrents").where(SIZE_, LT, 15);
        });

        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` < ? or "
                 "not exists (select * from `torrents` where `size` < ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(7), QVariant(15)}));
    }
    // With QueryBuilder &
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").where(ID, LT, 7)
                .orWhereNotExists(createQuery()->from("torrents")
                                                .where(SIZE_, LT, 15));

        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` < ? or "
                 "not exists (select * from `torrents` where `size` < ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(7), QVariant(15)}));
    }
    // With std::shared_ptr<QueryBuilder>
    {
        auto builder = createQuery();

        // Ownership of the std::shared_ptr<QueryBuilder>
        const auto builderForExists = createQuery();
        builderForExists->from("torrents").where(SIZE_, LT, 15);

        builder->select("*").from("torrent_peers").where(ID, LT, 7)
                .orWhereNotExists(builderForExists);

        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` < ? or "
                 "not exists (select * from `torrents` where `size` < ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(7), QVariant(15)}));
    }
}

void tst_MySql_QueryBuilder::whereRowValues() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents")
                .whereRowValues({NAME, SIZE_}, EQ, {"test3", 3});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where (`name`, `size`) = (?, ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(QString("test3")), QVariant(3)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents")
                .whereRowValuesEq({NAME, SIZE_}, {"test3", 3});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where (`name`, `size`) = (?, ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(QString("test3")), QVariant(3)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents")
                .whereRowValues({SIZE_, Progress}, GT, {3, 50});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where (`size`, `progress`) > (?, ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(3), QVariant(50)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, EQ, 1)
                .orWhereRowValuesEq({NAME, SIZE_}, {"test3", 3});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? or (`name`, `size`) = (?, ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(1), QVariant(QString("test3")),
                                  QVariant(3)}));
    }
}

void tst_MySql_QueryBuilder::whereRowValues_Empty() const
{
    // Different size
    TVERIFY_THROWS_EXCEPTION(
                InvalidArgumentError,
                createQuery()->select("*").from("torrents")
                .whereRowValues({NAME}, EQ, {"test3", 3}));
    TVERIFY_THROWS_EXCEPTION(
            InvalidArgumentError,
            createQuery()->select("*").from("torrents")
            .whereRowValues({}, EQ, {"test3", 3}));
    // Empty columns/values
    TVERIFY_THROWS_EXCEPTION(
            InvalidArgumentError,
            createQuery()->select("*").from("torrents")
            .whereRowValues({}, EQ, {}));
}

void tst_MySql_QueryBuilder::whereRowValues_ColumnExpression() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents").where(ID, EQ, 1)
            .orWhereRowValues({Raw(NAME), SIZE_}, EQ, {"test3", 3});
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where `id` = ? or (name, `size`) = (?, ?)");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(1), QVariant(QString("test3")),
                              QVariant(3)}));
}

void tst_MySql_QueryBuilder::whereRowValues_ValueExpression() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents")
                .whereRowValues({NAME, SIZE_}, EQ, {"test3", Raw(3)});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where (`name`, `size`) = (?, 3)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(QString("test3"))}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents")
                .whereRowValues({NAME, SIZE_}, EQ, {Raw("'test3'"), 3});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where (`name`, `size`) = ('test3', ?)");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(3)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents")
                .whereRowValuesEq({NAME, SIZE_}, {Raw("'test3'"), Raw(3)});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where (`name`, `size`) = ('test3', 3)");
        QVERIFY(builder->getBindings().isEmpty());
    }
}

/* where dates */

void tst_MySql_QueryBuilder::whereDate()
{
    auto builder = createQuery();

    QDate date(2022, 1, 12);

    builder->select("*").from("torrents")
            .whereDate(CREATED_AT, EQ, date);

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where date(`created_at`) = ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(date.toString(Qt::ISODate))}));
}

void tst_MySql_QueryBuilder::whereTime()
{
    auto builder = createQuery();

    QTime time(9, 10, 4);

    builder->select("*").from("torrents")
            .whereTime(CREATED_AT, GE, time);

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where time(`created_at`) >= ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(time.toString(Qt::ISODate))}));
}

void tst_MySql_QueryBuilder::whereDay()
{
    auto builder = createQuery();

    builder->select("*").from("torrents")
            .whereDay(CREATED_AT, EQ, 5);

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where day(`created_at`) = ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(5)}));
}

void tst_MySql_QueryBuilder::whereMonth()
{
    auto builder = createQuery();

    builder->select("*").from("torrents")
            .whereMonth(CREATED_AT, GE, 11);

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where month(`created_at`) >= ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(11)}));
}

void tst_MySql_QueryBuilder::whereYear()
{
    auto builder = createQuery();

    builder->select("*").from("torrents")
            .whereYear(CREATED_AT, GE, 2015);

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where year(`created_at`) >= ?");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(2015)}));
}

void tst_MySql_QueryBuilder::orderBy() const
{
    auto builder = createQuery();

    builder->from("torrents");

    builder->orderBy(NAME, ASC);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` order by `name` asc");

    builder->orderBy(ID, DESC);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` order by `name` asc, `id` desc");

    builder->reorder()
            .orderByDesc(NAME);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` order by `name` desc");

    builder->reorder(ID, ASC);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` order by `id` asc");
}

void tst_MySql_QueryBuilder::latestOldest() const
{
    auto builder = createQuery();

    builder->from("torrents");

    builder->latest(NAME);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` order by `name` desc");

    builder->reorder().oldest(NAME);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` order by `name` asc");
}

void tst_MySql_QueryBuilder::inRandomOrder() const
{
    {
        auto builder = createQuery();

        builder->from("torrents");

        builder->inRandomOrder();
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` order by RAND()");
    }
    {
        auto builder = createQuery();

        builder->from("torrents");

        builder->inRandomOrder("123");
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` order by RAND(123)");
    }
    {
        auto builder = createQuery();

        builder->from("torrents");

        builder->inRandomOrder("`column_name`");
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` order by RAND(`column_name`)");
    }
}

void tst_MySql_QueryBuilder::limitOffset() const
{
    auto builder = createQuery();

    builder->from("torrents");

    builder->limit(10);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` limit 10");

    builder->offset(5);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` limit 10 offset 5");
}

void tst_MySql_QueryBuilder::takeSkip() const
{
    auto builder = createQuery();

    builder->from("torrents");

    builder->take(15);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` limit 15");

    builder->skip(5);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` limit 15 offset 5");
}

void tst_MySql_QueryBuilder::forPage() const
{
    auto builder = createQuery();

    builder->from("torrents");

    builder->forPage(2, 10);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` limit 10 offset 10");

    builder->forPage(5);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` limit 30 offset 120");
}

void tst_MySql_QueryBuilder::lock() const
{
    // lock for update
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4).lock();
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? for update");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(4)}));
    }
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4).lockForUpdate();
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? for update");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(4)}));
    }
    // shared lock
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4).lock(false);
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? lock in share mode");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(4)}));
    }
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4).sharedLock();
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? lock in share mode");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(4)}));
    }
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4)
                .lock("lock in share mode");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? lock in share mode");
        QCOMPARE(builder->getBindings(),
                 QList<QVariant>({QVariant(4)}));
    }
}

void tst_MySql_QueryBuilder::insert() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").insert({{NAME, "xyz"}, {SIZE_, 6}});
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "insert into `torrents` (`name`, `size`) values (?, ?)");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant("xyz"), QVariant(6)}));
}

void tst_MySql_QueryBuilder::insert_WithExpression() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents")
                .insert({{NAME, DB::raw("'xyz'")}, {SIZE_, 6},
                         {Progress, DB::raw(2)}});
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "insert into `torrents` (`name`, `progress`, `size`) values ('xyz', 2, ?)");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(6)}));
}

void tst_MySql_QueryBuilder::update() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        const auto [affected, query] = connection.query()->from("torrents")
                                       .whereEq(ID, 10)
                                       .update({{NAME, "xyz"}, {SIZE_, 6}});

        // Affecting statements must return -1 if pretending
        QVERIFY(affected == -1);
        QVERIFY(!query.isActive());
        QVERIFY(!query.isValid());
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "update `torrents` set `name` = ?, `size` = ? where `id` = ?");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant("xyz"), QVariant(6), QVariant(10)}));
}

void tst_MySql_QueryBuilder::update_WithExpression() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents")
                .whereEq(ID, 10)
                .update({{NAME, DB::raw("'xyz'")}, {SIZE_, 6},
                         {Progress, DB::raw(2)}});
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "update `torrents` set `name` = 'xyz', `size` = ?, `progress` = 2 "
             "where `id` = ?");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(6), QVariant(10)}));
}

void tst_MySql_QueryBuilder::upsert_UseUpsertAlias() const
{
    const auto configVersion = sl("8.0.19");

    // Need to be set before pretending
    auto &mysqlConnection = dynamic_cast<MySqlConnection &>(DB::connection(m_connection));
    mysqlConnection.setConfigVersion(configVersion);

    auto log = mysqlConnection.pretend([](auto &connection)
    {
        connection.query()->from("tag_properties")
                .upsert({{{"tag_id", 1}, {"color", "pink"},   {"position", 0}},
                         {{"tag_id", 1}, {"color", "purple"}, {"position", 4}}},
                        {"position"},
                        {"color"});
    });

    // MySQL >=8.0.19 uses upsert alias
    const auto useUpsertAlias = mysqlConnection.useUpsertAlias();
    QVERIFY(useUpsertAlias);

    // Also verify other connection data members in the game
    QCOMPARE(mysqlConnection.isMaria(), false);
    QCOMPARE(mysqlConnection.version(), std::make_optional(configVersion));

    QCOMPARE(log.size(), 1);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "insert into `tag_properties` (`color`, `position`, `tag_id`) "
             "values (?, ?, ?), (?, ?, ?) as `tinyorm_upsert_alias` "
             "on duplicate key update "
             "`color` = `tinyorm_upsert_alias`.`color`");
    QCOMPARE(log0.boundValues,
             QList<QVariant>({QVariant(QString("pink")),   QVariant(0), QVariant(1),
                              QVariant(QString("purple")), QVariant(4), QVariant(1)}));
}

void tst_MySql_QueryBuilder::upsert_UseUpsertAlias_Disabled() const
{
    const auto configVersion = sl("8.0.18");

    // Need to be set before pretending
    auto &mysqlConnection = dynamic_cast<MySqlConnection &>(DB::connection(m_connection));
    mysqlConnection.setConfigVersion(configVersion);

    auto log = mysqlConnection.pretend([](auto &connection)
    {
        connection.query()->from("tag_properties")
                .upsert({{{"tag_id", 1}, {"color", "pink"},   {"position", 0}},
                         {{"tag_id", 1}, {"color", "purple"}, {"position", 4}}},
                        {"position"},
                        {"color"});
    });

    // MySQL <8.0.19 doesn't use upsert alias
    const auto useUpsertAlias = mysqlConnection.useUpsertAlias();
    QVERIFY(!useUpsertAlias);

    // Also verify other connection data members in the game
    QCOMPARE(mysqlConnection.isMaria(), false);
    QCOMPARE(mysqlConnection.version(), std::make_optional(configVersion));

    QCOMPARE(log.size(), 1);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "insert into `tag_properties` (`color`, `position`, `tag_id`) "
             "values (?, ?, ?), (?, ?, ?) "
             "on duplicate key update `color` = values(`color`)");
    QCOMPARE(log0.boundValues,
             QList<QVariant>({QVariant(QString("pink")),   QVariant(0), QVariant(1),
                              QVariant(QString("purple")), QVariant(4), QVariant(1)}));
}

void tst_MySql_QueryBuilder::upsert_UseUpsertAlias_DefaultValue() const
{
    auto &mysqlConnection = dynamic_cast<MySqlConnection &>(DB::connection(m_connection));
    mysqlConnection.setConfigVersion({});

    auto log = mysqlConnection.pretend([&mysqlConnection](auto &connection)
    {
        connection.query()->from("tag_properties")
                .upsert({{{"tag_id", 1}, {"color", "pink"},   {"position", 0}},
                         {{"tag_id", 1}, {"color", "purple"}, {"position", 4}}},
                        {"position"},
                        {"color"});

        /* Default value for the use upsert alias feature during pretending will be false
           because no version was provided through the database configuration. */
        const auto useUpsertAlias = mysqlConnection.useUpsertAlias();
        QVERIFY(!useUpsertAlias);

        // Also verify other connection data members in the game
        QCOMPARE(mysqlConnection.isMaria(), false);
        QCOMPARE(mysqlConnection.version(), std::nullopt);
    });

    QCOMPARE(log.size(), 1);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "insert into `tag_properties` (`color`, `position`, `tag_id`) "
             "values (?, ?, ?), (?, ?, ?) "
             "on duplicate key update `color` = values(`color`)");
    QCOMPARE(log0.boundValues,
             QList<QVariant>({QVariant(QString("pink")),   QVariant(0), QVariant(1),
                              QVariant(QString("purple")), QVariant(4), QVariant(1)}));
}

void tst_MySql_QueryBuilder::upsert_UseUpsertAlias_Maria() const
{
    const auto configVersion = sl("11.3.2-MariaDB");

    // Need to be set before pretending
    auto &mysqlConnection = dynamic_cast<MySqlConnection &>(DB::connection(m_connection));
    mysqlConnection.setConfigVersion(configVersion);

    auto log = mysqlConnection.pretend([](auto &connection)
    {
        connection.query()->from("tag_properties")
                .upsert({{{"tag_id", 1}, {"color", "pink"},   {"position", 0}},
                         {{"tag_id", 1}, {"color", "purple"}, {"position", 4}}},
                        {"position"},
                        {"color"});
    });

    // MariaDB doesn't use/support the upsert alias
    const auto useUpsertAlias = mysqlConnection.useUpsertAlias();
    QVERIFY(!useUpsertAlias);

    // Also verify other connection data members in the game
    QCOMPARE(mysqlConnection.isMaria(), true);
    QCOMPARE(mysqlConnection.version(), std::make_optional(configVersion));

    QCOMPARE(log.size(), 1);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "insert into `tag_properties` (`color`, `position`, `tag_id`) "
             "values (?, ?, ?), (?, ?, ?) "
             "on duplicate key update `color` = values(`color`)");
    QCOMPARE(log0.boundValues,
             QList<QVariant>({QVariant(QString("pink")),   QVariant(0), QVariant(1),
                              QVariant(QString("purple")), QVariant(4), QVariant(1)}));
}

void tst_MySql_QueryBuilder::upsert_WithoutUpdate_UpdateAll_UseUpsertAlias() const
{
    const auto configVersion = sl("8.0.19");

    // Need to be set before pretending
    auto &mysqlConnection = dynamic_cast<MySqlConnection &>(DB::connection(m_connection));
    mysqlConnection.setConfigVersion(configVersion);

    auto log = mysqlConnection.pretend([](auto &connection)
    {
        connection.query()->from("tag_properties")
                .upsert({{{"tag_id", 2}, {"color", "pink"},   {"position", 0}},
                         {{"tag_id", 1}, {"color", "purple"}, {"position", 4}}},
                        {"position"});
    });

    // MySQL >=8.0.19 uses upsert alias
    const auto useUpsertAlias = mysqlConnection.useUpsertAlias();
    QVERIFY(useUpsertAlias);

    // Also verify other connection data members in the game
    QCOMPARE(mysqlConnection.isMaria(), false);
    QCOMPARE(mysqlConnection.version(), configVersion);

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "insert into `tag_properties` (`color`, `position`, `tag_id`) "
             "values (?, ?, ?), (?, ?, ?) as `tinyorm_upsert_alias` "
             "on duplicate key update "
             "`color` = `tinyorm_upsert_alias`.`color`, "
             "`position` = `tinyorm_upsert_alias`.`position`, "
             "`tag_id` = `tinyorm_upsert_alias`.`tag_id`");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(QString("pink")),   QVariant(0), QVariant(2),
                              QVariant(QString("purple")), QVariant(4), QVariant(1)}));
}

void tst_MySql_QueryBuilder::
     upsert_WithoutUpdate_UpdateAll_UseUpsertAlias_Disabled() const
{
    const auto configVersion = sl("8.0.18");

    // Need to be set before pretending
    auto &mysqlConnection = dynamic_cast<MySqlConnection &>(DB::connection(m_connection));
    mysqlConnection.setConfigVersion(configVersion);

    auto log = mysqlConnection.pretend([](auto &connection)
    {
        connection.query()->from("tag_properties")
                .upsert({{{"tag_id", 2}, {"color", "pink"},   {"position", 0}},
                         {{"tag_id", 1}, {"color", "purple"}, {"position", 4}}},
                        {"position"});
    });

    // MySQL <8.0.19 doesn't use upsert alias
    const auto useUpsertAlias = mysqlConnection.useUpsertAlias();
    QVERIFY(!useUpsertAlias);

    // Also verify other connection data members in the game
    QCOMPARE(mysqlConnection.isMaria(), false);
    QCOMPARE(mysqlConnection.version(), configVersion);

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "insert into `tag_properties` (`color`, `position`, `tag_id`) "
             "values (?, ?, ?), (?, ?, ?) "
             "on duplicate key update `color` = values(`color`), "
             "`position` = values(`position`), "
             "`tag_id` = values(`tag_id`)");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(QString("pink")),   QVariant(0), QVariant(2),
                              QVariant(QString("purple")), QVariant(4), QVariant(1)}));
}

void tst_MySql_QueryBuilder::upsert_WithoutUpdate_UpdateAll_UseUpsertAlias_Maria() const
{
    const auto configVersion = sl("11.3.2-MariaDB");

    // Need to be set before pretending
    auto &mysqlConnection = dynamic_cast<MySqlConnection &>(DB::connection(m_connection));
    mysqlConnection.setConfigVersion(configVersion);

    auto log = mysqlConnection.pretend([](auto &connection)
    {
        connection.query()->from("tag_properties")
                .upsert({{{"tag_id", 2}, {"color", "pink"},   {"position", 0}},
                         {{"tag_id", 1}, {"color", "purple"}, {"position", 4}}},
                        {"position"});
    });

    // MariaDB doesn't use/support the upsert alias
    const auto useUpsertAlias = mysqlConnection.useUpsertAlias();
    QVERIFY(!useUpsertAlias);

    // Also verify other connection data members in the game
    QCOMPARE(mysqlConnection.isMaria(), true);
    QCOMPARE(mysqlConnection.version(), configVersion);

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "insert into `tag_properties` (`color`, `position`, `tag_id`) "
             "values (?, ?, ?), (?, ?, ?) "
             "on duplicate key update `color` = values(`color`), "
             "`position` = values(`position`), "
             "`tag_id` = values(`tag_id`)");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(QString("pink")),   QVariant(0), QVariant(2),
                              QVariant(QString("purple")), QVariant(4), QVariant(1)}));
}

void tst_MySql_QueryBuilder::remove() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").remove(2222);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "delete from `torrents` where `torrents`.`id` = ?");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(2222)}));
}

void tst_MySql_QueryBuilder::remove_WithExpression() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").remove(DB::raw(2223));
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "delete from `torrents` where `torrents`.`id` = 2223");
    QVERIFY(firstLog.boundValues.isEmpty());
}

/* Builds Queries */

void tst_MySql_QueryBuilder::tap() const
{
    auto builder = createQuery();

    auto callbackInvoked = false;
    auto &tappedBuilder = builder->tap([&callbackInvoked](QueryBuilder &query)
    {
        callbackInvoked = true;

        return query;
    });

    QVERIFY((std::is_same_v<decltype (tappedBuilder), decltype (*builder)>));
    QVERIFY(callbackInvoked);
    // It must be the same QueryBuilder (the same memory address)
    QVERIFY(std::addressof(*builder) == std::addressof(tappedBuilder));
}

void tst_MySql_QueryBuilder::sole() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").whereEq(NAME, dummy_NONEXISTENT).sole();
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select * from `torrents` where `name` = ? limit 2");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(dummy_NONEXISTENT)}));
}

void tst_MySql_QueryBuilder::soleValue() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents")
                .whereEq(NAME, dummy_NONEXISTENT)
                .soleValue(NAME);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select `name` from `torrents` where `name` = ? limit 2");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(dummy_NONEXISTENT)}));
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

std::shared_ptr<QueryBuilder>
tst_MySql_QueryBuilder::createQuery() const
{
    return DB::connection(m_connection).query();
}

QTEST_MAIN(tst_MySql_QueryBuilder)

#include "tst_mysql_querybuilder.moc"
