#include <QCoreApplication>
#include <QTest>

#include "orm/macros/sqldrivermappings.hpp"
#include TINY_INCLUDE_TSqlDriver

#include "orm/db.hpp"
#include "orm/utils/nullvariant.hpp"
#include "orm/utils/query.hpp"

#include "databases.hpp"
#include "macros.hpp"

#include "models/massassignmentmodels.hpp"
#include "models/setting.hpp"
#include "models/torrent.hpp"

using Orm::Constants::ASTERISK;
using Orm::Constants::CREATED_AT;
using Orm::Constants::HASH_;
using Orm::Constants::ID;
using Orm::Constants::LT;
using Orm::Constants::NAME;
using Orm::Constants::Progress;
using Orm::Constants::QMYSQL;
using Orm::Constants::QSQLITE;
using Orm::Constants::SIZE_;
using Orm::Constants::UPDATED_AT;

using Orm::DB;
using Orm::TTimeZone;
using Orm::Utils::NullVariant;

using TQueryError;

using QueryUtils = Orm::Utils::Query;
using TypeUtils = Orm::Utils::Type;

using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::Exceptions::ModelNotFoundError;
using Orm::Tiny::Model;
using Orm::Tiny::Types::ModelsCollection;

using TestUtils::Databases;

using Models::Role;
using Models::Setting;
using Models::TagProperty;
using Models::Torrent;
using Models::Torrent_GuardableColumn;
using Models::TorrentPeer;
using Models::TorrentPreviewableFile;

// TEST tests, look at commit history for inspiration for new tests silverqx
class tst_Model : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
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

    void whereExists() const;

    void find() const;
    void findOrNew_Found() const;
    void findOrNew_NotFound() const;
    void findOrFail_Found() const;
    void findOrFail_NotFoundFailed() const;

    void findMany() const;
    void findMany_Empty() const;

    void findOr() const;
    void findOr_WithReturnType() const;

    void first() const;

    void firstOr() const;
    void firstOr_WithReturnType() const;

    void firstWhere() const;
    void firstWhereEq() const;
    void firstOrNew_Found() const;
    void firstOrNew_NotFound() const;
    void firstOrCreate_Found() const;
    void firstOrCreate_NotFound() const;

    void wasChanged() const;

    void fresh_OnlyAttributes() const;
    void refresh_OnlyAttributes() const;

    void create() const;
    void create_Failed() const;

    void incrementAndDecrement() const;
    void incrementAndDecrement_OnNonExistentModel_WithoutAll() const;

    void update() const;
    void update_OnNonExistentModel() const;
    void update_NonExistentAttribute() const;
    void update_SameValue() const;

    void upsert() const;

    void truncate() const;

    void massAssignment_isGuardableColumn() const;

    /* HasTimestamps */
    void touch_WithAttribute() const;

    /* Touching timestamps */
    void addTouch_setTouchedRelations_getTouchedRelations_touches_clearTouches() const;

    /* Attributes - Unix timestamps */
    void getAttribute_UnixTimestamp_With_UDates() const;
    void getAttribute_UnixTimestamp_WithOut_UDates() const;

    void getAttribute_UnixTimestamp_With_UDates_Null() const;
    void getAttribute_UnixTimestamp_WithOut_UDates_Null() const;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Model::initTestCase_data() const
{
    const auto connections = Databases::createConnections();

    if (connections.isEmpty())
        QSKIP(TestUtils::AutoTestSkippedAny.arg(TypeUtils::classPureBasename(*this))
                                           .toUtf8().constData(), );

    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : connections)
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

void tst_Model::save_Insert() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent;

    const auto addedOn = QDateTime({2020, 10, 1}, {20, 22, 10}, TTimeZone::UTC);
    torrent.setAttribute(NAME, "test50")
            .setAttribute(SIZE_, 50)
            .setAttribute(Progress, 50)
            .setAttribute("added_on", addedOn)
            .setAttribute(HASH_, "5079e3af2768cdf52ec84c1f320333f68401dc61");

    QVERIFY(!torrent.exists);
    const auto result = torrent.save();
    QVERIFY(result);
    QVERIFY(torrent.exists);

    // Check attributes after save
    QVERIFY(torrent.getKey().isValid());
    QVERIFY(torrent.getKeyCasted() > 6);
    QCOMPARE(torrent.getAttribute(NAME), QVariant("test50"));
    QCOMPARE(torrent.getAttribute(SIZE_), QVariant(50));
    QCOMPARE(torrent.getAttribute(Progress), QVariant(50));
    QCOMPARE(torrent.getAttribute("added_on"), QVariant(addedOn));
    QCOMPARE(torrent.getAttribute(HASH_),
             QVariant("5079e3af2768cdf52ec84c1f320333f68401dc61"));
    QVERIFY(torrent.getAttribute(CREATED_AT).isValid());
    QVERIFY(torrent.getAttribute(UPDATED_AT).isValid());

    // Get the fresh model from the database
    auto torrentToVerify = Torrent::find(torrent.getKey());
    QVERIFY(torrentToVerify);
    QVERIFY(torrentToVerify->exists);

    // And check attributes again
    QCOMPARE(torrentToVerify->getKey(), torrent.getKey());
    QCOMPARE(torrentToVerify->getAttribute(NAME), QVariant("test50"));
    QCOMPARE(torrentToVerify->getAttribute(SIZE_), QVariant(50));
    QCOMPARE(torrentToVerify->getAttribute(Progress), QVariant(50));
    QCOMPARE(torrentToVerify->getAttribute("added_on"), QVariant(addedOn));
    QCOMPARE(torrentToVerify->getAttribute(HASH_),
             QVariant("5079e3af2768cdf52ec84c1f320333f68401dc61"));
    QVERIFY(torrentToVerify->getAttribute(CREATED_AT).isValid());
    QVERIFY(torrentToVerify->getAttribute(UPDATED_AT).isValid());

    // Remove it
    torrent.remove();
    QVERIFY(!torrent.exists);
}

