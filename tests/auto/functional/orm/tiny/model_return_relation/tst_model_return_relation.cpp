#include <QCoreApplication>
#include <QTest>

#include "databases.hpp"

#include "models/torrent_returnrelation.hpp"

using Orm::Constants::CREATED_AT;
using Orm::Constants::UPDATED_AT;

using Orm::One;

using TypeUtils = Orm::Utils::Type;

using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::Relations::Pivot;
using Orm::Tiny::Types::ModelsCollection;

using TestUtils::Databases;

using Models::Tag;
using Models::Tag_ReturnRelation;
using Models::Tagged;
using Models::Torrent_ReturnRelation;
using Models::TorrentPeer;
using Models::TorrentPreviewableFile;
using Models::User;

/* This test case is connection independent and it only runs against the MySQL database.
   The following test methods are practically identical as in the tst_model_relations
   test case with only one difference, that the relationship methods return
   the std::unique_ptr<Relation<>> base Relation instance instead of
   the derived relation, eg. std::unique_ptr<HasMany<>>.
   This behavior is considered an additional feature and is possible thanks to
   the polymorphism.
   In the early stages of the TinyORM project this was the only way how the relationship
   methods instantiated the actual Relation. Nevertheless, I refactored it to the current
   state so that it can also return the Derived Relation instance eg. HasMany.
   And even in later stages of development, I wanted to remove this polymorphic behavior,
   precisely to make the Relation's addEagerConstraints(), initRelation(), match() only
   templated methods, it was during the TinyBuilder::eagerLoadRelations() refactor
   to also accept the ModelsCollection<Model *>, when it was ideal to made these
   methods templated only, but I rejected it exactly because of this feature.
   In the end, I have decided to keep these Relation's methods polymorphic so as
   not to lose this feature. 🙃
   And because of all of this, this test case exists, to test this behavior 😎. */

class tst_Model_Return_Relation : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase() const;

    void getRelation_EagerLoad_HasOne() const;
    void getRelation_EagerLoad_HasMany() const;
    void getRelation_EagerLoad_BelongsTo() const;
    void getRelation_EagerLoad_BelongsToMany_BasicPivot_WithPivotAttributes() const;
    void getRelation_EagerLoad_BelongsToMany_CustomPivot_WithPivotAttributes() const;

    void getRelationValue_LazyLoad_HasOne() const;
    void getRelationValue_LazyLoad_HasMany() const;
    void getRelationValue_LazyLoad_BelongsTo() const;
    void getRelationValue_LazyLoad_BelongsToMany_BasicPivot_WithPivotAttributes() const;
    void getRelationValue_LazyLoad_BelongsToMany_CustomPivot_WithPivotAttributes() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Connection name used in this test case. */
    QString m_connection;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Model_Return_Relation::initTestCase()
{
    ConnectionOverride::connection = m_connection =
            Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );
}

void tst_Model_Return_Relation::cleanupTestCase() const
{
    // Reset connection override
    ConnectionOverride::connection.clear();
}

void tst_Model_Return_Relation::getRelation_EagerLoad_HasOne() const
{
    auto torrent = Torrent_ReturnRelation::with("torrentPeer")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    QCOMPARE(torrent->getKey(), QVariant(2));

    // TorrentPeer has-one relation
    QVERIFY(torrent->relationLoaded("torrentPeer"));
    auto *torrentPeer = torrent->getRelation<TorrentPeer, One>("torrentPeer");
    QVERIFY(torrentPeer);
    QVERIFY(torrentPeer->exists);
    QCOMPARE(torrentPeer->getKey(), QVariant(2));
    QCOMPARE(torrentPeer->getAttribute("torrent_id"), QVariant(2));
    QCOMPARE(typeid (torrentPeer), typeid (TorrentPeer *));
}

void tst_Model_Return_Relation::getRelation_EagerLoad_HasMany() const
{
    auto torrent = Torrent_ReturnRelation::with("torrentFiles")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    auto torrentId = torrent->getKey();
    QCOMPARE(torrentId, QVariant(2));

    // TorrentPreviewableFile has-many relation
    QVERIFY(torrent->relationLoaded("torrentFiles"));
    auto files = torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};

    for (auto *const file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrentId);
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));
    }
}

