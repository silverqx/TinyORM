#include <QCoreApplication>
#include <QTest>

#include "databases.hpp"

#include "models/album.hpp"
#include "models/torrent.hpp"

using Orm::Constants::CREATED_AT;
using Orm::Constants::DELETED_AT;
using Orm::Constants::HASH_;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;
using Orm::Constants::UPDATED_AT;

using Orm::Utils::NullVariant;

using TypeUtils = Orm::Utils::Type;

using Orm::Tiny::AttributeItem;
using Orm::Tiny::ConnectionOverride;

using TestUtils::Databases;

using Models::Album;
using Models::Torrent;
using Models::User;

class tst_Model_HidesAttributes : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase() const;

    /* Serialization - HidesAttributes */
    void toMap_WithVisible() const;
    void toList_WithVisible() const;

    void toMap_WithHidden() const;
    void toList_WithHidden() const;

    void toMap_WithVisibleAndHidden() const;
    void toList_WithVisibleAndHidden() const;

    void toMap_WithVisible_WithRelations_HasOne_HasMany_BelongsTo() const;
    void toList_WithVisible_WithRelations_HasOne_HasMany_BelongsTo() const;
    void toMap_WithVisibleAndHidden_WithRelations_HasOne_HasMany_BelongsTo() const;
    void toList_WithVisibleAndHidden_WithRelations_HasOne_HasMany_BelongsTo() const;

    void toMap_WithVisible_WithRelations_BelongsToMany_UserRoles() const;
    void toList_WithVisible_WithRelations_BelongsToMany_UserRoles() const;
    void toMap_WithHidden_WithRelations_BelongsToMany_UserRoles() const;
    void toList_WithHidden_WithRelations_BelongsToMany_UserRoles() const;

    void toJson_WithVisible() const;
    void toJson_WithHidden() const;
    void toJson_WithVisisbleAndHidden() const;

    void toJson_WithVisible_WithRelations_HasOne_HasMany_BelongsTo() const;
    void toJson_WithHidden_WithRelations_HasOne_HasMany_BelongsTo() const;
    void toJson_WithVisisbleAndHidden_WithRelations_HasOne_HasMany_BelongsTo() const;

    void toJson_WithVisible_WithRelation_BelongsToMany_TorrentTags_TorrentStates() const;
    void toJson_WithHidden_WithRelation_BelongsToMany_TorrentTags_TorrentStates() const;
    void toJson_WithVisisbleAndHidden_WithRelation_BelongsToMany_TorrentTags_TorrentStates() const;

    void toJson_WithVisible_u_snakeAttributes_false() const;

    void makeVisible() const;
    void makeHidden() const;

    void hasVisible_hasHidden() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Connection name used in this test case. */
    QString m_connection;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Model_HidesAttributes::initTestCase()
{
    ConnectionOverride::connection = m_connection =
            Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );
}

void tst_Model_HidesAttributes::cleanupTestCase() const
{
    // Reset connection override
    ConnectionOverride::connection.clear();
}

/* Serialization - HidesAttributes */

void tst_Model_HidesAttributes::toMap_WithVisible() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, "user_id", "added_on"});

    QVariantMap serialized = torrent->toMap();
    QCOMPARE(serialized.size(), 3);

    QVariantMap expectedAttributes {
        {"added_on", "2020-08-04T20:11:10.000Z"},
        {ID,         4},
        {"user_id",  1},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrent->clearVisible();
}

void tst_Model_HidesAttributes::toList_WithVisible() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, "user_id", "added_on"});

    QList<AttributeItem> serialized = torrent->toList();
    QCOMPARE(serialized.size(), 3);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {ID,         4},
        {"user_id",  1},
        {"added_on", "2020-08-04T20:11:10.000Z"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrent->clearVisible();
}

void tst_Model_HidesAttributes::toMap_WithHidden() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setHidden({HASH_, NOTE, SIZE_});

    QVariantMap serialized = torrent->toMap();
    QCOMPARE(serialized.size(), 7);

    QVariantMap expectedAttributes {
        {"added_on", "2020-08-04T20:11:10.000Z"},
        {CREATED_AT, "2019-09-04T08:11:23.000Z"},
        {ID,         4},
        {NAME,       "test4"},
        {Progress,   400},
        {UPDATED_AT, "2021-01-04T18:46:31.000Z"},
        {"user_id",  1},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrent->clearHidden();
}

