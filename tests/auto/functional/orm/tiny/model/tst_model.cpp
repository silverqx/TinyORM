#include <QCoreApplication>
#include <QtSql/QSqlDriver>
#include <QtTest>

#include "orm/db.hpp"

#include "models/massassignmentmodels.hpp"
#include "models/setting.hpp"
#include "models/torrent.hpp"

#include "databases.hpp"

using namespace Orm::Constants;

using Orm::QueryError;
using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::ModelNotFoundError;

using TestUtils::Databases;

// TEST tests, look at commit history for inspiration for new tests silverqx
class tst_Model : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase_data() const;

    void save_Insert() const;
    void save_Insert_WithDefaultValues() const;
    void save_Insert_TableWithoutAutoincrementKey() const;
    void save_Update_Success() const;
    void save_Update_WithNullValue() const;
    void save_Update_Failed() const;

    void remove() const;
    void destroy() const;
    void destroyWithVector() const;

    void all() const;
    void all_Columns() const;

    void latest() const;
    void oldest() const;

    // TEST where conditions, and orWhere ... silverqx
    void where() const;
    void whereEq() const;
    void where_WithVector() const;
    void where_WithVector_Condition() const;

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

    void fresh_OnlyAttributes() const;
    void refresh_OnlyAttributes() const;

    void create() const;
    void create_Failed() const;

    void update() const;
    void update_OnNonExistentModel() const;
    void update_NonExistentAttribute() const;
    void update_SameValue() const;

    void truncate() const;

    void massAssignment_isGuardableColumn() const;
};

void tst_Model::initTestCase_data() const
{
    const auto &connections = Databases::createConnections();

    if (connections.isEmpty())
        QSKIP(QStringLiteral("%1 autotest skipped, environment variables "
                             "for ANY connection have not been defined.")
              .arg("tst_Model").toUtf8().constData(), );

    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : connections)
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

void tst_Model::save_Insert() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent;

    const auto addedOn = QDateTime::fromString("2020-10-01 20:22:10", Qt::ISODate);
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
    QVERIFY(torrent.getAttribute(ID).isValid());
    QVERIFY(torrent.getAttribute(ID).value<quint64>() > 6);
    QCOMPARE(torrent.getAttribute("name"), QVariant("test50"));
    QCOMPARE(torrent.getAttribute("size"), QVariant(50));
    QCOMPARE(torrent.getAttribute("progress"), QVariant(50));
    QCOMPARE(torrent.getAttribute("added_on"), QVariant(addedOn));
    QCOMPARE(torrent.getAttribute("hash"),
             QVariant("5079e3af2768cdf52ec84c1f320333f68401dc61"));
    QVERIFY(torrent.getAttribute(CREATED_AT).isValid());
    QVERIFY(torrent.getAttribute(UPDATED_AT).isValid());

    // Get the fresh record from the database
    auto torrentToVerify = Torrent::find(torrent.getAttribute(ID));
    QVERIFY(torrentToVerify);
    QVERIFY(torrentToVerify->exists);

    // And check attributes again
    QCOMPARE(torrentToVerify->getAttribute(ID), torrent.getAttribute(ID));
    QCOMPARE(torrentToVerify->getAttribute("name"), QVariant("test50"));
    QCOMPARE(torrentToVerify->getAttribute("size"), QVariant(50));
    QCOMPARE(torrentToVerify->getAttribute("progress"), QVariant(50));
    QCOMPARE(torrentToVerify->getAttribute("added_on"), QVariant(addedOn));
    QCOMPARE(torrentToVerify->getAttribute("hash"),
             QVariant("5079e3af2768cdf52ec84c1f320333f68401dc61"));
    QVERIFY(torrentToVerify->getAttribute(CREATED_AT).isValid());
    QVERIFY(torrentToVerify->getAttribute(UPDATED_AT).isValid());

    // Remove it
    torrent.remove();
    QVERIFY(!torrent.exists);
}

