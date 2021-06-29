#include <QCoreApplication>
#include <QtTest>

#include "orm/db.hpp"
#include "orm/query/querybuilder.hpp"

#include "databases.hpp"

using Expression = Orm::Query::Expression;
using QueryBuilder = Orm::Query::Builder;
using Raw = Orm::Query::Expression;

using TestUtils::Databases;

class tst_MySql_QueryBuilder : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void from() const;
    void from_TableWrappingQuotationMarks() const;
    void from_WithPrefix() const;
    void from_AliasWithPrefix() const;

    void fromRaw() const;
    void fromRaw_withWhere() const;
    void fromRaw_withBindings_withWhere() const;

    void fromSub_QStringOverload() const;
    void fromSub_QueryBuilderOverload_WithWhere() const;
    void fromSub_CallbackOverload() const;

    void joinSub_QStringOverload() const;
    void joinSub_QueryBuilderOverload_WithWhere() const;
    void joinSub_CallbackOverload() const;

    void select() const;
    void addSelect() const;

    void distinct() const;

    void orderBy() const;
    void latestOldest() const;

    void limitOffset() const;
    void takeSkip() const;
    void forPage() const;

    void basicWhere() const;
    void whereWithVectorValue() const;

    void basicOrWhere() const;
    void basicOrWhere_ColumnExpression() const;
    void orWhereWithVectorValue() const;
    void orWhereWithVectorValue_ColumnExpression() const;

    void whereColumn() const;
    void orWhereColumn() const;
    void orWhereColumn_ColumnExpression() const;
    void whereColumnWithVectorValue() const;
    void orWhereColumnWithVectorValue() const;
    void orWhereColumnWithVectorValue_ColumnExpression() const;

    void basicWhereIn() const;
    void basicWhereNotIn() const;
    void basicWhereNotIn_ColumnExpression() const;
    void emptyWhereIn() const;
    void emptyNotWhereIn() const;
    void whereIn_ValueExpression() const;

    void basicWhereNull() const;
    void basicWhereNotNull() const;
    void basicWhereNotNull_ColumnExpression() const;
    void whereNullWithVectorValue() const;
    void whereNotNullWithVectorValue() const;

    void lock() const;

private:
    /*! Create QueryBuilder instance for the given connection. */
    inline QSharedPointer<QueryBuilder>
    createQuery(const QString &connection) const
    { return DB::connection(connection).query(); }

    /*! Connection name used in this test case. */
    QString m_connection;
};

void tst_MySql_QueryBuilder::initTestCase()
{
    m_connection = Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(QStringLiteral("%1 autotest skipped, environment variables "
                             "for '%2' connection have not been defined.")
              .arg("tst_MySql_QueryBuilder", Databases::MYSQL).toUtf8().constData(), );
}

void tst_MySql_QueryBuilder::from() const
{
    auto builder = createQuery(m_connection);

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
    auto builder = createQuery(m_connection);

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
    auto builder = createQuery(m_connection);

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
    auto builder = createQuery(m_connection);

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
    auto builder = createQuery(m_connection);

    const auto &from = builder->getFrom();

    builder->fromRaw("(select max(last_seen_at) as last_seen_at "
                     "from `user_sessions`) as `sessions`");

    QVERIFY(std::holds_alternative<Expression>(from));
    QCOMPARE(builder->toSql(),
             "select * from (select max(last_seen_at) as last_seen_at "
             "from `user_sessions`) as `sessions`");
}