void tst_Model_HidesAttributes::toList_WithHidden() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setHidden({HASH_, NOTE, SIZE_});

    QList<AttributeItem> serialized = torrent->toList();
    QCOMPARE(serialized.size(), 7);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {ID,         4},
        {"user_id",  1},
        {NAME,       "test4"},
        {Progress,   400},
        {"added_on", "2020-08-04T20:11:10.000Z"},
        {CREATED_AT, "2019-09-04T08:11:23.000Z"},
        {UPDATED_AT, "2021-01-04T18:46:31.000Z"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrent->clearHidden();
}

void tst_Model_HidesAttributes::toMap_WithVisibleAndHidden() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, "user_id", "added_on"});
    torrent->setHidden({"added_on"});

    QVariantMap serialized = torrent->toMap();
    QCOMPARE(serialized.size(), 2);

    QVariantMap expectedAttributes {
        {ID,        4},
        {"user_id", 1},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrent->clearVisible();
    torrent->clearHidden();
}

void tst_Model_HidesAttributes::toList_WithVisibleAndHidden() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, "user_id", "added_on"});
    torrent->setHidden({"added_on"});

    QList<AttributeItem> serialized = torrent->toList();
    QCOMPARE(serialized.size(), 2);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {ID,        4},
        {"user_id", 1},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrent->clearVisible();
    torrent->clearHidden();
}

void
tst_Model_HidesAttributes::toMap_WithVisible_WithRelations_HasOne_HasMany_BelongsTo()
const
{
    auto torrent = Torrent::with({"torrentPeer", "user", "torrentFiles"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, "user_id", "torrentPeer", "torrentFiles"});

    QVariantMap serialized = torrent->toMap();

    QVariantMap expectedAttributes {
        {ID,              7},
        {"torrent_files", QVariantList {QVariantMap {
                              {CREATED_AT,    "2021-01-10T14:51:23.000Z"},
                              {"file_index",  0},
                              {"filepath",    "test7_file1.mkv"},
                              {ID,            10},
                              {NOTE,          "for serialization"},
                              {Progress,      512},
                              {SIZE_,         4562},
                              {"torrent_id",  7},
                              {UPDATED_AT,    "2021-01-10T17:46:31.000Z"},
                          }, QVariantMap {
                              {CREATED_AT,    "2021-01-11T14:51:23.000Z"},
                              {"file_index",  1},
                              {"filepath",    "test7_file2.mkv"},
                              {ID,            11},
                              {NOTE,          "for serialization"},
                              {Progress,      256},
                              {SIZE_,         2567},
                              {"torrent_id",  7},
                              {UPDATED_AT,    "2021-01-11T17:46:31.000Z"},
                          }, QVariantMap {
                              {CREATED_AT,    "2021-01-12T14:51:23.000Z"},
                              {"file_index",  2},
                              {"filepath",    "test7_file3.mkv"},
                              {ID,            12},
                              {NOTE,          "for serialization"},
                              {Progress,      768},
                              {SIZE_,         4279},
                              {"torrent_id",  7},
                              {UPDATED_AT,    "2021-01-12T17:46:31.000Z"},
                          }}},
        {"torrent_peer",  QVariantMap {
                              {CREATED_AT,       "2021-01-07T14:51:23.000Z"},
                              {ID,               5},
                              {"leechers",       7},
                              {"seeds",          NullVariant::Int()},
                              {"torrent_id",     7},
                              {"total_leechers", 7},
                              {"total_seeds",    7},
                              {UPDATED_AT,       "2021-01-07T17:46:31.000Z"},
                          }},
        {"user_id",       2},
    };

    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrent->clearVisible();
}

