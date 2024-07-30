#include <QCoreApplication>
#include <QTest>

#ifdef TINYDRIVERS_MYSQL_DRIVER
#  include "orm/drivers/mysql/version.hpp"
#endif

#include "orm/db.hpp"
#include "orm/utils/query.hpp"

#include "databases.hpp"
#include "macros.hpp"

#include "models/torrent.hpp"
#include "models/torrenteager.hpp"
#include "models/torrenteager_failed.hpp"
#include "models/torrentpeer.hpp"
#include "models/torrentpeereager.hpp"
#include "models/torrentpeereager_norelations.hpp"
#include "models/torrentpreviewablefileeager_withdefault.hpp"

using Orm::Constants::ASTERISK;
using Orm::Constants::CREATED_AT;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::Progress;
using Orm::Constants::QMYSQL;
using Orm::Constants::QPSQL;
using Orm::Constants::QSQLITE;
using Orm::Constants::SIZE_;
using Orm::Constants::UPDATED_AT;

using Orm::DB;
using Orm::Exceptions::RuntimeError;
using Orm::One;
using Orm::QtTimeZoneConfig;
using Orm::QtTimeZoneType;
using Orm::TTimeZone;

using QueryUtils = Orm::Utils::Query;
using TypeUtils = Orm::Utils::Type;

using Orm::Tiny::AttributeItem;
using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::Exceptions::RelationMappingNotFoundError;
using Orm::Tiny::Exceptions::RelationNotLoadedError;
using Orm::Tiny::Relations::Pivot;
using Orm::Tiny::Types::ModelsCollection;

using TestUtils::Databases;

using Models::Role;
using Models::Tag;
using Models::TagProperty;
using Models::Tagged;
using Models::Torrent;
using Models::TorrentEager;
using Models::TorrentEager_Failed;
using Models::TorrentEager_WithDefault;
using Models::TorrentPeer;
using Models::TorrentPeerEager;
using Models::TorrentPeerEager_NoRelations;
using Models::TorrentPreviewableFile;
using Models::TorrentPreviewableFileEager;
using Models::TorrentPreviewableFileEager_WithDefault;
using Models::TorrentPreviewableFileProperty;
using Models::TorrentPreviewableFilePropertyEager;

class tst_Model_Relations : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase_data() const;

    void getRelation_EagerLoad_ManyAndOne() const;
    void getRelation_EagerLoad_BelongsTo() const;
    void getRelationValue_EagerLoad_BelongsToMany_BasicPivot_WithPivotAttributes() const;
    void getRelationValue_EagerLoad_BelongsToMany_CustomPivot_WithPivotAttributes() const;
    void
    getRelationValue_EagerLoad_BelongsToMany_BasicPivot_WithoutPivotAttributes() const;
    void getRelation_EagerLoad_Failed() const;
    void eagerLoad_Failed() const;

    void getRelationValue_LazyLoad_ManyAndOne() const;
    void getRelationValue_LazyLoad_BelongsTo() const;
    void getRelationValue_LazyLoad_BelongsToMany_BasicPivot_WithPivotAttributes() const;
    void getRelationValue_LazyLoad_BelongsToMany_CustomPivot_WithPivotAttributes() const;
    void
    getRelationValue_LazyLoad_BelongsToMany_BasicPivot_WithoutPivotAttributes() const;
    void getRelationValue_LazyLoad_Failed() const;

    void u_with_Empty() const;
    void with_HasOne() const;
    void with_HasMany() const;
    void with_BelongsTo() const;
    void with_BelongsToMany() const;
    void with_BelongsToMany_Twice() const;
    void with_Vector_MoreRelations() const;
    void with_NonExistentRelation_Failed() const;

    void with_WithSelectConstraint() const;
    void with_WithSelectConstraint_WithWhitespaces() const;
    void with_WithSelectConstraint_BelongsToMany() const;
    void with_WithLambdaConstraint() const;
    void with_WithLambdaConstraint_BelongsToMany() const;

    void with_NestedRelations() const;
    void with_NestedRelations_WithSelectConstraint_HasOne() const;
    void with_NestedRelations_WithSelectConstraint_HasMany() const;
    void with_NestedRelations_WithSelectConstraint_BelongsToMany_HasOne() const;
    void with_NestedRelations_WithSelectConstraint_BelongsToMany_Twice() const;

    void without() const;
    void without_NestedRelations() const;
    void without_Vector_MoreRelations() const;

    void withOnly() const;

    void load_QList_WithItem() const;
    void load_QList_QString_lvalue() const;
    void load_QList_QString_rvalue() const;
    void load_QString() const;

    void load_WithSelectConstraint() const;
    void load_WithLambdaConstraint() const;
    void load_NonExistentRelation_Failed() const;

    void fresh() const;
    void fresh_WithSelectConstraint() const;

    void refresh_EagerLoad_OnlyRelations() const;
    void refresh_LazyLoad_OnlyRelations() const;

    void push_EagerLoad() const;
    void push_LazyLoad() const;
    // TEST test all return paths for push() silverqx

    void where_WithCallback() const;
    void orWhere_WithCallback() const;

    void belongsToMany_allRelatedIds() const;

    /* Default Models */
    void withoutDefaultModel_LazyLoad_HasOne() const;
    void withoutDefaultModel_LazyLoad_BelongsTo() const;
    void withoutDefaultModel_EagerLoad_HasOne() const;
    void withoutDefaultModel_EagerLoad_BelongsTo() const;

    void withDefaultModel_LazyLoad_Bool_HasOne() const;
    void withDefaultModel_LazyLoad_AttributesVector_HasOne() const;
    void withDefaultModel_LazyLoad_Bool_BelongsTo() const;
    void withDefaultModel_LazyLoad_AttributesVector_BelongsTo() const;

    void withDefaultModel_EagerLoad_Bool_HasOne() const;
    void withDefaultModel_EagerLoad_AttributesVector_HasOne() const;
    void withDefaultModel_EagerLoad_Bool_BelongsTo() const;
    void withDefaultModel_EagerLoad_AttributesVector_BelongsTo() const;

    /* Relation related */
    /* BelongsToMany related */
    void find() const;

    void findOr() const;
    void findOr_WithReturnType() const;

    void first() const;

    void firstOr() const;
    void firstOr_WithReturnType() const;

    void whereRowValues() const;

    void upsert() const;

    /* Casting Attributes */
    void withCasts_OnRelation_OneToMany() const;
    void withCasts_OnRelation_ManyToMany() const;

    void u_casts_OnCustomPivotModel_ManyToMany() const;

    /* QDateTime with/without timezone */
    /* Server timezone UTC */
    void timezone_TimestampAttribute_UtcOnServer_OnCustomPivotModel_ManyToMany() const;

    /* QtTimeZoneType::DontConvert */
    /* Server timezone UTC */
    void timezone_TimestampAttribute_UtcOnServer_DontConvert_OnCustomPivot_MtM() const;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Model_Relations::initTestCase_data() const
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

void tst_Model_Relations::getRelation_EagerLoad_ManyAndOne() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = TorrentEager::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // TorrentPreviewableFileEager has-many relation
    auto files = torrent->getRelation<TorrentPreviewableFileEager>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFileEager *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};
    // Expected file property IDs
    QList<QVariant> filePropertyIds {1, 2};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFileEager *));

        /* TorrentPreviewableFilePropertyEager has-one relation, loaded by
           dot notation in the u_with data member. */
        auto *fileProperty =
                file->getRelation<TorrentPreviewableFilePropertyEager, One>(
                    "fileProperty");
        QVERIFY(fileProperty);
        QVERIFY(fileProperty->exists);
        QCOMPARE(typeid (fileProperty), typeid (TorrentPreviewableFilePropertyEager *));
        QVERIFY(filePropertyIds.contains(fileProperty->getKey()));
        QCOMPARE(fileProperty->getAttribute("previewable_file_id"), file->getKey());
    }
}

void tst_Model_Relations::getRelation_EagerLoad_BelongsTo() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentPeer = TorrentPeerEager::find(2);
    QVERIFY(torrentPeer);
    QVERIFY(torrentPeer->exists);
    QCOMPARE(torrentPeer->getAttribute("torrent_id"), QVariant(2));

    // TorrentEager belongs to relation
    auto *torrent = torrentPeer->getRelation<TorrentEager, One>("torrent");
    QVERIFY(torrent);
    QCOMPARE(torrent->getKey(), torrentPeer->getAttribute("torrent_id"));
    QCOMPARE(typeid (torrent), typeid (TorrentEager *));
}

void tst_Model_Relations::
     getRelationValue_EagerLoad_BelongsToMany_BasicPivot_WithPivotAttributes() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto tag = Tag::with("torrents")->find(2);
    QVERIFY(tag);
    QVERIFY(tag->exists);

    // Torrent belongs-to-many relation (basic pivot)
    auto torrents = tag->getRelation<Torrent>("torrents");
    QCOMPARE(torrents.size(), 4);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent *>));

    // Expected torrent IDs
    QList<QVariant> torrentIds {2, 3, 4, 7};

    for (auto *torrent : torrents) {
        QVERIFY(torrent);
        QVERIFY(torrent->exists);
        QVERIFY(torrentIds.contains(torrent->getKey()));
        QCOMPARE(typeid (torrent), typeid (Torrent *));

        // Pivot relation
        auto *pivot = torrent->getRelation<Pivot, One>("pivot");
        QVERIFY(pivot);
        QVERIFY(pivot->exists);
        QCOMPARE(typeid (pivot), typeid (Pivot *));

        QVERIFY(pivot->usesTimestamps());
        QVERIFY(!pivot->getIncrementing());

        QCOMPARE(pivot->getForeignKey(), QString("tag_id"));
        QCOMPARE(pivot->getRelatedKey(), QString("torrent_id"));

        const auto &attributesHash = pivot->getAttributesHash();

        QCOMPARE(attributesHash.size(), static_cast<std::size_t>(5));

        QCOMPARE(pivot->getAttribute("tag_id"), (*tag)[ID]);
        // With pivot attributes, active
        QCOMPARE(pivot->getAttribute("active"), QVariant(1));
        QVERIFY(attributesHash.contains(CREATED_AT));
        QVERIFY(attributesHash.contains(UPDATED_AT));
    }
}