void tst_Model::save_Insert_WithDefaultValues() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent;

    const auto addedOn = QDateTime({2020, 10, 1}, {20, 22, 10}, TTimeZone::UTC);
    torrent.setAttribute(NAME, "test51")
            .setAttribute("added_on", addedOn)
            .setAttribute(HASH_, "5179e3af2768cdf52ec84c1f320333f68401dc61");

    QVERIFY(!torrent.exists);
    const auto result = torrent.save();
    QVERIFY(result);
    QVERIFY(torrent.exists);

    // Check attributes after save
    QVERIFY(torrent.getKey().isValid());
    QVERIFY(torrent.getKeyCasted() > 6);
    QCOMPARE(torrent.getAttribute(NAME), QVariant("test51"));
    QCOMPARE(torrent.getAttribute("added_on"), QVariant(addedOn));
    QCOMPARE(torrent.getAttribute(HASH_),
             QVariant("5179e3af2768cdf52ec84c1f320333f68401dc61"));
    QVERIFY(torrent.getAttribute(CREATED_AT).isValid());
    QVERIFY(torrent.getAttribute(UPDATED_AT).isValid());

    // Get the fresh model from the database
    auto torrentToVerify = Torrent::find(torrent.getKey());
    QVERIFY(torrentToVerify);
    QVERIFY(torrentToVerify->exists);

    // And check attributes again
    QVERIFY(torrentToVerify->getKey().isValid());
    QVERIFY(torrentToVerify->getKeyCasted() > 6);
    QCOMPARE(torrentToVerify->getAttribute(NAME), QVariant("test51"));
    QCOMPARE(torrentToVerify->getAttribute(SIZE_), QVariant(0));
    QCOMPARE(torrentToVerify->getAttribute(Progress), QVariant(0));
    QCOMPARE(torrentToVerify->getAttribute("added_on"), QVariant(addedOn));
    QCOMPARE(torrentToVerify->getAttribute(HASH_),
             QVariant("5179e3af2768cdf52ec84c1f320333f68401dc61"));
    QVERIFY(torrentToVerify->getAttribute(CREATED_AT).isValid());
    QVERIFY(torrentToVerify->getAttribute(UPDATED_AT).isValid());

    // Remove it
    torrent.remove();
    QVERIFY(!torrent.exists);
}

void tst_Model::save_Insert_TableWithoutAutoincrementKey() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Setting setting;

    setting.setAttribute(NAME, "setting1")
            .setAttribute("value", "value1");

    QVERIFY(!setting.exists);
    const auto result = setting.save();
    QVERIFY(result);
    QVERIFY(setting.exists);

    // Check attributes after save
    QVERIFY(!setting.getKey().isValid());
    QCOMPARE(setting.getAttribute(NAME), QVariant("setting1"));
    QCOMPARE(setting.getAttribute("value"), QVariant("value1"));
    QVERIFY(setting.getAttribute(CREATED_AT).isValid());
    QVERIFY(setting.getAttribute(UPDATED_AT).isValid());

    // Get the fresh model from the database
    auto settingToVerify = Setting::whereEq(NAME, "setting1")->first();
    QVERIFY(settingToVerify);
    QVERIFY(settingToVerify->exists);

    // And check attributes again
    QVERIFY(!settingToVerify->getKey().isValid());
    QCOMPARE(settingToVerify->getAttribute(NAME), QVariant("setting1"));
    QCOMPARE(settingToVerify->getAttribute("value"), QVariant("value1"));
    QVERIFY(settingToVerify->getAttribute(CREATED_AT).isValid());
    QVERIFY(settingToVerify->getAttribute(UPDATED_AT).isValid());

    // Remove it
    int affected = 0;
    std::tie(affected, std::ignore) =
            Setting::whereEq(NAME, "setting1")->remove();
    QCOMPARE(affected, 1);
}

void tst_Model::save_Update_Success() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFile::find(4);
    QVERIFY(torrentFile);
    QVERIFY(torrentFile->exists);
    QCOMPARE(torrentFile->getKey(), QVariant(4));

    torrentFile->setAttribute("filepath", "test3_file1-updated.mkv")
            .setAttribute(SIZE_, 5570)
            .setAttribute(Progress, 860);

    const auto result = torrentFile->save();
    QVERIFY(result);
    QVERIFY(torrentFile->exists);

    // Check
    auto torrentFileFresh = TorrentPreviewableFile::find(4);
    QVERIFY(torrentFileFresh);
    QVERIFY(torrentFileFresh->exists);
    QCOMPARE(torrentFileFresh->getAttribute("filepath"),
             QVariant("test3_file1-updated.mkv"));
    QCOMPARE(torrentFileFresh->getAttribute(SIZE_), QVariant(5570));
    QCOMPARE(torrentFileFresh->getAttribute(Progress), QVariant(860));

    // Revert
    torrentFile->setAttribute("filepath", "test3_file1.mkv")
            .setAttribute(SIZE_, 5568)
            .setAttribute(Progress, 870);
    torrentFile->save();
}

void tst_Model::save_Update_WithNullValue() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    if (DB::driverName(connection) != QSQLITE)
        QCOMPARE(peerVerify->getAttribute("total_seeds"), NullVariant::Int());

    // Revert
    peer->setAttribute("total_seeds", 4);
    peer->save();
}

void tst_Model::save_Update_Failed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto peer = TorrentPeer::find(3);
    QVERIFY(peer);
    QVERIFY(peer->exists);

    peer->setAttribute("total_seeds-NON_EXISTENT", 15);

    TVERIFY_THROWS_EXCEPTION(QueryError, peer->save());

    QVERIFY(peer->exists);
}

void tst_Model::remove() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFile::find(8);

    QVERIFY(torrentFile);
    QCOMPARE(torrentFile->getKey(), QVariant(8));
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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFiles =
            TorrentPreviewableFile::where({{ID, 7, "="},
                                           {ID, 8, "=", "or"}})
            ->orderBy(ID)
            .get();
    auto &torrentFile7 = torrentFiles[0];
    auto &torrentFile8 = torrentFiles[1];

    QCOMPARE(torrentFiles.size(), 2);
    QCOMPARE(torrentFile7.getKey(), QVariant(7));
    QCOMPARE(torrentFile8.getKey(), QVariant(8));
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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::all();

    QCOMPARE(torrents.size(), 7);

    const std::unordered_map<quint64, QString> expectedIdNames {
        {1, "test1"}, {2, "test2"}, {3, "test3"}, {4, "test4"},
        {5, "test5"}, {6, "test6"}, {7, "test7"},
    };
    for (const auto &torrent : std::as_const(torrents)) {
        const auto torrentId = torrent[ID].value<quint64>();

        QVERIFY(expectedIdNames.contains(torrentId));
        QCOMPARE(expectedIdNames.at(torrentId), torrent[NAME].value<QString>());
    }
}