void tst_Model_Return_Relation::getRelation_EagerLoad_BelongsTo() const
{
    auto torrent = Torrent_ReturnRelation::with("user")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    QCOMPARE(torrent->getKey(), QVariant(2));
    QCOMPARE(torrent->getAttribute("user_id"), QVariant(1));

    // User belongs-to relation
    QVERIFY(torrent->relationLoaded("user"));
    auto *user = torrent->getRelationValue<User, One>("user");
    QVERIFY(user);
    QVERIFY(user->exists);
    QCOMPARE(user->getKey(), QVariant(1));
    QCOMPARE(typeid (user), typeid (User *));

}

void tst_Model_Return_Relation::
     getRelation_EagerLoad_BelongsToMany_BasicPivot_WithPivotAttributes() const
{
    auto torrent = Torrent_ReturnRelation::with("tags")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    const auto torrentId = torrent->getKey();
    QCOMPARE(torrentId, QVariant(2));

    // Tag belongs-to-many relation (basic pivot)
    QVERIFY(torrent->relationLoaded("tags"));
    auto tags = torrent->getRelationValue<Tag_ReturnRelation>("tags");
    QCOMPARE(tags.size(), 4);
    QCOMPARE(typeid (tags), typeid (ModelsCollection<Tag_ReturnRelation *>));

    // Expected tag IDs and pivot attribute 'active', maps tagId to active
    std::unordered_map<quint64, int> activeMap {{1, 1}, {2, 1}, {3, 0}, {4, 1}};

    for (auto *const tag : tags) {
        QVERIFY(tag);
        QVERIFY(tag->exists);
        const auto tagId = tag->getKeyCasted();
        QVERIFY(activeMap.contains(tagId));
        QCOMPARE(typeid (tag), typeid (Tag_ReturnRelation *));

        /* Custom Pivot relation as the Tagged class, under the 'tagged' key
           in the m_relations hash. */
        QVERIFY(tag->relationLoaded("pivot"));
        auto *pivot = tag->getRelation<Pivot, One>("pivot");
        QVERIFY(pivot);
        QVERIFY(pivot->exists);
        QCOMPARE(typeid (pivot), typeid (Pivot *));

        QVERIFY(pivot->usesTimestamps());
        QVERIFY(!pivot->getIncrementing());

        QCOMPARE(pivot->getForeignKey(), QString("torrent_id"));
        QCOMPARE(pivot->getRelatedKey(), QString("tag_id"));

        const auto &attributesHash = pivot->getAttributesHash();

        QCOMPARE(attributesHash.size(), static_cast<std::size_t>(5));

        QCOMPARE(pivot->getAttribute("torrent_id"), torrentId);
        // With pivot attributes, active
        QCOMPARE(pivot->getAttribute<int>("active"), activeMap.at(tagId));
        QVERIFY(attributesHash.contains(CREATED_AT));
        QVERIFY(attributesHash.contains(UPDATED_AT));
    }
}

