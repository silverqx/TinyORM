#include <QCoreApplication>
#include <QTest>

#include "databases.hpp"
#include "macros.hpp"

#include "models/datetime_serializeoverride.hpp"
#include "models/torrent.hpp"

using Orm::Constants::CREATED_AT;
using Orm::Constants::HASH_;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;
using Orm::Constants::UPDATED_AT;

using Orm::TTimeZone;
using Orm::Utils::NullVariant;

using TypeUtils = Orm::Utils::Type;

using Orm::Tiny::AttributeItem;
using Orm::Tiny::ConnectionOverride;

using TestUtils::Databases;

using Models::Datetime_SerializeOverride;
using Models::Torrent;
using Models::TorrentPreviewableFile;
using Models::User;

class tst_Model_Appends : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase() const;

    /* Serialization - Appends */
    void append_setAppend_getAppend_hasAppend_clearAppends() const;

    void toMap_WithAppends() const;
    void toList_WithAppends() const;

    void toMap_WithAppends_WithVisible() const;
    void toList_WithAppends_WithVisible() const;

    void toMap_WithAppends_WithHidden() const;
    void toList_WithAppends_WithHidden() const;

    void toMap_WithAppends_WithVisibleAndHidden() const;
    void toList_WithAppends_WithVisibleAndHidden() const;

    void toMap_WithAppends_ForExistingAttribute() const;
    void toList_WithAppends_ForExistingAttribute() const;
    void toList_WithAppends_WithVisible_ForExistingAttribute() const;
    void toList_WithAppends_WithHidden_ForExistingAttribute() const;

    void toMap_WithAppends_OverrideSerializeDateTime() const;
    void toList_WithAppends_OverrideSerializeDateTime() const;

    void toMap_WithAppends_OnCustomPivot() const;
    void toList_WithAppends_OnCustomPivot() const;

    void toJson_WithAppends_WithVisible() const;
    void toJson_WithAppends_WithHidden() const;
    void toJson_WithAppends_WithVisisbleAndHidden() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Connection name used in this test case. */
    QString m_connection;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Model_Appends::initTestCase()
{
    ConnectionOverride::connection = m_connection =
            Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );
}

void tst_Model_Appends::cleanupTestCase() const
{
    // Reset connection override
    ConnectionOverride::connection.clear();
}

/* Serialization - Appends */

void tst_Model_Appends::append_setAppend_getAppend_hasAppend_clearAppends() const
{
    Torrent torrent;

    const auto &appends = torrent.getAppends();

    QVERIFY(appends.empty());

    // hasAppend()
    QVERIFY(!torrent.hasAppend(NAME));
    QVERIFY(!torrent.hasAppend(NOTE));
    QVERIFY(!torrent.hasAppend(SIZE_));

    // append(QString)
    torrent.append(NAME);
    QCOMPARE(appends, (std::set<QString> {NAME}));

    // hasAppend()
    QVERIFY(torrent.hasAppend(NAME));

    // append(std::set<QString>)
    torrent.append({SIZE_, NOTE});
    QCOMPARE(appends, (std::set<QString> {NAME, NOTE, SIZE_}));

    // hasAppend() - this is a little useless but whatever 🙃
    QVERIFY(torrent.hasAppend(NOTE));
    QVERIFY(torrent.hasAppend(SIZE_));

    // setAppends()
    torrent.setAppends({ID, CREATED_AT});
    QCOMPARE(appends, (std::set<QString> {CREATED_AT, ID}));

    // clearAppends()
    torrent.clearAppends();
    QVERIFY(appends.empty());

    // getAppends()
    QCOMPARE(torrent.getAppends(), appends);
    // In the end, the memory addresses must be the same
    QVERIFY(std::addressof(torrent.getAppends()) == std::addressof(appends));
}