void tst_Model::all_Columns() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    {
        auto torrents = Torrent::all();

        QCOMPARE(torrents.at(1).getAttributes().size(), 10);
    }
    {
        auto torrents = Torrent::all({ID, NAME});

        const auto &torrent = torrents.at(1);
        QCOMPARE(torrent.getAttributes().size(), 2);
        QCOMPARE(torrent.getAttributes().at(0).key, QString(ID));
        QCOMPARE(torrent.getAttributes().at(1).key, QString(NAME));
    }
}

void tst_Model::latest() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::latest()->get();
    const auto createdAtColumn = Torrent::getCreatedAtColumn();

    auto itTorrent = torrents.constBegin();
    while (itTorrent != torrents.constEnd()) {
        auto firstDate = itTorrent->getAttribute<QDateTime>(createdAtColumn);
        ++itTorrent;
        if (itTorrent != torrents.constEnd()) {
            auto secondDate = itTorrent->getAttribute<QDateTime>(createdAtColumn);
            QVERIFY(firstDate > secondDate);
        }
    }
}

void tst_Model::oldest() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::oldest()->get();
    const auto createdAtColumn = Torrent::getCreatedAtColumn();

    auto itTorrent = torrents.constBegin();
    while (itTorrent != torrents.constEnd()) {
        auto firstDate = itTorrent->getAttribute<QDateTime>(createdAtColumn);
        ++itTorrent;
        if (itTorrent != torrents.constEnd()) {
            auto secondDate = itTorrent->getAttribute<QDateTime>(createdAtColumn);
            QVERIFY(firstDate < secondDate);
        }
    }
}

void tst_Model::where() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::where(ID, "=", 3)->first();
        QVERIFY(torrent);
        QCOMPARE(torrent->getKey(), QVariant(3));
    }
    {
        auto torrents = Torrent::where(ID, ">=", 3)->get();

        QCOMPARE(torrents.size(), 5);

        const QList<QVariant> expectedIds {3, 4, 5, 6, 7};
        for (const auto &torrent : std::as_const(torrents))
            QVERIFY(expectedIds.contains(torrent.getKey()));
    }
}

void tst_Model::whereEq() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // number
    {
        auto torrent = Torrent::whereEq(ID, 3)->first();
        QVERIFY(torrent);
        QCOMPARE(torrent->getKey(), QVariant(3));
    }
    // string
    {
        auto torrent = Torrent::whereEq(NAME, "test3")->first();
        QVERIFY(torrent);
        QCOMPARE(torrent->getKey(), QVariant(3));
    }
    // QDateTime
    {
        auto torrent = Torrent::whereEq("added_on", QDateTime({2020, 8, 1}, {20, 11, 10},
                                                              TTimeZone::UTC))
                       ->first();
        QVERIFY(torrent);
        QCOMPARE(torrent->getKey(), QVariant(1));
    }
}

void tst_Model::where_WithVector() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::where({{ID, 3}})->first();
        QVERIFY(torrent);
        QCOMPARE(torrent->getKey(), QVariant(3));
    }
    {
        auto torrents = Torrent::where({{ID, 3, ">="}})->get();
        QCOMPARE(torrents.size(), 5);

        const std::unordered_map<quint64, QString> expectedIdNames {
            {3, "test3"}, {4, "test4"}, {5, "test5"}, {6, "test6"}, {7, "test7"},
        };
        for (const auto &torrent : std::as_const(torrents)) {
            const auto torrentId = torrent[ID].value<quint64>();

            QVERIFY(expectedIdNames.contains(torrentId));
            QCOMPARE(expectedIdNames.at(torrentId), torrent[NAME].value<QString>());
        }
    }
}

void tst_Model::where_WithVector_Condition() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    {
        auto torrents = Torrent::where({{SIZE_, 14}, {Progress, 400}})->get();
        QCOMPARE(torrents.size(), 1);
        QCOMPARE(torrents.at(0).getKey(), QVariant(4));
    }
    {
        auto torrents = Torrent::where({{SIZE_, 13}, {SIZE_, 14, "=", "or"}})->get();
        QCOMPARE(torrents.size(), 2);

        const QList<QVariant> expectedIds {3, 4};
        for (const auto &torrent : std::as_const(torrents))
            QVERIFY(expectedIds.contains(torrent[ID]));
    }
    {
        auto torrents = Torrent::where({{SIZE_, 13}, {SIZE_, 14, "=", "or"}})
                        ->where(Progress, "=", 400)
                        .get();
        QCOMPARE(torrents.size(), 1);
        QCOMPARE(torrents.at(0).getKey(), QVariant(4));
    }
}

void tst_Model::whereExists() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // With lambda expression
    {
        const auto actualIds = TorrentPeer::whereExists([](Orm::QueryBuilder &query)
        {
            query.from("torrents").where(SIZE_, LT, 15);
        })
            ->where(ID, LT, 7)
            .select(ID)
            .orderBy(ID)
            .pluck(ID);

        QList<QVariant> expectedIds {1, 2, 3, 4, 5, 6};

        QCOMPARE(actualIds, expectedIds);
    }
    // With QueryBuilder &
    {
        auto builder = DB::connection(connection).query();

        const auto actualIds = TorrentPeer::whereExists(builder->from("torrents")
                                                                .where(SIZE_, LT, 15))
                               ->where(ID, LT, 7)
                               .select(ID)
                               .orderBy(ID)
                               .pluck(ID);

        QList<QVariant> expectedIds {1, 2, 3, 4, 5, 6};

        QCOMPARE(actualIds, expectedIds);
    }
    // With std::shared_ptr<QueryBuilder>
    {
        auto builder = DB::connection(connection).query();

        // Ownership of the std::shared_ptr<QueryBuilder>
        const auto builderForExists = DB::connection(connection).query();
        builderForExists->from("torrents").where(SIZE_, LT, 15);

        const auto actualIds = TorrentPeer::whereExists(builderForExists)
                               ->where(ID, LT, 7)
                               .select(ID)
                               .orderBy(ID)
                               .pluck(ID);

        QList<QVariant> expectedIds {1, 2, 3, 4, 5, 6};

        QCOMPARE(actualIds, expectedIds);
    }
}

