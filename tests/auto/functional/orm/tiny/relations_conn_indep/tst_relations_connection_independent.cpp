#include <QCoreApplication>
#include <QTest>

#include "common/collection.hpp"
#include "databases.hpp"

#include "models/torrent.hpp"

using Orm::Constants::HASH_;
using Orm::Constants::ID;
using Orm::Constants::CREATED_AT;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;
using Orm::Constants::UPDATED_AT;

using Orm::One;

using TypeUtils = Orm::Utils::Type;

using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::Types::ModelsCollection;

using TestUtils::Databases;

using Common = TestUtils::Common::Collection;

using Models::Phone;
using Models::Role;
using Models::Tag;
using Models::Torrent;
using Models::TorrentPeer;
using Models::TorrentPreviewableFile;
using Models::User;

class tst_Relations_Connection_Independent : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void with_HasOne_SameRelatedModels() const;
    void with_HasMany_SameRelatedModels() const;
    void with_BelongsTo_SameRelatedModels() const;
    void with_BelongsToMany_SameRelatedModels() const;

    /* HasOne */
    void is_HasOne() const;
    void isNot_HasOne() const;

    /* BelongsTo */
    void is_BelongsTo() const;
    void isNot_BelongsTo() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Connection name used in this test case. */
    QString m_connection;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Relations_Connection_Independent::initTestCase()
{
    ConnectionOverride::connection = m_connection =
            Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );
}

/* The following tests test the bug where one related model is set on more parent models,
   but the related model was moved to the setRelation("xyz", std::move(related)),
   the result was that the related models where zero-initialized, eg. no attributes, ...
   The problem was only specific for the belongs-to relationship type, all other
   relationships were fine. I will write unit tests to test this scenario for all
   relationship types anyway as these unit tests also test if the attributes are
   set correctly. */

void tst_Relations_Connection_Independent::with_HasOne_SameRelatedModels() const
{
    auto torrents = Torrent::with("torrentPeer")->findMany({1, 2, 3, 4});
    QCOMPARE(torrents.size(), 4);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent>));
    QVERIFY(Common::verifyIds(torrents, {1, 2, 3, 4}));

    for (auto &torrent : torrents) {
        QVERIFY(torrent.exists);

        // Check relations
        QVERIFY(torrent.relationLoaded("torrentPeer"));
        QCOMPARE(torrent.getRelations().size(), 1);

        // Check attributes
        auto *peer = torrent.getRelation<TorrentPeer, One>("torrentPeer");
        QCOMPARE(typeid (peer), typeid (TorrentPeer *));
        const auto &attributes = peer->getAttributes();
        QCOMPARE(attributes.size(), 8);

        std::unordered_set<QString> expectedAttributes {
            ID, "torrent_id", "seeds", "total_seeds", "leechers", "total_leechers",
            CREATED_AT, UPDATED_AT,
        };
        for (const auto &attribute : attributes)
            QVERIFY(expectedAttributes.contains(attribute.key));
    }
}

void tst_Relations_Connection_Independent::with_HasMany_SameRelatedModels() const
{
    auto torrents = Torrent::with("torrentFiles")->findMany({4, 5});
    QCOMPARE(torrents.size(), 2);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent>));
    QVERIFY(Common::verifyIds(torrents, {4, 5}));

    for (auto &torrent : torrents) {
        QVERIFY(torrent.exists);

        // Check relations
        QVERIFY(torrent.relationLoaded("torrentFiles"));
        QCOMPARE(torrent.getRelations().size(), 1);

        // Check attributes
        std::unordered_set<QString> expectedAttributes {
            ID, "torrent_id", "file_index", "filepath", SIZE_, Progress, NOTE,
            CREATED_AT, UPDATED_AT,
        };

        auto files = torrent.getRelation<TorrentPreviewableFile>("torrentFiles");
        for (auto *file : files) {
            QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));
            const auto &attributes = file->getAttributes();
            QCOMPARE(attributes.size(), 9);

            for (const auto &attribute : attributes)
                QVERIFY(expectedAttributes.contains(attribute.key));
        }
    }
}

