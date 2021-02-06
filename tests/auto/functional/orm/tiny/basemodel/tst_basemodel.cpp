#include <QCoreApplication>
#include <QtTest>

#include "models/setting.hpp"
#include "models/torrent.hpp"

#include "database.hpp"

using namespace Orm;
// TODO tests, namespace silverqx
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

    void save_Insert() const;
    void save_InsertWithDefaultValues() const;
    void save_TableWithoutAutoincrementKey() const;
    void save_UpdateSuccess() const;
    void save_UpdateWithNullValue() const;
    void save_UpdateFailed() const;

    void remove() const;
    void destroy() const;
    void destroyWithVector() const;

    void all() const;
    void all_Columns() const;

    void latest() const;
    void oldest() const;

    // TODO tests, where conditions, and orWhere ... silverqx
    void where() const;
    void whereEq() const;
    void where_WithVector() const;
    void where_WithVector_Condition() const;

    void arrayOperator() const;

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

    void isCleanAndIsDirty() const;
    void wasChanged() const;

    void is() const;
    void isNot() const;

private:
    /*! The database connection instance. */
    ConnectionInterface &m_connection;
};

tst_BaseModel::tst_BaseModel()
    : m_connection(TestUtils::Database::createConnection())
{}

void tst_BaseModel::initTestCase()
{}

void tst_BaseModel::cleanupTestCase()
{}

void tst_BaseModel::save_Insert() const
{
    Torrent torrent;

    auto addedOn = QDateTime::fromString("2020-10-01 20:22:10", Qt::ISODate);
    torrent.setAttribute("name", "test50")
            .setAttribute("size", 50)
            .setAttribute("progress", 50)
            .setAttribute("added_on", addedOn)
            .setAttribute("hash", "5079e3af2768cdf52ec84c1f320333f68401dc61");

    QVERIFY(!torrent.exists);
    const auto result = torrent.save();
    QVERIFY(result);
    QVERIFY(torrent.exists);

    // Check attributes after save
    QVERIFY(torrent.getAttribute("id").isValid());
    QVERIFY(torrent.getAttribute("id").toULongLong() > 6);
    QCOMPARE(torrent.getAttribute("name"), QVariant("test50"));
    QCOMPARE(torrent.getAttribute("size"), QVariant(50));
    QCOMPARE(torrent.getAttribute("progress"), QVariant(50));
    QCOMPARE(torrent.getAttribute("added_on"),
             QVariant(addedOn));
    QCOMPARE(torrent.getAttribute("hash"),
             QVariant("5079e3af2768cdf52ec84c1f320333f68401dc61"));
    QVERIFY(torrent.getAttribute("created_at").isValid());
    QVERIFY(torrent.getAttribute("updated_at").isValid());

    // Get the fresh record from the database
    auto torrentToVerify = Torrent::find(torrent.getAttribute("id"));
    QVERIFY(torrentToVerify);
    QVERIFY(torrentToVerify->exists);

    // And check attributes again
    QCOMPARE(torrentToVerify->getAttribute("id"), torrent.getAttribute("id"));
    QCOMPARE(torrentToVerify->getAttribute("name"), QVariant("test50"));
    QCOMPARE(torrentToVerify->getAttribute("size"), QVariant(50));
    QCOMPARE(torrentToVerify->getAttribute("progress"), QVariant(50));
    QCOMPARE(torrentToVerify->getAttribute("added_on"),
             QVariant(addedOn));
    QCOMPARE(torrentToVerify->getAttribute("hash"),
             QVariant("5079e3af2768cdf52ec84c1f320333f68401dc61"));
    QVERIFY(torrentToVerify->getAttribute("created_at").isValid());
    QVERIFY(torrentToVerify->getAttribute("updated_at").isValid());

    // Remove it
    torrent.remove();
    QVERIFY(!torrent.exists);
}

