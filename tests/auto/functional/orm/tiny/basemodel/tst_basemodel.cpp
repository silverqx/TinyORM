#include <QCoreApplication>
#include <QtTest>

#include "models/torrent.hpp"

#include "database.hpp"

using namespace Orm;
// TODO namespace silverqx
using namespace Orm::Tiny;

class tst_BaseModel : public QObject
{
    Q_OBJECT

public:
    tst_BaseModel();
    ~tst_BaseModel() = default;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void remove() const;
    void get() const;
    void all() const;
    void get_AllColumns() const;
    void latest() const;
    void oldest() const;
    // TODO tests, where conditions, and orWhere ... silverqx
    void where() const;
    void whereEq() const;
    void arrayOperator() const;
    void vectorWhere() const;
    void vectorWhere_Condition() const;
    void find() const;
    void findOrNew_Found() const;
    void findOrNew_NotFound() const;
    void findOrFail_Found() const;
    void findOrFail_NotFoundFailed() const;
    void firstWhere() const;
    void firstWhereEq() const;
    void firstOrNew_Found() const;
    void firstOrNew_NotFound() const;
    void firstOrCreate_Found() const;
    void firstOrCreate_NotFound() const;
    // TODO tests, move to TinyBuilder tests silverqx
    void firstOrFail_Found() const;
    void firstOrFail_NotFoundFailed() const;

private:
    inline QueryBuilder createQuery() const
    { return QueryBuilder(m_connection, Grammar()); }

    ConnectionInterface &m_connection;
};

tst_BaseModel::tst_BaseModel()
    : m_connection(TestUtils::Database::createConnection())
{}

void tst_BaseModel::initTestCase()
{}

void tst_BaseModel::cleanupTestCase()
{}

void tst_BaseModel::remove() const
{
    auto torrentFile = TorrentPreviewableFile::find(7);

    QVERIFY(torrentFile->exists);

    {
        const auto result = torrentFile->remove();

        QVERIFY(result);
        QVERIFY(!torrentFile->exists);
    }
    // Save it back to the database
    {
        const auto result = torrentFile->save();

        QVERIFY(result);
        QVERIFY(torrentFile->exists);
    }
}

void tst_BaseModel::get() const
{
    Torrent torrent;
    auto torrents = torrent.query()->get();

    QCOMPARE(torrents.size(), 6);
    QCOMPARE(torrents.at(0).getAttribute("id"), QVariant(1));
    QCOMPARE(torrents.at(0).getAttribute("name"), QVariant("test1"));
    QCOMPARE(torrents.at(2).getAttribute("id"), QVariant(3));
    QCOMPARE(torrents.at(2).getAttribute("name"), QVariant("test3"));

    QCOMPARE(torrents.at(1).getAttributes().size(), 8);
}

void tst_BaseModel::all() const
{
    auto torrents = Torrent::all();

    QCOMPARE(torrents.size(), 6);
    QCOMPARE(torrents.at(0).getAttribute("id"), QVariant(1));
    QCOMPARE(torrents.at(0).getAttribute("name"), QVariant("test1"));
    QCOMPARE(torrents.at(2).getAttribute("id"), QVariant(3));
    QCOMPARE(torrents.at(2).getAttribute("name"), QVariant("test3"));
}

void tst_BaseModel::get_AllColumns() const
{
    {
        auto torrents = Torrent::all();

        QCOMPARE(torrents.at(1).getAttributes().size(), 8);
    }

    {
        auto torrents = Torrent::all({"id", "name"});

        const auto &torrent2 = torrents.at(1);
        QCOMPARE(torrent2.getAttributes().size(), 2);
        QCOMPARE(torrent2.getAttributes().at(0).key, QString("id"));
        QCOMPARE(torrent2.getAttributes().at(1).key, QString("name"));
    }

    {
        Torrent torrent;
        auto torrents = torrent.query()->get({"id", "name", "size"});

        const auto &torrent3 = torrents.at(1);
        QCOMPARE(torrent3.getAttributes().size(), 3);
        QCOMPARE(torrent3.getAttributes().at(0).key, QString("id"));
        QCOMPARE(torrent3.getAttributes().at(1).key, QString("name"));
        QCOMPARE(torrent3.getAttributes().at(2).key, QString("size"));
    }
}

