#include <QCoreApplication>
#include <QtTest>

#include "orm/db.hpp"
#include "orm/query/querybuilder.hpp"

#include "databases.hpp"

using namespace Orm::Constants;

using Orm::Query::Expression;

using Raw = Orm::Query::Expression;
using QueryBuilder = Orm::Query::Builder;

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

class tst_MySql_QueryBuilder : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void get() const;
    void get_ColumnExpression() const;

    void find() const;
    void find_ColumnAndValueExpression() const;

    void first() const;
    void first_ColumnExpression() const;

    void value() const;
    void value_ColumnExpression() const;

    void count() const;
    void count_Distinct() const;
    void min_Aggregate() const;
    void min_Aggregate_ColumnExpression() const;
    void max_Aggregate() const;
    void sum_Aggregate() const;
    void average_Aggregate() const;

    void select() const;
    void select_ColumnExpression() const;
    void addSelect() const;
    void addSelect_ColumnExpression() const;

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
    void where_QueryableValue() const;
    void where_QueryableColumn() const;

    void orWhere() const;
    void orWhere_ColumnExpression() const;
    void orWhere_WithVectorValue() const;
    void orWhere_WithVectorValue_ColumnExpression() const;
    void orWhereEq_QueryableValue() const;
    void orWhereEq_QueryableColumn() const;
    void orWhereEq_QueryableColumnAndValue() const;

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

    void orderBy() const;
    void latestOldest() const;

    void limitOffset() const;
    void takeSkip() const;
    void forPage() const;

    void lock() const;

    void insert() const;
    void insert_WithExpression() const;

    void update() const;
    void update_WithExpression() const;

    void remove() const;
    void remove_WithExpression() const;

private:
    /*! Create QueryBuilder instance for the given connection. */
    QSharedPointer<QueryBuilder> createQuery() const;

    /*! Connection name used in this test case. */
    QString m_connection = {};
};

void tst_MySql_QueryBuilder::initTestCase()
{
    m_connection = Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(QStringLiteral("%1 autotest skipped, environment variables "
                             "for '%2' connection have not been defined.")
              .arg("tst_MySql_QueryBuilder", Databases::MYSQL).toUtf8().constData(), );
}

void tst_MySql_QueryBuilder::get() const
{
    {
        auto log = DB::connection(m_connection).pretend([](auto &connection)
        {
            connection.query()->from("torrents").get({ID, NAME});
        });

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

    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select `id`, `name` from `torrents` where `id` = ? limit 1");
    QCOMPARE(firstLog.boundValues,
             QVector<QVariant>({QVariant(3)}));
}

void tst_MySql_QueryBuilder::find_ColumnAndValueExpression() const
{
    {
        auto log = DB::connection(m_connection).pretend([](auto &connection)
        {
            connection.query()->from("torrents").find(3, {ID, Raw(NAME)});
        });

        const auto &firstLog = log.first();

        QCOMPARE(log.size(), 1);
        QCOMPARE(firstLog.query,
                 "select `id`, name from `torrents` where `id` = ? limit 1");
        QCOMPARE(firstLog.boundValues,
                 QVector<QVariant>({QVariant(3)}));
    }

    {
        auto log = DB::connection(m_connection).pretend([](auto &connection)
        {
            connection.query()->from("torrents").find(Raw("1 + 3"), {ID, Raw(NAME)});
        });

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

    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select name from `torrents` limit 1");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_QueryBuilder::count() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").count();
    });

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
            connection.query()->from("torrents").distinct().count("size");
        });

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
            connection.query()->from("torrents").distinct().count({"size", "note"});
        });

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
            connection.query()->from("torrents").distinct({"size", "note"}).count();
        });

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
        connection.query()->from("torrents").min("size");
    });

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
        connection.query()->from("torrents").min(Raw("size"));
    });

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
        connection.query()->from("torrents").max("size");
    });

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
        connection.query()->from("torrents").sum("size");
    });

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
        connection.query()->from("torrents").avg("size");
    });

    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select avg(`size`) as `aggregate` from `torrents`");
    QVERIFY(firstLog.boundValues.isEmpty());
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

void tst_MySql_QueryBuilder::addSelect() const
{
    auto builder = createQuery();

    builder->from("torrents");

    builder->addSelect({ID, NAME});
    QCOMPARE(builder->toSql(),
             "select `id`, `name` from `torrents`");

    builder->addSelect("size");
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
             QVector<QVariant>({QVariant(10), QVariant(1520652582)}));
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

    // Ownership of the QSharedPointer<QueryBuilder>
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
             QVector<QVariant>({QVariant(5), QVariant("xyz")}));
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
             QVector<QVariant>({QVariant(5), QVariant("xyz")}));
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

    builder->select({NAME, "size"});
    QCOMPARE(builder->toSql(),
             "select distinct `name`, `size` from `torrents`");
}