void tst_Model_Appends::toMap_WithAppends() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->append("name_progress");

    QVariantMap serialized = torrent->toMap();
    QCOMPARE(serialized.size(), 11);

    QVariantMap expectedAttributes {
        {"added_on",      "2020-08-04T20:11:10.000Z"},
        {CREATED_AT,      "2019-09-04T08:11:23.000Z"},
        {HASH_,           "4579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {ID,              4},
        {NAME,            "test4"},
        {"name_progress", "test4 (400)"},
        {NOTE,            "after update revert updated_at"},
        {Progress,        400},
        {SIZE_,           14},
        {UPDATED_AT,      "2021-01-04T18:46:31.000Z"},
        {"user_id",       1},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrent->clearAppends();
}

void tst_Model_Appends::toList_WithAppends() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->append("name_progress");

    QList<AttributeItem> serialized = torrent->toList();
    QCOMPARE(serialized.size(), 11);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {ID,              4},
        {"user_id",       1},
        {NAME,            "test4"},
        {SIZE_,           14},
        {Progress,        400},
        {"added_on",      "2020-08-04T20:11:10.000Z"},
        {HASH_,           "4579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {NOTE,            "after update revert updated_at"},
        {CREATED_AT,      "2019-09-04T08:11:23.000Z"},
        {UPDATED_AT,      "2021-01-04T18:46:31.000Z"},
        {"name_progress", "test4 (400)"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrent->clearAppends();
}

void tst_Model_Appends::toMap_WithAppends_WithVisible() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, "user_id", "added_on", "name_size"});
    torrent->append({"name_progress", "name_size"});

    QVariantMap serialized = torrent->toMap();
    QCOMPARE(serialized.size(), 4);

    QVariantMap expectedAttributes {
        {"added_on",  "2020-08-04T20:11:10.000Z"},
        {ID,          4},
        {"name_size", "test4 (14)"},
        {"user_id",   1},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrent->clearVisible();
    torrent->clearAppends();
}

void tst_Model_Appends::toList_WithAppends_WithVisible() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, "user_id", "added_on", "name_progress"});
    torrent->append({"name_progress", "name_size"});

    QList<AttributeItem> serialized = torrent->toList();
    QCOMPARE(serialized.size(), 4);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {ID,              4},
        {"user_id",       1},
        {"added_on",      "2020-08-04T20:11:10.000Z"},
        {"name_progress", "test4 (400)"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrent->clearVisible();
    torrent->clearAppends();
}

void tst_Model_Appends::toMap_WithAppends_WithHidden() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setHidden({HASH_, NOTE, SIZE_, "name_size"});
    torrent->append({"name_progress", "name_size"});

    QVariantMap serialized = torrent->toMap();
    QCOMPARE(serialized.size(), 8);

    QVariantMap expectedAttributes {
        {"added_on",      "2020-08-04T20:11:10.000Z"},
        {CREATED_AT,      "2019-09-04T08:11:23.000Z"},
        {ID,              4},
        {NAME,            "test4"},
        {"name_progress", "test4 (400)"},
        {Progress,        400},
        {UPDATED_AT,      "2021-01-04T18:46:31.000Z"},
        {"user_id",       1},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrent->clearHidden();
    torrent->clearAppends();
}

void tst_Model_Appends::toList_WithAppends_WithHidden() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setHidden({HASH_, NOTE, SIZE_, "name_progress"});
    torrent->append({"name_progress", "name_size"});

    QList<AttributeItem> serialized = torrent->toList();
    QCOMPARE(serialized.size(), 8);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {ID,          4},
        {"user_id",   1},
        {NAME,        "test4"},
        {Progress,    400},
        {"added_on",  "2020-08-04T20:11:10.000Z"},
        {CREATED_AT,  "2019-09-04T08:11:23.000Z"},
        {UPDATED_AT,  "2021-01-04T18:46:31.000Z"},
        {"name_size", "test4 (14)"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrent->clearHidden();
    torrent->clearAppends();
}