void tst_Relations_Connection_Independent::with_BelongsTo_SameRelatedModels() const
{
    auto files = TorrentPreviewableFile::with("torrent")->findMany({5, 6, 7, 8});
    QCOMPARE(files.size(), 4);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile>));
    QVERIFY(Common::verifyIds(files, {5, 6, 7, 8}));

    for (auto &file : files) {
        QVERIFY(file.exists);

        // Check relations
        QVERIFY(file.relationLoaded("torrent"));
        QCOMPARE(file.getRelations().size(), 1);

        // Check attributes
        auto *torrent = file.getRelation<Torrent, One>("torrent");
        QCOMPARE(typeid (torrent), typeid (Torrent *));
        const auto &attributes = torrent->getAttributes();
        QCOMPARE(attributes.size(), 10);

        std::unordered_set<QString> expectedAttributes {
            ID, "user_id", NAME, SIZE_, Progress, "added_on", HASH_, NOTE,
            CREATED_AT, UPDATED_AT,
        };
        for (const auto &attribute : attributes)
            QVERIFY(expectedAttributes.contains(attribute.key));
    }
}

void tst_Relations_Connection_Independent::with_BelongsToMany_SameRelatedModels() const
{
    auto torrents = Torrent::with("tags")->findMany({2, 3});
    QCOMPARE(torrents.size(), 2);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent>));
    QVERIFY(Common::verifyIds(torrents, {2, 3}));

    for (auto &torrent : torrents) {
        QVERIFY(torrent.exists);

        // Check relations
        QVERIFY(torrent.relationLoaded("tags"));
        QCOMPARE(torrent.getRelations().size(), 1);

        // Check attributes
        std::unordered_set<QString> expectedAttributes {
            ID, NAME, NOTE, CREATED_AT, UPDATED_AT,
        };

        auto tags = torrent.getRelation<Tag>("tags");
        for (auto *tag : tags) {
            QCOMPARE(typeid (tag), typeid (Tag *));
            const auto &attributes = tag->getAttributes();
            QCOMPARE(attributes.size(), 5);

            for (const auto &attribute : attributes)
                QVERIFY(expectedAttributes.contains(attribute.key));
        }
    }
}

/* HasOne */

void tst_Relations_Connection_Independent::is_HasOne() const
{
    auto user = User::find(1);
    auto phone = Phone::find(1);

    // The same primary key, table name and connection name
    QVERIFY(user->phone()->is(phone));
}

void tst_Relations_Connection_Independent::isNot_HasOne() const
{
    auto user1 = User::find(1);

    // Different primary key
    {
        auto phone2 = Phone::find(2);

        QVERIFY(user1->phone()->isNot(phone2));
    }
    // Different table name (also different type)
    {
        auto role1 = Role::find(1);

        QVERIFY(user1->phone()->isNot(role1));
    }
    // Different connection name
    {
        auto phone1 = Phone::find(1);

        phone1->setConnection("dummy_connection");
        /* Disable connection override, so the isNot() can pickup a connection from
           the model itself (don't pickup an overridden connection). */
        ConnectionOverride::connection.clear();

        QVERIFY(user1->phone()->isNot(phone1));

        // Restore
        ConnectionOverride::connection = m_connection;
    }
}

/* BelongsTo */

void tst_Relations_Connection_Independent::is_BelongsTo() const
{
    auto phone = Phone::find(1);
    auto user = User::find(1);

    // The same primary key, table name and connection name
    QVERIFY(phone->user()->is(user));
}

void tst_Relations_Connection_Independent::isNot_BelongsTo() const
{
    auto phone1 = Phone::find(1);

    // Different primary key
    {
        auto user2 = User::find(2);

        QVERIFY(phone1->user()->isNot(user2));
    }
    // Different table name (also different type)
    {
        auto role1 = Role::find(1);

        QVERIFY(phone1->user()->isNot(role1));
    }
    // Different connection name
    {
        auto user1 = User::find(1);

        user1->setConnection("dummy_connection");
        /* Disable connection override, so the isNot() can pickup a connection from
           the model itself (don't pickup an overridden connection). */
        ConnectionOverride::connection.clear();

        QVERIFY(phone1->user()->isNot(user1));

        // Restore
        ConnectionOverride::connection = m_connection;
    }
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_Relations_Connection_Independent)

#include "tst_relations_connection_independent.moc"