void tst_Model::find() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(3);
    QVERIFY(torrent);
    QCOMPARE(torrent->getKey(), QVariant(3));
    QCOMPARE(torrent->getAttribute(NAME), QVariant("test3"));
}

void tst_Model::findOrNew_Found() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::findOrNew(3);

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 10);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent[NAME], QVariant("test3"));
    }
    {
        auto torrent = Torrent::findOrNew(3, {ID, NAME});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 2);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent[NAME], QVariant("test3"));
    }
}

void tst_Model::findOrNew_NotFound() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::findOrNew(999999);

        QVERIFY(!torrent.exists);
        QVERIFY(torrent.getAttributes().isEmpty());
        QCOMPARE(torrent[ID], QVariant());
        QCOMPARE(torrent[NAME], QVariant());
    }
    {
        auto torrent = Torrent::findOrNew(999999, {ID, NAME});

        QVERIFY(!torrent.exists);
        QVERIFY(torrent.getAttributes().isEmpty());
        QCOMPARE(torrent[ID], QVariant());
        QCOMPARE(torrent[NAME], QVariant());
    }
}

void tst_Model::findOrFail_Found() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::findOrFail(3);

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 10);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent[NAME], QVariant("test3"));
    }
    {
        auto torrent = Torrent::findOrFail(3, {ID, NAME});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 2);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent[NAME], QVariant("test3"));
    }
}

void tst_Model::findOrFail_NotFoundFailed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    TVERIFY_THROWS_EXCEPTION(ModelNotFoundError, Torrent::findOrFail(999999));
    TVERIFY_THROWS_EXCEPTION(ModelNotFoundError, Torrent::findOrFail(999999, {ID, NAME}));
}

void tst_Model::findMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::findMany({2, 3, 5});
    QCOMPARE(torrents.size(), 3);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent>));

    const std::unordered_set<quint64> expectedIds {2, 3, 5};

    for (const auto &torrent : std::as_const(torrents))
        QVERIFY(expectedIds.contains(torrent[ID].template value<quint64>()));
}

void tst_Model::findMany_Empty() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::findMany({});

    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent>));
    QVERIFY(torrents.isEmpty());
}

void tst_Model::findOr() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Callback invoked
    {
        auto callbackInvoked = false;
        auto torrent = Torrent::findOr(100, {ASTERISK}, [&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(!torrent);
        QVERIFY(callbackInvoked);
    }
    // Callback invoked (second overload)
    {
        auto callbackInvoked = false;
        auto torrent = Torrent::findOr(100, [&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(!torrent);
        QVERIFY(callbackInvoked);
    }
    // Callback not invoked
    {
        auto callbackInvoked = false;
        auto torrent = Torrent::findOr(1, [&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(torrent);
        QVERIFY(!callbackInvoked);
        QCOMPARE(torrent->getKey(), QVariant(1));
        QCOMPARE(torrent->getAttribute(NAME), QVariant("test1"));
    }
}

void tst_Model::findOr_WithReturnType() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Callback invoked
    {
        auto [torrent, result] = Torrent::findOr<int>(100, []()
        {
            return 1;
        });

        QVERIFY(!torrent);
        QCOMPARE(result, 1);
    }
    // Callback invoked (second overload)
    {
        auto [torrent, result] = Torrent::findOr<int>(100, {ID, NAME}, []()
        {
            return 1;
        });

        QVERIFY(!torrent);
        QCOMPARE(result, 1);
    }
    // Callback not invoked
    {
        auto [torrent, result] = Torrent::findOr<int>(1, []()
        {
            return 1;
        });

        QVERIFY(torrent);
        QCOMPARE(result, 0);
        QCOMPARE(torrent->getKey(), QVariant(1));
        QCOMPARE(torrent->getAttribute(NAME), QVariant("test1"));
    }
}

void tst_Model::first() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::whereKey(3)->first();
    QVERIFY(torrent);
    QCOMPARE(torrent->getKey(), QVariant(3));
    QCOMPARE(torrent->getAttribute(NAME), QVariant("test3"));
}

void tst_Model::firstOr() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Callback invoked
    {
        auto callbackInvoked = false;
        auto torrent = Torrent::whereKey(100)->firstOr({ASTERISK}, [&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(!torrent);
        QVERIFY(callbackInvoked);
    }
    // Callback invoked (second overload)
    {
        auto callbackInvoked = false;
        auto torrent = Torrent::whereKey(100)->firstOr([&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(!torrent);
        QVERIFY(callbackInvoked);
    }
    // Callback not invoked
    {
        auto callbackInvoked = false;
        auto torrent = Torrent::whereKey(1)->firstOr([&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(torrent);
        QVERIFY(!callbackInvoked);
        QCOMPARE(torrent->getKey(), QVariant(1));
        QCOMPARE(torrent->getAttribute(NAME), QVariant("test1"));
    }
}

void tst_Model::firstOr_WithReturnType() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Callback invoked
    {
        auto [torrent, result] = Torrent::whereKey(100)->firstOr<int>([]()
        {
            return 1;
        });

        QVERIFY(!torrent);
        QCOMPARE(result, 1);
    }
    // Callback invoked (second overload)
    {
        auto [torrent, result] = Torrent::whereKey(100)->firstOr<int>({ID, NAME}, []()
        {
            return 1;
        });

        QVERIFY(!torrent);
        QCOMPARE(result, 1);
    }
    // Callback not invoked
    {
        auto [torrent, result] = Torrent::whereKey(1)->firstOr<int>([]()
        {
            return 1;
        });

        QVERIFY(torrent);
        QCOMPARE(result, 0);
        QCOMPARE(torrent->getKey(), QVariant(1));
        QCOMPARE(torrent->getAttribute(NAME), QVariant("test1"));
    }
}

void tst_Model::firstWhere() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFile3 = TorrentPreviewableFile::firstWhereEq(ID, 3);

    QVERIFY(torrentFile3->exists);
    QCOMPARE((*torrentFile3)[ID], QVariant(3));
    QCOMPARE((*torrentFile3)["filepath"], QVariant("test2_file2.mkv"));
}

void tst_Model::firstOrNew_Found() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::firstOrNew({{NAME, "test3"}});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 10);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent[NAME], QVariant("test3"));
        QCOMPARE(torrent[SIZE_], QVariant(13));
        QCOMPARE(torrent[Progress], QVariant(300));
    }
    {
        auto torrent = Torrent::firstOrNew(
                           {{NAME, "test3"}},

                           {{SIZE_, 113},
                            {Progress, 313}});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 10);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent[NAME], QVariant("test3"));
        QCOMPARE(torrent[SIZE_], QVariant(13));
        QCOMPARE(torrent[Progress], QVariant(300));
    }
}