void tst_MySql_QueryBuilder::fromRaw_withWhere() const
{
    auto builder = createQuery(m_connection);

    const auto &from = builder->getFrom();

    // fromRaw() with where on main query
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

void tst_MySql_QueryBuilder::fromRaw_withBindings_withWhere() const
{
    auto builder = createQuery(m_connection);

    const auto &from = builder->getFrom();

    // fromRaw() with bindings and with where on main query
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
    auto builder = createQuery(m_connection);

    builder->fromSub("select max(last_seen_at) as last_seen_at from `user_sessions`",
                     "sessions");

    QVERIFY(std::holds_alternative<Expression>(builder->getFrom()));
    QCOMPARE(builder->toSql(),
             "select * from (select max(last_seen_at) as last_seen_at "
             "from `user_sessions`) as `sessions`");
}

void tst_MySql_QueryBuilder::fromSub_QueryBuilderOverload_WithWhere() const
{
    auto builder = createQuery(m_connection);

    auto fromQuery = createQuery(m_connection)
                       ->from("user_sessions")
                       .select({"id", "name"})
                       .where("id", "<", 5);

    builder->fromSub(fromQuery, "sessions")
            .whereEq("name", "xyz");

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
    auto builder = createQuery(m_connection);

    builder->fromSub([](auto &query)
    {
        query.from("user_sessions")
             .select({"id", "name"})
             .where("id", "<", 5);
    }, "sessions").whereEq("name", "xyz");

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
    auto builder = createQuery(m_connection);

    builder->from("users")
            .joinSub("select id as files_id, `user_id`, `name` from `user_sessions`",
                     "sessions", "users.id", "=", "sessions.user_id", "left");

    QCOMPARE(builder->toSql(),
             "select * from `users` "
             "left join (select id as files_id, `user_id`, `name` "
             "from `user_sessions`) as `sessions` "
             "on `users`.`id` = `sessions`.`user_id`");
}

void tst_MySql_QueryBuilder::joinSub_QueryBuilderOverload_WithWhere() const
{
    auto builder = createQuery(m_connection);

    auto fromQuery = createQuery(m_connection)
                     ->from("user_sessions")
                     .select({"id as files_id", "user_id", "name"})
                     .where("user_id", "<", 5);

    builder->from("users")
            .joinSub(fromQuery, "sessions", "users.id", "=", "sessions.user_id")
            .whereEq("name", "xyz");

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
    auto builder = createQuery(m_connection);

    builder->from("users")
            .joinSub([](auto &query)
    {
        query.from("user_sessions")
                .select({"id as files_id", "user_id", "name"})
                .where("user_id", "<", 5);
    }, "sessions", "users.id", "=", "sessions.user_id", "left")

            .whereEq("name", "xyz");

    QCOMPARE(builder->toSql(),
             "select * from `users` "
             "left join (select `id` as `files_id`, `user_id`, `name` "
                 "from `user_sessions` where `user_id` < ?) as `sessions` "
             "on `users`.`id` = `sessions`.`user_id` "
             "where `name` = ?");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant(5), QVariant("xyz")}));
}

void tst_MySql_QueryBuilder::select() const
{
    auto builder = createQuery(m_connection);

    builder->from("torrents");

    builder->select({"id", "name"});
    QCOMPARE(builder->toSql(),
             "select `id`, `name` from `torrents`");

    builder->select();
    QCOMPARE(builder->toSql(),
             "select * from `torrents`");

    builder->select("id");
    QCOMPARE(builder->toSql(),
             "select `id` from `torrents`");
}

void tst_MySql_QueryBuilder::addSelect() const
{
    auto builder = createQuery(m_connection);

    builder->from("torrents");

    builder->addSelect({"id", "name"});
    QCOMPARE(builder->toSql(),
             "select `id`, `name` from `torrents`");

    builder->addSelect("size");
    QCOMPARE(builder->toSql(),
             "select `id`, `name`, `size` from `torrents`");

    builder->addSelect("*");
    QCOMPARE(builder->toSql(),
             "select `id`, `name`, `size`, * from `torrents`");
}

void tst_MySql_QueryBuilder::distinct() const
{
    auto builder = createQuery(m_connection);

    builder->from("torrents");

    auto distinct = std::get<bool>(builder->getDistinct());
    QCOMPARE(distinct, false);

    builder->distinct();
    distinct = std::get<bool>(builder->getDistinct());
    QCOMPARE(distinct, true);
    QCOMPARE(builder->toSql(),
             "select distinct * from `torrents`");

    builder->select({"name", "size"});
    QCOMPARE(builder->toSql(),
             "select distinct `name`, `size` from `torrents`");
}