void tst_Model_HidesAttributes::
     toList_WithVisible_WithRelations_HasOne_HasMany_BelongsTo() const
{
    auto torrent = Torrent::with({"torrentPeer", "user", "torrentFiles"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, "user_id", "torrentPeer", "torrentFiles"});

    QList<AttributeItem> serialized = torrent->toList();

    // Verify
    /* Here we will have to compare all serialized relation attributes separately
       because the Model::m_relations is the std::unordered_map so the relations are
       serialized in random order. */
    const auto keyProj = [](const auto &attribute)
    {
        return attribute.key;
    };

    // torrent_peer
    {
        const auto it = ranges::find(serialized, "torrent_peer", keyProj);
        if (it == serialized.end())
            QFAIL("The \"torrent_peer\" key not found in the \"serialized\" result.");

        auto actualAttributes = serialized.takeAt(std::distance(serialized.begin(), it))
                                    .value.value<QList<AttributeItem>>();

        QList<AttributeItem> expectedAttributes {
            {ID,               5},
            {"torrent_id",     7},
            {"seeds",          NullVariant::Int()},
            {"total_seeds",    7},
            {"leechers",       7},
            {"total_leechers", 7},
            {CREATED_AT,       "2021-01-07T14:51:23.000Z"},
            {UPDATED_AT,       "2021-01-07T17:46:31.000Z"},
        };

        QCOMPARE(actualAttributes, expectedAttributes);
    }
    // torrent_files
    {
        const auto it = ranges::find(serialized, "torrent_files", keyProj);
        if (it == serialized.end())
            QFAIL("The \"torrent_files\" key not found in the \"serialized\" result.");

        auto actualAttributes = serialized.takeAt(std::distance(serialized.begin(), it))
                                    .value.value<QVariantList>();

        QVariantList expectedAttributes {QVariant::fromValue(QList<AttributeItem> {
            {ID,            10},
            {"torrent_id",  7},
            {"file_index",  0},
            {"filepath",    "test7_file1.mkv"},
            {SIZE_,         4562},
            {Progress,      512},
            {NOTE,          "for serialization"},
            {CREATED_AT,    "2021-01-10T14:51:23.000Z"},
            {UPDATED_AT,    "2021-01-10T17:46:31.000Z"},
        }), QVariant::fromValue(QList<AttributeItem> {
            {ID,            11},
            {"torrent_id",  7},
            {"file_index",  1},
            {"filepath",    "test7_file2.mkv"},
            {SIZE_,         2567},
            {Progress,      256},
            {NOTE,          "for serialization"},
            {CREATED_AT,    "2021-01-11T14:51:23.000Z"},
            {UPDATED_AT,    "2021-01-11T17:46:31.000Z"},
        }), QVariant::fromValue(QList<AttributeItem> {
            {ID,            12},
            {"torrent_id",  7},
            {"file_index",  2},
            {"filepath",    "test7_file3.mkv"},
            {SIZE_,         4279},
            {Progress,      768},
            {NOTE,          "for serialization"},
            {CREATED_AT,    "2021-01-12T14:51:23.000Z"},
            {UPDATED_AT,    "2021-01-12T17:46:31.000Z"},
        })};

        QCOMPARE(actualAttributes, expectedAttributes);
    }
    // Compare the rest of attributes - torrent itself
    {
        QList<AttributeItem> expectedAttributes {
            {ID,        7},
            {"user_id", 2},
        };

        QCOMPARE(serialized, expectedAttributes);
    }

    // Restore
    torrent->clearVisible();
}

void tst_Model_HidesAttributes::
     toMap_WithVisibleAndHidden_WithRelations_HasOne_HasMany_BelongsTo() const
{
    auto torrent = Torrent::with({"torrentPeer", "user", "torrentFiles"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, "user_id", SIZE_, "torrentPeer", "torrentFiles"});
    torrent->setHidden({SIZE_, "torrentPeer"});

    QVariantMap serialized = torrent->toMap();

    QVariantMap expectedAttributes {
        {ID,              7},
        {"torrent_files", QVariantList {QVariantMap {
                              {CREATED_AT,    "2021-01-10T14:51:23.000Z"},
                              {"file_index",  0},
                              {"filepath",    "test7_file1.mkv"},
                              {ID,            10},
                              {NOTE,          "for serialization"},
                              {Progress,      512},
                              {SIZE_,         4562},
                              {"torrent_id",  7},
                              {UPDATED_AT,    "2021-01-10T17:46:31.000Z"},
                          }, QVariantMap {
                              {CREATED_AT,    "2021-01-11T14:51:23.000Z"},
                              {"file_index",  1},
                              {"filepath",    "test7_file2.mkv"},
                              {ID,            11},
                              {NOTE,          "for serialization"},
                              {Progress,      256},
                              {SIZE_,         2567},
                              {"torrent_id",  7},
                              {UPDATED_AT,    "2021-01-11T17:46:31.000Z"},
                          }, QVariantMap {
                              {CREATED_AT,    "2021-01-12T14:51:23.000Z"},
                              {"file_index",  2},
                              {"filepath",    "test7_file3.mkv"},
                              {ID,            12},
                              {NOTE,          "for serialization"},
                              {Progress,      768},
                              {SIZE_,         4279},
                              {"torrent_id",  7},
                              {UPDATED_AT,    "2021-01-12T17:46:31.000Z"},
                          }}},
        {"user_id",       2},
    };

    QCOMPARE(serialized, expectedAttributes);

    // Restore
    torrent->clearVisible();
    torrent->clearHidden();
}