void tst_Model_Appends::toMap_WithAppends_WithVisibleAndHidden() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, "user_id", "added_on", "name_progress", "name_size"});
    torrent->setHidden({"added_on", "name_progress"});
    torrent->append({"name_progress", "name_size"});

    QVariantMap serialized = torrent->toMap();
    QCOMPARE(serialized.size(), 3);

    QVariantMap expectedAttributes {
        {ID,          4},
        {"name_size", "test4 (14)"},
        {"user_id",   1},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrent->clearVisible();
    torrent->clearHidden();
    torrent->clearAppends();
}

void tst_Model_Appends::toList_WithAppends_WithVisibleAndHidden() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, "user_id", "added_on", "name_progress", "name_size"});
    torrent->setHidden({"added_on", "name_size"});
    torrent->append({"name_progress", "name_size"});

    QList<AttributeItem> serialized = torrent->toList();
    QCOMPARE(serialized.size(), 3);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {ID,              4},
        {"user_id",       1},
        {"name_progress", "test4 (400)"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrent->clearVisible();
    torrent->clearHidden();
    torrent->clearAppends();
}

void tst_Model_Appends::toMap_WithAppends_ForExistingAttribute() const
{
    auto torrentFile = TorrentPreviewableFile::find(10);
    QVERIFY(torrentFile);
    QVERIFY(torrentFile->exists);

    // Prepare
    torrentFile->setVisible({ID, "torrent_id", "filepath", SIZE_});
    torrentFile->append("filepath");

    QVariantMap serialized = torrentFile->toMap();
    QCOMPARE(serialized.size(), 4);

    QVariantMap expectedAttributes {
        {"filepath",   "test7_file1.mkv (dummy-STRING)"},
        {ID,           10},
        {SIZE_,        4562},
        {"torrent_id", 7},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrentFile->clearVisible();
    torrentFile->clearAppends();
}

void tst_Model_Appends::toList_WithAppends_ForExistingAttribute() const
{
    auto torrentFile = TorrentPreviewableFile::find(10);
    QVERIFY(torrentFile);
    QVERIFY(torrentFile->exists);

    // Prepare
    torrentFile->append("filepath");

    QList<AttributeItem> serialized = torrentFile->toList();
    QCOMPARE(serialized.size(), 9);

    QList<AttributeItem> expectedAttributes {
        {ID,           10},
        {"torrent_id", 7},
        {"file_index", 0},
        {SIZE_,        4562},
        {Progress,     512},
        {NOTE,         "for serialization"},
        {CREATED_AT,   "2021-01-10T14:51:23.000Z"},
        {UPDATED_AT,   "2021-01-10T17:46:31.000Z"},
        {"filepath",   "test7_file1.mkv (dummy-STRING)"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrentFile->clearAppends();
}

void tst_Model_Appends::toList_WithAppends_WithVisible_ForExistingAttribute() const
{
    auto torrentFile = TorrentPreviewableFile::find(10);
    QVERIFY(torrentFile);
    QVERIFY(torrentFile->exists);

    // Prepare
    torrentFile->setVisible({ID, "torrent_id", "filepath", SIZE_});
    torrentFile->append("filepath");

    QList<AttributeItem> serialized = torrentFile->toList();
    QCOMPARE(serialized.size(), 4);

    QList<AttributeItem> expectedAttributes {
        {ID,           10},
        {"torrent_id", 7},
        {SIZE_,        4562},
        {"filepath",   "test7_file1.mkv (dummy-STRING)"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrentFile->clearVisible();
    torrentFile->clearAppends();
}

void tst_Model_Appends::toList_WithAppends_WithHidden_ForExistingAttribute() const
{
    auto torrentFile = TorrentPreviewableFile::find(10);
    QVERIFY(torrentFile);
    QVERIFY(torrentFile->exists);

    // Prepare
    torrentFile->setHidden({"filepath"});
    torrentFile->append("filepath");

    QList<AttributeItem> serialized = torrentFile->toList();
    QCOMPARE(serialized.size(), 8);

    QList<AttributeItem> expectedAttributes {
        {ID,           10},
        {"torrent_id", 7},
        {"file_index", 0},
        {SIZE_,        4562},
        {Progress,     512},
        {NOTE,         "for serialization"},
        {CREATED_AT,   "2021-01-10T14:51:23.000Z"},
        {UPDATED_AT,   "2021-01-10T17:46:31.000Z"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrentFile->clearHidden();
    torrentFile->clearAppends();
}

void tst_Model_Appends::toMap_WithAppends_OverrideSerializeDateTime() const
{
    auto datetime = Datetime_SerializeOverride::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, TTimeZone::UTC)},
        {"time",     sl("14:11:15")},
    });

    // Prepare
    datetime.append({"datetime_test", "date_test", "time_test"});

    QVariantMap serialized = datetime.toMap();
    QCOMPARE(serialized.size(), 6);

    /* No casts and u_date are defined for the date and datetime attributes so
       they will be serialized as the QString instances (in the storage format). */
    QVariantMap expectedAttributes {
        {"date",          "2023-05-14 10:11:12"},
        {"date_test",     "14.06.2023"},
        {"time",          "14:11:15"},
        {"datetime",      "2023-05-13 10:11:12"},
        {"datetime_test", "13.06.2023 10:11:12.0 UTC"},
        {"time_test",     "14-11-20.0"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    datetime.clearAppends();
}

void
tst_Model_Appends::toList_WithAppends_OverrideSerializeDateTime() const
{
    auto datetime = Datetime_SerializeOverride::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, TTimeZone::UTC)},
        {"time",     QTime(14, 11, 15)}, // It must also accept QTime() instances
    });

    // Prepare
    datetime.append({"datetime_test", "date_test", "time_test"});

    QList<AttributeItem> serialized = datetime.toList();
    QCOMPARE(serialized.size(), 6);

    /* No casts and u_date are defined for the date and datetime attributes so
       they will be serialized as the QString instances (in the storage format). */
    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {"datetime",      "2023-05-13 10:11:12"},
        {"date",          "2023-05-14 10:11:12"},
        {"time",          "14:11:15"},
        {"date_test",     "14.06.2023"},
        {"datetime_test", "13.06.2023 10:11:12.0 UTC"},
        {"time_test",     "14-11-20.0"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    datetime.clearAppends();
}

void tst_Model_Appends::toMap_WithAppends_OnCustomPivot() const
{
    auto user = User::with("roles_appends")->find(1);
    QVERIFY(user);
    QVERIFY(user->exists);

    // Prepare
    user->setVisible({ID, NAME, "roles_appends"});

    QVariantMap serialized = user->toMap();

    QVariantMap expectedAttributes {
        {ID,              1},
        {NAME,            "andrej"},
        {"roles_appends", QVariantList {QVariantMap {
                              {"added_on",     "2022-08-01T13:36:56.000Z"},
                              {ID,             1},
                              {NAME,           "role one"},
                              {"subscription", QVariantMap {
                                                   {"active",    true},
                                                   {"is_active", true},
                                                   {"role_id",   1},
                                                   {"user_id",   1},
                                               }},
                          }, QVariantMap {
                              {"added_on",     "2022-08-02T13:36:56.000Z"},
                              {ID,             2},
                              {NAME,           "role two"},
                              {"subscription", QVariantMap {
                                                   {"active",    false},
                                                   {"is_active", false},
                                                   {"role_id",   2},
                                                   {"user_id",   1},
                                               }},
                          }, QVariantMap {
                              {"added_on",     NullVariant::QDateTime()},
                              {ID,             3},
                              {NAME,           "role three"},
                              {"subscription", QVariantMap {
                                                   {"active",    true},
                                                   {"is_active", true},
                                                   {"role_id",   3},
                                                   {"user_id",   1},
                                               }},
                          }}},
    };

    QCOMPARE(serialized, expectedAttributes);

    // Restore
    user->clearVisible();
}

void tst_Model_Appends::toList_WithAppends_OnCustomPivot() const
{
    auto user = User::with("roles_appends")->find(1);
    QVERIFY(user);
    QVERIFY(user->exists);

    // Prepare
    user->setVisible({ID, NAME, "roles_appends"});

    QList<AttributeItem> serialized = user->toList();

    QList<AttributeItem> expectedAttributes {
        {ID,              1},
        {NAME,            "andrej"},
        {"roles_appends", QVariantList {QVariant::fromValue(QList<AttributeItem> {
                              {ID,             1},
                              {NAME,           "role one"},
                              {"added_on",     "2022-08-01T13:36:56.000Z"},
                              {"subscription", QVariant::fromValue(QList<AttributeItem> {
                                                   {"user_id",   1},
                                                   {"role_id",   1},
                                                   {"active",    true},
                                                   {"is_active", true},
                                               })},
                          }), QVariant::fromValue(QList<AttributeItem> {
                              {ID,             2},
                              {NAME,           "role two"},
                              {"added_on",     "2022-08-02T13:36:56.000Z"},
                              {"subscription", QVariant::fromValue(QList<AttributeItem> {
                                                   {"user_id",   1},
                                                   {"role_id",   2},
                                                   {"active",    false},
                                                   {"is_active", false},
                                               })},
                          }), QVariant::fromValue(QList<AttributeItem> {
                              {ID,             3},
                              {NAME,           "role three"},
                              {"added_on",     NullVariant::QDateTime()},
                              {"subscription", QVariant::fromValue(QList<AttributeItem> {
                                                   {"user_id",   1},
                                                   {"role_id",   3},
                                                   {"active",    true},
                                                   {"is_active", true},
                                               })},
                          })}},
    };

    QCOMPARE(serialized, expectedAttributes);

    // Restore
    user->clearVisible();
}

void tst_Model_Appends::toJson_WithAppends_WithVisible() const
{
    auto torrent = Torrent::find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, NAME, NOTE, "name_progress"});
    torrent->append("name_progress");

    QByteArray json = torrent->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"TTT({
    "id": 7,
    "name": "test7",
    "name_progress": "test7 (700)",
    "note": "for serialization"
}
)TTT");

    QCOMPARE(json, expectedJson);

    // Restore
    torrent->clearVisible();
    torrent->clearAppends();
}