void tst_BaseModel::latest() const
{
    auto torrents = Torrent::latest()->get();
    const auto createdAtColumn = Torrent::getCreatedAtColumn();

    auto itTorrent = torrents.constBegin();
    while (itTorrent != torrents.constEnd()) {
        auto firstDate = itTorrent->getAttribute(createdAtColumn).toDateTime();
        ++itTorrent;
        if (itTorrent != torrents.constEnd()) {
            auto secondDate = itTorrent->getAttribute(createdAtColumn).toDateTime();
            QVERIFY(firstDate > secondDate);
        }
    }
}

void tst_BaseModel::oldest() const
{
    auto torrents = Torrent::oldest()->get();
    const auto createdAtColumn = Torrent::getCreatedAtColumn();

    auto itTorrent = torrents.constBegin();
    while (itTorrent != torrents.constEnd()) {
        auto firstDate = itTorrent->getAttribute(createdAtColumn).toDateTime();
        ++itTorrent;
        if (itTorrent != torrents.constEnd()) {
            auto secondDate = itTorrent->getAttribute(createdAtColumn).toDateTime();
            QVERIFY(firstDate < secondDate);
        }
    }
}

void tst_BaseModel::where() const
{
    {
        auto torrent = Torrent::where("id", "=", 3)->first();
        QVERIFY(torrent);
        QCOMPARE(torrent->getAttribute("id"), QVariant(3));
    }
    {
        auto torrents = Torrent::where("id", ">=", 3)->get();
        QCOMPARE(torrents.size(), 4);
        QCOMPARE(torrents.at(0).getAttribute("id"), QVariant(3));
        QCOMPARE(torrents.at(1).getAttribute("id"), QVariant(4));
        QCOMPARE(torrents.at(2).getAttribute("id"), QVariant(5));
        QCOMPARE(torrents.at(3).getAttribute("id"), QVariant(6));
    }
}

void tst_BaseModel::whereEq() const
{
    // number
    {
        auto torrent = Torrent::whereEq("id", 3)->first();
        QVERIFY(torrent);
        QCOMPARE(torrent->getAttribute("id"), QVariant(3));
    }
    // string
    {
        auto torrent = Torrent::whereEq("name", "test3")->first();
        QVERIFY(torrent);
        QCOMPARE(torrent->getAttribute("id"), QVariant(3));
    }
    // QDateTime
    {
        auto torrent = Torrent::whereEq(
                           "added_on",
                           QDateTime::fromString("2020-08-01 20:11:10", Qt::ISODate))
                       ->first();
        QVERIFY(torrent);
        QCOMPARE(torrent->getAttribute("id"), QVariant(1));
    }
}

void tst_BaseModel::arrayOperator() const
{
    auto torrent = Torrent::where("id", "=", 2)->first();
    QVERIFY(torrent);
    QCOMPARE((*torrent)["id"], QVariant(2));
    QCOMPARE((*torrent)["name"], QVariant("test2"));
    QCOMPARE((*torrent)["added_on"],
            QVariant(QDateTime::fromString("2020-08-02 20:11:10", Qt::ISODate)));
}

void tst_BaseModel::vectorWhere() const
{
    {
        auto torrent = Torrent::where({{"id", 3}})->first();
        QVERIFY(torrent);
        QCOMPARE(torrent->getAttribute("id"), QVariant(3));
    }
    {
        auto torrents = Torrent::where({{"id", 3, ">="}})->get();
        QCOMPARE(torrents.size(), 4);
        QCOMPARE(torrents.at(0).getAttribute("id"), QVariant(3));
        QCOMPARE(torrents.at(1).getAttribute("id"), QVariant(4));
        QCOMPARE(torrents.at(2).getAttribute("id"), QVariant(5));
        QCOMPARE(torrents.at(3).getAttribute("id"), QVariant(6));
    }
}