void tst_Model::firstOrNew_NotFound() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::firstOrNew({{NAME, "test100"}});

        QVERIFY(!torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 1);
        QCOMPARE(torrent[NAME], QVariant("test100"));
    }
    {
        auto torrent = Torrent::firstOrNew(
                           {{NAME, "test100"}},

                           {{SIZE_, 113},
                            {Progress, 313}});

        QVERIFY(!torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 3);
        QCOMPARE(torrent[NAME], QVariant("test100"));
        QCOMPARE(torrent[SIZE_], QVariant(113));
        QCOMPARE(torrent[Progress], QVariant(313));
    }
}

void tst_Model::firstOrCreate_Found() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::firstOrCreate({{NAME, "test3"}});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 10);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent[NAME], QVariant("test3"));
        QCOMPARE(torrent[SIZE_], QVariant(13));
        QCOMPARE(torrent[Progress], QVariant(300));
    }
    {
        const auto addedOn = QDateTime::currentDateTimeUtc();

        auto torrent = Torrent::firstOrCreate(
                           {{NAME, "test3"}},

                           {{SIZE_, 33},
                            {Progress, 33},
                            {"added_on", addedOn},
                            {HASH_, "0009e3af2768cdf52ec84c1f320333f68401dc60"}});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 10);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent[NAME], QVariant("test3"));
        QCOMPARE(torrent[SIZE_], QVariant(13));
        QCOMPARE(torrent[Progress], QVariant(300));
        QCOMPARE(torrent["added_on"],
                QVariant(QDateTime({2020, 8, 3}, {20, 11, 10}, TTimeZone::UTC)));
        QCOMPARE(torrent[HASH_], QVariant("3579e3af2768cdf52ec84c1f320333f68401dc6e"));
    }
}

void tst_Model::firstOrCreate_NotFound() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    const auto addedOn = QDateTime({2020, 10, 1}, {20, 22, 10}, TTimeZone::UTC);

    auto torrent = Torrent::firstOrCreate(
                       {{NAME, "test100"}},

                       {{SIZE_, 113},
                        {Progress, 313},
                        {"added_on", addedOn},
                        {HASH_, "1999e3af2768cdf52ec84c1f320333f68401dc6e"}});

    QVERIFY(torrent.exists);
    QCOMPARE(torrent.getAttributes().size(), 8);
    QVERIFY(torrent[ID]->value<quint64>() > 6);
    QCOMPARE(torrent[NAME], QVariant("test100"));
    QCOMPARE(torrent[SIZE_], QVariant(113));
    QCOMPARE(torrent[Progress], QVariant(313));
    QCOMPARE(torrent["added_on"], QVariant(addedOn));
    QCOMPARE(torrent[HASH_], QVariant("1999e3af2768cdf52ec84c1f320333f68401dc6e"));

    const auto result = torrent.remove();
    QVERIFY(result);
    QVERIFY(!torrent.exists);
}

void tst_Model::wasChanged() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(3);

    QVERIFY(!torrent->wasChanged());
    QVERIFY(!torrent->wasChanged(NAME));

    torrent->setAttribute(NAME, "test3 changed");

    QVERIFY(!torrent->wasChanged());
    QVERIFY(!torrent->wasChanged(NAME));

    torrent->save();

    QVERIFY(torrent->wasChanged());
    QVERIFY(torrent->wasChanged(NAME));
    QVERIFY(!torrent->wasChanged(SIZE_));

    // Restore the name
    torrent->setAttribute(NAME, "test3");
    torrent->save();
}

void tst_Model::fresh_OnlyAttributes() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
        QCOMPARE(torrent->getKey(), QVariant(3));

        torrent->setAttribute(NAME, "test3 fresh");
        QCOMPARE(torrent->getAttribute(NAME), QVariant("test3 fresh"));

        auto freshTorrent = torrent->fresh();
        QVERIFY(freshTorrent);
        QVERIFY(&*torrent != &*freshTorrent);
        QVERIFY(freshTorrent->exists);
        QCOMPARE(freshTorrent->getKey(), QVariant(3));
        QCOMPARE(freshTorrent->getAttribute(NAME), QVariant("test3"));
    }
}

void tst_Model::refresh_OnlyAttributes() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
        QCOMPARE(torrent->getKey(), QVariant(3));

        auto original = torrent->getAttribute(NAME);
        QCOMPARE(original, QVariant("test3"));

        torrent->setAttribute(NAME, "test3 refresh");
        QCOMPARE(torrent->getAttribute(NAME), QVariant("test3 refresh"));

        auto &refreshedTorrent = torrent->refresh();

        QVERIFY(&*torrent == &refreshedTorrent);
        QVERIFY(refreshedTorrent.exists);
        QCOMPARE(refreshedTorrent.getKey(), torrent->getKey());
        QCOMPARE(refreshedTorrent.getAttribute(NAME), original);
    }
}

void tst_Model::create() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    const auto addedOn = QDateTime({2021, 2, 1}, {20, 22, 10}, TTimeZone::UTC);

    auto torrent = Torrent::create({
        {NAME,       "test100"},
        {SIZE_,      100},
        {Progress,   333},
        {"added_on", addedOn},
        {HASH_,      "1009e3af2768cdf52ec84c1f320333f68401dc6e"},
    });

    QVERIFY(torrent.exists);
    QVERIFY(torrent[ID]->isValid());
    QVERIFY(torrent[ID]->value<quint64>() > 6);
    QCOMPARE(torrent[NAME], QVariant("test100"));
    QCOMPARE(torrent[SIZE_], QVariant(100));
    QCOMPARE(torrent[Progress], QVariant(333));
    QCOMPARE(torrent["added_on"], QVariant(addedOn));
    QCOMPARE(torrent[HASH_], QVariant("1009e3af2768cdf52ec84c1f320333f68401dc6e"));

    QVERIFY(!torrent.isDirty());
    QVERIFY(!torrent.wasChanged());

    torrent.setAttribute(NAME, "test100 create");
    torrent.save();

    QVERIFY(torrent.exists);
    QCOMPARE(torrent[NAME], QVariant("test100 create"));

    QVERIFY(torrent.wasChanged());

    torrent.remove();

    QVERIFY(!torrent.exists);
}