void tst_BaseModel::save_InsertWithDefaultValues() const
{
    Torrent torrent;

    auto addedOn = QDateTime::fromString("2020-10-01 20:22:10", Qt::ISODate);
    torrent.setAttribute("name", "test51")
            .setAttribute("added_on", addedOn)
            .setAttribute("hash", "5179e3af2768cdf52ec84c1f320333f68401dc61");

    QVERIFY(!torrent.exists);
    const auto result = torrent.save();
    QVERIFY(result);
    QVERIFY(torrent.exists);

    // Check attributes after save
    QVERIFY(torrent.getAttribute("id").isValid());
    QVERIFY(torrent.getAttribute("id").toULongLong() > 6);
    QCOMPARE(torrent.getAttribute("name"), QVariant("test51"));
    QCOMPARE(torrent.getAttribute("added_on"),
             QVariant(addedOn));
    QCOMPARE(torrent.getAttribute("hash"),
             QVariant("5179e3af2768cdf52ec84c1f320333f68401dc61"));
    QVERIFY(torrent.getAttribute("created_at").isValid());
    QVERIFY(torrent.getAttribute("updated_at").isValid());

    // Get the fresh record from the database
    auto torrentToVerify = Torrent::find(torrent.getAttribute("id"));
    QVERIFY(torrentToVerify);
    QVERIFY(torrentToVerify->exists);

    // And check attributes again
    QVERIFY(torrentToVerify->getAttribute("id").isValid());
    QVERIFY(torrentToVerify->getAttribute("id").toULongLong() > 6);
    QCOMPARE(torrentToVerify->getAttribute("name"), QVariant("test51"));
    QCOMPARE(torrentToVerify->getAttribute("size"), QVariant(0));
    QCOMPARE(torrentToVerify->getAttribute("progress"), QVariant(0));
    QCOMPARE(torrentToVerify->getAttribute("added_on"),
             QVariant(addedOn));
    QCOMPARE(torrentToVerify->getAttribute("hash"),
             QVariant("5179e3af2768cdf52ec84c1f320333f68401dc61"));
    QVERIFY(torrentToVerify->getAttribute("created_at").isValid());
    QVERIFY(torrentToVerify->getAttribute("updated_at").isValid());

    // Remove it
    torrent.remove();
    QVERIFY(!torrent.exists);
}

void tst_BaseModel::save_TableWithoutAutoincrementKey() const
{
    Setting setting;

    setting.setAttribute("name", "setting1")
            .setAttribute("value", "value1");

    QVERIFY(!setting.exists);
    const auto result = setting.save();
    QVERIFY(result);
    QVERIFY(setting.exists);

    // Check attributes after save
    QVERIFY(!setting.getAttribute("id").isValid());
    QCOMPARE(setting.getAttribute("name"), QVariant("setting1"));
    QCOMPARE(setting.getAttribute("value"), QVariant("value1"));
    QVERIFY(setting.getAttribute("created_at").isValid());
    QVERIFY(setting.getAttribute("updated_at").isValid());

    // Get the fresh record from the database
    auto settingToVerify = Setting::whereEq("name", "setting1")->first();
    QVERIFY(settingToVerify);
    QVERIFY(settingToVerify->exists);

    // And check attributes again
    QVERIFY(!settingToVerify->getAttribute("id").isValid());
    QCOMPARE(settingToVerify->getAttribute("name"), QVariant("setting1"));
    QCOMPARE(settingToVerify->getAttribute("value"), QVariant("value1"));
    QVERIFY(settingToVerify->getAttribute("created_at").isValid());
    QVERIFY(settingToVerify->getAttribute("updated_at").isValid());

    // Remove it
    int affected;
    std::tie(affected, std::ignore) =
            Setting::whereEq("name", "setting1")->remove();
    QCOMPARE(affected, 1);
}

void tst_BaseModel::save_UpdateSuccess() const
{
    auto torrentFile = TorrentPreviewableFile::find(4);
    QVERIFY(torrentFile);
    QVERIFY(torrentFile->exists);
    QCOMPARE(torrentFile->getAttribute("id"), QVariant(4));

    torrentFile->setAttribute("filepath", "test3_file1-updated.mkv")
            .setAttribute("size", 5570)
            .setAttribute("progress", 860);

    const auto result = torrentFile->save();
    QVERIFY(result);
    QVERIFY(torrentFile->exists);

    // Check
    auto torrentFileFresh = TorrentPreviewableFile::find(4);
    QVERIFY(torrentFileFresh);
    QVERIFY(torrentFileFresh->exists);
    QCOMPARE(torrentFileFresh->getAttribute("filepath"),
             QVariant("test3_file1-updated.mkv"));
    QCOMPARE(torrentFileFresh->getAttribute("size"), QVariant(5570));
    QCOMPARE(torrentFileFresh->getAttribute("progress"), QVariant(860));

    // TODO tests, now remove
    torrentFile->setAttribute("filepath", "test3_file1.mkv")
            .setAttribute("size", 5568)
            .setAttribute("progress", 870);
    torrentFile->save();
}