void tst_Model::save_Insert_WithDefaultValues() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent;

    const auto addedOn = QDateTime::fromString("2020-10-01 20:22:10", Qt::ISODate);
    torrent.setAttribute("name", "test51")
            .setAttribute("added_on", addedOn)
            .setAttribute("hash", "5179e3af2768cdf52ec84c1f320333f68401dc61");

    QVERIFY(!torrent.exists);
    const auto result = torrent.save();
    QVERIFY(result);
    QVERIFY(torrent.exists);

    // Check attributes after save
    QVERIFY(torrent.getAttribute(ID).isValid());
    QVERIFY(torrent.getAttribute(ID).value<quint64>() > 6);
    QCOMPARE(torrent.getAttribute("name"), QVariant("test51"));
    QCOMPARE(torrent.getAttribute("added_on"), QVariant(addedOn));
    QCOMPARE(torrent.getAttribute("hash"),
             QVariant("5179e3af2768cdf52ec84c1f320333f68401dc61"));
    QVERIFY(torrent.getAttribute(CREATED_AT).isValid());
    QVERIFY(torrent.getAttribute(UPDATED_AT).isValid());

    // Get the fresh record from the database
    auto torrentToVerify = Torrent::find(torrent.getAttribute(ID));
    QVERIFY(torrentToVerify);
    QVERIFY(torrentToVerify->exists);

    // And check attributes again
    QVERIFY(torrentToVerify->getAttribute(ID).isValid());
    QVERIFY(torrentToVerify->getAttribute(ID).value<quint64>() > 6);
    QCOMPARE(torrentToVerify->getAttribute("name"), QVariant("test51"));
    QCOMPARE(torrentToVerify->getAttribute("size"), QVariant(0));
    QCOMPARE(torrentToVerify->getAttribute("progress"), QVariant(0));
    QCOMPARE(torrentToVerify->getAttribute("added_on"), QVariant(addedOn));
    QCOMPARE(torrentToVerify->getAttribute("hash"),
             QVariant("5179e3af2768cdf52ec84c1f320333f68401dc61"));
    QVERIFY(torrentToVerify->getAttribute(CREATED_AT).isValid());
    QVERIFY(torrentToVerify->getAttribute(UPDATED_AT).isValid());

    // Remove it
    torrent.remove();
    QVERIFY(!torrent.exists);
}

void tst_Model::save_Insert_TableWithoutAutoincrementKey() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Setting setting;

    setting.setAttribute("name", "setting1")
            .setAttribute("value", "value1");

    QVERIFY(!setting.exists);
    const auto result = setting.save();
    QVERIFY(result);
    QVERIFY(setting.exists);

    // Check attributes after save
    QVERIFY(!setting.getAttribute(ID).isValid());
    QCOMPARE(setting.getAttribute("name"), QVariant("setting1"));
    QCOMPARE(setting.getAttribute("value"), QVariant("value1"));
    QVERIFY(setting.getAttribute(CREATED_AT).isValid());
    QVERIFY(setting.getAttribute(UPDATED_AT).isValid());

    // Get the fresh record from the database
    auto settingToVerify = Setting::whereEq("name", "setting1")->first();
    QVERIFY(settingToVerify);
    QVERIFY(settingToVerify->exists);

    // And check attributes again
    QVERIFY(!settingToVerify->getAttribute(ID).isValid());
    QCOMPARE(settingToVerify->getAttribute("name"), QVariant("setting1"));
    QCOMPARE(settingToVerify->getAttribute("value"), QVariant("value1"));
    QVERIFY(settingToVerify->getAttribute(CREATED_AT).isValid());
    QVERIFY(settingToVerify->getAttribute(UPDATED_AT).isValid());

    // Remove it
    int affected = 0;
    std::tie(affected, std::ignore) =
            Setting::whereEq("name", "setting1")->remove();
    QCOMPARE(affected, 1);
}