void tst_Model_Relations::
     getRelationValue_EagerLoad_BelongsToMany_CustomPivot_WithPivotAttributes() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with("tags")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Tag belongs-to-many relation (custom Tagged pivot)
    auto tags = torrent->getRelation<Tag>("tags");
    QCOMPARE(tags.size(), 4);
    QCOMPARE(typeid (tags), typeid (ModelsCollection<Tag *>));

    // Expected tag IDs and pivot attribute 'active', maps tagId to active
    std::unordered_map<quint64, int> activeMap {{1, 1}, {2, 1}, {3, 0}, {4, 1}};

    for (auto *tag : tags) {
        QVERIFY(tag);
        QVERIFY(tag->exists);
        const auto tagId = tag->getKeyCasted();
        QVERIFY(activeMap.contains(tagId));
        QCOMPARE(typeid (tag), typeid (Tag *));

        /* Custom Pivot relation as the Tagged class, under the 'tagged' key
           in the m_relations hash. */
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

        QCOMPARE(tagged->getAttribute("torrent_id"), (*torrent)[ID]);
        // With pivot attributes, active
        QCOMPARE(tagged->getAttribute("active"), QVariant(activeMap.at(tagId)));
        QVERIFY(attributesHash.contains(CREATED_AT));
        QVERIFY(attributesHash.contains(UPDATED_AT));
    }
}

void tst_Model_Relations::
     getRelationValue_EagerLoad_BelongsToMany_BasicPivot_WithoutPivotAttributes() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto tag = Tag::with("torrents_WithoutPivotAttributes")->find(2);
    QVERIFY(tag);
    QVERIFY(tag->exists);

    // Torrent belongs-to-many relation (basic pivot)
    auto torrents = tag->getRelation<Torrent>("torrents_WithoutPivotAttributes");
    QCOMPARE(torrents.size(), 4);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent *>));

    // Expected torrent IDs
    QList<QVariant> torrentIds {2, 3, 4, 7};

    for (auto *torrent : torrents) {
        QVERIFY(torrent);
        QVERIFY(torrent->exists);
        QVERIFY(torrentIds.contains(torrent->getKey()));
        QCOMPARE(typeid (torrent), typeid (Torrent *));

        // Pivot relation
        auto *pivot = torrent->getRelation<Pivot, One>("pivot");
        QVERIFY(pivot);
        QVERIFY(pivot->exists);
        QCOMPARE(typeid (pivot), typeid (Pivot *));

        QVERIFY(!pivot->usesTimestamps());
        QVERIFY(!pivot->getIncrementing());

        QCOMPARE(pivot->getForeignKey(), QString("tag_id"));
        QCOMPARE(pivot->getRelatedKey(), QString("torrent_id"));

        const auto &attributesHash = pivot->getAttributesHash();

        QCOMPARE(attributesHash.size(), static_cast<std::size_t>(2));

        QCOMPARE(pivot->getAttribute("tag_id"), (*tag)[ID]);
    }
}

void tst_Model_Relations::getRelation_EagerLoad_Failed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent;

    // Many relation
    TVERIFY_THROWS_EXCEPTION(
                RelationNotLoadedError,
                (torrent.getRelation<TorrentPreviewableFile>("torrentFiles")));
    // One relation, obtained as QList, also possible
    TVERIFY_THROWS_EXCEPTION(
                RelationNotLoadedError,
                (torrent.getRelation<TorrentPeer>("torrentFiles")));
    // Many relation
    TVERIFY_THROWS_EXCEPTION(
                RelationNotLoadedError,
                (torrent.getRelation<TorrentPeer, One>("torrentFiles")));
    // BelongsTo relation
    TVERIFY_THROWS_EXCEPTION(
                RelationNotLoadedError,
                (TorrentPeer().getRelation<Torrent, One>("torrent")));
    // BelongsToMany relation
    TVERIFY_THROWS_EXCEPTION(
                RelationNotLoadedError,
                (torrent.getRelation<Tag>("tags")));
}

void tst_Model_Relations::eagerLoad_Failed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    TVERIFY_THROWS_EXCEPTION(RelationMappingNotFoundError,
                             TorrentEager_Failed::find(1));
}

void tst_Model_Relations::getRelationValue_LazyLoad_ManyAndOne() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // TorrentPreviewableFile has-many relation
    auto files = torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};
    // Expected file property IDs
    QList<QVariant> filePropertyIds {1, 2};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));

        /* TorrentPreviewableFileProperty has-one relation, loaded by
           dot notation in the u_with data member. */
        auto *fileProperty =
                file->getRelationValue<TorrentPreviewableFileProperty, One>(
                    "fileProperty");
        QVERIFY(fileProperty);
        QVERIFY(fileProperty->exists);
        QCOMPARE(typeid (fileProperty), typeid (TorrentPreviewableFileProperty *));
        QVERIFY(filePropertyIds.contains(fileProperty->getKey()));
        QCOMPARE(fileProperty->getAttribute("previewable_file_id"), file->getKey());
    }
}

void tst_Model_Relations::getRelationValue_LazyLoad_BelongsTo() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentPeer = TorrentPeer::find(2);
    QVERIFY(torrentPeer);
    QVERIFY(torrentPeer->exists);
    QCOMPARE(torrentPeer->getAttribute("torrent_id"), QVariant(2));

    // Torrent belongs to relation
    auto *torrent = torrentPeer->getRelationValue<Torrent, One>("torrent");
    QVERIFY(torrent);
    QCOMPARE(torrent->getKey(), QVariant(2));
    QCOMPARE(typeid (torrent), typeid (Torrent *));
}

void tst_Model_Relations::
     getRelationValue_LazyLoad_BelongsToMany_BasicPivot_WithPivotAttributes() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto tag = Tag::find(2);
    QVERIFY(tag);
    QVERIFY(tag->exists);

    // Torrent belongs-to-many relation (basic pivot)
    auto torrents = tag->getRelationValue<Torrent>("torrents");
    QCOMPARE(torrents.size(), 4);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent *>));

    // Expected torrent IDs
    QList<QVariant> torrentIds {2, 3, 4, 7};

    for (auto *torrent : torrents) {
        QVERIFY(torrent);
        QVERIFY(torrent->exists);
        QVERIFY(torrentIds.contains(torrent->getKey()));
        QCOMPARE(typeid (torrent), typeid (Torrent *));

        // Pivot relation
        auto *pivot = torrent->getRelation<Pivot, One>("pivot");
        QVERIFY(pivot);
        QVERIFY(pivot->exists);
        QCOMPARE(typeid (pivot), typeid (Pivot *));

        QVERIFY(pivot->usesTimestamps());
        QVERIFY(!pivot->getIncrementing());

        QCOMPARE(pivot->getForeignKey(), QString("tag_id"));
        QCOMPARE(pivot->getRelatedKey(), QString("torrent_id"));

        const auto &attributesHash = pivot->getAttributesHash();

        QCOMPARE(attributesHash.size(), static_cast<std::size_t>(5));

        QCOMPARE(pivot->getAttribute("tag_id"), (*tag)[ID]);
        // With pivot attributes, active
        QCOMPARE(pivot->getAttribute("active"), QVariant(1));
        QVERIFY(attributesHash.contains(CREATED_AT));
        QVERIFY(attributesHash.contains(UPDATED_AT));
    }
}

void tst_Model_Relations::
     getRelationValue_LazyLoad_BelongsToMany_CustomPivot_WithPivotAttributes() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Tag belongs-to-many relation (custom Tagged pivot)
    auto tags = torrent->getRelationValue<Tag>("tags");
    QCOMPARE(tags.size(), 4);
    QCOMPARE(typeid (tags), typeid (ModelsCollection<Tag *>));

    // Expected tag IDs and pivot attribute 'active', maps tagId to active
    std::unordered_map<quint64, int> activeMap {{1, 1}, {2, 1}, {3, 0}, {4, 1}};

    for (auto *tag : tags) {
        QVERIFY(tag);
        QVERIFY(tag->exists);
        const auto tagId = tag->getKeyCasted();
        QVERIFY(activeMap.contains(tagId));
        QCOMPARE(typeid (tag), typeid (Tag *));

        /* Custom Pivot relation as the Tagged class, under the 'tagged' key
           in the m_relations hash. */
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

        QCOMPARE(tagged->getAttribute("torrent_id"), (*torrent)[ID]);
        // With pivot attributes, active
        QCOMPARE(tagged->getAttribute("active"), QVariant(activeMap.at(tagId)));
        QVERIFY(attributesHash.contains(CREATED_AT));
        QVERIFY(attributesHash.contains(UPDATED_AT));
    }
}

void tst_Model_Relations::
     getRelationValue_LazyLoad_BelongsToMany_BasicPivot_WithoutPivotAttributes() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto tag = Tag::find(2);
    QVERIFY(tag);
    QVERIFY(tag->exists);

    // Torrent belongs-to-many relation (basic pivot)
    auto torrents = tag->getRelationValue<Torrent>("torrents_WithoutPivotAttributes");
    QCOMPARE(torrents.size(), 4);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent *>));

    // Expected torrent IDs
    QList<QVariant> torrentIds {2, 3, 4, 7};

    for (auto *torrent : torrents) {
        QVERIFY(torrent);
        QVERIFY(torrent->exists);
        QVERIFY(torrentIds.contains(torrent->getKey()));
        QCOMPARE(typeid (torrent), typeid (Torrent *));

        // Pivot relation
        auto *pivot = torrent->getRelation<Pivot, One>("pivot");
        QVERIFY(pivot);
        QVERIFY(pivot->exists);
        QCOMPARE(typeid (pivot), typeid (Pivot *));

        QVERIFY(!pivot->usesTimestamps());
        QVERIFY(!pivot->getIncrementing());

        QCOMPARE(pivot->getForeignKey(), QString("tag_id"));
        QCOMPARE(pivot->getRelatedKey(), QString("torrent_id"));

        const auto &attributesHash = pivot->getAttributesHash();

        QCOMPARE(attributesHash.size(), static_cast<std::size_t>(2));

        QCOMPARE(pivot->getAttribute("tag_id"), (*tag)[ID]);
    }
}