void tst_MySql_QueryBuilder::from() const
{
    auto builder = createQuery();

    const auto &from = builder->getFrom();

    QVERIFY(std::holds_alternative<std::monostate>(from));

    const auto tableTorrents = QStringLiteral("torrents");
    builder->from(tableTorrents);

    QVERIFY(std::holds_alternative<QString>(from));
    QCOMPARE(std::get<QString>(from), tableTorrents);
    QCOMPARE(builder->toSql(),
             "select * from `torrents`");

    const auto tableTorrentPeers = QStringLiteral("torrent_peers");
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
        const auto table = QStringLiteral("some`table");
        builder->from(table);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from), table);
        QCOMPARE(builder->toSql(),
                 "select * from `some``table`");
    }
    {
        const auto table = QStringLiteral("some\"table");
        builder->from(table);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from), table);
        QCOMPARE(builder->toSql(),
                 "select * from `some\"table`");
    }
    {
        const auto table = QStringLiteral("some'table");
        builder->from(table);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from), table);
        QCOMPARE(builder->toSql(),
                 "select * from `some'table`");
    }
    // Wrapping as whole constant
    {
        const auto table = QStringLiteral("baz");
        builder->select("x.y as foo.bar").from(table);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from), table);
        QCOMPARE(builder->toSql(),
                 "select `x`.`y` as `foo.bar` from `baz`");
    }
    // Wrapping with space in database name
    {
        const auto table = QStringLiteral("baz");
        builder->select("w x.y.z as foo.bar").from(table);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from), table);
        QCOMPARE(builder->toSql(),
                 "select `w x`.`y`.`z` as `foo.bar` from `baz`");
    }
    // Wrapping with as
    {
        const auto table = QStringLiteral("table as alias");
        builder->select("*").from(table);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from), table);
        QCOMPARE(builder->toSql(),
                 "select * from `table` as `alias`");
    }
    // Wrapping with as
    {
        const auto table = QStringLiteral("table");
        const auto alias = QStringLiteral("alias");
        builder->from(table, alias);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from),
                 QStringLiteral("%1 as %2").arg(table, alias));
        QCOMPARE(builder->toSql(),
                 "select * from `table` as `alias`");
    }
}

void tst_MySql_QueryBuilder::from_WithPrefix() const
{
    auto builder = createQuery();

    const auto prefix = QStringLiteral("xyz_");
    const auto table = QStringLiteral("table");
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

    const auto prefix = QStringLiteral("xyz_");
    builder->getConnection().setTablePrefix(prefix);

    {
        const auto table = QStringLiteral("table");
        const auto alias = QStringLiteral("alias");
        builder->from(table, alias);

        QVERIFY(std::holds_alternative<QString>(from));
        QCOMPARE(std::get<QString>(from),
                 QStringLiteral("%1 as %2").arg(table, alias));
        QCOMPARE(builder->toSql(),
                 "select * from `xyz_table` as `xyz_alias`");
    }

    {
        const auto table = QStringLiteral("table as alias");
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
             QVector<QVariant>({QVariant(1520652582)}));
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
             QVector<QVariant>({QVariant(10), QVariant(1520652582)}));
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

    // Ownership of the QSharedPointer<QueryBuilder>
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
             QVector<QVariant>({QVariant(5), QVariant("xyz")}));
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
             QVector<QVariant>({QVariant(5), QVariant("xyz")}));
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

    // Ownership of the QSharedPointer<QueryBuilder>
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
             QVector<QVariant>({QVariant(5), QVariant("xyz")}));
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
             QVector<QVariant>({QVariant(5), QVariant("xyz")}));
}

void tst_MySql_QueryBuilder::where() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, "=", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereEq(ID, 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereEq(ID, 3)
                .whereEq(NAME, "test3");
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? and `name` = ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(3), QVariant("test3")}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, "!=", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` != ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, "<>", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` <> ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, ">", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, ">", 3)
                .where(NAME, LIKE, "test%");
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? and `name` like ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(3), QVariant("test%")}));
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
                 QVector<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where({{ID, 3}, {"size", 10, ">"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where (`id` = ? and `size` > ?)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(3), QVariant(10)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where({{ID, 3}, {"size", 10, ">"}})
                .where({{"progress", 100, ">="}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where (`id` = ? and `size` > ?) "
                 "and (`progress` >= ?)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(3), QVariant(10), QVariant(100)}));
    }
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
                 QVector<QVariant>({QVariant(13)}));
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
                 QVector<QVariant>({QVariant(13)}));
    }
}

void tst_MySql_QueryBuilder::orWhere() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, ">", 4)
                .orWhere("progress", ">=", 300);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? or `progress` >= ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4), QVariant(300)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, ">", 4)
                .orWhereEq(NAME, "test3");
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? or `name` = ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4), QVariant("test3")}));
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
             QVector<QVariant>({QVariant(4), QVariant("test3")}));
}