void tst_Model::create_Failed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    const auto addedOn = QDateTime({2021, 2, 1}, {20, 22, 10}, TTimeZone::UTC);

    Torrent torrent;
    TVERIFY_THROWS_EXCEPTION(QueryError,
    (torrent = Torrent::create({
        {"name-NON_EXISTENT", "test100"},
        {SIZE_,               100},
        {Progress,            333},
        {"added_on",          addedOn},
        {HASH_,               "1009e3af2768cdf52ec84c1f320333f68401dc6e"},
    })));

    QVERIFY(!torrent.exists);
    QVERIFY(torrent.getAttributes().isEmpty());
}

void tst_Model::incrementAndDecrement() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto timeBeforeIncrement = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeIncrement.time();
        timeBeforeIncrement.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    auto torrent4_1 = Torrent::find(4);
    QVERIFY(torrent4_1);
    QVERIFY(torrent4_1->exists);

    const auto &updatedAtColumn = torrent4_1->getUpdatedAtColumn();

    auto sizeOriginal = torrent4_1->getAttribute(SIZE_);
    auto progressOriginal = torrent4_1->getAttribute(Progress);
    auto updatedAtOriginal = torrent4_1->getAttribute(updatedAtColumn);
    QCOMPARE(sizeOriginal, QVariant(14));
    QCOMPARE(progressOriginal, QVariant(400));
    QCOMPARE(updatedAtOriginal,
             QVariant(QDateTime({2021, 1, 4}, {18, 46, 31}, TTimeZone::UTC)));

    // Increment
    torrent4_1->increment(SIZE_, 2, {{Progress, 444}});

    // Little unnecessary but the logic is true
    QVERIFY(!torrent4_1->isDirty());
    QVERIFY(!torrent4_1->isDirty(SIZE_));
    QVERIFY(!torrent4_1->isDirty(Progress));
    QVERIFY(torrent4_1->wasChanged());
    QVERIFY(torrent4_1->wasChanged(SIZE_));
    QVERIFY(torrent4_1->wasChanged(Progress));

    auto torrent4_2 = Torrent::find(4);
    QVERIFY(torrent4_2);
    QVERIFY(torrent4_2->exists);
    QCOMPARE(torrent4_2->getAttribute(SIZE_), QVariant(16));
    QCOMPARE(torrent4_2->getAttribute(Progress), QVariant(444));
    QVERIFY(torrent4_2->getAttribute<QDateTime>(updatedAtColumn) >= timeBeforeIncrement);

    // Decremented and restore updated at column
    torrent4_2->decrement(SIZE_, 2, {{Progress, 400},
                                    {updatedAtColumn, updatedAtOriginal}});

    // Little unnecessary but the logic is true
    QVERIFY(!torrent4_2->isDirty());
    QVERIFY(!torrent4_2->isDirty(SIZE_));
    QVERIFY(!torrent4_2->isDirty(Progress));
    QVERIFY(!torrent4_2->isDirty(updatedAtColumn));
    QVERIFY(torrent4_2->wasChanged());
    QVERIFY(torrent4_2->wasChanged(SIZE_));
    QVERIFY(torrent4_2->wasChanged(Progress));
    QVERIFY(torrent4_2->wasChanged(updatedAtColumn));

    auto torrent4_3 = Torrent::find(4);
    QVERIFY(torrent4_3);
    QVERIFY(torrent4_3->exists);
    QCOMPARE(torrent4_3->getAttribute(SIZE_), QVariant(14));
    QCOMPARE(torrent4_3->getAttribute(Progress), QVariant(400));
    QCOMPARE(torrent4_3->getAttribute(updatedAtColumn), updatedAtOriginal);
}

void tst_Model::incrementAndDecrement_OnNonExistentModel_WithoutAll() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent;

    QVERIFY(!torrent.exists);

    auto [affected, query] = torrent.increment(SIZE_, 1, {{Progress, 101}}, false);

    // Must return -1 if updating all rows and the 'all' param. is in default (false)
    QVERIFY(affected == -1);
    QVERIFY(!query.isActive());
    QVERIFY(!query.isValid());
    QVERIFY(!torrent.isDirty());
    QVERIFY(!torrent.wasChanged());
}

void tst_Model::update() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto timeBeforeUpdate = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeUpdate.time();
        timeBeforeUpdate.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    auto torrent = Torrent::find(4);

    const auto &updatedAtColumn = torrent->getUpdatedAtColumn();

    auto progressOriginal = torrent->getAttribute(Progress);
    auto updatedAtOriginal = torrent->getAttribute(updatedAtColumn);

    QVERIFY(torrent->exists);
    QCOMPARE(progressOriginal, QVariant(400));
    QCOMPARE(updatedAtOriginal,
             QVariant(QDateTime({2021, 1, 4}, {18, 46, 31}, TTimeZone::UTC)));

    auto result = torrent->update({{Progress, 449}});

    QVERIFY(result);
    QCOMPARE(torrent->getAttribute(Progress), QVariant(449));
    QVERIFY(!torrent->isDirty());
    QVERIFY(torrent->wasChanged());

    // Verify value in the database
    auto torrentVerify = Torrent::find(4);
    QVERIFY(torrentVerify->exists);
    QCOMPARE(torrentVerify->getAttribute(Progress), QVariant(449));
    QVERIFY(torrentVerify->getAttribute<QDateTime>(updatedAtColumn) >=
            timeBeforeUpdate);

    // Revert value back
    auto resultRevert = torrent->update({{Progress, progressOriginal},
                                         {updatedAtColumn, updatedAtOriginal}});
    QVERIFY(resultRevert);
    QCOMPARE(torrent->getAttribute(Progress), progressOriginal);
    /* Needed to convert toDateTime() because Model::update() set update_at
       attribute as QString. */
    QCOMPARE(torrent->getAttribute<QDateTime>(updatedAtColumn),
             updatedAtOriginal.value<QDateTime>());
}

void tst_Model::update_OnNonExistentModel() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent;

    QVERIFY(!torrent.exists);

    QVERIFY(!torrent.update({{Progress, 333}}));
}