void tst_Model_Return_Relation::
     getRelation_EagerLoad_BelongsToMany_CustomPivot_WithPivotAttributes() const
{
    auto torrent = Torrent_ReturnRelation::with("tagsCustom")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    const auto torrentId = torrent->getKey();
    QCOMPARE(torrentId, QVariant(2));

    // Tag belongs-to-many relation (custom Tagged pivot)
    QVERIFY(torrent->relationLoaded("tagsCustom"));
    auto tags = torrent->getRelationValue<Tag>("tagsCustom");
    QCOMPARE(tags.size(), 4);
    QCOMPARE(typeid (tags), typeid (ModelsCollection<Tag *>));

    // Expected tag IDs and pivot attribute 'active', maps tagId to active
    std::unordered_map<quint64, int> activeMap {{1, 1}, {2, 1}, {3, 0}, {4, 1}};

    for (auto *const tag : tags) {
        QVERIFY(tag);
        QVERIFY(tag->exists);
        const auto tagId = tag->getKeyCasted();
        QVERIFY(activeMap.contains(tagId));
        QCOMPARE(typeid (tag), typeid (Tag *));

        /* Custom Pivot relation as the Tagged class, under the 'tagged' key
           in the m_relations hash. */
        QVERIFY(tag->relationLoaded("tagged"));
        auto *tagged = tag->getRelation<Tagged, One>("tagged");
        QVERIFY(tagged);
        QVERIFY(tagged->exists);
        QCOMPARE(typeid (tagged), typeid (Tagged *));

        QVERIFY(tagged->usesTimestamps());
        QVERIFY(!tagged->getIncrementing());

        QCOMPARE(tagged->getForeignKey(), QString("torrent_id"));
        QCOMPARE(tagged->getRelatedKey(), QString("tag_id"));

        const auto &attributesHash = tagged->getAttributesHash();

        QCOMPARE(attributesHash.size(), static_cast<std::size_t>(5));

        QCOMPARE(tagged->getAttribute("torrent_id"), torrentId);
        // With pivot attributes, active
        QCOMPARE(tagged->getAttribute<int>("active"), activeMap.at(tagId));
        QVERIFY(attributesHash.contains(CREATED_AT));
        QVERIFY(attributesHash.contains(UPDATED_AT));
    }
}

void tst_Model_Return_Relation::getRelationValue_LazyLoad_HasOne() const
{
    auto torrent = Torrent_ReturnRelation::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    QCOMPARE(torrent->getKey(), QVariant(2));

    // TorrentPeer has-one relation
    QVERIFY(!torrent->relationLoaded("torrentPeer"));
    auto *torrentPeer = torrent->getRelationValue<TorrentPeer, One>("torrentPeer");
    QVERIFY(torrent->relationLoaded("torrentPeer"));
    QVERIFY(torrentPeer);
    QVERIFY(torrentPeer->exists);
    QCOMPARE(torrentPeer->getKey(), QVariant(2));
    QCOMPARE(torrentPeer->getAttribute("torrent_id"), QVariant(2));
    QCOMPARE(typeid (torrentPeer), typeid (TorrentPeer *));
}

void tst_Model_Return_Relation::getRelationValue_LazyLoad_HasMany() const
{
    auto torrent = Torrent_ReturnRelation::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    auto torrentId = torrent->getKey();
    QCOMPARE(torrentId, QVariant(2));

    // TorrentPreviewableFile has-many relation
    QVERIFY(!torrent->relationLoaded("torrentFiles"));
    auto files = torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    QVERIFY(torrent->relationLoaded("torrentFiles"));
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};

    for (auto *const file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrentId);
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));
    }
}

void tst_Model_Return_Relation::getRelationValue_LazyLoad_BelongsTo() const
{
    auto torrent = Torrent_ReturnRelation::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    QCOMPARE(torrent->getKey(), QVariant(2));
    QCOMPARE(torrent->getAttribute("user_id"), QVariant(1));

    // User belongs-to relation
    QVERIFY(!torrent->relationLoaded("user"));
    auto *user = torrent->getRelationValue<User, One>("user");
    QVERIFY(torrent->relationLoaded("user"));
    QVERIFY(user);
    QVERIFY(user->exists);
    QCOMPARE(user->getKey(), QVariant(1));
    QCOMPARE(typeid (user), typeid (User *));
}