void tst_BaseModel::vectorWhere_Condition() const
{
    {
        auto torrents = Torrent::where({{"size", 14}, {"progress", 400}})->get();
        QCOMPARE(torrents.size(), 1);
        QCOMPARE(torrents.at(0).getAttribute("id"), QVariant(4));
    }
    {
        auto torrents = Torrent::where({{"size", 13}, {"size", 14, "=", "or"}})->get();
        QCOMPARE(torrents.size(), 2);
        QCOMPARE(torrents.at(0).getAttribute("id"), QVariant(3));
        QCOMPARE(torrents.at(1).getAttribute("id"), QVariant(4));
    }
    {
        auto torrents = Torrent::where({{"size", 13}, {"size", 14, "=", "or"}})
                        ->where("progress", "=", 400)
                        .get();
        QCOMPARE(torrents.size(), 1);
        QCOMPARE(torrents.at(0).getAttribute("id"), QVariant(4));
    }
}

void tst_BaseModel::find() const
{
    auto torrent = Torrent::find(3);
    QVERIFY(torrent);
    QCOMPARE(torrent->getAttribute("id"), QVariant(3));
}

void tst_BaseModel::findOrNew_Found() const
{
    {
        auto torrent = Torrent::findOrNew(3);

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 8);
        QCOMPARE(torrent["id"], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
    }
    {
        auto torrent = Torrent::findOrNew(3, {"id", "name"});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 2);
        QCOMPARE(torrent["id"], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
    }
}

void tst_BaseModel::findOrNew_NotFound() const
{
    {
        auto torrent = Torrent::findOrNew(999999);

        QVERIFY(!torrent.exists);
        QVERIFY(torrent.getAttributes().isEmpty());
        QCOMPARE(torrent["id"], QVariant());
        QCOMPARE(torrent["name"], QVariant());
    }
    {
        auto torrent = Torrent::findOrNew(999999, {"id", "name"});

        QVERIFY(!torrent.exists);
        QVERIFY(torrent.getAttributes().isEmpty());
        QCOMPARE(torrent["id"], QVariant());
        QCOMPARE(torrent["name"], QVariant());
    }
}

void tst_BaseModel::findOrFail_Found() const
{
    {
        auto torrent = Torrent::findOrFail(3);

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 8);
        QCOMPARE(torrent["id"], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
    }
    {
        auto torrent = Torrent::findOrFail(3, {"id", "name"});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 2);
        QCOMPARE(torrent["id"], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
    }
}

void tst_BaseModel::findOrFail_NotFoundFailed() const
{
    QVERIFY_EXCEPTION_THROWN(Torrent::findOrFail(999999),
                             ModelNotFoundError);
    QVERIFY_EXCEPTION_THROWN(Torrent::findOrFail(999999, {"id", "name"}),
                             ModelNotFoundError);
}

void tst_BaseModel::firstWhere() const
{
    {
        auto torrentFile3 = TorrentPreviewableFile::firstWhere("id", "=", 3);

        QVERIFY(torrentFile3->exists);
        QCOMPARE((*torrentFile3)["id"], QVariant(3));
        QCOMPARE((*torrentFile3)["filepath"], QVariant("test2_file2.mkv"));
    }
    {
        auto torrentFile1 = TorrentPreviewableFile::firstWhere("id", "<", 4);

        QVERIFY(torrentFile1->exists);
        QCOMPARE((*torrentFile1)["id"], QVariant(1));
        QCOMPARE((*torrentFile1)["filepath"], QVariant("test1_file1.mkv"));
    }
}

void tst_BaseModel::firstWhereEq() const
{
    auto torrentFile3 = TorrentPreviewableFile::firstWhereEq("id", 3);

    QVERIFY(torrentFile3->exists);
    QCOMPARE((*torrentFile3)["id"], QVariant(3));
    QCOMPARE((*torrentFile3)["filepath"], QVariant("test2_file2.mkv"));
}