void tst_Model_HidesAttributes::
     toList_WithVisibleAndHidden_WithRelations_HasOne_HasMany_BelongsTo() const
{
    auto torrent = Torrent::with({"torrentPeer", "user", "torrentFiles"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, "user_id", NOTE, "torrentPeer", "torrentFiles"});
    torrent->setHidden({NOTE, "torrentFiles"});

    QList<AttributeItem> serialized = torrent->toList();

    // Verify
    /* Here we will have to compare all serialized relation attributes separately
       because the Model::m_relations is the std::unordered_map so the relations are
       serialized in random order. */
    const auto keyProj = [](const auto &attribute)
    {
        return attribute.key;
    };

    // torrent_peer
    {
        const auto it = ranges::find(serialized, "torrent_peer", keyProj);
        if (it == serialized.end())
            QFAIL("The \"torrent_peer\" key not found in the \"serialized\" result.");

        auto actualAttributes = serialized.takeAt(std::distance(serialized.begin(), it))
                                    .value.value<QList<AttributeItem>>();

        QList<AttributeItem> expectedAttributes {
            {ID,               5},
            {"torrent_id",     7},
            {"seeds",          NullVariant::Int()},
            {"total_seeds",    7},
            {"leechers",       7},
            {"total_leechers", 7},
            {CREATED_AT,       "2021-01-07T14:51:23.000Z"},
            {UPDATED_AT,       "2021-01-07T17:46:31.000Z"},
        };

        QCOMPARE(actualAttributes, expectedAttributes);
    }
    // Compare the rest of attributes - torrent itself
    {
        QList<AttributeItem> expectedAttributes {
            {ID,        7},
            {"user_id", 2},
        };

        QCOMPARE(serialized, expectedAttributes);
    }

    // Restore
    torrent->clearVisible();
    torrent->clearHidden();
}

void tst_Model_HidesAttributes::
     toMap_WithVisible_WithRelations_BelongsToMany_UserRoles() const
{
    auto user = User::with("roles")->find(1);
    QVERIFY(user);
    QVERIFY(user->exists);

    // Prepare
    user->setVisible({ID, NAME, "roles"});

    QVariantMap serialized = user->toMap();

    QVariantMap expectedAttributes {
        {ID,      1},
        {NAME,    "andrej"},
        {"roles", QVariantList {QVariantMap {
                      {"added_on",     "2022-08-01T13:36:56.000Z"},
                      {ID,             1},
                      {NAME,           "role one"},
                      {"subscription", QVariantMap {
                                           {"active",  true},
                                           {"role_id", 1},
                                           {"user_id", 1},
                                       }},
                  }, QVariantMap {
                      {"added_on",     "2022-08-02T13:36:56.000Z"},
                      {ID,             2},
                      {NAME,           "role two"},
                      {"subscription", QVariantMap {
                                           {"active",  false},
                                           {"role_id", 2},
                                           {"user_id", 1},
                                       }},
                  }, QVariantMap {
                      {"added_on",     NullVariant::QDateTime()},
                      {ID,             3},
                      {NAME,           "role three"},
                      {"subscription", QVariantMap {
                                           {"active",  true},
                                           {"role_id", 3},
                                           {"user_id", 1},
                                       }},
                  }}},
    };

    QCOMPARE(serialized, expectedAttributes);

    // Restore
    user->clearVisible();
}

void tst_Model_HidesAttributes::
     toList_WithVisible_WithRelations_BelongsToMany_UserRoles() const
{
    auto user = User::with("roles")->find(1);
    QVERIFY(user);
    QVERIFY(user->exists);

    // Prepare
    user->setVisible({ID, NAME, "roles"});

    QList<AttributeItem> serialized = user->toList();

    QList<AttributeItem> expectedAttributes {
        {ID,      1},
        {NAME,    "andrej"},
        {"roles", QVariantList {QVariant::fromValue(QList<AttributeItem> {
                      {ID,             1},
                      {NAME,           "role one"},
                      {"added_on",     "2022-08-01T13:36:56.000Z"},
                      {"subscription", QVariant::fromValue(QList<AttributeItem> {
                                           {"user_id", 1},
                                           {"role_id", 1},
                                           {"active",  true},
                                       })},
                  }), QVariant::fromValue(QList<AttributeItem> {
                      {ID,             2},
                      {NAME,           "role two"},
                      {"added_on",     "2022-08-02T13:36:56.000Z"},
                      {"subscription", QVariant::fromValue(QList<AttributeItem> {
                                           {"user_id", 1},
                                           {"role_id", 2},
                                           {"active",  false},
                                       })},
                  }), QVariant::fromValue(QList<AttributeItem> {
                      {ID,             3},
                      {NAME,           "role three"},
                      {"added_on",     NullVariant::QDateTime()},
                      {"subscription", QVariant::fromValue(QList<AttributeItem> {
                                           {"user_id", 1},
                                           {"role_id", 3},
                                           {"active",  true},
                                       })},
                  })}},
    };

    QCOMPARE(serialized, expectedAttributes);

    // Restore
    user->clearVisible();
}