void tst_Model::save_Update_Success() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFile::find(4);
    QVERIFY(torrentFile);
    QVERIFY(torrentFile->exists);
    QCOMPARE(torrentFile->getAttribute(ID), QVariant(4));

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

    // Revert
    torrentFile->setAttribute("filepath", "test3_file1.mkv")
            .setAttribute("size", 5568)
            .setAttribute("progress", 870);
    torrentFile->save();
}

void tst_Model::save_Update_WithNullValue() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto peer = TorrentPeer::find(4);
    QVERIFY(peer);
    QVERIFY(peer->exists);
    QCOMPARE(peer->getAttribute("total_seeds"), QVariant(4));

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
    /* SQLite doesn't return correct underlying type in the QVariant for null values
       like MySQL driver does, skip this compare for the SQLite database. */
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (DB::connection(connection).driverName() != "QSQLITE")
        QCOMPARE(peerVerify->getAttribute("total_seeds"),
                 QVariant(QMetaType(QMetaType::Int)));
#else
    if (DB::connection(connection).driverName() != "QSQLITE")
        QCOMPARE(peerVerify->getAttribute("total_seeds"), QVariant(QVariant::Int));
#endif

    // Revert
    peer->setAttribute("total_seeds", 4);
    peer->save();
}

void tst_Model::save_Update_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto peer = TorrentPeer::find(3);
    QVERIFY(peer);
    QVERIFY(peer->exists);

    peer->setAttribute("total_seeds-NON_EXISTENT", 15);

    QVERIFY_EXCEPTION_THROWN(peer->save(), QueryError);

    QVERIFY(peer->exists);
}

void tst_Model::remove() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

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

void tst_Model::destroy() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFile::find(8);

    QVERIFY(torrentFile);
    QCOMPARE(torrentFile->getAttribute(ID), QVariant(8));
    QVERIFY(torrentFile->exists);

    // Delete record
    auto count = TorrentPreviewableFile::destroy(8);
    QCOMPARE(count, static_cast<std::size_t>(1));

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

void tst_Model::destroyWithVector() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrentFiles =
            TorrentPreviewableFile::where({{ID, 7, "="},
                                           {ID, 8, "=", "or"}})
            ->orderBy(ID)
            .get();
    auto &torrentFile7 = torrentFiles[0];
    auto &torrentFile8 = torrentFiles[1];

    QCOMPARE(torrentFiles.size(), 2);
    QCOMPARE(torrentFile7.getAttribute(ID), QVariant(7));
    QCOMPARE(torrentFile8.getAttribute(ID), QVariant(8));
    QVERIFY(torrentFile7.exists);
    QVERIFY(torrentFile8.exists);

    // Delete both at once
    auto count = TorrentPreviewableFile::destroy({7, 8});
    QCOMPARE(count, static_cast<std::size_t>(2));

    /* This is normal, Eloquent behaves the same way, there is no way
       to set 'exists' to the false value in the torrentFiles vector
       from the TinyORM. */
    torrentFile7.exists = false;
    torrentFile8.exists = false;

    // Check if they was really deleted from the database
    auto torrentFilesCheck =
            TorrentPreviewableFile::where({{ID, 7, "="},
                                           {ID, 8, "=", "or"}})->get();
    QCOMPARE(torrentFilesCheck.size(), 0);

    // Save them back to the database, recreate them
    auto saveResult7 = torrentFile7.save();
    auto saveResult8 = torrentFile8.save();

    QVERIFY(saveResult7);
    QVERIFY(saveResult8);
    QVERIFY(torrentFile7.exists);
    QVERIFY(torrentFile8.exists);
}

void tst_Model::all() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::all();

    QCOMPARE(torrents.size(), 6);

    const std::unordered_map<quint64, QString> expectedIdNames {
        {1, "test1"}, {2, "test2"}, {3, "test3"},
        {4, "test4"}, {5, "test5"}, {6, "test6"},
    };
    for (const auto &torrent : torrents) {
        const auto torrentId = torrent[ID].value<quint64>();

        QVERIFY(expectedIdNames.contains(torrentId));
        QCOMPARE(expectedIdNames.at(torrentId), torrent["name"].value<QString>());
    }
}