void tst_MySql_QueryBuilder::orderBy() const
{
    auto builder = createQuery(m_connection);

    builder->from("torrents");

    builder->orderBy("name", "asc");
    QCOMPARE(builder->toSql(),
             "select * from `torrents` order by `name` asc");

    builder->orderBy("id", "desc");
    QCOMPARE(builder->toSql(),
             "select * from `torrents` order by `name` asc, `id` desc");

    builder->reorder()
            .orderByDesc("name");
    QCOMPARE(builder->toSql(),
             "select * from `torrents` order by `name` desc");

    builder->reorder("id", "asc");
    QCOMPARE(builder->toSql(),
             "select * from `torrents` order by `id` asc");
}

void tst_MySql_QueryBuilder::latestOldest() const
{
    auto builder = createQuery(m_connection);

    builder->from("torrents");

    builder->latest("name");
    QCOMPARE(builder->toSql(),
             "select * from `torrents` order by `name` desc");

    builder->reorder().oldest("name");
    QCOMPARE(builder->toSql(),
             "select * from `torrents` order by `name` asc");
}

void tst_MySql_QueryBuilder::limitOffset() const
{
    auto builder = createQuery(m_connection);

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
    auto builder = createQuery(m_connection);

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
    auto builder = createQuery(m_connection);

    builder->from("torrents");

    builder->forPage(2, 10);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` limit 10 offset 10");

    builder->forPage(5);
    QCOMPARE(builder->toSql(),
             "select * from `torrents` limit 30 offset 120");
}

void tst_MySql_QueryBuilder::basicWhere() const
{
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").where("id", "=", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").whereEq("id", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").whereEq("id", 3)
                .whereEq("name", "test3");
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? and `name` = ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(3), QVariant("test3")}));
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").where("id", "!=", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` != ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").where("id", "<>", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` <> ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").where("id", ">", 3);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").where("id", ">", 3)
                .where("name", "like", "test%");
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? and `name` like ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(3), QVariant("test%")}));
    }
}