void
tst_Model_HidesAttributes::toMap_WithHidden_WithRelations_BelongsToMany_UserRoles() const
{
    auto user = User::with("roles")->find(1);
    QVERIFY(user);
    QVERIFY(user->exists);

    // Prepare
    user->setHidden({NAME, NOTE, "roles"});

    QVariantMap serialized = user->toMap();

    QVariantMap expectedAttributes {
        {CREATED_AT,  "2022-01-01T14:51:23.000Z"},
        {DELETED_AT,  NullVariant::QDateTime()},
        {ID,          1},
        {"is_banned", false},
        {UPDATED_AT,  "2022-01-01T17:46:31.000Z"},
    };

    QCOMPARE(serialized, expectedAttributes);

    // Restore
    user->clearHidden();
}

void
tst_Model_HidesAttributes::toList_WithHidden_WithRelations_BelongsToMany_UserRoles()
const
{
    auto user = User::with("roles")->find(1);
    QVERIFY(user);
    QVERIFY(user->exists);

    // Prepare
    user->setHidden({NAME, NOTE, "roles"});

    QList<AttributeItem> serialized = user->toList();

    QList<AttributeItem> expectedAttributes {
        {ID,          1},
        {"is_banned", false},
        {CREATED_AT,  "2022-01-01T14:51:23.000Z"},
        {UPDATED_AT,  "2022-01-01T17:46:31.000Z"},
        {DELETED_AT,  NullVariant::QDateTime()},
    };

    QCOMPARE(serialized, expectedAttributes);

    // Restore
    user->clearHidden();
}

void tst_Model_HidesAttributes::toJson_WithVisible() const
{
    auto torrent = Torrent::find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, NAME, NOTE});

    QByteArray json = torrent->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"({
    "id": 7,
    "name": "test7",
    "note": "for serialization"
}
)");

    QCOMPARE(json, expectedJson);

    // Restore
    torrent->clearVisible();
}

void tst_Model_HidesAttributes::toJson_WithHidden() const
{
    auto torrent = Torrent::find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setHidden({SIZE_, NAME, NOTE});

    QByteArray json = torrent->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"({
    "added_on": "2020-08-07T20:11:10.000Z",
    "created_at": "2021-11-07T08:13:23.000Z",
    "hash": "7579e3af2768cdf52ec84c1f320333f68401dc6e",
    "id": 7,
    "progress": 700,
    "updated_at": "2021-01-07T18:46:31.000Z",
    "user_id": 2
}
)");

    QCOMPARE(json, expectedJson);

    // Restore
    torrent->clearHidden();
}

void tst_Model_HidesAttributes::toJson_WithVisisbleAndHidden() const
{
    auto torrent = Torrent::find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, NAME, NOTE});
    torrent->setHidden({NOTE});

    QByteArray json = torrent->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"({
    "id": 7,
    "name": "test7"
}
)");

    QCOMPARE(json, expectedJson);

    // Restore
    torrent->clearVisible();
    torrent->clearHidden();
}

void tst_Model_HidesAttributes::
     toJson_WithVisible_WithRelations_HasOne_HasMany_BelongsTo() const
{
    auto torrent = Torrent::with({"torrentPeer", "user", "torrentFiles"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, NAME, "user_id", "user"});

    QByteArray json = torrent->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"({
    "id": 7,
    "name": "test7",
    "user": {
        "created_at": "2022-01-02T14:51:23.000Z",
        "deleted_at": null,
        "id": 2,
        "is_banned": false,
        "name": "silver",
        "note": null,
        "updated_at": "2022-01-02T17:46:31.000Z"
    },
    "user_id": 2
}
)");

    QCOMPARE(json, expectedJson);

    // Restore
    torrent->clearVisible();
}

void tst_Model_HidesAttributes::
     toJson_WithHidden_WithRelations_HasOne_HasMany_BelongsTo() const
{
    auto torrent = Torrent::with({"torrentPeer", "user", "torrentFiles"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setHidden({NOTE, CREATED_AT, SIZE_, "torrentPeer", "torrentFiles"});

    QByteArray json = torrent->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"({
    "added_on": "2020-08-07T20:11:10.000Z",
    "hash": "7579e3af2768cdf52ec84c1f320333f68401dc6e",
    "id": 7,
    "name": "test7",
    "progress": 700,
    "updated_at": "2021-01-07T18:46:31.000Z",
    "user": {
        "created_at": "2022-01-02T14:51:23.000Z",
        "deleted_at": null,
        "id": 2,
        "is_banned": false,
        "name": "silver",
        "note": null,
        "updated_at": "2022-01-02T17:46:31.000Z"
    },
    "user_id": 2
}
)");

    QCOMPARE(json, expectedJson);

    // Restore
    torrent->clearHidden();
}