void tst_Model_Relations::getRelationValue_LazyLoad_Failed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Many relation
    QCOMPARE((Torrent().getRelationValue<TorrentPreviewableFile>("notExists")),
             ModelsCollection<TorrentPreviewableFile *>());
    // One relation
    QCOMPARE((Torrent().getRelationValue<TorrentPeer, One>("notExists")),
             nullptr);
    // One relation, obtained as QList, also possible
    QCOMPARE((Torrent().getRelationValue<TorrentPeer>("notExists")),
             ModelsCollection<TorrentPeer *>());
    // Just to be sure try BelongsToMany relation
    QCOMPARE((Torrent().getRelationValue<Tag>("notExists")),
             ModelsCollection<Tag *>());
}

void tst_Model_Relations::u_with_Empty() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent;

    QVERIFY(torrent.getRelations().empty());
}

void tst_Model_Relations::with_HasOne() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with("torrentPeer")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto *peer = torrent->getRelation<TorrentPeer, One>("torrentPeer");
    QVERIFY(peer);
    QVERIFY(peer->exists);
    QCOMPARE(peer->getKey(), QVariant(2));
    QCOMPARE(peer->getAttribute("torrent_id"), torrent->getKey());
    QCOMPARE(typeid (peer), typeid (TorrentPeer *));
}

void tst_Model_Relations::with_HasMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with("torrentFiles")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));
    }
}

void tst_Model_Relations::with_BelongsTo() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto fileProperty = TorrentPreviewableFileProperty::with("torrentFile")->find(2);
    QVERIFY(fileProperty);
    QVERIFY(fileProperty->exists);
    QCOMPARE(fileProperty->getAttribute("previewable_file_id"), QVariant(3));

    auto *file = fileProperty->getRelation<TorrentPreviewableFile, One>("torrentFile");
    QVERIFY(file);
    QVERIFY(file->exists);
    QCOMPARE(file->getKey(), QVariant(3));
    QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));
}

void tst_Model_Relations::with_BelongsToMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with("tags")->find(3);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto tags = torrent->getRelation<Tag>("tags");
    QCOMPARE(tags.size(), 2);
    QCOMPARE(typeid (tags), typeid (ModelsCollection<Tag *>));

    // Expected tag IDs
    QList<QVariant> tagIds {2, 4};
    for (auto *tag : tags) {
        QVERIFY(tag);
        QVERIFY(tag->exists);
        QVERIFY(tagIds.contains(tag->getKey()));
        QCOMPARE(typeid (tag), typeid (Tag *));

        const auto &relations = tag->getRelations();
        QCOMPARE(relations.size(), static_cast<std::size_t>(2));
        QVERIFY(relations.contains("tagged"));
        QVERIFY(relations.contains("tagProperty"));

        // Custom pivot relation - Tagged
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

        QCOMPARE(tagged->getAttribute("torrent_id"), (*torrent)[ID]);
        // With pivot attributes, active
        QCOMPARE(tagged->getAttribute("active"), QVariant(1));
        QVERIFY(attributesHash.contains(CREATED_AT));
        QVERIFY(attributesHash.contains(UPDATED_AT));
    }
}

void tst_Model_Relations::with_BelongsToMany_Twice() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with("tags.roles")->find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto tags = torrent->getRelation<Tag>("tags");
    QCOMPARE(tags.size(), 1);
    QCOMPARE(typeid (tags), typeid (ModelsCollection<Tag *>));

    // Expected tag IDs
    std::unordered_set<Tag::KeyType> tagIds {2};
    // Expected role attribute names and positions hash
    std::unordered_map<QString, QList<AttributeItem>::size_type>
    expectedRoleAttributes {
        {ID,         0},
        {NAME,       1},
        {"added_on", 2},
    };
    // Expected role attribute values - ID and name
    std::map<Role::KeyType, QVariant> expectedRoles {
        {1, "role one"},
        {3, "role three"},
    };
    const auto torrentId = torrent->getKeyCasted();

    for (auto *tag : tags) {
        QVERIFY(tag);
        QVERIFY(tag->exists);
        const auto tagId = tag->getKeyCasted();
        QVERIFY(tagIds.contains(tagId));
        QCOMPARE(typeid (tag), typeid (Tag *));

        {
            const auto &relations = tag->getRelations();
            QCOMPARE(relations.size(), static_cast<std::size_t>(3));
            QVERIFY(relations.contains("tagged"));
            QVERIFY(relations.contains("tagProperty"));
            QVERIFY(relations.contains("roles"));
        }

        // Custom pivot relation - Tagged
        {
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
            QCOMPARE(tagged->getAttribute("active"), true);
            QVERIFY(attributesHash.contains(CREATED_AT));
            QVERIFY(attributesHash.contains(UPDATED_AT));
        }

        // Roles belongs-to-many relation (basic pivot)
        {
            auto roles = tag->getRelation<Role>("roles");
            QCOMPARE(roles.size(), 2);
            QCOMPARE(typeid (roles), typeid (ModelsCollection<Role *>));
            // Attribute names and positions hash
            QCOMPARE(roles.at(0)->getAttributesHash(), expectedRoleAttributes);
            // Attribute values
            auto actualRoles = roles.pluck<Role::KeyType>(NAME, ID);
            QCOMPARE(actualRoles, expectedRoles);

            // Pivot relation
            for (auto *role : roles) {
                const auto &relations = role->getRelations();
                QCOMPARE(relations.size(), static_cast<std::size_t>(1));
                QVERIFY(relations.contains("acl"));

                auto *pivot = role->getRelation<Pivot, One>("acl");
                QVERIFY(pivot);
                QVERIFY(pivot->exists);
                QCOMPARE(typeid (pivot), typeid (Pivot *));

                QCOMPARE(pivot->getForeignKey(), QString("tag_id"));
                QCOMPARE(pivot->getRelatedKey(), QString("role_id"));

                const auto &attributesHash = pivot->getAttributesHash();

                QCOMPARE(attributesHash.size(), static_cast<std::size_t>(3));

                QCOMPARE(pivot->getAttribute("tag_id"), tagId);
                QVERIFY(expectedRoles.contains(
                            pivot->getAttribute<Tag::KeyType>("role_id")));
                // With pivot attributes, active
                QVERIFY(attributesHash.contains("active"));
            }
        }
    }
}

void tst_Model_Relations::with_Vector_MoreRelations() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with({"torrentFiles", "torrentPeer"})->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // TorrentPeer has-one relation
    auto *peer = torrent->getRelation<TorrentPeer, One>("torrentPeer");
    QVERIFY(peer);
    QVERIFY(peer->exists);
    QCOMPARE(peer->getKey(), QVariant(2));
    QCOMPARE(peer->getAttribute("torrent_id"), torrent->getKey());

    // TorrentPreviewableFile has-many relation
    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));

        // No TorrentPreviewableFileProperty loaded
        TVERIFY_THROWS_EXCEPTION(
                    RuntimeError,
                    (file->getRelation<TorrentPreviewableFileProperty, One>(
                         "fileProperty")));
    }
}

void tst_Model_Relations::with_NonExistentRelation_Failed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    TVERIFY_THROWS_EXCEPTION(RelationMappingNotFoundError,
                             Torrent::with("torrentFiles-NON_EXISTENT")->find(1));
}

void tst_Model_Relations::with_WithSelectConstraint() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with({"torrentFiles:id,torrent_id,filepath"})->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);

        // Check whether constraints were correctly applied
        const auto &attributes = file->getAttributes();
        QCOMPARE(attributes.size(), 3);

        std::unordered_set<QString> expectedAttributes {ID, "torrent_id", "filepath"};
        for (const auto &attribute : attributes)
            QVERIFY(expectedAttributes.contains(attribute.key));

        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));
    }
}

void tst_Model_Relations::with_WithSelectConstraint_WithWhitespaces() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with({"  torrentFiles  :  id  ,  torrent_id ,filepath  "})
                   ->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);

        // Check whether constraints were correctly applied
        const auto &attributes = file->getAttributes();
        QCOMPARE(attributes.size(), 3);

        std::unordered_set<QString> expectedAttributes {ID, "torrent_id", "filepath"};
        for (const auto &attribute : attributes)
            QVERIFY(expectedAttributes.contains(attribute.key));

        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));
    }
}

void tst_Model_Relations::with_WithSelectConstraint_BelongsToMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with({"tags:id,name"})->find(3);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto tags = torrent->getRelation<Tag>("tags");
    QCOMPARE(tags.size(), 2);
    QCOMPARE(typeid (tags), typeid (ModelsCollection<Tag *>));

    // Expected tag IDs
    QList<QVariant> tagIds {2, 4};
    for (auto *tag : tags) {
        QVERIFY(tag);
        QVERIFY(tag->exists);

        // Check whether constraints were correctly applied
        const auto &attributes = tag->getAttributes();
        QCOMPARE(attributes.size(), 2);

        std::unordered_set<QString> expectedAttributes {ID, NAME};
        for (const auto &attribute : attributes)
            QVERIFY(expectedAttributes.contains(attribute.key));

        QVERIFY(tagIds.contains(tag->getKey()));
        QCOMPARE(typeid (tag), typeid (Tag *));

        const auto &relations = tag->getRelations();
        QCOMPARE(relations.size(), static_cast<std::size_t>(2));
        QVERIFY(relations.contains("tagged"));
        QVERIFY(relations.contains("tagProperty"));

        // Custom pivot relation - Tagged
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

        QCOMPARE(tagged->getAttribute("torrent_id"), (*torrent)[ID]);
        // With pivot attributes, active
        QCOMPARE(tagged->getAttribute("active"), QVariant(1));
        QVERIFY(attributesHash.contains(CREATED_AT));
        QVERIFY(attributesHash.contains(UPDATED_AT));
    }
}

void tst_Model_Relations::with_WithLambdaConstraint() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with({{"torrentFiles", [](auto &query)
                                   {
                                       query.select({ID, "torrent_id", "filepath"});
                                   }}})->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);

        // Check whether constraints were correctly applied
        const auto &attributes = file->getAttributes();
        QCOMPARE(attributes.size(), 3);

        std::unordered_set<QString> expectedAttributes {ID, "torrent_id", "filepath"};
        for (const auto &attribute : attributes)
            QVERIFY(expectedAttributes.contains(attribute.key));

        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));
    }
}