void tst_Model::update_NonExistentAttribute() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(3);

    QVERIFY(torrent->exists);

    /* Zero attributes will be changed because torrent.isDirty() == false and
       Torrent::save() returns true in this case, it's like calling the update() with
       no or empty attributes. */
    QVERIFY(torrent->update({{"progress-NON_EXISTENT", 333}}));
}

void tst_Model::update_SameValue() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(3);
    QVERIFY(torrent->exists);

    const auto &updatedAtColumn = torrent->getUpdatedAtColumn();
    auto updatedAt = torrent->getAttribute(updatedAtColumn);

    /* Doesn't send update query to the database, this is different from
       the TinyBuilder::update() method. */
    auto result = torrent->update({{Progress, 300}});

    QVERIFY(result);
    QVERIFY(!torrent->isDirty());
    QVERIFY(!torrent->wasChanged());

    // Verify value in the database
    auto torrentVerify = Torrent::find(3);
    QVERIFY(torrentVerify->exists);
    QCOMPARE(torrentVerify->getAttribute(updatedAtColumn), updatedAt);
}

void tst_Model::upsert() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto timeBeforeUpdate = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeUpdate.time();
        timeBeforeUpdate.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    // Get an original timestamp values for compare and restoration
    const auto &createdAtColumn = TagProperty::getCreatedAtColumn();
    const auto &updatedAtColumn = TagProperty::getUpdatedAtColumn();
    auto tagProperty1Original = TagProperty::whereEq("tag_id", 1)
                                ->orderBy("position")
                                .first();
    auto createdAtOriginal = tagProperty1Original->getAttribute(createdAtColumn);
    auto updatedAtOriginal = tagProperty1Original->getAttribute(updatedAtColumn);

    // Should update one row (color column) and insert one row
    {
        auto [affected, query] =
                TagProperty::upsert(
                    {{{"tag_id", 1}, {"color", "pink"},   {"position", 0}},
                     {{"tag_id", 1}, {"color", "purple"}, {"position", 4}}},
                    {"position"},
                    {"color"});

        QVERIFY(query);
        QVERIFY(!query->isValid() && !query->isSelect() && query->isActive());
        if (DB::driverName(connection) == QMYSQL)
            /* For MySQL the affected-rows value per row is 1 if the row is inserted
               as a new row, 2 if an existing row is updated, and 0 if an existing row
               is set to its current values. */
            QCOMPARE(affected, 3);
        else
            QCOMPARE(affected, 2);

        // Validate one update and one insert
        auto tagProperties = TagProperty::whereEq("tag_id", 1)
                             ->orderBy("position")
                             .get();

        auto tagPropertiesSize = tagProperties.size();
        QCOMPARE(tagPropertiesSize, 2);

        QList<QList<QVariant>> result;
        result.reserve(tagPropertiesSize);

        for (const auto &tagProperty : std::as_const(tagProperties))
            result.append({tagProperty.getAttribute("color"),
                           tagProperty.getAttribute("position"),
                           tagProperty.getAttribute("tag_id")});

        QList<QList<QVariant>> expextedResult {
            {"pink",   0, 1},
            {"purple", 4, 1},
        };

        QCOMPARE(result, expextedResult);

        // Timestamps must be compared manually
        auto tagProperty1 = tagProperties.at(0);
        QCOMPARE(tagProperty1.getAttribute<QDateTime>(createdAtColumn),
                 createdAtOriginal);
        QVERIFY(tagProperty1.getAttribute<QDateTime>(updatedAtColumn) >=
                timeBeforeUpdate);
        auto tagProperty2 = tagProperties.at(1);
        QVERIFY(tagProperty2.getAttribute<QDateTime>(createdAtColumn) >=
                timeBeforeUpdate);
        QVERIFY(tagProperty1.getAttribute<QDateTime>(updatedAtColumn) >=
                timeBeforeUpdate);
    }

    // Restore db
    {
        auto [affected, query] = DB::table("tag_properties", connection)
                                 ->whereEq("position", 4)
                                 .remove();

        QVERIFY(!query.isValid() && !query.isSelect() && query.isActive());
        QCOMPARE(affected, 1);
    }
    {
        auto [affected, query] = DB::table("tag_properties", connection)
                                 ->whereEq("id", 1)
                                 .update({{"color", "white"},
                                          {createdAtColumn, createdAtOriginal},
                                          {updatedAtColumn, updatedAtOriginal}});

        QVERIFY(!query.isValid() && !query.isSelect() && query.isActive());
        QCOMPARE(affected, 1);
    }

    // Validate restored db
    {
        auto validateQuery = DB::table("tag_properties", connection)
                             ->whereEq("tag_id", 1)
                             .orderBy("position")
                             .get();

        QVERIFY(validateQuery.isSelect() && validateQuery.isActive());
        QCOMPARE(QueryUtils::queryResultSize(validateQuery), 1);
        QVERIFY(validateQuery.first());
        QVERIFY(validateQuery.isValid());
        QCOMPARE(validateQuery.value("color"), QVariant(QString("white")));
        QCOMPARE(validateQuery.value("position").value<int>(), 0);
        QCOMPARE(validateQuery.value(createdAtColumn).value<QDateTime>(),
                 createdAtOriginal);
        QCOMPARE(validateQuery.value(updatedAtColumn).value<QDateTime>(),
                 updatedAtOriginal);
    }
}

void tst_Model::truncate() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Setting setting;
    setting.setAttribute(NAME, "truncate");
    setting.setAttribute("value", "yes");

    QVERIFY(!setting.exists);
    const auto result = setting.save();
    QVERIFY(result);
    QVERIFY(setting.exists);

    // Get the fresh model from the database
    auto settingToVerify = Setting::whereEq(NAME, "truncate")->first();
    QVERIFY(settingToVerify);
    QVERIFY(settingToVerify->exists);

    // And check attributes
    QCOMPARE(settingToVerify->getAttribute(NAME), QVariant("truncate"));
    QCOMPARE(settingToVerify->getAttribute("value"), QVariant("yes"));

    Setting::truncate();

    QCOMPARE(Setting::all().size(), 0);
}