void tst_Model::all_Columns() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    {
        auto torrents = Torrent::all();

        QCOMPARE(torrents.at(1).getAttributes().size(), 9);
    }
    {
        auto torrents = Torrent::all({ID, "name"});

        const auto &torrent = torrents.at(1);
        QCOMPARE(torrent.getAttributes().size(), 2);
        QCOMPARE(torrent.getAttributes().at(0).key, QString(ID));
        QCOMPARE(torrent.getAttributes().at(1).key, QString("name"));
    }
}

void tst_Model::latest() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

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

void tst_Model::oldest() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

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

void tst_Model::where() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::where(ID, "=", 3)->first();
        QVERIFY(torrent);
        QCOMPARE(torrent->getAttribute(ID), QVariant(3));
    }
    {
        auto torrents = Torrent::where(ID, ">=", 3)->get();

        QCOMPARE(torrents.size(), 4);

        const QVector<QVariant> expectedIds {3, 4, 5, 6};
        for (const auto &torrent : torrents)
            QVERIFY(expectedIds.contains(torrent.getAttribute(ID)));
    }
}

void tst_Model::whereEq() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    // number
    {
        auto torrent = Torrent::whereEq(ID, 3)->first();
        QVERIFY(torrent);
        QCOMPARE(torrent->getAttribute(ID), QVariant(3));
    }
    // string
    {
        auto torrent = Torrent::whereEq("name", "test3")->first();
        QVERIFY(torrent);
        QCOMPARE(torrent->getAttribute(ID), QVariant(3));
    }
    // QDateTime
    {
        auto torrent = Torrent::whereEq(
                           "added_on",
                           QDateTime::fromString("2020-08-01 20:11:10", Qt::ISODate))
                       ->first();
        QVERIFY(torrent);
        QCOMPARE(torrent->getAttribute(ID), QVariant(1));
    }
}

void tst_Model::where_WithVector() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::where({{ID, 3}})->first();
        QVERIFY(torrent);
        QCOMPARE(torrent->getAttribute(ID), QVariant(3));
    }
    {
        auto torrents = Torrent::where({{ID, 3, ">="}})->get();
        QCOMPARE(torrents.size(), 4);

        const std::unordered_map<quint64, QString> expectedIdNames {
            {3, "test3"}, {4, "test4"}, {5, "test5"}, {6, "test6"},
        };
        for (const auto &torrent : torrents) {
            const auto torrentId = torrent[ID].value<quint64>();

            QVERIFY(expectedIdNames.contains(torrentId));
            QCOMPARE(expectedIdNames.at(torrentId), torrent["name"].value<QString>());
        }
    }
}

void tst_Model::where_WithVector_Condition() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    {
        auto torrents = Torrent::where({{"size", 14}, {"progress", 400}})->get();
        QCOMPARE(torrents.size(), 1);
        QCOMPARE(torrents.at(0).getAttribute(ID), QVariant(4));
    }
    {
        auto torrents = Torrent::where({{"size", 13}, {"size", 14, "=", "or"}})->get();
        QCOMPARE(torrents.size(), 2);

        const QVector<QVariant> expectedIds {3, 4};
        for (const auto &torrent : torrents)
            QVERIFY(expectedIds.contains(torrent[ID]));
    }
    {
        auto torrents = Torrent::where({{"size", 13}, {"size", 14, "=", "or"}})
                        ->where("progress", "=", 400)
                        .get();
        QCOMPARE(torrents.size(), 1);
        QCOMPARE(torrents.at(0).getAttribute(ID), QVariant(4));
    }
}

void tst_Model::find() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(3);
    QVERIFY(torrent);
    QCOMPARE(torrent->getAttribute(ID), QVariant(3));
}