void tst_Model_Relations::with_WithLambdaConstraint_BelongsToMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with({{"tags", [](auto &query)
                                   {
                                       query.select({ID, NAME});
                                   }}})->find(3);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto tags = torrent->getRelation<Tag>("tags");
    QCOMPARE(tags.size(), 2);
    QCOMPARE(typeid (tags), typeid (ModelsCollection<Tag *>));

    // Expected tag IDs
    QList<QVariant> tagIds {2, 4};
    for (auto *tag : tags) {
        QVERIFY(tag);
        QVERIFY(tag->exists);

        // Check whether constraints were correctly applied
        const auto &attributes = tag->getAttributes();
        QCOMPARE(attributes.size(), 2);

        std::unordered_set<QString> expectedAttributes {ID, NAME};
        for (const auto &attribute : attributes)
            QVERIFY(expectedAttributes.contains(attribute.key));

        QVERIFY(tagIds.contains(tag->getKey()));
        QCOMPARE(typeid (tag), typeid (Tag *));

        const auto &relations = tag->getRelations();
        QCOMPARE(relations.size(), static_cast<std::size_t>(2));
        QVERIFY(relations.contains("tagged"));
        QVERIFY(relations.contains("tagProperty"));

        // Custom pivot relation - Tagged
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

        QCOMPARE(tagged->getAttribute("torrent_id"), (*torrent)[ID]);
        // With pivot attributes, active
        QCOMPARE(tagged->getAttribute("active"), QVariant(1));
        QVERIFY(attributesHash.contains(CREATED_AT));
        QVERIFY(attributesHash.contains(UPDATED_AT));
    }
}

void tst_Model_Relations::with_NestedRelations() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with("torrentFiles.fileProperty")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // TorrentPreviewableFile has-many relation
    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};
    // Expected file property IDs
    QList<QVariant> filePropertyIds {1, 2};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));

        /* TorrentPreviewableFileProperty has-one relation, loaded by
           dot notation in the u_with data member. */
        auto *fileProperty =
                file->getRelation<TorrentPreviewableFileProperty, One>(
                    "fileProperty");
        QVERIFY(fileProperty);
        QVERIFY(fileProperty->exists);
        QCOMPARE(typeid (fileProperty), typeid (TorrentPreviewableFileProperty *));
        QVERIFY(filePropertyIds.contains(fileProperty->getKey()));
        QCOMPARE(fileProperty->getAttribute("previewable_file_id"), file->getKey());
    }
}

void tst_Model_Relations::with_NestedRelations_WithSelectConstraint_HasOne() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with("torrentFiles.fileProperty:id,previewable_file_id")
                   ->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // TorrentPreviewableFile has-many relation
    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    std::unordered_set<TorrentPreviewableFile::KeyType> fileIds {2, 3};
    // Expected file property IDs
    std::unordered_set<TorrentPreviewableFileProperty::KeyType> filePropertyIds {1, 2};
    // Expected file property attributes
    std::unordered_map<QString, QList<AttributeItem>::size_type>
    filePropertyAttributes {
        {ID,                    0},
        {"previewable_file_id", 1},
    };

    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKeyCasted()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));

        /* TorrentPreviewableFileProperty has-one relation, loaded by
           dot notation in the u_with data member. */
        auto *fileProperty =
                file->getRelation<TorrentPreviewableFileProperty, One>(
                    "fileProperty");
        QVERIFY(fileProperty);
        QVERIFY(fileProperty->exists);
        QCOMPARE(typeid (fileProperty), typeid (TorrentPreviewableFileProperty *));
        QVERIFY(filePropertyIds.contains(fileProperty->getKeyCasted()));
        QCOMPARE(fileProperty->getAttribute("previewable_file_id"), file->getKey());
        QCOMPARE(fileProperty->getAttributesHash(), filePropertyAttributes);
    }
}

void tst_Model_Relations::with_NestedRelations_WithSelectConstraint_HasMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto peer = TorrentPeer::with("torrent.torrentFiles:id,torrent_id")->find(2);
    QVERIFY(peer);
    QVERIFY(peer->exists);

    // Torrent belongs-to relation
    auto *torrent = peer->getRelation<Torrent, One>("torrent");
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    QCOMPARE(typeid (torrent), typeid (Torrent *));
    const auto torrentId = torrent->getKey();
    QCOMPARE(torrentId, peer->getAttribute("torrent_id"));
    QCOMPARE(torrent->getAttributesHash().size(), 10);

    // TorrentPreviewableFile has-many relation
    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected torrent previewable file IDs
    std::unordered_set<TorrentPreviewableFile::KeyType> fileIds {2, 3};
    // Expected torrent previewable file attributes
    std::unordered_map<QString, QList<AttributeItem>::size_type>
    fileAttributes {
        {ID,           0},
        {"torrent_id", 1},
    };

    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrentId);
        QVERIFY(fileIds.contains(file->getKeyCasted()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));
        QCOMPARE(file->getAttributesHash(), fileAttributes);
    }
}

void tst_Model_Relations::
     with_NestedRelations_WithSelectConstraint_BelongsToMany_HasOne() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with("tags.tagProperty:id,tag_id")->find(3);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // Tag belongs-to-many relation (custom Tagged pivot)
    auto tags = torrent->getRelation<Tag>("tags");
    QCOMPARE(tags.size(), 2);
    QCOMPARE(typeid (tags), typeid (ModelsCollection<Tag *>));

    // Expected tag IDs
    std::unordered_set<TorrentPreviewableFile::KeyType> tagIds {2, 4};
    // Expected tag property IDs
    std::unordered_set<TorrentPreviewableFileProperty::KeyType> tagPropertyIds {2, 4};
    // Expected tag property attributes
    std::unordered_map<QString, QList<AttributeItem>::size_type>
    tagPropertyAttributes {
        {ID,       0},
        {"tag_id", 1},
    };

    for (auto *tag : tags) {
        QVERIFY(tag);
        QVERIFY(tag->exists);
        QVERIFY(tagIds.contains(tag->getKeyCasted()));
        QCOMPARE(typeid (tag), typeid (Tag *));

        /* TagProperty has-one relation, loaded by dot notation in the u_with
           data member. */
        auto *tagProperty = tag->getRelation<TagProperty, One>("tagProperty");
        QVERIFY(tagProperty);
        QVERIFY(tagProperty->exists);
        QCOMPARE(typeid (tagProperty), typeid (TagProperty *));
        QVERIFY(tagPropertyIds.contains(tagProperty->getKeyCasted()));
        QCOMPARE(tagProperty->getAttribute("tag_id"), tag->getKey());
        QCOMPARE(tagProperty->getAttributesHash(), tagPropertyAttributes);
    }
}

void tst_Model_Relations::
     with_NestedRelations_WithSelectConstraint_BelongsToMany_Twice() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with("tags.roles:id,name")->find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto tags = torrent->getRelation<Tag>("tags");
    QCOMPARE(tags.size(), 1);
    QCOMPARE(typeid (tags), typeid (ModelsCollection<Tag *>));

    // Expected tag IDs
    std::unordered_set<Tag::KeyType> tagIds {2};
    // Expected role attribute names and positions hash
    std::unordered_map<QString, QList<AttributeItem>::size_type>
    expectedRoleAttributes {
        {ID,         0},
        {NAME,       1},
    };
    // Expected role attribute values - ID and name
    std::map<Role::KeyType, QVariant> expectedRoles {
        {1, "role one"},
        {3, "role three"},
    };
    const auto torrentId = torrent->getKeyCasted();

    for (auto *tag : tags) {
        QVERIFY(tag);
        QVERIFY(tag->exists);
        const auto tagId = tag->getKeyCasted();
        QVERIFY(tagIds.contains(tagId));
        QCOMPARE(typeid (tag), typeid (Tag *));

        {
            const auto &relations = tag->getRelations();
            QCOMPARE(relations.size(), static_cast<std::size_t>(3));
            QVERIFY(relations.contains("tagged"));
            QVERIFY(relations.contains("tagProperty"));
            QVERIFY(relations.contains("roles"));
        }

        // Custom pivot relation - Tagged
        {
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
            QCOMPARE(tagged->getAttribute("active"), true);
            QVERIFY(attributesHash.contains(CREATED_AT));
            QVERIFY(attributesHash.contains(UPDATED_AT));
        }

        // Roles belongs-to-many relation (basic pivot)
        {
            auto roles = tag->getRelation<Role>("roles");
            QCOMPARE(roles.size(), 2);
            QCOMPARE(typeid (roles), typeid (ModelsCollection<Role *>));
            // Attribute names and positions hash
            QCOMPARE(roles.at(0)->getAttributesHash(), expectedRoleAttributes);
            // Attribute values
            auto actualRoles = roles.pluck<Role::KeyType>(NAME, ID);
            QCOMPARE(actualRoles, expectedRoles);

            // Pivot relation
            for (auto *role : roles) {
                const auto &relations = role->getRelations();
                QCOMPARE(relations.size(), static_cast<std::size_t>(1));
                QVERIFY(relations.contains("acl"));

                auto *pivot = role->getRelation<Pivot, One>("acl");
                QVERIFY(pivot);
                QVERIFY(pivot->exists);
                QCOMPARE(typeid (pivot), typeid (Pivot *));

                QCOMPARE(pivot->getForeignKey(), QString("tag_id"));
                QCOMPARE(pivot->getRelatedKey(), QString("role_id"));

                const auto &attributesHash = pivot->getAttributesHash();

                QCOMPARE(attributesHash.size(), static_cast<std::size_t>(3));

                QCOMPARE(pivot->getAttribute("tag_id"), tagId);
                QVERIFY(expectedRoles.contains(
                            pivot->getAttribute<Tag::KeyType>("role_id")));
                // With pivot attributes, active
                QVERIFY(attributesHash.contains("active"));
            }
        }
    }
}