void tst_Model::massAssignment_isGuardableColumn() const
{
    /* This test has to be here because it internally calls columns listing against
       database, so it is connection-dependent. */
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent_GuardableColumn torrent;

    torrent.fill({{UPDATED_AT, QDateTime::currentDateTimeUtc()}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent.getAttributes().size(), 1);
}

/* HasTimestamps */

void tst_Model::touch_WithAttribute() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Verify an original added_on value
    auto addedOnOriginal = Torrent::find(1)->getAttribute("added_on");
    QCOMPARE(addedOnOriginal,
             QVariant(QDateTime({2020, 8, 1}, {20, 11, 10}, TTimeZone::UTC)));

    // Save a time before touch
    auto timeBeforeTouch = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeTouch.time();
        timeBeforeTouch.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    // Make touch
    auto result = Torrent::find(1)->touch("added_on");
    QVERIFY(result);

    // Verify a new touched added_on value
    auto addedOnTouched = Torrent::find(1)->getAttribute("added_on");
    QVERIFY(addedOnTouched.value<QDateTime>() >= timeBeforeTouch);

    // Restore
    result = Torrent::find(1)->update({{"added_on", addedOnOriginal}});
    QVERIFY(result);

    // Verify restored added_on value
    auto addedOnRestored = Torrent::find(1)->getAttribute("added_on");
    QCOMPARE(addedOnRestored, addedOnOriginal);
}

void
tst_Model::addTouch_setTouchedRelations_getTouchedRelations_touches_clearTouches() const
{
    Torrent torrent;

    const auto &touches = torrent.getTouchedRelations();

    QVERIFY(touches.empty());

    // touches()
    QVERIFY(!torrent.touches("attribute_1"));
    QVERIFY(!torrent.touches("attribute_2"));
    QVERIFY(!torrent.touches("attribute_3"));

    // addTouch(QString)
    torrent.addTouch("attribute_1");
    QCOMPARE(touches, (QStringList {"attribute_1"}));

    // touches()
    QVERIFY(torrent.touches("attribute_1"));

    // addTouches(std::set<QString>)
    torrent.addTouches({"attribute_2", "attribute_3"});
    QCOMPARE(touches, (QStringList {"attribute_1", "attribute_2", "attribute_3"}));

    // touches() - this is a little useless but whatever 🙃
    QVERIFY(torrent.touches("attribute_2"));
    QVERIFY(torrent.touches("attribute_3"));

    // setTouchedRelations()
    torrent.setTouchedRelations({"attribute_4", "attribute_5"});
    QCOMPARE(touches, (QStringList {"attribute_4", "attribute_5"}));

    // clearTouches()
    torrent.clearTouches();
    QVERIFY(touches.empty());

    // getTouchedRelations()
    QCOMPARE(torrent.getTouchedRelations(), touches);
    // In the end, the memory addresses must be the same
    QVERIFY(std::addressof(torrent.getTouchedRelations()) == std::addressof(touches));
}

/* Attributes - Unix timestamps - u_dateFormat = 'U' */

namespace
{
    class Role_WithoutUDates final : public Model<Role_WithoutUDates> // NOLINT(bugprone-exception-escape, misc-no-recursion)
    {
        friend Model;
        using Model::Model;

        /*! The table associated with the model. */
        QString u_table {"roles"};

        /*! Indicates whether the model should be timestamped. */
        bool u_timestamps = false;
    };
} // namespace

void tst_Model::getAttribute_UnixTimestamp_With_UDates() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto role = Role::find(1);
    QVERIFY(role);
    QVERIFY(role->exists);

    auto addedOn = role->getAttribute("added_on");
    QVERIFY(addedOn.isValid() && !addedOn.isNull());

    // Also the SQLite returns the QDateTime
    QCOMPARE(addedOn.typeId(), QMetaType::QDateTime);
    // This is most important, should return QDateTime and not int
    QCOMPARE(addedOn.value<QDateTime>(),
             QDateTime::fromSecsSinceEpoch(1659361016, TTimeZone::UTC));
}

void tst_Model::getAttribute_UnixTimestamp_WithOut_UDates() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto role = Role_WithoutUDates::find(1);
    QVERIFY(role);
    QVERIFY(role->exists);

    auto addedOn = role->getAttribute("added_on");
    QVERIFY(addedOn.isValid() && !addedOn.isNull());

    /* Only MySQL returns correct underlying type, the SQLite doesn't care and PostgreSQL
       returns ULongLong. */
    if (DB::driverName(connection) == QMYSQL)
        QCOMPARE(addedOn.typeId(), QMetaType::LongLong);
    else
        QVERIFY(addedOn.canConvert<qint64>());

    // This is most important, should return int and not QDateTime
    QCOMPARE(addedOn, QVariant(static_cast<qint64>(1659361016)));
}

void tst_Model::getAttribute_UnixTimestamp_With_UDates_Null() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto role = Role_WithoutUDates::find(3);
    QVERIFY(role);
    QVERIFY(role->exists);

    auto addedOn = role->getAttribute("added_on");
    // Compare is null
    QVERIFY(addedOn.isValid() && addedOn.isNull());

    // Compare also the type (excluding SQLite)
    // SQLite doesn't return correct underlying type in the QVariant
    if (DB::driverName(connection) != QSQLITE)
        QCOMPARE(addedOn.typeId(), QMetaType::LongLong);

    /* SQLite doesn't return correct underlying type in the QVariant for null values
       like MySQL driver does, skip this compare for the SQLite database. */
    if (DB::driverName(connection) != QSQLITE)
        QCOMPARE(addedOn, NullVariant::LongLong());
}

void tst_Model::getAttribute_UnixTimestamp_WithOut_UDates_Null() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto role = Role_WithoutUDates::find(3);
    QVERIFY(role);
    QVERIFY(role->exists);

    auto addedOn = role->getAttribute("added_on");
    // Compare is null
    QVERIFY(addedOn.isValid() && addedOn.isNull());

    // Compare also the type (excluding SQLite)
    // SQLite doesn't return correct underlying type in the QVariant
    if (DB::driverName(connection) != QSQLITE)
        QCOMPARE(addedOn.typeId(), QMetaType::LongLong);

    /* SQLite doesn't return correct underlying type in the QVariant for null values
       like MySQL driver does, skip this compare for the SQLite database. */
    if (DB::driverName(connection) != QSQLITE)
        QCOMPARE(addedOn, NullVariant::LongLong());
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_Model)

#include "tst_model.moc"