void tst_Model_HidesAttributes::
     toJson_WithVisisbleAndHidden_WithRelations_HasOne_HasMany_BelongsTo() const
{
    auto torrent = Torrent::with({"torrentPeer", "user", "torrentFiles"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, NAME, NOTE, SIZE_, "user_id", "torrentFiles",
                         "torrentPeer"});
    torrent->setHidden({NOTE, "user", "torrentPeer"});

    QByteArray json = torrent->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"({
    "id": 7,
    "name": "test7",
    "size": 17,
    "torrent_files": [
        {
            "created_at": "2021-01-10T14:51:23.000Z",
            "file_index": 0,
            "filepath": "test7_file1.mkv",
            "id": 10,
            "note": "for serialization",
            "progress": 512,
            "size": 4562,
            "torrent_id": 7,
            "updated_at": "2021-01-10T17:46:31.000Z"
        },
        {
            "created_at": "2021-01-11T14:51:23.000Z",
            "file_index": 1,
            "filepath": "test7_file2.mkv",
            "id": 11,
            "note": "for serialization",
            "progress": 256,
            "size": 2567,
            "torrent_id": 7,
            "updated_at": "2021-01-11T17:46:31.000Z"
        },
        {
            "created_at": "2021-01-12T14:51:23.000Z",
            "file_index": 2,
            "filepath": "test7_file3.mkv",
            "id": 12,
            "note": "for serialization",
            "progress": 768,
            "size": 4279,
            "torrent_id": 7,
            "updated_at": "2021-01-12T17:46:31.000Z"
        }
    ],
    "user_id": 2
}
)");

    QCOMPARE(json, expectedJson);

    // Restore
    torrent->clearVisible();
    torrent->clearHidden();
}

void tst_Model_HidesAttributes::
     toJson_WithVisible_WithRelation_BelongsToMany_TorrentTags_TorrentStates() const
{
    auto torrent = Torrent::with({"tags", "torrentStates"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, NAME, NOTE, "user_id", "tags"});

    QByteArray json = torrent->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"({
    "id": 7,
    "name": "test7",
    "note": "for serialization",
    "tags": [
        {
            "created_at": "2021-01-11T11:51:28.000Z",
            "id": 1,
            "name": "tag1",
            "note": null,
            "tag_property": {
                "color": "white",
                "created_at": "2021-02-11T12:41:28.000Z",
                "id": 1,
                "position": 0,
                "tag_id": 1,
                "updated_at": "2021-02-11T22:17:11.000Z"
            },
            "tagged": {
                "active": true,
                "created_at": "2021-03-01T17:31:58.000Z",
                "tag_id": 1,
                "torrent_id": 7,
                "updated_at": "2021-03-01T18:49:22.000Z"
            },
            "updated_at": "2021-01-11T23:47:11.000Z"
        },
        {
            "created_at": "2021-01-12T11:51:28.000Z",
            "id": 2,
            "name": "tag2",
            "note": null,
            "tag_property": {
                "color": "blue",
                "created_at": "2021-02-12T12:41:28.000Z",
                "id": 2,
                "position": 1,
                "tag_id": 2,
                "updated_at": "2021-02-12T22:17:11.000Z"
            },
            "tagged": {
                "active": true,
                "created_at": "2021-03-02T17:31:58.000Z",
                "tag_id": 2,
                "torrent_id": 7,
                "updated_at": "2021-03-02T18:49:22.000Z"
            },
            "updated_at": "2021-01-12T23:47:11.000Z"
        },
        {
            "created_at": "2021-01-13T11:51:28.000Z",
            "id": 3,
            "name": "tag3",
            "note": null,
            "tag_property": {
                "color": "red",
                "created_at": "2021-02-13T12:41:28.000Z",
                "id": 3,
                "position": 2,
                "tag_id": 3,
                "updated_at": "2021-02-13T22:17:11.000Z"
            },
            "tagged": {
                "active": false,
                "created_at": "2021-03-03T17:31:58.000Z",
                "tag_id": 3,
                "torrent_id": 7,
                "updated_at": "2021-03-03T18:49:22.000Z"
            },
            "updated_at": "2021-01-13T23:47:11.000Z"
        }
    ],
    "user_id": 2
}
)");

    QCOMPARE(json, expectedJson);

    // Restore
    torrent->clearVisible();
}