void tst_Model_Relations::without() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = TorrentEager::without("torrentPeer")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto &relations = torrent->getRelations();
    QVERIFY(!relations.contains("torrentPeer"));
    QVERIFY(relations.contains("torrentFiles"));
    QCOMPARE(relations.size(), static_cast<std::size_t>(1));
}

void tst_Model_Relations::without_NestedRelations() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = TorrentEager::without("torrentFiles")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto &relations = torrent->getRelations();
    QVERIFY(!relations.contains("torrentFiles"));
    QVERIFY(relations.contains("torrentPeer"));
    QCOMPARE(relations.size(), static_cast<std::size_t>(1));
}

void tst_Model_Relations::without_Vector_MoreRelations() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = TorrentEager::without({"torrentPeer", "torrentFiles"})->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVERIFY(torrent->getRelations().empty());
}

void tst_Model_Relations::withOnly() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = TorrentEager::withOnly("torrentPeer")->find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto &relations = torrent->getRelations();
    QVERIFY(relations.contains("torrentPeer"));
    QVERIFY(!relations.contains("torrentFiles"));
    QCOMPARE(relations.size(), static_cast<std::size_t>(1));
}

void tst_Model_Relations::load_QList_WithItem() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    QCOMPARE(torrent->getKey(), QVariant(2));

    QVERIFY(torrent->getRelations().empty());

    torrent->load({{"torrentFiles"}, {"torrentPeer"}});

    QVERIFY(torrent->relationLoaded("torrentFiles"));
    QVERIFY(torrent->relationLoaded("torrentPeer"));
    QCOMPARE(torrent->getRelations().size(), 2);

    // TorrentPeer has-one relation
    auto *peer = torrent->getRelation<TorrentPeer, One>("torrentPeer");
    QVERIFY(peer);
    QVERIFY(peer->exists);
    QCOMPARE(peer->getAttribute("torrent_id"), torrent->getKey());
    QCOMPARE(peer->getKey(), QVariant(2));
    QCOMPARE(typeid (peer), typeid (TorrentPeer *));

    // TorrentPreviewableFile has-many relation
    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));

        // No TorrentPreviewableFileProperty loaded
        QVERIFY(file->getRelations().empty());
    }
}

void tst_Model_Relations::load_QList_QString_lvalue() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    QCOMPARE(torrent->getKey(), QVariant(2));

    QVERIFY(torrent->getRelations().empty());

    const QList<QString> relations {"torrentFiles", "torrentPeer"};
    torrent->load(relations);

    QVERIFY(torrent->relationLoaded("torrentFiles"));
    QVERIFY(torrent->relationLoaded("torrentPeer"));
    QCOMPARE(torrent->getRelations().size(), 2);

    // TorrentPeer has-one relation
    auto *peer = torrent->getRelation<TorrentPeer, One>("torrentPeer");
    QVERIFY(peer);
    QVERIFY(peer->exists);
    QCOMPARE(peer->getAttribute("torrent_id"), torrent->getKey());
    QCOMPARE(peer->getKey(), QVariant(2));
    QCOMPARE(typeid (peer), typeid (TorrentPeer *));

    // TorrentPreviewableFile has-many relation
    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));

        // No TorrentPreviewableFileProperty loaded
        QVERIFY(file->getRelations().empty());
    }
}

void tst_Model_Relations::load_QList_QString_rvalue() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    QCOMPARE(torrent->getKey(), QVariant(2));

    QVERIFY(torrent->getRelations().empty());

    torrent->load({"torrentFiles", "torrentPeer"});

    QVERIFY(torrent->relationLoaded("torrentFiles"));
    QVERIFY(torrent->relationLoaded("torrentPeer"));
    QCOMPARE(torrent->getRelations().size(), 2);

    // TorrentPeer has-one relation
    auto *peer = torrent->getRelation<TorrentPeer, One>("torrentPeer");
    QVERIFY(peer);
    QVERIFY(peer->exists);
    QCOMPARE(peer->getAttribute("torrent_id"), torrent->getKey());
    QCOMPARE(peer->getKey(), QVariant(2));
    QCOMPARE(typeid (peer), typeid (TorrentPeer *));

    // TorrentPreviewableFile has-many relation
    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));

        // No TorrentPreviewableFileProperty loaded
        QVERIFY(file->getRelations().empty());
    }
}

void tst_Model_Relations::load_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    QCOMPARE(torrent->getKey(), QVariant(2));

    QVERIFY(torrent->getRelations().empty());

    torrent->load("torrentPeer");

    QVERIFY(torrent->relationLoaded("torrentPeer"));
    QCOMPARE(torrent->getRelations().size(), 1);

    // TorrentPeer has-one relation
    auto *peer = torrent->getRelation<TorrentPeer, One>("torrentPeer");
    QVERIFY(peer);
    QVERIFY(peer->exists);
    QCOMPARE(peer->getAttribute("torrent_id"), torrent->getKey());
    QCOMPARE(peer->getKey(), QVariant(2));
    QCOMPARE(typeid (peer), typeid (TorrentPeer *));
}

void tst_Model_Relations::load_WithSelectConstraint() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    QCOMPARE(torrent->getKey(), QVariant(2));

    QVERIFY(torrent->getRelations().empty());

    torrent->load("torrentFiles:id,torrent_id,filepath");

    QVERIFY(torrent->relationLoaded("torrentFiles"));
    QCOMPARE(torrent->getRelations().size(), 1);

    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);

        // Check whether constraints were correctly applied
        const auto &attributes = file->getAttributes();
        QCOMPARE(attributes.size(), 3);

        std::unordered_set<QString> expectedAttributes {ID, "torrent_id", "filepath"};
        for (const auto &attribute : attributes)
            QVERIFY(expectedAttributes.contains(attribute.key));

        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));
        QVERIFY(file->getRelations().empty());
    }
}

void tst_Model_Relations::load_WithLambdaConstraint() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    QCOMPARE(torrent->getKey(), QVariant(2));

    QVERIFY(torrent->getRelations().empty());

    torrent->load({{"torrentFiles", [](auto &query)
                    {
                        query.select({ID, "torrent_id", "filepath"});
                    }}});

    QVERIFY(torrent->relationLoaded("torrentFiles"));
    QCOMPARE(torrent->getRelations().size(), 1);

    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);

        // Check whether constraints were correctly applied
        const auto &attributes = file->getAttributes();
        QCOMPARE(attributes.size(), 3);

        std::unordered_set<QString> expectedAttributes {ID, "torrent_id", "filepath"};
        for (const auto &attribute : attributes)
            QVERIFY(expectedAttributes.contains(attribute.key));

        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));
        QVERIFY(file->getRelations().empty());
    }
}

void tst_Model_Relations::load_NonExistentRelation_Failed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);

    QVERIFY(torrent->getRelations().empty());

    TVERIFY_THROWS_EXCEPTION(RelationMappingNotFoundError,
                             torrent->load("torrentFiles-NON_EXISTENT"));
    QVERIFY(torrent->getRelations().empty());
}

void tst_Model_Relations::fresh() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    QCOMPARE(torrent->getKey(), QVariant(2));

    QVERIFY(torrent->getRelations().empty());

    torrent->setAttribute(NAME, "test2 fresh");
    QCOMPARE(torrent->getAttribute(NAME), QVariant("test2 fresh"));

    auto freshTorrent = torrent->fresh("torrentFiles");
    QVERIFY(freshTorrent);
    QVERIFY(&*torrent != &*freshTorrent);
    QVERIFY(freshTorrent->exists);
    QCOMPARE(freshTorrent->getKey(), (*torrent)[ID]);
    QCOMPARE(freshTorrent->getAttribute(NAME), QVariant("test2"));

    auto files = freshTorrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);

        // Check whether constraints were correctly applied
        const auto &attributes = file->getAttributes();
        QCOMPARE(attributes.size(), 9);

        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));
    }
}

void tst_Model_Relations::fresh_WithSelectConstraint() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    QCOMPARE(torrent->getKey(), QVariant(2));

    QVERIFY(torrent->getRelations().empty());

    torrent->setAttribute(NAME, "test2 fresh");
    QCOMPARE(torrent->getAttribute(NAME), QVariant("test2 fresh"));

    auto freshTorrent = torrent->fresh("torrentFiles:id,torrent_id,filepath");
    QVERIFY(freshTorrent);
    QVERIFY(&*torrent != &*freshTorrent);
    QVERIFY(freshTorrent->exists);
    QCOMPARE(freshTorrent->getKey(), (*torrent)[ID]);
    QCOMPARE(freshTorrent->getAttribute(NAME), QVariant("test2"));

    auto files = freshTorrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QList<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);

        // Check whether constraints were correctly applied
        const auto &attributes = file->getAttributes();
        QCOMPARE(attributes.size(), 3);

        std::unordered_set<QString> expectedAttributes {ID, "torrent_id", "filepath"};
        for (const auto &attribute : attributes)
            QVERIFY(expectedAttributes.contains(attribute.key));

        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));
    }
}