void tst_BaseModel::firstOrNew_Found() const
{
    {
        auto torrent = Torrent::firstOrNew({{"id", 3}});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 8);
        QCOMPARE(torrent["id"], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
        QCOMPARE(torrent["size"], QVariant(13));
        QCOMPARE(torrent["progress"], QVariant(300));
    }
    {
        auto torrent = Torrent::firstOrNew(
                           {{"id", 3}},

                           {{"name", "test3"},
                            {"size", 113},
                            {"progress", 313}});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 8);
        QCOMPARE(torrent["id"], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
        QCOMPARE(torrent["size"], QVariant(13));
        QCOMPARE(torrent["progress"], QVariant(300));
    }
}

void tst_BaseModel::firstOrNew_NotFound() const
{
    {
        auto torrent = Torrent::firstOrNew({{"id", 100}});

        QVERIFY(!torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 1);
        QCOMPARE(torrent["id"], QVariant(100));
        QCOMPARE(torrent["name"], QVariant());
    }
    {
        auto torrent = Torrent::firstOrNew(
                           {{"id", 100}},

                           {{"name", "test100"},
                            {"size", 113},
                            {"progress", 313}});

        QVERIFY(!torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 4);
        QCOMPARE(torrent["id"], QVariant(100));
        QCOMPARE(torrent["name"], QVariant("test100"));
        QCOMPARE(torrent["size"], QVariant(113));
        QCOMPARE(torrent["progress"], QVariant(313));
    }
}

void tst_BaseModel::firstOrCreate_Found() const
{
    {
        auto torrent = Torrent::firstOrCreate({{"id", 3}});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 8);
        QCOMPARE(torrent["id"], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
        QCOMPARE(torrent["size"], QVariant(13));
        QCOMPARE(torrent["progress"], QVariant(300));
    }
    {
        const auto addedOn = QDateTime::currentDateTime();

        auto torrent = Torrent::firstOrCreate(
                           {{"id", 3}},

                           {{"name", "test3"},
                            {"size", 33},
                            {"progress", 33},
                            {"added_on", addedOn},
                            {"hash", "3579e3af2768cdf52ec84c1f320333f68401dc60"}});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 8);
        QCOMPARE(torrent["id"], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
        QCOMPARE(torrent["size"], QVariant(13));
        QCOMPARE(torrent["progress"], QVariant(300));
    }
}

void tst_BaseModel::firstOrCreate_NotFound() const
{
    const auto addedOn = QDateTime::currentDateTime();

    auto torrent = Torrent::firstOrCreate(
                       {{"id", 100}},

                       {{"name", "test100"},
                        {"size", 113},
                        {"progress", 313},
                        {"added_on", addedOn},
                        {"hash", "1999e3af2768cdf52ec84c1f320333f68401dc6e"}});

    QVERIFY(torrent.exists);
    QCOMPARE(torrent.getAttributes().size(), 8);
    QCOMPARE(torrent["id"], QVariant(100));
    QCOMPARE(torrent["name"], QVariant("test100"));
    QCOMPARE(torrent["size"], QVariant(113));
    QCOMPARE(torrent["progress"], QVariant(313));
    QCOMPARE(torrent["added_on"], QVariant(addedOn));
    QCOMPARE(torrent["hash"], QVariant("1999e3af2768cdf52ec84c1f320333f68401dc6e"));

    const auto result = torrent.remove();
    QVERIFY(result);
    QVERIFY(!torrent.exists);
}

void tst_BaseModel::firstOrFail_Found() const
{
    {
        auto torrent = Torrent::whereEq("id", 3)->firstOrFail();

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 8);
        QCOMPARE(torrent["id"], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
    }
    {
        auto torrent = Torrent::whereEq("id", 3)->firstOrFail({"id", "name"});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 2);
        QCOMPARE(torrent["id"], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
    }
}

void tst_BaseModel::firstOrFail_NotFoundFailed() const
{
    QVERIFY_EXCEPTION_THROWN(Torrent::whereEq("id", 999999)->firstOrFail(),
                             ModelNotFoundError);
    QVERIFY_EXCEPTION_THROWN(Torrent::whereEq("id", 999999)->firstOrFail({"id", "name"}),
                             ModelNotFoundError);
}

QTEST_MAIN(tst_BaseModel)

#include "tst_basemodel.moc"