void tst_MySql_QueryBuilder::whereWithVectorValue() const
{
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").where({{"id", 3}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where (`id` = ?)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant> {QVariant(3)});
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").where({{"id", 3}, {"size", 10, ">"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where (`id` = ? and `size` > ?)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(3), QVariant(10)}));
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").where({{"id", 3}, {"size", 10, ">"}})
                .where({{"progress", 100, ">="}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where (`id` = ? and `size` > ?) "
                 "and (`progress` >= ?)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(3), QVariant(10), QVariant(100)}));
    }
}

void tst_MySql_QueryBuilder::basicOrWhere() const
{
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").where("id", ">", 4)
                .orWhere("progress", ">=", 300);
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? or `progress` >= ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4), QVariant(300)}));
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").where("id", ">", 4)
                .orWhereEq("name", "test3");
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` > ? or `name` = ?");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4), QVariant("test3")}));
    }
}

void tst_MySql_QueryBuilder::basicOrWhere_ColumnExpression() const
{
    auto builder = createQuery(m_connection);

    builder->select("*").from("torrents").where(Raw("id"), ">", 4)
            .orWhereEq(Raw("`name`"), "test3");
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where id > ? or `name` = ?");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant(4), QVariant("test3")}));
}

void tst_MySql_QueryBuilder::orWhereWithVectorValue() const
{
    auto builder = createQuery(m_connection);

    builder->select("*").from("torrents").where({{"id", 3}, {"size", 10, ">"}})
            .orWhere({{"progress", 100, ">="}});
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where (`id` = ? and `size` > ?) or "
             "(`progress` >= ?)");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant(3), QVariant(10), QVariant(100)}));
}

void tst_MySql_QueryBuilder::orWhereWithVectorValue_ColumnExpression() const
{
    auto builder = createQuery(m_connection);

    builder->select("*").from("torrents")
            .where({{Raw("id"), 3}, {Raw("`size`"), 10, ">"}})
            .orWhere({{Raw("progress"), 100, ">="}});
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where (id = ? and `size` > ?) or "
             "(progress >= ?)");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant(3), QVariant(10), QVariant(100)}));
}

void tst_MySql_QueryBuilder::whereColumn() const
{
    auto builder = createQuery(m_connection);

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
        auto builder = createQuery(m_connection);

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
        auto builder = createQuery(m_connection);

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
    auto builder = createQuery(m_connection);

    builder->select("*").from("torrent_previewable_files")
            .whereColumnEq(Raw("filepath"), Raw("`note`"))
            .orWhereColumn(Raw("size"), ">", Raw("progress"));
    QCOMPARE(builder->toSql(),
             "select * from `torrent_previewable_files` where filepath = `note` "
             "or size > progress");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>());
}

void tst_MySql_QueryBuilder::whereColumnWithVectorValue() const
{
    {
        auto builder = createQuery(m_connection);

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
        auto builder = createQuery(m_connection);

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

void tst_MySql_QueryBuilder::orWhereColumnWithVectorValue() const
{
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_previewable_files").whereEq("id", 2)
                .orWhereColumn({{"filepath", "note"},
                                {"size", "progress", ">"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` "
                 "where `id` = ? or (`filepath` = `note` or `size` > `progress`)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(2)}));
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_previewable_files").whereEq("id", 2)
                .orWhereColumn({{"filepath", "note"},
                                {"size", "progress", ">", "and"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` "
                 "where `id` = ? or (`filepath` = `note` and `size` > `progress`)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(2)}));
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_previewable_files").whereEq("id", 2)
                .orWhereColumn({{"filepath", "note"},
                                {"size", "progress", ">", "or"}});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_previewable_files` "
                 "where `id` = ? or (`filepath` = `note` or `size` > `progress`)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(2)}));
    }
}

void tst_MySql_QueryBuilder::orWhereColumnWithVectorValue_ColumnExpression() const
{
    auto builder = createQuery(m_connection);

    builder->select("*").from("torrent_previewable_files").whereEq("id", 2)
            .orWhereColumn({{Raw("filepath"), Raw("`note`")},
                            {"size", Raw("progress"), ">"}});
    QCOMPARE(builder->toSql(),
             "select * from `torrent_previewable_files` "
             "where `id` = ? or (filepath = `note` or `size` > progress)");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant(2)}));
}

void tst_MySql_QueryBuilder::basicWhereIn() const
{
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").whereIn("id", {2, 3, 4});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` in (?, ?, ?)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(2), QVariant(3), QVariant(4)}));
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").where("id", "=", 1)
                .orWhereIn("id", {2, 3, 4});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? or `id` in (?, ?, ?)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(1), QVariant(2),
                                    QVariant(3), QVariant(4)}));
    }
}

void tst_MySql_QueryBuilder::basicWhereNotIn() const
{
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").whereNotIn("id", {2, 3, 4});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` not in (?, ?, ?)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(2), QVariant(3), QVariant(4)}));
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").where("id", "=", 1)
                .orWhereNotIn("id", {2, 3, 4});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? or `id` not in (?, ?, ?)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(1), QVariant(2),
                                    QVariant(3), QVariant(4)}));
    }
}

void tst_MySql_QueryBuilder::basicWhereNotIn_ColumnExpression() const
{
    auto builder = createQuery(m_connection);

    builder->select("*").from("torrents").where("id", "=", 1)
            .orWhereNotIn(Raw("id"), {2, 3, 4});
    QCOMPARE(builder->toSql(),
             "select * from `torrents` where `id` = ? or id not in (?, ?, ?)");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant(1), QVariant(2), QVariant(3), QVariant(4)}));
}