void tst_Model_Relations::refresh_EagerLoad_OnlyRelations() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = TorrentEager::find(3);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto &relations = torrent->getRelations();
    QCOMPARE(relations.size(), 2);

    // Validate original attribute values in relations
    auto filesOriginal =
            torrent->getRelation<TorrentPreviewableFileEager>("torrentFiles");
    auto filepathOriginal =
            filesOriginal.first()->getAttribute("filepath");
    auto *peerOriginal =
            torrent->getRelation<TorrentPeerEager_NoRelations, One>("torrentPeer");
    auto seedsOriginal =
            peerOriginal->getAttribute("seeds");
    QVERIFY(filepathOriginal == QVariant("test3_file1.mkv"));
    QVERIFY(seedsOriginal == QVariant(3));

    // Change attributes in relations
    filesOriginal.first()->setAttribute("filepath", "test3_file1-refresh.mkv");
    peerOriginal->setAttribute("seeds", 33);

    // Validate changed attributes in relations
    auto filepathOriginalChanged =
            torrent->getRelationValue<TorrentPreviewableFileEager>("torrentFiles")
            .first()->getAttribute("filepath");
    auto seedsOriginalChanged =
            torrent->getRelationValue<TorrentPeerEager_NoRelations, One>("torrentPeer")
            ->getAttribute("seeds");
    QVERIFY(filepathOriginalChanged == QVariant("test3_file1-refresh.mkv"));
    QVERIFY(seedsOriginalChanged == QVariant(33));

    // Memory address of the key and value for the relation
    const auto *const
    relationFilesKeyOriginal = std::addressof(relations.find("torrentFiles")->first);
    auto *const
    relationFilesValueOriginal = std::addressof(relations.find("torrentFiles")->second);
    const auto *const
    relationPeerKeyOriginal = std::addressof(relations.find("torrentPeer")->first);
    auto *const
    relationPeerValueOriginal = std::addressof(relations.find("torrentPeer")->second);

    torrent->refresh();

    QCOMPARE(relations.size(), 2);
    /* Values in the std::unordered_map container has to be the same, because
       only loaded relations will be replaced with std::move directly
       to the relation std::variant reference in the Model::load() method. */
    QVERIFY(relationFilesKeyOriginal ==
            std::addressof(relations.find("torrentFiles")->first));
    QVERIFY(relationFilesValueOriginal ==
            std::addressof(relations.find("torrentFiles")->second));
    QVERIFY(relationPeerKeyOriginal ==
            std::addressof(relations.find("torrentPeer")->first));
    QVERIFY(relationPeerValueOriginal ==
            std::addressof(relations.find("torrentPeer")->second));

    // Validate refreshed attributes in relations
    auto filesRefreshed =
            torrent->getRelation<TorrentPreviewableFileEager>("torrentFiles");
    auto filepathRefreshed = filesRefreshed.first()->getAttribute("filepath");
    auto *peerRefreshed =
            torrent->getRelation<TorrentPeerEager_NoRelations, One>("torrentPeer");
    auto seedsRefreshed = peerRefreshed->getAttribute("seeds");
    QVERIFY(filepathOriginal == filepathRefreshed);
    QVERIFY(seedsOriginal == seedsRefreshed);
}

void tst_Model_Relations::refresh_LazyLoad_OnlyRelations() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(3);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto &relations = torrent->getRelations();
    QVERIFY(relations.empty());

    // Validate original attribute values in relations
    auto filesOriginal =
            torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    auto filepathOriginal =
            filesOriginal.first()->getAttribute("filepath");
    auto *peerOriginal =
            torrent->getRelationValue<TorrentPeer, One>("torrentPeer");
    auto seedsOriginal =
            peerOriginal->getAttribute("seeds");
    QCOMPARE(relations.size(), 2);
    QVERIFY(filepathOriginal == QVariant("test3_file1.mkv"));
    QVERIFY(seedsOriginal == QVariant(3));

    // Change attributes in relations
    filesOriginal.first()->setAttribute("filepath", "test3_file1-refresh.mkv");
    peerOriginal->setAttribute("seeds", 33);

    // Validate changed attributes in relations
    auto filepathOriginalChanged =
            torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles")
            .first()->getAttribute("filepath");
    auto seedsOriginalChanged =
            torrent->getRelationValue<TorrentPeer, One>("torrentPeer")
            ->getAttribute("seeds");
    QVERIFY(filepathOriginalChanged == QVariant("test3_file1-refresh.mkv"));
    QVERIFY(seedsOriginalChanged == QVariant(33));

    // Memory address of the key and value for the relation
    const auto *const
    relationFilesKeyOriginal = std::addressof(relations.find("torrentFiles")->first);
    auto *const
    relationFilesValueOriginal = std::addressof(relations.find("torrentFiles")->second);
    const auto *const
    relationPeerKeyOriginal = std::addressof(relations.find("torrentPeer")->first);
    auto *const
    relationPeerValueOriginal = std::addressof(relations.find("torrentPeer")->second);

    torrent->refresh();

    QCOMPARE(relations.size(), 2);
    /* Values in the std::unordered_map container has to be the same, because
       only loaded relations will be replaced with std::move directly
       to the relation std::variant reference in the Model::load() method. */
    QVERIFY(relationFilesKeyOriginal ==
            std::addressof(relations.find("torrentFiles")->first));
    QVERIFY(relationFilesValueOriginal ==
            std::addressof(relations.find("torrentFiles")->second));
    QVERIFY(relationPeerKeyOriginal ==
            std::addressof(relations.find("torrentPeer")->first));
    QVERIFY(relationPeerValueOriginal ==
            std::addressof(relations.find("torrentPeer")->second));

    // Validate refreshed attributes in relations
    auto filesRefreshed =
            torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    auto filepathRefreshed = filesRefreshed.first()->getAttribute("filepath");
    auto *peerRefreshed =
            torrent->getRelationValue<TorrentPeer, One>("torrentPeer");
    auto seedsRefreshed = peerRefreshed->getAttribute("seeds");
    QVERIFY(filepathOriginal == filepathRefreshed);
    QVERIFY(seedsOriginal == seedsRefreshed);
}

void tst_Model_Relations::push_EagerLoad() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = TorrentEager::find(2);

    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    /* Also contains torrentPeer relation, which is not needed for this test, but
       I will not create the new eager model class. */
    QCOMPARE(torrent->getRelations().size(), static_cast<std::size_t>(2));

    const auto findFile2 = [](const auto *file)
    {
        return (*file)[ID].template value<quint64>() == 2
                && (*file)["torrent_id"].template value<quint64>() == 2;
    };

    auto files = torrent->getRelation<TorrentPreviewableFileEager>("torrentFiles");
    auto itFile = std::find_if(files.cbegin(), files.cend(), findFile2);
    if (itFile == files.cend())
        QFAIL("File was not found in the files vector.");
    auto *file = *itFile;
    auto *fileProperty =
            file->getRelation<TorrentPreviewableFilePropertyEager, One>("fileProperty");

    QCOMPARE(files.size(), 2);
    QVERIFY(file);
    QVERIFY(file->exists);
    QVERIFY(fileProperty);
    QVERIFY(fileProperty->exists);

    auto torrentNameOriginal = torrent->getAttribute(NAME);
    auto fileFilepathOriginal = file->getAttribute("filepath");
    auto propertyNameOriginal = fileProperty->getAttribute(NAME);

    QCOMPARE(torrentNameOriginal, QVariant("test2"));
    QCOMPARE(fileFilepathOriginal, QVariant("test2_file1.mkv"));
    QCOMPARE(propertyNameOriginal, QVariant("test2_file1"));

    // Modify values in relations
    torrent->setAttribute(NAME, "test2 push");
    file->setAttribute("filepath", "test2_file1-push.mkv");
    fileProperty->setAttribute(NAME, "test2_file1 push");

    QVERIFY(torrent->push());

    // Verify saved values
    auto torrentVerify = TorrentEager::find(2);

    QVERIFY(torrentVerify);
    QVERIFY(torrentVerify->exists);

    QCOMPARE(torrent->getRelations().size(), static_cast<std::size_t>(2));

    auto filesVerify =
            torrentVerify->getRelation<TorrentPreviewableFileEager>("torrentFiles");
    auto itFileVerify =
            std::find_if(filesVerify.cbegin(), filesVerify.cend(), findFile2);
    if (itFileVerify == filesVerify.cend())
        QFAIL("File to verify was not found in the filesVerify vector.");
    auto *fileVerify = *itFileVerify;
    auto *filePropertyVerify =
            fileVerify->getRelation<TorrentPreviewableFilePropertyEager, One>(
                "fileProperty");

    QCOMPARE(filesVerify.size(), 2);
    QVERIFY(fileVerify);
    QVERIFY(fileVerify->exists);
    QVERIFY(filePropertyVerify);
    QVERIFY(filePropertyVerify->exists);

    QCOMPARE(torrentVerify->getAttribute(NAME), QVariant("test2 push"));
    QCOMPARE(fileVerify->getAttribute("filepath"), QVariant("test2_file1-push.mkv"));
    QCOMPARE(filePropertyVerify->getAttribute(NAME), QVariant("test2_file1 push"));

    // Revert values back
    torrentVerify->setAttribute(NAME, "test2");
    fileVerify->setAttribute("filepath", "test2_file1.mkv");
    filePropertyVerify->setAttribute(NAME, "test2_file1");

    torrentVerify->push();
}

void tst_Model_Relations::push_LazyLoad() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);

    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVERIFY(torrent->getRelations().empty());

    const auto findFile2 = [](const auto *file)
    {
        return (*file)[ID].template value<quint64>() == 2
                && (*file)["torrent_id"].template value<quint64>() == 2;
    };

    auto files = torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    auto itFile = std::find_if(files.cbegin(), files.cend(), findFile2);
    if (itFile == files.cend())
        QFAIL("File was not found in the files vector.");
    auto *file = *itFile;
    auto *fileProperty =
            file->getRelationValue<TorrentPreviewableFileProperty, One>("fileProperty");

    QCOMPARE(files.size(), 2);
    QVERIFY(file);
    QVERIFY(file->exists);
    QVERIFY(fileProperty);
    QVERIFY(fileProperty->exists);

    auto torrentNameOriginal = torrent->getAttribute(NAME);
    auto fileFilepathOriginal = file->getAttribute("filepath");
    auto propertyNameOriginal = fileProperty->getAttribute(NAME);

    QCOMPARE(torrentNameOriginal, QVariant("test2"));
    QCOMPARE(fileFilepathOriginal, QVariant("test2_file1.mkv"));
    QCOMPARE(propertyNameOriginal, QVariant("test2_file1"));

    // Modify values in relations
    torrent->setAttribute(NAME, "test2 push");
    file->setAttribute("filepath", "test2_file1-push.mkv");
    fileProperty->setAttribute(NAME, "test2_file1 push");

    QVERIFY(torrent->push());

    // Verify saved values
    auto torrentVerify = Torrent::find(2);

    QVERIFY(torrentVerify);
    QVERIFY(torrentVerify->exists);

    QVERIFY(torrentVerify->getRelations().empty());

    auto filesVerify =
            torrentVerify->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    auto itFileVerify =
            std::find_if(filesVerify.cbegin(), filesVerify.cend(), findFile2);
    if (itFileVerify == filesVerify.cend())
        QFAIL("File to verify was not found in the filesVerify vector.");
    auto *fileVerify = *itFileVerify;
    auto *filePropertyVerify =
            fileVerify->getRelationValue<TorrentPreviewableFileProperty, One>(
                "fileProperty");

    QCOMPARE(filesVerify.size(), 2);
    QVERIFY(fileVerify);
    QVERIFY(fileVerify->exists);
    QVERIFY(filePropertyVerify);
    QVERIFY(filePropertyVerify->exists);

    QCOMPARE(torrentVerify->getAttribute(NAME), QVariant("test2 push"));
    QCOMPARE(fileVerify->getAttribute("filepath"), QVariant("test2_file1-push.mkv"));
    QCOMPARE(filePropertyVerify->getAttribute(NAME), QVariant("test2_file1 push"));

    // Revert values back
    torrentVerify->setAttribute(NAME, "test2");
    fileVerify->setAttribute("filepath", "test2_file1.mkv");
    filePropertyVerify->setAttribute(NAME, "test2_file1");

    torrentVerify->push();
}