void tst_Model_HidesAttributes::
     toJson_WithHidden_WithRelation_BelongsToMany_TorrentTags_TorrentStates() const
{
    auto torrent = Torrent::with({"tags", "torrentStates"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setHidden({NAME, NOTE, "tags"});

    QByteArray json = torrent->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"({
    "added_on": "2020-08-07T20:11:10.000Z",
    "created_at": "2021-11-07T08:13:23.000Z",
    "hash": "7579e3af2768cdf52ec84c1f320333f68401dc6e",
    "id": 7,
    "progress": 700,
    "size": 17,
    "torrent_states": [
        {
            "id": 1,
            "name": "Active",
            "pivot": {
                "active": 1,
                "state_id": 1,
                "torrent_id": 7
            }
        },
        {
            "id": 4,
            "name": "Downloading",
            "pivot": {
                "active": 0,
                "state_id": 4,
                "torrent_id": 7
            }
        }
    ],
    "updated_at": "2021-01-07T18:46:31.000Z",
    "user_id": 2
}
)");

    QCOMPARE(json, expectedJson);

    // Restore
    torrent->clearHidden();
}

void tst_Model_HidesAttributes::
toJson_WithVisisbleAndHidden_WithRelation_BelongsToMany_TorrentTags_TorrentStates() const
{
    auto torrent = Torrent::with({"tags", "torrentStates"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Prepare
    torrent->setVisible({ID, NAME, NOTE, "user_id", "tags", "torrentStates"});
    torrent->setHidden({NOTE, "torrentStates"});

    QByteArray json = torrent->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"({
    "id": 7,
    "name": "test7",
    "tags": [
        {
            "created_at": "2021-01-11T11:51:28.000Z",
            "id": 1,
            "name": "tag1",
            "note": null,
            "tag_property": {
                "color": "white",
                "created_at": "2021-02-11T12:41:28.000Z",
                "id": 1,
                "position": 0,
                "tag_id": 1,
                "updated_at": "2021-02-11T22:17:11.000Z"
            },
            "tagged": {
                "active": true,
                "created_at": "2021-03-01T17:31:58.000Z",
                "tag_id": 1,
                "torrent_id": 7,
                "updated_at": "2021-03-01T18:49:22.000Z"
            },
            "updated_at": "2021-01-11T23:47:11.000Z"
        },
        {
            "created_at": "2021-01-12T11:51:28.000Z",
            "id": 2,
            "name": "tag2",
            "note": null,
            "tag_property": {
                "color": "blue",
                "created_at": "2021-02-12T12:41:28.000Z",
                "id": 2,
                "position": 1,
                "tag_id": 2,
                "updated_at": "2021-02-12T22:17:11.000Z"
            },
            "tagged": {
                "active": true,
                "created_at": "2021-03-02T17:31:58.000Z",
                "tag_id": 2,
                "torrent_id": 7,
                "updated_at": "2021-03-02T18:49:22.000Z"
            },
            "updated_at": "2021-01-12T23:47:11.000Z"
        },
        {
            "created_at": "2021-01-13T11:51:28.000Z",
            "id": 3,
            "name": "tag3",
            "note": null,
            "tag_property": {
                "color": "red",
                "created_at": "2021-02-13T12:41:28.000Z",
                "id": 3,
                "position": 2,
                "tag_id": 3,
                "updated_at": "2021-02-13T22:17:11.000Z"
            },
            "tagged": {
                "active": false,
                "created_at": "2021-03-03T17:31:58.000Z",
                "tag_id": 3,
                "torrent_id": 7,
                "updated_at": "2021-03-03T18:49:22.000Z"
            },
            "updated_at": "2021-01-13T23:47:11.000Z"
        }
    ],
    "user_id": 2
}
)");

    QCOMPARE(json, expectedJson);

    // Restore
    torrent->clearVisible();
    torrent->clearHidden();
}

void tst_Model_HidesAttributes::toJson_WithVisible_u_snakeAttributes_false() const
{
    auto album = Album::find(1);
    QVERIFY(album);
    QVERIFY(album->exists);

    // Prepare
    album->setVisible({ID, NAME, "albumImages"});

    QByteArray json = album->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"({
    "albumImages": [
        {
            "album_id": 1,
            "created_at": "2023-03-01T15:24:37.000Z",
            "ext": "png",
            "id": 1,
            "name": "album1_image1",
            "size": 726,
            "updated_at": "2023-04-01T14:35:47.000Z"
        }
    ],
    "id": 1,
    "name": "album1"
}
)");

    QCOMPARE(json, expectedJson);

    // Restore
    album->clearVisible();
}