void tst_Model::findOrNew_Found() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::findOrNew(3);

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 9);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
    }
    {
        auto torrent = Torrent::findOrNew(3, {ID, "name"});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 2);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
    }
}

void tst_Model::findOrNew_NotFound() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::findOrNew(999999);

        QVERIFY(!torrent.exists);
        QVERIFY(torrent.getAttributes().isEmpty());
        QCOMPARE(torrent[ID], QVariant());
        QCOMPARE(torrent["name"], QVariant());
    }
    {
        auto torrent = Torrent::findOrNew(999999, {ID, "name"});

        QVERIFY(!torrent.exists);
        QVERIFY(torrent.getAttributes().isEmpty());
        QCOMPARE(torrent[ID], QVariant());
        QCOMPARE(torrent["name"], QVariant());
    }
}

void tst_Model::findOrFail_Found() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::findOrFail(3);

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 9);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
    }
    {
        auto torrent = Torrent::findOrFail(3, {ID, "name"});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 2);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
    }
}

void tst_Model::findOrFail_NotFoundFailed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    QVERIFY_EXCEPTION_THROWN(Torrent::findOrFail(999999),
                             ModelNotFoundError);
    QVERIFY_EXCEPTION_THROWN(Torrent::findOrFail(999999, {ID, "name"}),
                             ModelNotFoundError);
}

void tst_Model::firstWhere() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    {
        auto torrentFile3 = TorrentPreviewableFile::firstWhere(ID, "=", 3);

        QVERIFY(torrentFile3->exists);
        QCOMPARE((*torrentFile3)[ID], QVariant(3));
        QCOMPARE((*torrentFile3)["filepath"], QVariant("test2_file2.mkv"));
    }
    {
        auto torrentFile1 =
                TorrentPreviewableFile::orderBy(ID)->firstWhere(ID, "<", 4);

        QVERIFY(torrentFile1->exists);
        QCOMPARE((*torrentFile1)[ID], QVariant(1));
        QCOMPARE((*torrentFile1)["filepath"], QVariant("test1_file1.mkv"));
    }
}

void tst_Model::firstWhereEq() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrentFile3 = TorrentPreviewableFile::firstWhereEq(ID, 3);

    QVERIFY(torrentFile3->exists);
    QCOMPARE((*torrentFile3)[ID], QVariant(3));
    QCOMPARE((*torrentFile3)["filepath"], QVariant("test2_file2.mkv"));
}

void tst_Model::firstOrNew_Found() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::firstOrNew({{"name", "test3"}});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 9);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
        QCOMPARE(torrent["size"], QVariant(13));
        QCOMPARE(torrent["progress"], QVariant(300));
    }
    {
        auto torrent = Torrent::firstOrNew(
                           {{"name", "test3"}},

                           {{"size", 113},
                            {"progress", 313}});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 9);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
        QCOMPARE(torrent["size"], QVariant(13));
        QCOMPARE(torrent["progress"], QVariant(300));
    }
}

void tst_Model::firstOrNew_NotFound() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::firstOrNew({{"name", "test100"}});

        QVERIFY(!torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 1);
        QCOMPARE(torrent["name"], QVariant("test100"));
    }
    {
        auto torrent = Torrent::firstOrNew(
                           {{"name", "test100"}},

                           {{"size", 113},
                            {"progress", 313}});

        QVERIFY(!torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 3);
        QCOMPARE(torrent["name"], QVariant("test100"));
        QCOMPARE(torrent["size"], QVariant(113));
        QCOMPARE(torrent["progress"], QVariant(313));
    }
}