void tst_Model_Relations::where_WithCallback() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto files = Torrent::find(5)->torrentFiles()
                 ->where([](auto &query)
    {
        query.whereEq(ID, 6).orWhereEq("file_index", 2);
    })
                 .get();

    QCOMPARE(files.size(), 2);

    // Expected file IDs
    QList<QVariant> fileIds {6, 8};
    for (auto &file : files) {
        QVERIFY(file.exists);
        QVERIFY(fileIds.contains(file[ID]));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile));
    }
}

void tst_Model_Relations::orWhere_WithCallback() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto files = Torrent::find(5)->torrentFiles()
                 ->where(Progress, ">", 990)
                 .orWhere([](auto &query)
    {
        query.whereEq(ID, 8).whereEq("file_index", 2);
    })
                 .get();

    QCOMPARE(files.size(), 2);

    // Expected file IDs
    QList<QVariant> fileIds {6, 8};
    for (auto &file : files) {
        QVERIFY(file.exists);
        QVERIFY(fileIds.contains(file[ID]));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile));
    }
}

void tst_Model_Relations::belongsToMany_allRelatedIds() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(3);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    const auto relatedIds = torrent->tags()->allRelatedIds();

    QCOMPARE(relatedIds.size(), 2);

    const QList<QVariant> expectedIds {2, 4};

    for (const auto &relatedId : relatedIds)
        QVERIFY(expectedIds.contains(relatedId));
}

/* Default Models */

void tst_Model_Relations::withoutDefaultModel_LazyLoad_HasOne() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFile::find(7);
    QVERIFY(torrentFile->exists);

    auto *fileProperty =
            torrentFile->getRelationValue<TorrentPreviewableFileProperty, One>(
                "fileProperty");

    QVERIFY(fileProperty == nullptr);
}

void tst_Model_Relations::withoutDefaultModel_LazyLoad_BelongsTo() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFile::find(9);
    QVERIFY(torrentFile->exists);

    auto *torrent = torrentFile->getRelationValue<Torrent, One>("torrent");

    QVERIFY(torrent == nullptr);
}

void tst_Model_Relations::withoutDefaultModel_EagerLoad_HasOne() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFileEager_WithDefault::find(7);
    QVERIFY(torrentFile->exists);

    auto *fileProperty =
            torrentFile->getRelation<TorrentPreviewableFilePropertyEager, One>(
                "fileProperty");

    QVERIFY(fileProperty == nullptr);
}

void tst_Model_Relations::withoutDefaultModel_EagerLoad_BelongsTo() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFileEager_WithDefault::find(9);
    QVERIFY(torrentFile->exists);

    auto *torrent = torrentFile->getRelation<TorrentEager_WithDefault, One>("torrent");

    QVERIFY(torrent == nullptr);
}

void tst_Model_Relations::withDefaultModel_LazyLoad_Bool_HasOne() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFile::find(7);
    QVERIFY(torrentFile->exists);

    auto *fileProperty =
            torrentFile->getRelationValue<TorrentPreviewableFileProperty, One>(
                "fileProperty_WithBoolDefault");

    QVERIFY(fileProperty != nullptr);
    QVERIFY(!fileProperty->exists);
    QCOMPARE(typeid (fileProperty), typeid (TorrentPreviewableFileProperty *));
    QCOMPARE(fileProperty->getAttributes().size(), 1);
    QCOMPARE((*fileProperty)["previewable_file_id"], QVariant(7));
}

void tst_Model_Relations::withDefaultModel_LazyLoad_AttributesVector_HasOne() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFile::find(7);
    QVERIFY(torrentFile->exists);

    auto *fileProperty =
            torrentFile->getRelationValue<TorrentPreviewableFileProperty, One>(
                "fileProperty_WithVectorDefaults");

    QVERIFY(fileProperty != nullptr);
    QVERIFY(!fileProperty->exists);
    QCOMPARE(typeid (fileProperty), typeid (TorrentPreviewableFileProperty *));
    QCOMPARE(fileProperty->getAttributes().size(), 3);
    QCOMPARE((*fileProperty)["previewable_file_id"], QVariant(7));
    QCOMPARE((*fileProperty)[NAME], QVariant("default_fileproperty_name"));
    QCOMPARE((*fileProperty)[SIZE_], QVariant(321));
}

void tst_Model_Relations::withDefaultModel_LazyLoad_Bool_BelongsTo() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFile::find(9);
    QVERIFY(torrentFile->exists);

    auto *torrent =
            torrentFile->getRelationValue<Torrent, One>("torrent_WithBoolDefault");

    QVERIFY(torrent != nullptr);
    QVERIFY(!torrent->exists);
    QCOMPARE(typeid (torrent), typeid (Torrent *));
    QCOMPARE(torrent->getAttributes().size(), 0);
}

void tst_Model_Relations::withDefaultModel_LazyLoad_AttributesVector_BelongsTo() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFile::find(9);
    QVERIFY(torrentFile->exists);

    auto *torrent =
            torrentFile->getRelationValue<Torrent, One>("torrent_WithVectorDefaults");

    QVERIFY(torrent != nullptr);
    QVERIFY(!torrent->exists);
    QCOMPARE(typeid (torrent), typeid (Torrent *));
    QCOMPARE(torrent->getAttributes().size(), 2);
    QCOMPARE((*torrent)[NAME], QVariant("default_torrent_name"));
    QCOMPARE((*torrent)[SIZE_], QVariant(123));
}

void tst_Model_Relations::withDefaultModel_EagerLoad_Bool_HasOne() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFileEager_WithDefault::find(7);
    QVERIFY(torrentFile->exists);

    auto *fileProperty =
            torrentFile->getRelation<TorrentPreviewableFilePropertyEager, One>(
                "fileProperty_WithBoolDefault");

    QVERIFY(fileProperty != nullptr);
    QVERIFY(!fileProperty->exists);
    QCOMPARE(typeid (fileProperty), typeid (TorrentPreviewableFilePropertyEager *));
    QCOMPARE(fileProperty->getAttributes().size(), 1);
    QCOMPARE((*fileProperty)["previewable_file_id"], QVariant(7));
}

void tst_Model_Relations::withDefaultModel_EagerLoad_AttributesVector_HasOne() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFileEager_WithDefault::find(7);
    QVERIFY(torrentFile->exists);

    auto *fileProperty =
            torrentFile->getRelation<TorrentPreviewableFilePropertyEager, One>(
                "fileProperty_WithVectorDefaults");

    QVERIFY(fileProperty != nullptr);
    QVERIFY(!fileProperty->exists);
    QCOMPARE(typeid (fileProperty), typeid (TorrentPreviewableFilePropertyEager *));
    QCOMPARE(fileProperty->getAttributes().size(), 3);
    QCOMPARE((*fileProperty)["previewable_file_id"], QVariant(7));
    QCOMPARE((*fileProperty)[NAME], QVariant("default_fileproperty_name"));
    QCOMPARE((*fileProperty)[SIZE_], QVariant(321));
}

void tst_Model_Relations::withDefaultModel_EagerLoad_Bool_BelongsTo() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFileEager_WithDefault::find(9);
    QVERIFY(torrentFile->exists);

    auto *torrent =
            torrentFile->getRelation<TorrentEager_WithDefault, One>(
                "torrent_WithBoolDefault");

    QVERIFY(torrent != nullptr);
    QVERIFY(!torrent->exists);
    QCOMPARE(typeid (torrent), typeid (TorrentEager_WithDefault *));
    QCOMPARE(torrent->getAttributes().size(), 0);
}

void tst_Model_Relations::withDefaultModel_EagerLoad_AttributesVector_BelongsTo() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFileEager_WithDefault::find(9);
    QVERIFY(torrentFile->exists);

    auto *torrent =
            torrentFile->getRelation<TorrentEager_WithDefault, One>(
                "torrent_WithVectorDefaults");

    QVERIFY(torrent != nullptr);
    QVERIFY(!torrent->exists);
    QCOMPARE(typeid (torrent), typeid (TorrentEager_WithDefault *));
    QCOMPARE(torrent->getAttributes().size(), 2);
    QCOMPARE((*torrent)[NAME], QVariant("default_torrent_name"));
    QCOMPARE((*torrent)[SIZE_], QVariant(123));
}

/* Relation related */

/* BelongsToMany related */

void tst_Model_Relations::find() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto tag = Torrent::find(3)->tags()->find(2);
    QVERIFY(tag);
    QCOMPARE(tag->getKey(), QVariant(2));
    QCOMPARE(tag->getAttribute(NAME), QVariant("tag2"));
}