void tst_BaseModel::save_UpdateWithNullValue() const
{
    auto peer = TorrentPeer::find(4);
    QVERIFY(peer);
    QVERIFY(peer->exists);

    peer->setAttribute("total_seeds", QVariant());

    peer->save();

    // Verify after save
    QVERIFY(!peer->getAttribute("total_seeds").isValid());
    QVERIFY(peer->getAttribute("total_seeds").isNull());
    QCOMPARE(peer->getAttribute("total_seeds"), QVariant());

    // Verify record from the database
    auto peerVerify = TorrentPeer::find(4);
    QVERIFY(peerVerify);
    QVERIFY(peerVerify->exists);

    QVERIFY(peerVerify->getAttribute("total_seeds").isValid());
    QVERIFY(peerVerify->getAttribute("total_seeds").isNull());
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QCOMPARE(peerVerify->getAttribute("total_seeds"), QVariant(QMetaType(QMetaType::Int)));
#else
    QCOMPARE(peerVerify->getAttribute("total_seeds"), QVariant(QVariant::Int));
#endif

    // TODO tests, remove silverqx
    peer->setAttribute("total_seeds", 4);
    peer->save();
}

void tst_BaseModel::save_UpdateFailed() const
{
    auto peer = TorrentPeer::find(3);
    QVERIFY(peer);
    QVERIFY(peer->exists);

    peer->setAttribute("total_seeds-NON_EXISTENT", 15);

    QVERIFY_EXCEPTION_THROWN(peer->save(), QueryError);
}

void tst_BaseModel::remove() const
{
    auto torrentFile = TorrentPreviewableFile::find(7);

    QVERIFY(torrentFile);
    QVERIFY(torrentFile->exists);

    {
        const auto result = torrentFile->remove();

        QVERIFY(result);
        QVERIFY(!torrentFile->exists);
    }
    // Save it back to the database, recreate them
    {
        const auto result = torrentFile->save();

        QVERIFY(result);
        QVERIFY(torrentFile->exists);
    }
}

void tst_BaseModel::destroy() const
{
    auto torrentFile = TorrentPreviewableFile::find(8);

    QVERIFY(torrentFile);
    QCOMPARE(torrentFile->getAttribute("id"), QVariant(8));
    QVERIFY(torrentFile->exists);

    // Delete record
    auto count = TorrentPreviewableFile::destroy(8);
    QCOMPARE(count, 1);

    /* This is normal, Eloquent behaves the same way, there is no way
       to set 'exists' to the false value in the torrentFiles vector
       from the TinyORM. */
    torrentFile->exists = false;

    // Check if it was really deleted from the database
    auto torrentFileCheck = TorrentPreviewableFile::find(8);
    QVERIFY(!torrentFileCheck);

    // Save it back to the database, recreate it
    auto saveResult = torrentFile->save();

    QVERIFY(saveResult);
    QVERIFY(torrentFile->exists);
}