void tst_Model::firstOrCreate_Found() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::firstOrCreate({{"name", "test3"}});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 9);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
        QCOMPARE(torrent["size"], QVariant(13));
        QCOMPARE(torrent["progress"], QVariant(300));
    }
    {
        const auto addedOn = QDateTime::currentDateTime();

        auto torrent = Torrent::firstOrCreate(
                           {{"name", "test3"}},

                           {{"size", 33},
                            {"progress", 33},
                            {"added_on", addedOn},
                            {"hash", "0009e3af2768cdf52ec84c1f320333f68401dc60"}});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 9);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
        QCOMPARE(torrent["size"], QVariant(13));
        QCOMPARE(torrent["progress"], QVariant(300));
        QCOMPARE(torrent["added_on"],
                QVariant(QDateTime::fromString("2020-08-03 20:11:10", Qt::ISODate)));
        QCOMPARE(torrent["hash"], QVariant("3579e3af2768cdf52ec84c1f320333f68401dc6e"));
    }
}

void tst_Model::firstOrCreate_NotFound() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    const auto addedOn = QDateTime::fromString("2020-10-01 20:22:10", Qt::ISODate);

    auto torrent = Torrent::firstOrCreate(
                       {{"name", "test100"}},

                       {{"size", 113},
                        {"progress", 313},
                        {"added_on", addedOn},
                        {"hash", "1999e3af2768cdf52ec84c1f320333f68401dc6e"}});

    QVERIFY(torrent.exists);
    QCOMPARE(torrent.getAttributes().size(), 8);
    QVERIFY(torrent[ID]->value<quint64>() > 6);
    QCOMPARE(torrent["name"], QVariant("test100"));
    QCOMPARE(torrent["size"], QVariant(113));
    QCOMPARE(torrent["progress"], QVariant(313));
    QCOMPARE(torrent["added_on"], QVariant(addedOn));
    QCOMPARE(torrent["hash"], QVariant("1999e3af2768cdf52ec84c1f320333f68401dc6e"));

    const auto result = torrent.remove();
    QVERIFY(result);
    QVERIFY(!torrent.exists);
}

void tst_Model::isCleanAndIsDirty() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(3);

    QVERIFY(torrent->isClean());
    QVERIFY(!torrent->isDirty());
    QVERIFY(torrent->isClean("name"));
    QVERIFY(!torrent->isDirty("name"));

    torrent->setAttribute("name", "test3 dirty");

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
    torrent->setAttribute("name", "test3");
    torrent->save();
}

void tst_Model::wasChanged() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(3);

    QVERIFY(!torrent->wasChanged());
    QVERIFY(!torrent->wasChanged("name"));

    torrent->setAttribute("name", "test3 changed");

    QVERIFY(!torrent->wasChanged());
    QVERIFY(!torrent->wasChanged("name"));

    torrent->save();

    QVERIFY(torrent->wasChanged());
    QVERIFY(torrent->wasChanged("name"));
    QVERIFY(!torrent->wasChanged("size"));

    // Restore the name
    torrent->setAttribute("name", "test3");
    torrent->save();
}

void tst_Model::is() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent2_1 = Torrent::find(2);
    auto torrent2_2 = Torrent::find(2);

    // The same primary key, table name and connection name
    QVERIFY(torrent2_1->is(torrent2_2));
}

void tst_Model::isNot() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent2_1 = Torrent::find(2);
    auto torrent2_2 = Torrent::find(2);
    auto torrent3 = Torrent::find(3);
    auto file4 = TorrentPreviewableFile::find(4);

    // Different primary key
    QVERIFY(torrent2_1->isNot(torrent3));
    // Different table name
    QVERIFY(torrent2_1->isNot(file4));

    // Different connection name
    torrent2_2->setConnection("dummy_connection");
    /* Disable connection override, so isNot() can pickup a connection from the model
       itself and not overriden connection. */
    ConnectionOverride::connection = "";
    QVERIFY(torrent2_1->isNot(torrent2_2));
}