void tst_MySql_QueryBuilder::orWhere_WithVectorValue() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents").where({{ID, 3}, {"size", 10, ">"}})
            .orWhere({{"progress", 100, ">="}});
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where (`id` = ? and `size` > ?) or "
             "(`progress` >= ?)");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant(3), QVariant(10), QVariant(100)}));
}

void tst_MySql_QueryBuilder::orWhere_WithVectorValue_ColumnExpression() const
{
    auto builder = createQuery();

    builder->select("*").from("torrents")
            .where({{Raw(ID), 3}, {Raw("`size`"), 10, ">"}})
            .orWhere({{Raw("progress"), 100, ">="}});
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where (id = ? and `size` > ?) or "
             "(progress >= ?)");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant(3), QVariant(10), QVariant(100)}));
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
                 QVector<QVariant>({QVariant(2)}));
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
                 QVector<QVariant>({QVariant(2)}));
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
                 QVector<QVariant>({QVariant(2), QVariant(13)}));
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
                 QVector<QVariant>({QVariant(2), QVariant(13)}));
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
                 QVector<QVariant>({QVariant(2)}));
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
                 QVector<QVariant>({QVariant(2)}));
    }
}

void tst_MySql_QueryBuilder::whereColumn() const
{
    auto builder = createQuery();

    builder->select("*").from("torrent_previewable_files")
            .whereColumn("filepath", "=", "note")
            .whereColumn("size", ">=", "progress");
    QCOMPARE(builder->toSql(),
             "select * from `torrent_previewable_files` where `filepath` = `note` "
             "and `size` >= `progress`");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>());
}

void tst_MySql_QueryBuilder::orWhereColumn() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_previewable_files")
                .whereColumnEq("filepath", "note")
                .orWhereColumnEq("size", "progress");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` where `filepath` = `note` "
                 "or `size` = `progress`");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_previewable_files")
                .whereColumnEq("filepath", "note")
                .orWhereColumn("size", ">", "progress");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` where `filepath` = `note` "
                 "or `size` > `progress`");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>());
    }
}

void tst_MySql_QueryBuilder::orWhereColumn_ColumnExpression() const
{
    auto builder = createQuery();

    builder->select("*").from("torrent_previewable_files")
            .whereColumnEq(Raw("filepath"), Raw("`note`"))
            .orWhereColumn(Raw("size"), ">", Raw("progress"));
    QCOMPARE(builder->toSql(),
             "select * from `torrent_previewable_files` where filepath = `note` "
             "or size > progress");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>());
}

