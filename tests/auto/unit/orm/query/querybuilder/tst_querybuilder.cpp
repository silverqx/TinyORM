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

    void table() const;
    void from() const;

    void select() const;
    void addSelect() const;

    void distinct() const;

    void orderBy() const;
    void latestOldest() const;
    void limitOffset() const;
    void takeSkip() const;
    void forPage() const;

    void basicWhere() const;

private:
    inline QueryBuilder createQuery() const
    { return QueryBuilder(m_db, Grammar()); }

    DatabaseConnection &m_db;
};

tst_QueryBuilder::tst_QueryBuilder()
    : m_db(Utils::Database::createConnection())
{}

void tst_QueryBuilder::initTestCase()
{}

void tst_QueryBuilder::cleanupTestCase()
{}

void tst_QueryBuilder::table() const
{
    auto builder = createQuery();

    const auto &tableEmpty = QStringLiteral("");
    QCOMPARE(builder.getTable(), tableEmpty);
    QCOMPARE(builder.getFrom(), tableEmpty);

    const auto &tableTorrents = QStringLiteral("torrents");
    builder.table(tableTorrents);

    QCOMPARE(builder.getTable(), tableTorrents);
    QCOMPARE(builder.getFrom(), tableTorrents);
    QCOMPARE(builder.toSql(),
             "select * from torrents");

    const auto &tableTorrentPeers = QStringLiteral("torrent_peers");
    builder.table(tableTorrentPeers);

    QCOMPARE(builder.getTable(), tableTorrentPeers);
    QCOMPARE(builder.getFrom(), tableTorrentPeers);
    QCOMPARE(builder.toSql(),
             "select * from torrent_peers");
}

void tst_QueryBuilder::from() const
{
    auto builder = createQuery();

    const auto &tableEmpty = QStringLiteral("");
    QCOMPARE(builder.getTable(), tableEmpty);
    QCOMPARE(builder.getFrom(), tableEmpty);

    const auto &tableTorrents = QStringLiteral("torrents");
    builder.from(tableTorrents);

    QCOMPARE(builder.getTable(), tableTorrents);
    QCOMPARE(builder.getFrom(), tableTorrents);
    QCOMPARE(builder.toSql(),
             "select * from torrents");

    const auto &tableTorrentPeers = QStringLiteral("torrent_peers");
    builder.from(tableTorrentPeers);

    QCOMPARE(builder.getTable(), tableTorrentPeers);
    QCOMPARE(builder.getFrom(), tableTorrentPeers);
    QCOMPARE(builder.toSql(),
             "select * from torrent_peers");
}

void tst_QueryBuilder::select() const
{
    auto builder = createQuery();

    builder.from("torrents");

    builder.select({"id", "name"});
    QCOMPARE(builder.toSql(),
             "select id, name from torrents");

    builder.select();
    QCOMPARE(builder.toSql(),
             "select * from torrents");

    builder.select("id");
    QCOMPARE(builder.toSql(),
             "select id from torrents");
}

void tst_QueryBuilder::addSelect() const
{
    auto builder = createQuery();

    builder.from("torrents");

    builder.addSelect({"id", "name"});
    QCOMPARE(builder.toSql(),
             "select id, name from torrents");

    builder.addSelect("size");
    QCOMPARE(builder.toSql(),
             "select id, name, size from torrents");

    builder.addSelect("*");
    QCOMPARE(builder.toSql(),
             "select id, name, size, * from torrents");
}

void tst_QueryBuilder::distinct() const
{
    auto builder = createQuery();

    builder.from("torrents");

    auto distinct = builder.getDistinct();
    QCOMPARE(distinct, false);

    builder.distinct();
    distinct = builder.getDistinct();
    QCOMPARE(distinct, true);
    QCOMPARE(builder.toSql(),
             "select distinct * from torrents");

    builder.select({"name", "size"});
    QCOMPARE(builder.toSql(),
             "select distinct name, size from torrents");
}

void tst_QueryBuilder::orderBy() const
{
    auto builder = createQuery();

    builder.from("torrents");

    builder.orderBy("name", "asc");
    QCOMPARE(builder.toSql(),
             "select * from torrents order by name asc");

    builder.orderBy("id", "desc");
    QCOMPARE(builder.toSql(),
             "select * from torrents order by name asc, id desc");

    builder.reorder()
            .orderByDesc("name");
    QCOMPARE(builder.toSql(),
             "select * from torrents order by name desc");

    builder.reorder("id", "asc");
    QCOMPARE(builder.toSql(),
             "select * from torrents order by id asc");
}

void tst_QueryBuilder::latestOldest() const
{
    auto builder = createQuery();

    builder.from("torrents");

    builder.latest("name");
    QCOMPARE(builder.toSql(),
             "select * from torrents order by name desc");

    builder.reorder().oldest("name");
    QCOMPARE(builder.toSql(),
             "select * from torrents order by name asc");
}

void tst_QueryBuilder::limitOffset() const
{
    auto builder = createQuery();

    builder.from("torrents");

    builder.limit(10);
    QCOMPARE(builder.toSql(),
             "select * from torrents limit 10");

    builder.offset(5);
    QCOMPARE(builder.toSql(),
             "select * from torrents limit 10 offset 5");
}

void tst_QueryBuilder::takeSkip() const
{
    auto builder = createQuery();

    builder.from("torrents");

    builder.take(15);
    QCOMPARE(builder.toSql(),
             "select * from torrents limit 15");

    builder.skip(5);
    QCOMPARE(builder.toSql(),
             "select * from torrents limit 15 offset 5");
}

void tst_QueryBuilder::forPage() const
{
    auto builder = createQuery();

    builder.from("torrents");

    builder.forPage(2, 10);
    QCOMPARE(builder.toSql(),
             "select * from torrents limit 10 offset 10");

    builder.forPage(5);
    QCOMPARE(builder.toSql(),
             "select * from torrents limit 30 offset 120");
}

void tst_QueryBuilder::basicWhere() const
{
    auto builder = createQuery();

    builder.select("*").from("torrents").where("id", "=", 3);
    QCOMPARE(builder.toSql(),
             "select * from torrents where id = ?");
    QCOMPARE(builder.getBindings(),
             QVector<QVariant> {QVariant(3)});
}

QTEST_MAIN(tst_QueryBuilder)

#include "tst_querybuilder.moc"