void tst_BaseModel::destroyWithVector() const
{
    auto torrentFiles =
            TorrentPreviewableFile::where({{"id", 7, "="},
                                           {"id", 8, "=", "or"}})->get();
    auto &torrentFile7 = torrentFiles[0];
    auto &torrentFile8 = torrentFiles[1];

    QCOMPARE(torrentFiles.size(), 2);
    QCOMPARE(torrentFile7.getAttribute("id"), QVariant(7));
    QCOMPARE(torrentFile8.getAttribute("id"), QVariant(8));
    QVERIFY(torrentFile7.exists);
    QVERIFY(torrentFile8.exists);

    // Delete both at once
    auto count = TorrentPreviewableFile::destroy({7, 8});
    QCOMPARE(count, 2);

    /* This is normal, Eloquent behaves the same way, there is no way
       to set 'exists' to the false value in the torrentFiles vector
       from the TinyORM. */
    torrentFile7.exists = false;
    torrentFile8.exists = false;

    // Check if they was really deleted from the database
    auto torrentFilesCheck =
            TorrentPreviewableFile::where({{"id", 7, "="},
                                           {"id", 8, "=", "or"}})->get();
    QCOMPARE(torrentFilesCheck.size(), 0);

    // Save them back to the database, recreate them
    auto saveResult7 = torrentFile7.save();
    auto saveResult8 = torrentFile8.save();

    QVERIFY(saveResult7);
    QVERIFY(saveResult8);
    QVERIFY(torrentFile7.exists);
    QVERIFY(torrentFile8.exists);
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

void tst_BaseModel::all_Columns() const
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

void tst_BaseModel::where_WithVector() const
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

void tst_BaseModel::where_WithVector_Condition() const
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

void tst_BaseModel::arrayOperator() const
{
    auto torrent = Torrent::where("id", "=", 2)->first();
    QVERIFY(torrent);
    QCOMPARE((*torrent)["id"], QVariant(2));
    QCOMPARE((*torrent)["name"], QVariant("test2"));
    QCOMPARE((*torrent)["added_on"],
            QVariant(QDateTime::fromString("2020-08-02 20:11:10", Qt::ISODate)));
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

void tst_BaseModel::isCleanAndIsDirty() const
{
    auto torrent = Torrent::find(4);

    QVERIFY(torrent->isClean());
    QVERIFY(!torrent->isDirty());
    QVERIFY(torrent->isClean("name"));
    QVERIFY(!torrent->isDirty("name"));

    torrent->setAttribute("name", "test4 dirty");

    QVERIFY(!torrent->isClean());
    QVERIFY(torrent->isDirty());
    QVERIFY(!torrent->isClean("name"));
    QVERIFY(torrent->isDirty("name"));
    QVERIFY(torrent->isClean("size"));
    QVERIFY(!torrent->isDirty("size"));

    torrent->save();

    QVERIFY(torrent->isClean());
    QVERIFY(!torrent->isDirty());
    QVERIFY(torrent->isClean("name"));
    QVERIFY(!torrent->isDirty("name"));
    QVERIFY(torrent->isClean("size"));
    QVERIFY(!torrent->isDirty("size"));

    // Restore the name
    torrent->setAttribute("name", "test4");
    torrent->save();
}

void tst_BaseModel::wasChanged() const
{
    auto torrent = Torrent::find(4);

    QVERIFY(!torrent->wasChanged());
    QVERIFY(!torrent->wasChanged("name"));

    torrent->setAttribute("name", "test4 changed");

    QVERIFY(!torrent->wasChanged());
    QVERIFY(!torrent->wasChanged("name"));

    torrent->save();

    QVERIFY(torrent->wasChanged());
    QVERIFY(torrent->wasChanged("name"));
    QVERIFY(!torrent->wasChanged("size"));

    // Restore the name
    torrent->setAttribute("name", "test4");
    torrent->save();
}

void tst_BaseModel::is() const
{
    auto torrent2_1 = Torrent::find(2);
    auto torrent2_2 = Torrent::find(2);

    // The same primary key, table name and connection name
    QVERIFY(torrent2_1->is(torrent2_2));
}

void tst_BaseModel::isNot() const
{
    auto torrent2_1 = Torrent::find(2);
    auto torrent2_2 = Torrent::find(2);
    auto torrent3 = Torrent::find(3);
    auto file4 = TorrentPreviewableFile::find(4);

    // Different primary key
    QVERIFY(torrent2_1->isNot(torrent3));
    // Different table name
    QVERIFY(torrent2_1->isNot(file4));

    // Different connection name
    torrent2_2->setConnection("crystal");
    QVERIFY(torrent2_1->isNot(torrent2_2));
}

QTEST_MAIN(tst_BaseModel)

#include "tst_basemodel.moc"