void tst_Model_Relations::findOr() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Callback invoked
    {
        auto callbackInvoked = false;
        auto tag = Torrent::find(3)->tags()
                   ->findOr(100, {ASTERISK}, [&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(!tag);
        QVERIFY(callbackInvoked);
    }
    // Callback invoked (second overload)
    {
        auto callbackInvoked = false;
        auto tag = Torrent::find(3)->tags()->findOr(100, [&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(!tag);
        QVERIFY(callbackInvoked);
    }
    // Callback not invoked
    {
        auto callbackInvoked = false;
        auto tag = Torrent::find(3)->tags()->findOr(2, [&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(tag);
        QVERIFY(!callbackInvoked);
        QCOMPARE(tag->getKey(), QVariant(2));
        QCOMPARE(tag->getAttribute(NAME), QVariant("tag2"));
    }
}

void tst_Model_Relations::findOr_WithReturnType() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Callback invoked
    {
        auto [tag, result] = Torrent::find(3)->tags()->findOr<int>(100, []()
        {
            return 1;
        });

        QVERIFY(!tag);
        QCOMPARE(result, 1);
    }
    // Callback invoked (second overload)
    {
        auto [tag, result] = Torrent::find(3)->tags()
                             ->findOr<int>(100, {ID, NAME}, []()
        {
            return 1;
        });

        QVERIFY(!tag);
        QCOMPARE(result, 1);
    }
    // Callback not invoked
    {
        auto [tag, result] = Torrent::find(3)->tags()->findOr<int>(2, []()
        {
            return 1;
        });

        QVERIFY(tag);
        QCOMPARE(result, 0);
        QCOMPARE(tag->getKey(), QVariant(2));
        QCOMPARE(tag->getAttribute(NAME), QVariant("tag2"));
    }
}

void tst_Model_Relations::first() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto tag = Torrent::find(3)->tags()->whereKey(2).first();
    QVERIFY(tag);
    QCOMPARE(tag->getKey(), QVariant(2));
    QCOMPARE(tag->getAttribute(NAME), QVariant("tag2"));
}

void tst_Model_Relations::firstOr() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Callback invoked
    {
        auto callbackInvoked = false;
        auto tag = Torrent::find(3)->tags()->whereKey(100)
                   .firstOr({ASTERISK}, [&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(!tag);
        QVERIFY(callbackInvoked);
    }
    // Callback invoked (second overload)
    {
        auto callbackInvoked = false;
        auto tag = Torrent::find(3)->tags()->whereKey(100)
                   .firstOr([&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(!tag);
        QVERIFY(callbackInvoked);
    }
    // Callback not invoked
    {
        auto callbackInvoked = false;
        auto tag = Torrent::find(3)->tags()->whereKey(2).firstOr([&callbackInvoked]()
        {
            callbackInvoked = true;
        });

        QVERIFY(tag);
        QVERIFY(!callbackInvoked);
        QCOMPARE(tag->getKey(), QVariant(2));
        QCOMPARE(tag->getAttribute(NAME), QVariant("tag2"));
    }
}

void tst_Model_Relations::firstOr_WithReturnType() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Callback invoked
    {
        auto [tag, result] = Torrent::find(3)->tags()->whereKey(100)
                             .firstOr<int>([]()
        {
            return 1;
        });

        QVERIFY(!tag);
        QCOMPARE(result, 1);
    }
    // Callback invoked (second overload)
    {
        auto [tag, result] = Torrent::find(3)->tags()->whereKey(100)
                             .firstOr<int>({ID, NAME}, []()
        {
            return 1;
        });

        QVERIFY(!tag);
        QCOMPARE(result, 1);
    }
    // Callback not invoked
    {
        auto [tag, result] = Torrent::find(3)->tags()->whereKey(2)
                             .firstOr<int>([]()
        {
            return 1;
        });

        QVERIFY(tag);
        QCOMPARE(result, 0);
        QCOMPARE(tag->getKey(), QVariant(2));
        QCOMPARE(tag->getAttribute(NAME), QVariant("tag2"));
    }
}

void tst_Model_Relations::whereRowValues() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto tag = Torrent::find(2)->tags()
               ->whereRowValuesEq({ID, NAME}, {1, "tag1"})
               .first();

    QVERIFY(tag);
    QCOMPARE(tag->getKey(), QVariant(1));
    QCOMPARE(tag->getAttribute(NAME), QVariant("tag1"));
}

void tst_Model_Relations::upsert() const
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
                Tag::without("tagProperty")->find(1)->tagProperty()->upsert(
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

/* Casting Attributes */

void tst_Model_Relations::withCasts_OnRelation_OneToMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrentFile = Torrent::find(1)->torrentFiles()
                       ->withCasts({{Progress, Orm::Tiny::CastType::UInteger}})
                       .first();

    QVERIFY(torrentFile);
    QVERIFY(torrentFile->exists);
    QCOMPARE(torrentFile->getKeyCasted(), 1);

    auto attribute = torrentFile->getAttribute(Progress);

    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::UInt);
    else if (driverName == Orm::QPSQL)
        QCOMPARE(typeId, QMetaType::UInt);
    else if (driverName == Orm::QSQLITE)
        QCOMPARE(typeId, QMetaType::UInt);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<uint>(), static_cast<uint>(200));
}

void tst_Model_Relations::withCasts_OnRelation_ManyToMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto tag = Torrent::find(2)->tags()
               ->orderBy(ID)
               .withCasts({{ID, Orm::Tiny::CastType::UInt}})
               .first();

    QVERIFY(tag);
    QVERIFY(tag->exists);
    QCOMPARE(tag->getKeyCasted(), 1);

    auto attribute = tag->getKey();

    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::UInt);
    else if (driverName == Orm::QPSQL)
        QCOMPARE(typeId, QMetaType::UInt);
    else if (driverName == Orm::QSQLITE)
        QCOMPARE(typeId, QMetaType::UInt);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<uint>(), static_cast<uint>(1));
}

void tst_Model_Relations::u_casts_OnCustomPivotModel_ManyToMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto tag = Torrent::find(2)->tags()
               ->orderBy(ID)
               .first();

    QVERIFY(tag);
    QVERIFY(tag->exists);
    QCOMPARE(tag->getKeyCasted(), 1);

    auto *tagged = tag->getRelation<Tagged, One>("tagged");
    QVERIFY(tagged);
    QVERIFY(tagged->exists);

    auto attribute = tagged->getAttribute("active");

    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Bool);
    else if (driverName == Orm::QPSQL)
        QCOMPARE(typeId, QMetaType::Bool);
    else if (driverName == Orm::QSQLITE)
        QCOMPARE(typeId, QMetaType::Bool);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<bool>(), true);
}

/* QDateTime with/without timezone */

/* Server timezone UTC */

void tst_Model_Relations::
timezone_TimestampAttribute_UtcOnServer_OnCustomPivotModel_ManyToMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Get the fresh model from the database
    auto tag = Torrent::find(2)->tags()
               ->orderBy(ID)
               .first();

    QVERIFY(tag);
    QVERIFY(tag->exists);
    QCOMPARE(tag->getKeyCasted(), 1);

    auto *tagged = tag->getRelation<Tagged, One>("tagged");
    QVERIFY(tagged);
    QVERIFY(tagged->exists);

    // Test
    const auto timestampDbVariant = tagged->getAttribute(CREATED_AT);
    QVERIFY(timestampDbVariant.isValid());
    QVERIFY(!timestampDbVariant.isNull());

    QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

    /* The time zone must be as is defined in the qt_timezone connection
       configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
       behavior of all QtSql drivers. */
    const auto timestampActual = timestampDbVariant.value<QDateTime>();
    const auto timestampExpected = QDateTime({2021, 2, 21}, {17, 31, 58}, TTimeZone::UTC);

    QCOMPARE(timestampActual, timestampExpected);
    QCOMPARE(timestampActual, timestampExpected.toLocalTime());
    QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
}

/* QtTimeZoneType::DontConvert */

/* Server timezone UTC */

void tst_Model_Relations::
timezone_TimestampAttribute_UtcOnServer_DontConvert_OnCustomPivot_MtM() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    /* The QDateTime's time zone is ignored with the QtTimeZoneType::DontConvert
       connection option, only toString(m_queryGrammar->getDateFormat()) is applied. */

    DB::setQtTimeZone(QtTimeZoneConfig {QtTimeZoneType::DontConvert}, connection);
    QCOMPARE(DB::qtTimeZone(connection),
             QtTimeZoneConfig {QtTimeZoneType::DontConvert});

    // Get the fresh model from the database
    auto tag = Torrent::find(2)->tags()
               ->orderBy(ID)
               .first();

    QVERIFY(tag);
    QVERIFY(tag->exists);
    QCOMPARE(tag->getKeyCasted(), 1);

    auto *tagged = tag->getRelation<Tagged, One>("tagged");
    QVERIFY(tagged);
    QVERIFY(tagged->exists);

    // Test
    const auto timestampDbVariant = tagged->getAttribute(CREATED_AT);
    QVERIFY(timestampDbVariant.isValid());
    QVERIFY(!timestampDbVariant.isNull());

    QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

    /* The time zone must be as is defined in the qt_timezone connection
       configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
       behavior of all QtSql drivers. */
    const auto timestampActual = timestampDbVariant.value<QDateTime>();

/* Qt >=v6.8 fixes time zone handling, it calls toUTC() on QDateTime instance while
   sending QDateTime()-s to the database, calls SET time_zone = '+00:00' while opening
   a database connection, and returns QDateTime() instances with the UTC time zone during
   retrieving column values for both normal and prepared queries.
   This is the reason why we must use #if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
   everywhere for QtSql QMYSQL and QPSQL and tinymysql_lib_utc_qdatetime >= 20240618
   for TinyDrivers TinyMySql. */
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) || \
    (defined(tinymysql_lib_utc_qdatetime) && tinymysql_lib_utc_qdatetime >= 20240618)
    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL || driverName == QPSQL
    ) {
        const auto timestampExpected = QDateTime({2021, 2, 21}, {17, 31, 58},
                                                 QTimeZone::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
    }
    else if (driverName == QSQLITE) {
        const auto timestampExpected = QDateTime({2021, 2, 21}, {17, 31, 58});

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
    }
    else
        Q_UNREACHABLE();
#else
    const auto timestampExpected = QDateTime({2021, 2, 21}, {17, 31, 58});

    QCOMPARE(timestampActual, timestampExpected);
    QCOMPARE(timestampActual, timestampExpected.toLocalTime());
    QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
#endif

    // Restore
    DB::setQtTimeZone(QtTimeZoneConfig::utc(), connection);
    QCOMPARE(DB::qtTimeZone(connection), QtTimeZoneConfig::utc());
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_Model_Relations)

#include "tst_model_relations.moc"