void tst_MySql_QueryBuilder::whereColumn_WithVectorValue() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_previewable_files")
                .whereColumn({{"filepath", "note"},
                              {"size", "progress", ">"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` where (`filepath` = `note` "
                 "and `size` > `progress`)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_previewable_files")
                .whereColumn({{"filepath", "note"},
                              {"size", "progress", ">", "or"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` where (`filepath` = `note` "
                 "or `size` > `progress`)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>());
    }
}

void tst_MySql_QueryBuilder::orWhereColumn_WithVectorValue() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_previewable_files").whereEq(ID, 2)
                .orWhereColumn({{"filepath", "note"},
                                {"size", "progress", ">"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` "
                 "where `id` = ? or (`filepath` = `note` or `size` > `progress`)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(2)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_previewable_files").whereEq(ID, 2)
                .orWhereColumn({{"filepath", "note"},
                                {"size", "progress", ">", "and"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` "
                 "where `id` = ? or (`filepath` = `note` and `size` > `progress`)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(2)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_previewable_files").whereEq(ID, 2)
                .orWhereColumn({{"filepath", "note"},
                                {"size", "progress", ">", "or"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` "
                 "where `id` = ? or (`filepath` = `note` or `size` > `progress`)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(2)}));
    }
}

void tst_MySql_QueryBuilder::orWhereColumn_WithVectorValue_ColumnExpression() const
{
    auto builder = createQuery();

    builder->select("*").from("torrent_previewable_files").whereEq(ID, 2)
            .orWhereColumn({{Raw("filepath"), Raw("`note`")},
                            {"size", Raw("progress"), ">"}});
    QCOMPARE(builder->toSql(),
             "select * from `torrent_previewable_files` "
             "where `id` = ? or (filepath = `note` or `size` > progress)");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant(2)}));
}

void tst_MySql_QueryBuilder::whereIn() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereIn(ID, {2, 3, 4});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` in (?, ?, ?)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(2), QVariant(3), QVariant(4)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, "=", 1)
                .orWhereIn(ID, {2, 3, 4});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? or `id` in (?, ?, ?)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(1), QVariant(2),
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
                 QVector<QVariant>({QVariant(2), QVariant(3), QVariant(4)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, "=", 1)
                .orWhereNotIn(ID, {2, 3, 4});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? or `id` not in (?, ?, ?)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(1), QVariant(2),
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
             QVector<QVariant>({QVariant(1), QVariant(2), QVariant(3), QVariant(4)}));
}

void tst_MySql_QueryBuilder::whereIn_Empty() const
{
    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereIn(ID, {});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where 0 = 1");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, "=", 1)
                .orWhereIn(ID, {});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? or 0 = 1");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(1)}));
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
                 QVector<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").where(ID, "=", 1)
                .orWhereNotIn(ID, {});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? or 1 = 1");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(1)}));
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
                 QVector<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereEq(ID, 2)
                .orWhereIn(ID, {Raw(3)});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? or `id` in (3)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(2)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrents").whereIn(NAME, {Raw("'xyz'")});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `name` in ('xyz')");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>());
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
                 QVector<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4)
                .whereNull("seeds");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? and `seeds` is null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 3)
                .orWhereNull("seeds");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? or `seeds` is null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(3)}));
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
                 QVector<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4)
                .whereNotNull("seeds");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? and `seeds` is not null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 3)
                .orWhereNotNull("seeds");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? or `seeds` is not null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(3)}));
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
             QVector<QVariant>({QVariant(3)}));
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
                 QVector<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4)
                .whereNull({"seeds", "total_seeds"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? and `seeds` is null "
                 "and `total_seeds` is null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 3)
                .orWhereNull({"seeds", "total_seeds"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? or `seeds` is null "
                 "or `total_seeds` is null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(3)}));
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
                 QVector<QVariant>());
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4)
                .whereNotNull({"seeds", "total_seeds"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? and `seeds` is not null "
                 "and `total_seeds` is not null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }

    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 3)
                .orWhereNotNull({"seeds", "total_seeds"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? or `seeds` is not null "
                 "or `total_seeds` is not null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(3)}));
    }
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
                 QVector<QVariant>({QVariant(4)}));
    }
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4).lockForUpdate();
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? for update");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }
    // shared lock
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4).lock(false);
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? lock in share mode");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4).sharedLock();
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? lock in share mode");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }
    {
        auto builder = createQuery();

        builder->select("*").from("torrent_peers").whereEq(ID, 4)
                .lock("lock in share mode");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? lock in share mode");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }
}

void tst_MySql_QueryBuilder::insert() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").insert({{NAME, "xyz"}, {"size", 6}});
    });

    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "insert into `torrents` (`name`, `size`) values (?, ?)");
    QCOMPARE(firstLog.boundValues,
             QVector<QVariant>({QVariant("xyz"), QVariant(6)}));
}

void tst_MySql_QueryBuilder::insert_WithExpression() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents")
                .insert({{NAME, DB::raw("'xyz'")}, {"size", 6},
                         {"progress", DB::raw(2)}});
    });

    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "insert into `torrents` (`name`, `progress`, `size`) values ('xyz', 2, ?)");
    QCOMPARE(firstLog.boundValues,
             QVector<QVariant>({QVariant(6)}));
}

void tst_MySql_QueryBuilder::update() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents")
                .whereEq(ID, 10)
                .update({{NAME, "xyz"}, {"size", 6}});
    });

    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "update `torrents` set `name` = ?, `size` = ? where `id` = ?");
    QCOMPARE(firstLog.boundValues,
             QVector<QVariant>({QVariant("xyz"), QVariant(6), QVariant(10)}));
}

void tst_MySql_QueryBuilder::update_WithExpression() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents")
                .whereEq(ID, 10)
                .update({{NAME, DB::raw("'xyz'")}, {"size", 6},
                         {"progress", DB::raw(2)}});
    });

    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "update `torrents` set `name` = 'xyz', `size` = ?, `progress` = 2 "
             "where `id` = ?");
    QCOMPARE(firstLog.boundValues,
             QVector<QVariant>({QVariant(6), QVariant(10)}));
}

void tst_MySql_QueryBuilder::remove() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").remove(2222);
    });

    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "delete from `torrents` where `torrents`.`id` = ?");
    QCOMPARE(firstLog.boundValues,
             QVector<QVariant>({QVariant(2222)}));
}

void tst_MySql_QueryBuilder::remove_WithExpression() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        connection.query()->from("torrents").remove(DB::raw(2223));
    });

    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "delete from `torrents` where `torrents`.`id` = 2223");
    QVERIFY(firstLog.boundValues.isEmpty());
}

QSharedPointer<QueryBuilder>
tst_MySql_QueryBuilder::createQuery() const
{
    return DB::connection(m_connection).query();
}

QTEST_MAIN(tst_MySql_QueryBuilder)

#include "tst_mysql_querybuilder.moc"