void tst_Model::fresh_OnlyAttributes() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    // Doesn't exist
    {
        Torrent torrent;
        QVERIFY(torrent.fresh() == std::nullopt);
        QVERIFY(!torrent.exists);
    }
    // Exist
    {
        auto torrent = Torrent::find(3);
        QVERIFY(torrent);
        QVERIFY(torrent->exists);
        QCOMPARE(torrent->getAttribute(ID), QVariant(3));

        torrent->setAttribute("name", "test3 fresh");
        QCOMPARE(torrent->getAttribute("name"), QVariant("test3 fresh"));

        auto freshTorrent = torrent->fresh();
        QVERIFY(freshTorrent);
        QVERIFY(&*torrent != &*freshTorrent);
        QVERIFY(freshTorrent->exists);
        QCOMPARE(freshTorrent->getAttribute(ID), QVariant(3));
        QCOMPARE(freshTorrent->getAttribute("name"), QVariant("test3"));
    }
}

void tst_Model::refresh_OnlyAttributes() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    // Doens't exist
    {
        Torrent torrent;
        QVERIFY(&torrent.refresh() == &torrent);
        QVERIFY(!torrent.exists);
    }
    // Exists
    {
        auto torrent = Torrent::find(3);
        QVERIFY(torrent);
        QVERIFY(torrent->exists);
        QCOMPARE(torrent->getAttribute(ID), QVariant(3));

        auto original = torrent->getAttribute("name");
        QCOMPARE(original, QVariant("test3"));

        torrent->setAttribute("name", "test3 refresh");
        QCOMPARE(torrent->getAttribute("name"), QVariant("test3 refresh"));

        auto &refreshedTorrent = torrent->refresh();

        QVERIFY(&*torrent == &refreshedTorrent);
        QVERIFY(refreshedTorrent.exists);
        QCOMPARE(refreshedTorrent.getAttribute(ID), torrent->getAttribute(ID));
        QCOMPARE(refreshedTorrent.getAttribute("name"), original);
    }
}

void tst_Model::create() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    const auto addedOn = QDateTime::fromString("2021-02-01 20:22:10", Qt::ISODate);

    auto torrent = Torrent::create({
        {"name",     "test100"},
        {"size",     100},
        {"progress", 333},
        {"added_on", addedOn},
        {"hash",     "1009e3af2768cdf52ec84c1f320333f68401dc6e"},
    });

    QVERIFY(torrent.exists);
    QVERIFY(torrent[ID]->isValid());
    QVERIFY(torrent[ID]->value<quint64>() > 6);
    QCOMPARE(torrent["name"], QVariant("test100"));
    QCOMPARE(torrent["size"], QVariant(100));
    QCOMPARE(torrent["progress"], QVariant(333));
    QCOMPARE(torrent["added_on"], QVariant(addedOn));
    QCOMPARE(torrent["hash"], QVariant("1009e3af2768cdf52ec84c1f320333f68401dc6e"));

    QVERIFY(!torrent.isDirty());
    QVERIFY(!torrent.wasChanged());

    torrent.setAttribute("name", "test100 create");
    torrent.save();

    QVERIFY(torrent.exists);
    QCOMPARE(torrent["name"], QVariant("test100 create"));

    QVERIFY(torrent.wasChanged());

    torrent.remove();

    QVERIFY(!torrent.exists);
}

void tst_Model::create_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    const auto addedOn = QDateTime::fromString("2021-02-01 20:22:10", Qt::ISODate);

    Torrent torrent;
    QVERIFY_EXCEPTION_THROWN((torrent = Torrent::create({
        {"name-NON_EXISTENT", "test100"},
        {"size",              100},
        {"progress",          333},
        {"added_on",          addedOn},
        {"hash",              "1009e3af2768cdf52ec84c1f320333f68401dc6e"},
    })), QueryError);

    QVERIFY(!torrent.exists);
    QVERIFY(torrent.getAttributes().isEmpty());
}