void tst_Model_Appends::toJson_WithAppends_WithHidden() const
{
    auto torrent = Torrent::find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setHidden({SIZE_, NAME, NOTE, "name_progress"});
    torrent->append({"name_progress", "name_size"});

    QByteArray json = torrent->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"TTT({
    "added_on": "2020-08-07T20:11:10.000Z",
    "created_at": "2021-11-07T08:13:23.000Z",
    "hash": "7579e3af2768cdf52ec84c1f320333f68401dc6e",
    "id": 7,
    "name_size": "test7 (17)",
    "progress": 700,
    "updated_at": "2021-01-07T18:46:31.000Z",
    "user_id": 2
}
)TTT");

    QCOMPARE(json, expectedJson);

    // Restore
    torrent->clearHidden();
    torrent->clearAppends();
}

void tst_Model_Appends::toJson_WithAppends_WithVisisbleAndHidden() const
{
    auto torrent = Torrent::find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, NAME, NOTE, "name_progress", "name_size"});
    torrent->setHidden({NOTE, "name_progress"});
    torrent->append({"name_progress", "name_size"});

    QByteArray json = torrent->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"TTT({
    "id": 7,
    "name": "test7",
    "name_size": "test7 (17)"
}
)TTT");

    QCOMPARE(json, expectedJson);

    // Restore
    torrent->clearVisible();
    torrent->clearHidden();
    torrent->clearAppends();
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_Model_Appends)

#include "tst_model_appends.moc"