void tst_MySql_QueryBuilder::emptyWhereIn() const
{
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").whereIn("id", {});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where 0 = 1");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>());
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").where("id", "=", 1)
                .orWhereIn("id", {});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? or 0 = 1");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(1)}));
    }
}

void tst_MySql_QueryBuilder::emptyNotWhereIn() const
{
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").whereNotIn("id", {});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where 1 = 1");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>());
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").where("id", "=", 1)
                .orWhereNotIn("id", {});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? or 1 = 1");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(1)}));
    }
}

void tst_MySql_QueryBuilder::whereIn_ValueExpression() const
{
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").whereIn("id", {Raw(3)});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` in (3)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>());
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").whereEq("id", {2})
                .orWhereIn("id", {Raw(3)});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `id` = ? or `id` in (3)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(2)}));
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrents").whereIn("name", {Raw("'xyz'")});
        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where `name` in ('xyz')");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>());
    }
}

void tst_MySql_QueryBuilder::basicWhereNull() const
{
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereNull("seeds");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `seeds` is null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>());
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereEq("id", 4)
                .whereNull("seeds");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? and `seeds` is null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereEq("id", 3)
                .orWhereNull("seeds");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? or `seeds` is null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(3)}));
    }
}

void tst_MySql_QueryBuilder::basicWhereNotNull() const
{
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereNotNull("seeds");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `seeds` is not null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>());
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereEq("id", 4)
                .whereNotNull("seeds");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? and `seeds` is not null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereEq("id", 3)
                .orWhereNotNull("seeds");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? or `seeds` is not null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(3)}));
    }
}

void tst_MySql_QueryBuilder::basicWhereNotNull_ColumnExpression() const
{
    auto builder = createQuery(m_connection);

    builder->select("*").from("torrent_peers").whereEq("id", 3)
            .orWhereNotNull(Raw("seeds"));
    QCOMPARE(builder->toSql(),
             "select * from `torrent_peers` where `id` = ? or seeds is not null");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant(3)}));
}

void tst_MySql_QueryBuilder::whereNullWithVectorValue() const
{
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereNull({"seeds", "total_seeds"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `seeds` is null "
                 "and `total_seeds` is null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>());
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereEq("id", 4)
                .whereNull({"seeds", "total_seeds"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? and `seeds` is null "
                 "and `total_seeds` is null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereEq("id", 3)
                .orWhereNull({"seeds", "total_seeds"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? or `seeds` is null "
                 "or `total_seeds` is null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(3)}));
    }
}

void tst_MySql_QueryBuilder::whereNotNullWithVectorValue() const
{
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereNotNull({"seeds",
                                                                 "total_seeds"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `seeds` is not null "
                 "and `total_seeds` is not null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>());
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereEq("id", 4)
                .whereNotNull({"seeds", "total_seeds"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? and `seeds` is not null "
                 "and `total_seeds` is not null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }

    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereEq("id", 3)
                .orWhereNotNull({"seeds", "total_seeds"});
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? or `seeds` is not null "
                 "or `total_seeds` is not null");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(3)}));
    }
}

void tst_MySql_QueryBuilder::lock() const
{
    // lock for update
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereEq("id", 4).lock();
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? for update");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereEq("id", 4).lockForUpdate();
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? for update");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }
    // shared lock
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereEq("id", 4).lock(false);
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? lock in share mode");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereEq("id", 4).sharedLock();
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? lock in share mode");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }
    {
        auto builder = createQuery(m_connection);

        builder->select("*").from("torrent_peers").whereEq("id", 4)
                .lock("lock in share mode");
        QCOMPARE(builder->toSql(),
                 "select * from `torrent_peers` where `id` = ? lock in share mode");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(4)}));
    }
}

QTEST_MAIN(tst_MySql_QueryBuilder)

#include "tst_mysql_querybuilder.moc"