void tst_Model::update() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto timeBeforeUpdate = QDateTime::currentDateTime();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeUpdate.time();
        timeBeforeUpdate.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    auto torrent = Torrent::find(4);

    auto &updatedAtColumn = torrent->getUpdatedAtColumn();

    auto progressOriginal = torrent->getAttribute("progress");
    auto updatedAtOriginal = torrent->getAttribute(updatedAtColumn);

    QVERIFY(torrent->exists);
    QCOMPARE(progressOriginal, QVariant(400));
    QCOMPARE(updatedAtOriginal,
             QVariant(QDateTime::fromString("2021-01-04 18:46:31", Qt::ISODate)));

    auto result = torrent->update({{"progress", 449}});

    QVERIFY(result);
    QCOMPARE(torrent->getAttribute("progress"), QVariant(449));
    QVERIFY(!torrent->isDirty());
    QVERIFY(torrent->wasChanged());

    // Verify value in the database
    auto torrentVerify = Torrent::find(4);
    QVERIFY(torrentVerify->exists);
    QCOMPARE(torrentVerify->getAttribute("progress"), QVariant(449));
    QVERIFY(torrentVerify->getAttribute(updatedAtColumn).toDateTime()
            >= timeBeforeUpdate);

    // Revert value back
    auto resultRevert = torrent->update({{"progress", progressOriginal},
                                         {updatedAtColumn, updatedAtOriginal}});
    QVERIFY(resultRevert);
    QCOMPARE(torrent->getAttribute("progress"), progressOriginal);
    /* Needed to convert toDateTime() because Model::update() set update_at
       attribute as QString. */
    QCOMPARE(torrent->getAttribute(updatedAtColumn).toDateTime(),
             updatedAtOriginal.toDateTime());
}

void tst_Model::update_OnNonExistentModel() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent;

    QVERIFY(!torrent.exists);

    QVERIFY(!torrent.update({{"progress", 333}}));
}

void tst_Model::update_NonExistentAttribute() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(3);

    QVERIFY(torrent->exists);

    /* Zero attributes will be changed because torrent.isDirty() == false and
       Torrent::save() returns true in this case. */
    QVERIFY(torrent->update({{"progress-NON_EXISTENT", 333}}));
}

void tst_Model::update_SameValue() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(3);
    QVERIFY(torrent->exists);

    auto &updatedAtColumn = torrent->getUpdatedAtColumn();
    auto updatedAt = torrent->getAttribute(updatedAtColumn);

    /* Doesn't send update query to the database, this is different from
       the TinyBuilder::update() method. */
    auto result = torrent->update({{"progress", 300}});

    QVERIFY(result);
    QVERIFY(!torrent->isDirty());
    QVERIFY(!torrent->wasChanged());

    // Verify value in the database
    auto torrentVerify = Torrent::find(3);
    QVERIFY(torrentVerify->exists);
    QCOMPARE(torrentVerify->getAttribute(updatedAtColumn), updatedAt);
}

void tst_Model::truncate() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Setting setting;
    setting.setAttribute("name", "truncate");
    setting.setAttribute("value", "yes");

    QVERIFY(!setting.exists);
    const auto result = setting.save();
    QVERIFY(result);
    QVERIFY(setting.exists);

    // Get the fresh record from the database
    auto settingToVerify = Setting::whereEq("name", "truncate")->first();
    QVERIFY(settingToVerify);
    QVERIFY(settingToVerify->exists);

    // And check attributes
    QCOMPARE(settingToVerify->getAttribute("name"), QVariant("truncate"));
    QCOMPARE(settingToVerify->getAttribute("value"), QVariant("yes"));

    Setting::truncate();

    QCOMPARE(Setting::all().size(), 0);
}

void tst_Model::massAssignment_isGuardableColumn() const
{
    /* This test has to be here because it internally calls columns listing against
       database, so it is connection-dependent. */
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent_GuardableColumn torrent;

    torrent.fill({{UPDATED_AT, QDateTime::currentDateTime()}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent.getAttributes().size(), 1);
}

QTEST_MAIN(tst_Model)

#include "tst_model.moc"