void tst_Model_Return_Relation::
     getRelationValue_LazyLoad_BelongsToMany_BasicPivot_WithPivotAttributes() const
{
    auto torrent = Torrent_ReturnRelation::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    const auto torrentId = torrent->getKey();
    QCOMPARE(torrentId, QVariant(2));

    // Tag belongs-to-many relation (basic pivot)
    QVERIFY(!torrent->relationLoaded("tags"));
    auto tags = torrent->getRelationValue<Tag_ReturnRelation>("tags");
    QVERIFY(torrent->relationLoaded("tags"));
    QCOMPARE(tags.size(), 4);
    QCOMPARE(typeid (tags), typeid (ModelsCollection<Tag_ReturnRelation *>));

    // Expected tag IDs and pivot attribute 'active', maps tagId to active
    std::unordered_map<quint64, int> activeMap {{1, 1}, {2, 1}, {3, 0}, {4, 1}};

    for (auto *const tag : tags) {
        QVERIFY(tag);
        QVERIFY(tag->exists);
        const auto tagId = tag->getKeyCasted();
        QVERIFY(activeMap.contains(tagId));
        QCOMPARE(typeid (tag), typeid (Tag_ReturnRelation *));

        /* Custom Pivot relation as the Tagged class, under the 'tagged' key
           in the m_relations hash. */
        QVERIFY(tag->relationLoaded("pivot"));
        auto *pivot = tag->getRelation<Pivot, One>("pivot");
        QVERIFY(pivot);
        QVERIFY(pivot->exists);
        QCOMPARE(typeid (pivot), typeid (Pivot *));

        QVERIFY(pivot->usesTimestamps());
        QVERIFY(!pivot->getIncrementing());

        QCOMPARE(pivot->getForeignKey(), QString("torrent_id"));
        QCOMPARE(pivot->getRelatedKey(), QString("tag_id"));

        const auto &attributesHash = pivot->getAttributesHash();

        QCOMPARE(attributesHash.size(), static_cast<std::size_t>(5));

        QCOMPARE(pivot->getAttribute("torrent_id"), torrentId);
        // With pivot attributes, active
        QCOMPARE(pivot->getAttribute<int>("active"), activeMap.at(tagId));
        QVERIFY(attributesHash.contains(CREATED_AT));
        QVERIFY(attributesHash.contains(UPDATED_AT));
    }
}

void tst_Model_Return_Relation::
     getRelationValue_LazyLoad_BelongsToMany_CustomPivot_WithPivotAttributes() const
{
    auto torrent = Torrent_ReturnRelation::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    const auto torrentId = torrent->getKey();
    QCOMPARE(torrentId, QVariant(2));

    // Tag belongs-to-many relation (custom Tagged pivot)
    QVERIFY(!torrent->relationLoaded("tagsCustom"));
    auto tags = torrent->getRelationValue<Tag>("tagsCustom");
    QVERIFY(torrent->relationLoaded("tagsCustom"));
    QCOMPARE(tags.size(), 4);
    QCOMPARE(typeid (tags), typeid (ModelsCollection<Tag *>));

    // Expected tag IDs and pivot attribute 'active', maps tagId to active
    std::unordered_map<quint64, int> activeMap {{1, 1}, {2, 1}, {3, 0}, {4, 1}};

    for (auto *const tag : tags) {
        QVERIFY(tag);
        QVERIFY(tag->exists);
        const auto tagId = tag->getKeyCasted();
        QVERIFY(activeMap.contains(tagId));
        QCOMPARE(typeid (tag), typeid (Tag *));

        /* Custom Pivot relation as the Tagged class, under the 'tagged' key
           in the m_relations hash. */
        QVERIFY(tag->relationLoaded("tagged"));
        auto *tagged = tag->getRelation<Tagged, One>("tagged");
        QVERIFY(tagged);
        QVERIFY(tagged->exists);
        QCOMPARE(typeid (tagged), typeid (Tagged *));

        QVERIFY(tagged->usesTimestamps());
        QVERIFY(!tagged->getIncrementing());

        QCOMPARE(tagged->getForeignKey(), QString("torrent_id"));
        QCOMPARE(tagged->getRelatedKey(), QString("tag_id"));

        const auto &attributesHash = tagged->getAttributesHash();

        QCOMPARE(attributesHash.size(), static_cast<std::size_t>(5));

        QCOMPARE(tagged->getAttribute("torrent_id"), torrentId);
        // With pivot attributes, active
        QCOMPARE(tagged->getAttribute<int>("active"), activeMap.at(tagId));
        QVERIFY(attributesHash.contains(CREATED_AT));
        QVERIFY(attributesHash.contains(UPDATED_AT));
    }
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_Model_Return_Relation)

#include "tst_model_return_relation.moc"