void tst_Model_HidesAttributes::makeVisible() const
{
    Torrent torrent;
    QVERIFY(!torrent.exists);
    QVERIFY(torrent.getVisible().empty());
    QVERIFY(torrent.getHidden().empty());

    // Prepare
    torrent.setHidden({CREATED_AT, SIZE_, NOTE});
    {
        std::set<QString> expectedHidden {CREATED_AT, SIZE_, NOTE};
        QCOMPARE(torrent.getHidden(), expectedHidden);
    }

    /* If the u_visible is empty (empty u_visible means that all attributes are
       visible, so no merge is needed). */
    {
        torrent.makeVisible({ID, NAME});

        std::set<QString> expectedVisible {ID, NAME};
        QVERIFY(torrent.getVisible().empty());

        std::set<QString> expectedHidden {CREATED_AT, SIZE_, NOTE};
        QCOMPARE(torrent.getHidden(), expectedHidden);
    }

    // Prepare for the rest of the testing
    torrent.setVisible({ID, NAME});
    {
        std::set<QString> expectedVisible {ID, NAME};
        QCOMPARE(torrent.getVisible(), expectedVisible);
    }

    // If the u_visible already contains some attributes
    {
        torrent.makeVisible({SIZE_, NOTE});

        std::set<QString> expectedVisible {ID, NAME, NOTE, SIZE_};
        QCOMPARE(torrent.getVisible(), expectedVisible);
        // Must remove from u_hidden
        std::set<QString> expectedHidden {CREATED_AT};
        QCOMPARE(torrent.getHidden(), expectedHidden);
    }
    // If the u_visible already contains SAME attributes
    {
        torrent.makeVisible({SIZE_, NOTE, CREATED_AT});

        std::set<QString> expectedVisible {CREATED_AT, ID, NAME, NOTE, SIZE_};
        QCOMPARE(torrent.getVisible(), expectedVisible);
        // Must remove from u_hidden
        QVERIFY(torrent.getHidden().empty());
    }
    // QString overload
    {
        torrent.setVisible({NAME});
        torrent.makeVisible(SIZE_);

        std::set<QString> expectedVisible {NAME, SIZE_};
        QCOMPARE(torrent.getVisible(), expectedVisible);
        QVERIFY(torrent.getHidden().empty());
    }

    // Restore
    torrent.clearVisible();
    torrent.clearHidden();
}

void tst_Model_HidesAttributes::makeHidden() const
{
    Torrent torrent;
    QVERIFY(!torrent.exists);
    QVERIFY(torrent.getVisible().empty());
    QVERIFY(torrent.getHidden().empty());

    // If the u_hidden is empty
    {
        torrent.makeHidden({ID, NAME});

        std::set<QString> expectedHidden {ID, NAME};
        QCOMPARE(torrent.getHidden(), expectedHidden);
        QVERIFY(torrent.getVisible().empty());
    }
    // If the u_hidden already contains some attributes
    {
        torrent.makeHidden({SIZE_, NOTE});

        std::set<QString> expectedHidden {ID, NAME, NOTE, SIZE_};
        QCOMPARE(torrent.getHidden(), expectedHidden);
        QVERIFY(torrent.getVisible().empty());
    }
    // If the u_hidden already contains SAME attributes
    {
        torrent.makeHidden({SIZE_, NOTE, CREATED_AT});

        std::set<QString> expectedHidden {CREATED_AT, ID, NAME, NOTE, SIZE_};
        QCOMPARE(torrent.getHidden(), expectedHidden);
        QVERIFY(torrent.getVisible().empty());
    }
    // QString overload
    {
        torrent.setHidden({NAME});
        torrent.makeHidden(SIZE_);

        std::set<QString> expectedHidden {NAME, SIZE_};
        QCOMPARE(torrent.getHidden(), expectedHidden);
        QVERIFY(torrent.getVisible().empty());
    }

    // Restore
    torrent.clearHidden();
}

void tst_Model_HidesAttributes::hasVisible_hasHidden() const
{
    Torrent torrent;
    QVERIFY(!torrent.exists);
    QVERIFY(torrent.getVisible().empty());
    QVERIFY(torrent.getHidden().empty());

    torrent.setVisible({ID, NAME});
    torrent.setHidden({NOTE});

    std::set<QString> expectedVisible {ID, NAME};
    QCOMPARE(torrent.getVisible(), expectedVisible);
    std::set<QString> expectedHidden {NOTE};
    QCOMPARE(torrent.getHidden(), expectedHidden);

    QVERIFY(torrent.hasVisible(ID));
    QVERIFY(torrent.hasVisible(NAME));
    QVERIFY(torrent.hasHidden(NOTE));

    // Restore
    torrent.clearVisible();
    torrent.clearHidden();
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_Model_HidesAttributes)

#include "tst_model_hidesattributes.moc"
