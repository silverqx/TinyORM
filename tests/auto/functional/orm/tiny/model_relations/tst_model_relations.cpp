#include <QCoreApplication>
#include <QtTest>

#include <typeinfo>

#include "orm/db.hpp"
#include "orm/utils/query.hpp"

#include "databases.hpp"

#include "models/torrent.hpp"
#include "models/torrenteager.hpp"
#include "models/torrenteager_failed.hpp"
#include "models/torrentpeer.hpp"
#include "models/torrentpeereager.hpp"
#include "models/torrentpeereager_norelations.hpp"
#include "models/torrentpreviewablefileeager_withdefault.hpp"

using Orm::Constants::AND;
using Orm::Constants::ASTERISK;
using Orm::Constants::CREATED_AT;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::LIKE;
using Orm::Constants::QMYSQL;
using Orm::Constants::SIZE_;
using Orm::Constants::UPDATED_AT;

using Orm::DB;
using Orm::Exceptions::RuntimeError;
using Orm::One;
using Orm::QtTimeZoneConfig;
using Orm::QtTimeZoneType;
using Orm::QueryBuilder;

using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::Exceptions::RelationNotFoundError;
using Orm::Tiny::Exceptions::RelationNotLoadedError;
using Orm::Tiny::Relations::Pivot;
using Orm::Tiny::Relations::Relation;
using Orm::Tiny::TinyBuilder;
using Orm::Tiny::Types::ModelsCollection;
using Orm::Utils::Helpers;

using QueryUtils = Orm::Utils::Query;
using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

using Models::FilePropertyProperty;
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
    void with_NestedRelations() const;
    void with_Vector_MoreRelations() const;
    void with_NonExistentRelation_Failed() const;

    void with_WithSelectConstraint() const;
    void with_WithSelectConstraint_WithWhitespaces() const;
    void with_WithSelectConstraint_BelongsToMany() const;
    void with_WithLambdaConstraint() const;
    void with_WithLambdaConstraint_BelongsToMany() const;

    void without() const;
    void without_NestedRelations() const;
    void without_Vector_MoreRelations() const;

    void withOnly() const;

    void load_QVector_WithItem() const;
    void load_QVector_QString_lvalue() const;
    void load_QVector_QString_rvalue() const;
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

    /* Querying Relationship Existence/Absence */
    void has_Basic_QString_OnHasMany() const;
    void has_Basic_UniquePtr_OnHasMany() const;
    void has_Basic_MethodPointer_OnHasMany() const;

    void has_Count_QString_OnHasMany() const;
    void has_Count_UniquePtr_OnHasMany() const;
    void has_Count_MethodPointer_OnHasMany() const;

    void whereHas_Basic_QString_QueryBuilder_OnHasMany() const;
    void whereHas_Basic_QString_TinyBuilder_OnHasMany() const;

    void whereHas_Count_QString_QueryBuilder_OnHasMany() const;
    void whereHas_Count_QString_TinyBuilder_OnHasMany() const;
    void whereHas_Count_MethodPointer_TinyBuilder_OnHasMany() const;

    void hasNested_Basic_OnHasMany() const;
    void hasNested_Count_OnHasMany() const;
    void hasNested_Count_TinyBuilder_OnHasMany() const;

    /* Relation related */
    /* Builds Queries */
    void chunk_Relation() const;
    void each_Relation() const;
    void chunkMap_Relation() const;
    void chunkMap_TemplatedReturnValue_Relation() const;
    void chunkById_Relation() const;
    void chunkById_WithAlias_Relation() const;
    void eachById_Relation() const;
    void eachById_WithAlias_Relation() const;

    /* BelongsToMany related */
    void find() const;

    void findOr() const;
    void findOr_WithReturnType() const;

    void first() const;

    void firstOr() const;
    void firstOr_WithReturnType() const;

    void whereRowValues() const;

    /* Builds Queries */
    void chunk() const;
    void chunk_ReturnFalse() const;
    void chunk_EnforceOrderBy() const;
    void chunk_EmptyResult() const;

    void each() const;
    void each_ReturnFalse() const;
    void each_EnforceOrderBy() const;
    void each_EmptyResult() const;

    void chunkMap() const;
    void chunkMap_EnforceOrderBy() const;
    void chunkMap_EmptyResult() const;

    void chunkMap_TemplatedReturnValue() const;
    void chunkMap_TemplatedReturnValue_OnRelationRef() const;
    void chunkMap_EnforceOrderBy_TemplatedReturnValue() const;
    void chunkMap_EmptyResult_TemplatedReturnValue() const;

    void chunkById() const;
    void chunkById_ReturnFalse() const;
    void chunkById_EmptyResult() const;

    void chunkById_WithAlias() const;
    void chunkById_ReturnFalse_WithAlias() const;
    void chunkById_EmptyResult_WithAlias() const;

    void eachById() const;
    void eachById_ReturnFalse() const;
    void eachById_EmptyResult() const;

    void eachById_WithAlias() const;
    void eachById_ReturnFalse_WithAlias() const;
    void eachById_EmptyResult_WithAlias() const;

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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = TorrentEager::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // TorrentPreviewableFileEager has-many relation
    auto files = torrent->getRelation<TorrentPreviewableFileEager>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFileEager *>));

    // Expected file IDs
    QVector<QVariant> fileIds {2, 3};
    // Expected file property IDs
    QVector<QVariant> filePropertyIds {1, 2};
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
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto tag = Tag::with("torrents")->find(2);
    QVERIFY(tag);
    QVERIFY(tag->exists);

    // Torrent belongs-to-many relation (basic pivot)
    auto torrents = tag->getRelation<Torrent>("torrents");
    QCOMPARE(torrents.size(), 2);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent *>));

    // Expected torrent IDs
    QVector<QVariant> torrentIds {2, 3};

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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with({"tags"})->find(2);
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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto tag = Tag::with("torrents_WithoutPivotAttributes")->find(2);
    QVERIFY(tag);
    QVERIFY(tag->exists);

    // Torrent belongs-to-many relation (basic pivot)
    auto torrents = tag->getRelation<Torrent>("torrents_WithoutPivotAttributes");
    QCOMPARE(torrents.size(), 2);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent *>));

    // Expected torrent IDs
    QVector<QVariant> torrentIds {2, 3};

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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent;

    // Many relation
    QVERIFY_EXCEPTION_THROWN(
                (torrent.getRelation<TorrentPreviewableFile>("torrentFiles")),
                RelationNotLoadedError);
    // One relation, obtained as QVector, also possible
    QVERIFY_EXCEPTION_THROWN(
                (torrent.getRelation<TorrentPeer>("torrentFiles")),
                RelationNotLoadedError);
    // Many relation
    QVERIFY_EXCEPTION_THROWN(
                (torrent.getRelation<TorrentPeer, One>("torrentFiles")),
                RelationNotLoadedError);
    // BelongsTo relation
    QVERIFY_EXCEPTION_THROWN(
                (TorrentPeer().getRelation<Torrent, One>("torrent")),
                RelationNotLoadedError);
    // BelongsToMany relation
    QVERIFY_EXCEPTION_THROWN(
                (torrent.getRelation<Tag>("tags")),
                RelationNotLoadedError);
}

void tst_Model_Relations::eagerLoad_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    QVERIFY_EXCEPTION_THROWN(TorrentEager_Failed::find(1),
                             RelationNotFoundError);
}

void tst_Model_Relations::getRelationValue_LazyLoad_ManyAndOne() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // TorrentPreviewableFile has-many relation
    auto files = torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QVector<QVariant> fileIds {2, 3};
    // Expected file property IDs
    QVector<QVariant> filePropertyIds {1, 2};
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
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto tag = Tag::find(2);
    QVERIFY(tag);
    QVERIFY(tag->exists);

    // Torrent belongs-to-many relation (basic pivot)
    auto torrents = tag->getRelationValue<Torrent>("torrents");
    QCOMPARE(torrents.size(), 2);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent *>));

    // Expected torrent IDs
    QVector<QVariant> torrentIds {2, 3};

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
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto tag = Tag::find(2);
    QVERIFY(tag);
    QVERIFY(tag->exists);

    // Torrent belongs-to-many relation (basic pivot)
    auto torrents = tag->getRelationValue<Torrent>("torrents_WithoutPivotAttributes");
    QCOMPARE(torrents.size(), 2);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent *>));

    // Expected torrent IDs
    QVector<QVariant> torrentIds {2, 3};

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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    // Many relation
    QCOMPARE((Torrent().getRelationValue<TorrentPreviewableFile>("notExists")),
             ModelsCollection<TorrentPreviewableFile *>());
    // One relation
    QCOMPARE((Torrent().getRelationValue<TorrentPeer, One>("notExists")),
             nullptr);
    // One relation, obtained as QVector, also possible
    QCOMPARE((Torrent().getRelationValue<TorrentPeer>("notExists")),
             ModelsCollection<TorrentPeer *>());
    // Just to be sure try BelongsToMany relation
    QCOMPARE((Torrent().getRelationValue<Tag>("notExists")),
             ModelsCollection<Tag *>());
}

void tst_Model_Relations::u_with_Empty() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent;

    QVERIFY(torrent.getRelations().empty());
}

void tst_Model_Relations::with_HasOne() const
{
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with("torrentFiles")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QVector<QVariant> fileIds {2, 3};
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
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with("tags")->find(3);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto tags = torrent->getRelation<Tag>("tags");
    QCOMPARE(tags.size(), 2);
    QCOMPARE(typeid (tags), typeid (ModelsCollection<Tag *>));

    // Expected tag IDs
    QVector<QVariant> tagIds {2, 4};
    for (auto *tag : tags) {
        QVERIFY(tag);
        QVERIFY(tag->exists);
        QVERIFY(tagIds.contains(tag->getKey()));
        QCOMPARE(typeid (tag), typeid (Tag *));

        const auto &relations = tag->getRelations();
        QCOMPARE(relations.size(), static_cast<std::size_t>(2));
        QVERIFY(relations.contains("tagged"));
        QVERIFY(relations.contains("tagProperty"));

        // Custom pivot relation
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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with("torrentFiles.fileProperty")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // TorrentPreviewableFile has-many relation
    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QVector<QVariant> fileIds {2, 3};
    // Expected file property IDs
    QVector<QVariant> filePropertyIds {1, 2};
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

void tst_Model_Relations::with_Vector_MoreRelations() const
{
    QFETCH_GLOBAL(QString, connection);

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
    QVector<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));

        // No TorrentPreviewableFileProperty loaded
        QVERIFY_EXCEPTION_THROWN(
                    (file->getRelation<TorrentPreviewableFileProperty, One>(
                         "fileProperty")),
                    RuntimeError);
    }
}

void tst_Model_Relations::with_NonExistentRelation_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    QVERIFY_EXCEPTION_THROWN(Torrent::with("torrentFiles-NON_EXISTENT")->find(1),
                             RelationNotFoundError);
}

void tst_Model_Relations::with_WithSelectConstraint() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with({"torrentFiles:id,torrent_id,filepath"})->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QVector<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);

        // Check whether constraints was correctly applied
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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with({"  torrentFiles  :  id  ,  torrent_id ,filepath  "})
                   ->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (files), typeid (ModelsCollection<TorrentPreviewableFile *>));

    // Expected file IDs
    QVector<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);

        // Check whether constraints was correctly applied
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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with({"tags:id,name"})->find(3);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto tags = torrent->getRelation<Tag>("tags");
    QCOMPARE(tags.size(), 2);
    QCOMPARE(typeid (tags), typeid (ModelsCollection<Tag *>));

    // Expected tag IDs
    QVector<QVariant> tagIds {2, 4};
    for (auto *tag : tags) {
        QVERIFY(tag);
        QVERIFY(tag->exists);

        // Check whether constraints was correctly applied
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

        // Custom pivot relation
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
    QFETCH_GLOBAL(QString, connection);

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
    QVector<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);

        // Check whether constraints was correctly applied
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
    QFETCH_GLOBAL(QString, connection);

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
    QVector<QVariant> tagIds {2, 4};
    for (auto *tag : tags) {
        QVERIFY(tag);
        QVERIFY(tag->exists);

        // Check whether constraints was correctly applied
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

        // Custom pivot relation
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

void tst_Model_Relations::without() const
{
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = TorrentEager::without({"torrentPeer", "torrentFiles"})->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVERIFY(torrent->getRelations().empty());
}

void tst_Model_Relations::withOnly() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = TorrentEager::withOnly("torrentPeer")->find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto &relations = torrent->getRelations();
    QVERIFY(relations.contains("torrentPeer"));
    QVERIFY(!relations.contains("torrentFiles"));
    QCOMPARE(relations.size(), static_cast<std::size_t>(1));
}

void tst_Model_Relations::load_QVector_WithItem() const
{
    QFETCH_GLOBAL(QString, connection);

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
    QVector<QVariant> fileIds {2, 3};
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

void tst_Model_Relations::load_QVector_QString_lvalue() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);
    QCOMPARE(torrent->getKey(), QVariant(2));

    QVERIFY(torrent->getRelations().empty());

    const QVector<QString> relations {"torrentFiles", "torrentPeer"};
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
    QVector<QVariant> fileIds {2, 3};
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

void tst_Model_Relations::load_QVector_QString_rvalue() const
{
    QFETCH_GLOBAL(QString, connection);

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
    QVector<QVariant> fileIds {2, 3};
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
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

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
    QVector<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);

        // Check whether constraints was correctly applied
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
    QFETCH_GLOBAL(QString, connection);

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
    QVector<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);

        // Check whether constraints was correctly applied
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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);

    QVERIFY(torrent->getRelations().empty());

    QVERIFY_EXCEPTION_THROWN(torrent->load("torrentFiles-NON_EXISTENT"),
                             RelationNotFoundError);
    QVERIFY(torrent->getRelations().empty());
}

void tst_Model_Relations::fresh() const
{
    QFETCH_GLOBAL(QString, connection);

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
    QVector<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);

        // Check whether constraints was correctly applied
        const auto &attributes = file->getAttributes();
        QCOMPARE(attributes.size(), 9);

        QCOMPARE(file->getAttribute("torrent_id"), torrent->getKey());
        QVERIFY(fileIds.contains(file->getKey()));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile *));
    }
}

void tst_Model_Relations::fresh_WithSelectConstraint() const
{
    QFETCH_GLOBAL(QString, connection);

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
    QVector<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);

        // Check whether constraints was correctly applied
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
    QFETCH_GLOBAL(QString, connection);

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
    uintptr_t relationFilesKeyOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->first);
    uintptr_t relationFilesValueOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->second);
    uintptr_t relationPeerKeyOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentPeer")->first);
    uintptr_t relationPeerValueOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentPeer")->second);

    torrent->refresh();

    QCOMPARE(relations.size(), 2);
    /* Values in the std::unordered_map container has to be the same, because
       only loaded relations will be replaced with std::move directly
       to the relation std::variant reference in the Model::load() method. */
    QVERIFY(relationFilesKeyOriginal ==
            reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->first));
    QVERIFY(relationFilesValueOriginal ==
            reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->second));
    QVERIFY(relationPeerKeyOriginal ==
            reinterpret_cast<uintptr_t>(&relations.find("torrentPeer")->first));
    QVERIFY(relationPeerValueOriginal ==
            reinterpret_cast<uintptr_t>(&relations.find("torrentPeer")->second));

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
    QFETCH_GLOBAL(QString, connection);

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
    uintptr_t relationFilesKeyOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->first);
    uintptr_t relationFilesValueOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->second);
    uintptr_t relationPeerKeyOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentPeer")->first);
    uintptr_t relationPeerValueOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentPeer")->second);

    torrent->refresh();

    QCOMPARE(relations.size(), 2);
    /* Values in the std::unordered_map container has to be the same, because
       only loaded relations will be replaced with std::move directly
       to the relation std::variant reference in the Model::load() method. */
    QVERIFY(relationFilesKeyOriginal ==
            reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->first));
    QVERIFY(relationFilesValueOriginal ==
            reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->second));
    QVERIFY(relationPeerKeyOriginal ==
            reinterpret_cast<uintptr_t>(&relations.find("torrentPeer")->first));
    QVERIFY(relationPeerValueOriginal ==
            reinterpret_cast<uintptr_t>(&relations.find("torrentPeer")->second));

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
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto files = Torrent::find(5)->torrentFiles()
                 ->where([](auto &query)
    {
        query.whereEq(ID, 6).orWhereEq("file_index", 2);
    })
                 .get();

    QCOMPARE(files.size(), 2);

    // Expected file IDs
    QVector<QVariant> fileIds {6, 8};
    for (auto &file : files) {
        QVERIFY(file.exists);
        QVERIFY(fileIds.contains(file[ID]));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile));
    }
}

void tst_Model_Relations::orWhere_WithCallback() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto files = Torrent::find(5)->torrentFiles()
                 ->where("progress", ">", 990)
                 .orWhere([](auto &query)
    {
        query.whereEq(ID, 8).whereEq("file_index", 2);
    })
                 .get();

    QCOMPARE(files.size(), 2);

    // Expected file IDs
    QVector<QVariant> fileIds {6, 8};
    for (auto &file : files) {
        QVERIFY(file.exists);
        QVERIFY(fileIds.contains(file[ID]));
        QCOMPARE(typeid (file), typeid (TorrentPreviewableFile));
    }
}

void tst_Model_Relations::belongsToMany_allRelatedIds() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(3);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    const auto relatedIds = torrent->tags()->allRelatedIds();

    QCOMPARE(relatedIds.size(), 2);

    const QVector<QVariant> expectedIds {2, 4};

    for (const auto &relatedId : relatedIds)
        QVERIFY(expectedIds.contains(relatedId));
}

void tst_Model_Relations::withoutDefaultModel_LazyLoad_HasOne() const
{
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFile::find(9);
    QVERIFY(torrentFile->exists);

    auto *torrent = torrentFile->getRelationValue<Torrent, One>("torrent");

    QVERIFY(torrent == nullptr);
}

void tst_Model_Relations::withoutDefaultModel_EagerLoad_HasOne() const
{
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrentFile = TorrentPreviewableFileEager_WithDefault::find(9);
    QVERIFY(torrentFile->exists);

    auto *torrent = torrentFile->getRelation<TorrentEager_WithDefault, One>("torrent");

    QVERIFY(torrent == nullptr);
}

void tst_Model_Relations::withDefaultModel_LazyLoad_Bool_HasOne() const
{
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

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

void tst_Model_Relations::has_Basic_QString_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::has("torrentFiles")->get();

    const QVector<QVariant> expectedIds {1, 2, 3, 4, 5};

    for (const auto &torrent : torrents)
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_Model_Relations::has_Basic_UniquePtr_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    // Has to live long enough to avoid dangling reference
    Torrent dummyModel;

    // Ownership of a unique_ptr()
    auto relation =
            Relation<Torrent, TorrentPreviewableFile>::noConstraints(
                [&dummyModel]()
    {
        return std::invoke(&Torrent::torrentFiles, dummyModel);
    });

    auto torrents = Torrent::has<TorrentPreviewableFile, void>(std::move(relation))
                    ->get();

    const QVector<QVariant> expectedIds {1, 2, 3, 4, 5};

    for (const auto &torrent : torrents)
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_Model_Relations::has_Basic_MethodPointer_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::has<TorrentPreviewableFile>(&Torrent::torrentFiles)
                    ->get();

    const QVector<QVariant> expectedIds {1, 2, 3, 4, 5};

    for (const auto &torrent : torrents)
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_Model_Relations::has_Count_QString_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::has("torrentFiles", ">", 1)
                    ->get();

    const QVector<QVariant> expectedIds {2, 5};

    for (const auto &torrent : torrents)
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_Model_Relations::has_Count_UniquePtr_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    // Has to live long enough to avoid dangling reference
    Torrent dummyModel;

    // Ownership of a unique_ptr()
    auto relation =
            Relation<Torrent, TorrentPreviewableFile>::noConstraints(
                [&dummyModel]()
    {
        return std::invoke(&Torrent::torrentFiles, dummyModel);
    });

    auto torrents = Torrent::has<TorrentPreviewableFile, void>(std::move(relation),
                                                               ">=", 2)
                    ->get();

    const QVector<QVariant> expectedIds {2, 5};

    for (const auto &torrent : torrents)
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_Model_Relations::has_Count_MethodPointer_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::has<TorrentPreviewableFile>(&Torrent::torrentFiles, ">=", 2)
                    ->get();

    const QVector<QVariant> expectedIds {2, 5};

    for (const auto &torrent : torrents)
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_Model_Relations::whereHas_Basic_QString_QueryBuilder_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::whereHas("torrentFiles", [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), QueryBuilder &>));

        query.where("filepath", LIKE, "%_file2.mkv");
    })
            ->get();

    const QVector<QVariant> expectedIds {2, 5};

    for (const auto &torrent : torrents)
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_Model_Relations::whereHas_Basic_QString_TinyBuilder_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::whereHas<TorrentPreviewableFile>("torrentFiles",
                                                              [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query),
                                TinyBuilder<TorrentPreviewableFile> &>));

        query.where("filepath", LIKE, "%_file2.mkv");
    })
            ->get();

    const QVector<QVariant> expectedIds {2, 5};

    for (const auto &torrent : torrents)
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_Model_Relations::whereHas_Count_QString_QueryBuilder_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::whereHas("torrentFiles", [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), QueryBuilder &>));

        query.where("progress", ">=", 870);
    }, ">=", 2)
            ->get();

    const QVector<QVariant> expectedIds {2, 5};

    for (const auto &torrent : torrents)
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_Model_Relations::whereHas_Count_QString_TinyBuilder_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::whereHas<TorrentPreviewableFile>("torrentFiles",
                                                              [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query),
                                TinyBuilder<TorrentPreviewableFile> &>));

        query.where("progress", ">=", 870);
    }, ">=", 2)
            ->get();

    const QVector<QVariant> expectedIds {2, 5};

    for (const auto &torrent : torrents)
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_Model_Relations::whereHas_Count_MethodPointer_TinyBuilder_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::whereHas<TorrentPreviewableFile>(&Torrent::torrentFiles,
                                                              [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query),
                                TinyBuilder<TorrentPreviewableFile> &>));

        query.where("progress", ">=", 870);
    }, ">=", 2)
            ->get();

    const QVector<QVariant> expectedIds {2, 5};

    for (const auto &torrent : torrents)
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_Model_Relations::hasNested_Basic_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::has<FilePropertyProperty>(
                        "torrentFiles.fileProperty.filePropertyProperty")
                    ->get();

    const QVector<QVariant> expectedIds {2, 3, 4, 5};

    for (const auto &torrent : torrents)
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_Model_Relations::hasNested_Count_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::has<FilePropertyProperty>(
                        "torrentFiles.fileProperty.filePropertyProperty", ">=", 2)
                    ->get();

    const QVector<QVariant> expectedIds {3, 5};

    for (const auto &torrent : torrents)
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_Model_Relations::hasNested_Count_TinyBuilder_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::has<FilePropertyProperty>(
                        "torrentFiles.fileProperty.filePropertyProperty", ">=", 2, AND,
                        [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query),
                                TinyBuilder<FilePropertyProperty> &>));

        query.where("value", ">=", 6);
    })
            ->get();

    const QVector<QVariant> expectedIds {5};

    for (const auto &torrent : torrents)
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

/* Relation related */

/* Builds Queries */

void tst_Model_Relations::chunk_Relation() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    using SizeType = ModelsCollection<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount>
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 2}, {2, 1}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(3);

    auto result = TorrentPreviewableFileProperty::find(5)->filePropertyProperty()
                  ->orderBy(ID)
                  .chunk(2, [&compareResultSize, &ids]
                            (ModelsCollection<FilePropertyProperty> &&models,
                             const qint64 page)
    {
        compareResultSize(models.size(), page);

        for (auto &&fileProperty : models)
            ids.emplace_back(fileProperty[ID]->template value<quint64>());

        return true;
    });

    QVERIFY(result);

    std::vector<quint64> expectedIds {6, 7, 8};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::each_Relation() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(3);
    std::vector<quint64> ids;
    ids.reserve(3);

    auto result = TorrentPreviewableFileProperty::find(5)->filePropertyProperty()
                  ->orderBy(ID)
                  .each([&indexes, &ids]
                        (FilePropertyProperty &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        return true;
    });

    QVERIFY(result);

    std::vector<qint64> expectedIndexes {0, 1, 2};
    std::vector<quint64> expectedIds {6, 7, 8};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

namespace
{
    /*! Used to compare results from the TinyBuilder::chunkMap() method for
        the FilePropertyProperty model. */
    struct IdAndName
    {
        /*! FilePropertyProperty ID. */
        quint64 id;
        /*! FilePropertyProperty name. */
        QString name;

        /*! Equality comparison operator for the IdAndName. */
        inline bool operator==(const IdAndName &right) const noexcept
        {
            return id == right.id && name == right.name;
        }
    };
} // namespace

void tst_Model_Relations::chunkMap_Relation() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto result = TorrentPreviewableFileProperty::find(5)->filePropertyProperty()
                  ->orderBy(ID)
                  .chunkMap([](FilePropertyProperty &&model)
    {
        auto nameRef = model[NAME];

        // Modify the name attribute
        nameRef = QStringLiteral("%1_mapped").arg(nameRef->template value<QString>());

        return std::move(model);
    });

    QVector<IdAndName> expectedResult {
        {6, "test5_file1_property1_mapped"},
        {7, "test5_file1_property2_mapped"},
        {8, "test5_file1_property3_mapped"},
    };

    // Transform the result so we can compare it
    auto resultTransformed = result
            | ranges::views::transform([](const FilePropertyProperty &model)
                                       -> IdAndName
    {
        return {model.getKeyCasted(),
                model.getAttribute<QString>(NAME)};
    })
            | ranges::to<QVector<IdAndName>>();

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(resultTransformed, expectedResult);
}

void tst_Model_Relations::chunkMap_TemplatedReturnValue_Relation() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto result = TorrentPreviewableFileProperty::find(5)->filePropertyProperty()
                  ->chunkMap<QString>([](FilePropertyProperty &&model)
    {
        // Return the modify name directly
        return QStringLiteral("%1_mapped").arg(model[NAME]->template value<QString>());
    });

    QVector<QString> expectedResult {
        {"test5_file1_property1_mapped"},
        {"test5_file1_property2_mapped"},
        {"test5_file1_property3_mapped"},
    };

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(result, expectedResult);
}

void tst_Model_Relations::chunkById_Relation() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    using SizeType = ModelsCollection<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount>
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 2}, {2, 1}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(3);

    auto result = TorrentPreviewableFileProperty::find(5)->filePropertyProperty()
                  ->orderBy(ID)
                  .chunkById(2, [&compareResultSize, &ids]
                                (ModelsCollection<FilePropertyProperty> &&models,
                                 const qint64 page)
    {
        compareResultSize(models.size(), page);

        for (auto &&fileProperty : models)
            ids.emplace_back(fileProperty.getKeyCasted());

        return true;
    });

    QVERIFY(result);

    std::vector<quint64> expectedIds {6, 7, 8};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::chunkById_WithAlias_Relation() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    using SizeType = ModelsCollection<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount>
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 2}, {2, 1}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(3);

    auto result = TorrentPreviewableFileProperty::find(5)->filePropertyProperty()
                  ->select({ASTERISK, "id as id_as"})
                  .orderBy(ID)
                  .chunkById(2, [&compareResultSize, &ids]
                                (ModelsCollection<FilePropertyProperty> &&models,
                                 const qint64 page)
    {
        compareResultSize(models.size(), page);

        for (auto &&tag : models)
            ids.emplace_back(tag.getKeyCasted());

        return true;
    },
            ID, "id_as");

    QVERIFY(result);

    std::vector<quint64> expectedIds {6, 7, 8};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::eachById_Relation() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(3);
    std::vector<quint64> ids;
    ids.reserve(3);

    auto result = TorrentPreviewableFileProperty::find(5)->filePropertyProperty()
                  ->orderBy(ID)
                  .eachById([&indexes, &ids]
                            (FilePropertyProperty &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        return true;
    });

    QVERIFY(result);

    std::vector<qint64> expectedIndexes {0, 1, 2};
    std::vector<quint64> expectedIds {6, 7, 8};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::eachById_WithAlias_Relation() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(3);
    std::vector<quint64> ids;
    ids.reserve(3);

    auto result = TorrentPreviewableFileProperty::find(5)->filePropertyProperty()
                  ->select({ASTERISK, "id as id_as"})
                  .orderBy(ID)
                  .eachById([&indexes, &ids]
                            (FilePropertyProperty &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        return true;
    },
            1000, ID, "id_as");

    QVERIFY(result);

    std::vector<qint64> expectedIndexes {0, 1, 2};
    std::vector<quint64> expectedIds {6, 7, 8};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::find() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto tag = Torrent::find(3)->tags()->find(2);
    QVERIFY(tag);
    QCOMPARE(tag->getKey(), QVariant(2));
    QCOMPARE(tag->getAttribute(NAME), QVariant("tag2"));
}

void tst_Model_Relations::findOr() const
{
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto tag = Torrent::find(3)->tags()->whereKey(2).first();
    QVERIFY(tag);
    QCOMPARE(tag->getKey(), QVariant(2));
    QCOMPARE(tag->getAttribute(NAME), QVariant("tag2"));
}

void tst_Model_Relations::firstOr() const
{
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto tag = Torrent::find(2)->tags()
               ->whereRowValuesEq({ID, NAME}, {1, "tag1"})
               .first();

    QVERIFY(tag);
    QCOMPARE(tag->getKey(), QVariant(1));
    QCOMPARE(tag->getAttribute(NAME), QVariant("tag1"));
}

/* Builds Queries */

namespace
{
    /*! Verify whether the custom Tagged pivot attribute was correctly hydrated. */
    const auto verifyTaggedPivot = [](Tag &tag)
    {
        /* Custom Pivot relation as the Tagged class, under the 'tagged' key
           in the m_relations hash. */
        auto *tagged = tag.getRelation<Tagged, One>("tagged");
        QVERIFY(tagged);
        QVERIFY(tagged->exists);
        QCOMPARE(typeid (tagged), typeid (Tagged *));

        QCOMPARE(tagged->getForeignKey(), QString("torrent_id"));
        QCOMPARE(tagged->getRelatedKey(), QString("tag_id"));

        const auto &attributesHash = tagged->getAttributesHash();

        QCOMPARE(attributesHash.size(), static_cast<std::size_t>(5));

        QCOMPARE(tagged->getAttribute("torrent_id"),
                 QVariant(static_cast<quint64>(2)));
    };
} // namespace

void tst_Model_Relations::chunk() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    using SizeType = ModelsCollection<Tag>::size_type;

    // <page, chunk_modelsCount>
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 2}, {2, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(4);

    auto result = Torrent::find(2)->tags()->orderBy(ID)
                  .chunk(2, [&compareResultSize, &ids]
                            (ModelsCollection<Tag> &&models, const qint64 page)
    {
        compareResultSize(models.size(), page);

        for (auto &&tag : models) {
            ids.emplace_back(tag[ID]->template value<quint64>());

            verifyTaggedPivot(tag);
        }

        return true;
    });

    QVERIFY(result);

    std::vector<quint64> expectedIds {1, 2, 3, 4};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::chunk_ReturnFalse() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    using SizeType = ModelsCollection<Tag>::size_type;

    // <page, chunk_modelsCount> (I leave it here also in this test, doesn't matter much)
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 2}, {2, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(2);

    auto result = Torrent::find(2)->tags()->orderBy(ID)
                  .chunk(2, [&compareResultSize, &ids]
                            (ModelsCollection<Tag> &&models, const qint64 page)
    {
        compareResultSize(models.size(), page);

        for (auto &&tag : models) {
            auto id = tag[ID]->template value<quint64>();
            ids.emplace_back(id);

            verifyTaggedPivot(tag);

            // Interrupt chunk-ing
            if (id == 2)
                return false;
        }

        return true;
    });

    QVERIFY(!result);

    std::vector<quint64> expectedIds {1, 2};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::chunk_EnforceOrderBy() const
{
    /* The TinBuilder version doesn't throws exception if the 'order by' clause is not
       specified, instead it adds a generic 'order by' clause
       on the Model::getQualifiedKeyName() (it sorts by the primary key by default). */
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    using SizeType = ModelsCollection<Tag>::size_type;

    // <page, chunk_modelsCount>
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 2}, {2, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(4);

    auto result = Torrent::find(2)->tags()
                  ->chunk(2, [&compareResultSize, &ids]
                             (ModelsCollection<Tag> &&models, const qint64 page)
    {
        compareResultSize(models.size(), page);

        for (auto &&tag : models) {
            ids.emplace_back(tag[ID]->template value<quint64>());

            verifyTaggedPivot(tag);
        }

        return true;
    });

    QVERIFY(result);

    std::vector<quint64> expectedIds {1, 2, 3, 4};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::chunk_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto callbackInvoked = false;

    auto result = Torrent::find(2)->tags()->whereEq("torrent_tags.name",
                                                    QStringLiteral("dummy-NON_EXISTENT"))
                  .orderBy(ID)
                  .chunk(2, [&callbackInvoked]
                            (ModelsCollection<Tag> &&/*unused*/, const qint64 /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Model_Relations::each() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(4);
    std::vector<quint64> ids;
    ids.reserve(4);

    auto result = Torrent::find(2)->tags()->orderBy(ID)
                  .each([&indexes, &ids]
                        (Tag &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        verifyTaggedPivot(model);

        return true;
    });

    QVERIFY(result);

    std::vector<qint64> expectedIndexes {0, 1, 2, 3};
    std::vector<quint64> expectedIds {1, 2, 3, 4};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::each_ReturnFalse() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(2);
    std::vector<quint64> ids;
    ids.reserve(2);

    auto result = Torrent::find(2)->tags()->orderBy(ID)
                  .each([&indexes, &ids]
                        (Tag &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        verifyTaggedPivot(model);

        return index != 1; // false/interrupt on 1
    });

    QVERIFY(!result);

    std::vector<qint64> expectedIndexes {0, 1};
    std::vector<quint64> expectedIds {1, 2};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::each_EnforceOrderBy() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    /* The TinBuilder version doesn't throws exception if the 'order by' clause is not
       specified, instead it adds a generic 'order by' clause
       on the Model::getQualifiedKeyName() (it sorts by the primary key by default). */
    std::vector<qint64> indexes;
    indexes.reserve(4);
    std::vector<quint64> ids;
    ids.reserve(4);

    auto result = Torrent::find(2)->tags()
                  ->each([&indexes, &ids]
                         (Tag &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        verifyTaggedPivot(model);

        return true;
    });

    QVERIFY(result);

    std::vector<qint64> expectedIndexes {0, 1, 2, 3};
    std::vector<quint64> expectedIds {1, 2, 3, 4};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::each_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto callbackInvoked = false;

    auto result = Torrent::find(2)->tags()->whereEq("torrent_tags.name",
                                                    QStringLiteral("dummy-NON_EXISTENT"))
                  .orderBy(ID)
                  .each([&callbackInvoked]
                        (Tag &&/*unused*/, const qint64 /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Model_Relations::chunkMap() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto result = Torrent::find(2)->tags()->orderBy(ID)
                  .chunkMap([](Tag &&model)
    {
        auto nameRef = model[NAME];

        // Modify the name attribute
        nameRef = QStringLiteral("%1_mapped").arg(nameRef->template value<QString>());

        verifyTaggedPivot(model);

        return std::move(model);
    });

    QVector<IdAndName> expectedResult {
        {1, "tag1_mapped"},
        {2, "tag2_mapped"},
        {3, "tag3_mapped"},
        {4, "tag4_mapped"},
    };

    // Transform the result so we can compare it
    auto resultTransformed = result
            | ranges::views::transform([](const Tag &model) -> IdAndName
    {
        return {model.getKeyCasted(),
                model.getAttribute<QString>(NAME)};
    })
            | ranges::to<QVector<IdAndName>>();

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(resultTransformed, expectedResult);
}

void tst_Model_Relations::chunkMap_EnforceOrderBy() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    /* The TinBuilder version doesn't throws exception if the 'order by' clause is not
       specified, instead it adds a generic 'order by' clause
       on the Model::getQualifiedKeyName() (it sorts by the primary key by default). */
    auto result = Torrent::find(2)->tags()->chunkMap([](Tag &&model)
    {
        auto nameRef = model[NAME];

        // Modify the name attribute
        nameRef = QStringLiteral("%1_mapped").arg(nameRef->template value<QString>());

        verifyTaggedPivot(model);

        return std::move(model);
    });

    QVector<IdAndName> expectedResult {
        {1, "tag1_mapped"},
        {2, "tag2_mapped"},
        {3, "tag3_mapped"},
        {4, "tag4_mapped"},
    };

    // Transform the result so I can compare it
    auto resultTransformed = result
            | ranges::views::transform([](const Tag &model) -> IdAndName
    {
        return {model.getKeyCasted(),
                model.getAttribute<QString>(NAME)};
    })
            | ranges::to<QVector<IdAndName>>();

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(resultTransformed, expectedResult);
}

void tst_Model_Relations::chunkMap_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto callbackInvoked = false;

    auto result = Torrent::find(2)->tags()
                  ->whereEq("torrent_tags.name", QStringLiteral("dummy-NON_EXISTENT"))
                  .chunkMap([&callbackInvoked](Tag &&model)
    {
        callbackInvoked = true;

        return std::move(model);
    });

    QVERIFY(!callbackInvoked);
    QVERIFY((std::is_same_v<decltype (result), ModelsCollection<Tag>>));
    QVERIFY(result.isEmpty());
}

void tst_Model_Relations::chunkMap_TemplatedReturnValue() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    // Ownership of a unique_ptr()
    const auto relation = Torrent::find(2)->tags();

    relation->orderBy(ID);

    auto result = relation->chunkMap<QString>([](Tag &&model)
    {
        verifyTaggedPivot(model);

        // Return the modify name directly
        return QStringLiteral("%1_mapped").arg(model[NAME]->template value<QString>());
    });

    QVector<QString> expectedResult {
        {"tag1_mapped"},
        {"tag2_mapped"},
        {"tag3_mapped"},
        {"tag4_mapped"},
    };

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(result, expectedResult);
}

void tst_Model_Relations::chunkMap_TemplatedReturnValue_OnRelationRef() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    /* Even if the chunkMap<> is called on the Relation & it can't crash or fail, it
       should normally works but the pivot table will not be hydrated. */
    auto result = Torrent::find(2)->tags()->orderBy(ID)
                  .chunkMap<QString>([](Tag &&model)
    {
        // Pivot table is not hydrated
        Q_ASSERT(!model.relationLoaded("tagged"));

        // Return the modify name directly
        return QStringLiteral("%1_mapped").arg(model[NAME]->template value<QString>());
    });

    QVector<QString> expectedResult {
        {"tag1_mapped"},
        {"tag2_mapped"},
        {"tag3_mapped"},
        {"tag4_mapped"},
    };

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(result, expectedResult);
}

void
tst_Model_Relations::chunkMap_EnforceOrderBy_TemplatedReturnValue() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    /* The TinBuilder version doesn't throws exception if the 'order by' clause is not
       specified, instead it adds a generic 'order by' clause
       on the Model::getQualifiedKeyName() (it sorts by the primary key by default). */

    // Ownership of a unique_ptr()
    const auto relation = Torrent::find(2)->tags();

    auto result = relation->chunkMap<QString>([](Tag &&model)
    {
        verifyTaggedPivot(model);

        // Return the modify name directly
        return QStringLiteral("%1_mapped").arg(model[NAME]->template value<QString>());
    });

    QVector<QString> expectedResult {
        {"tag1_mapped"},
        {"tag2_mapped"},
        {"tag3_mapped"},
        {"tag4_mapped"},
    };

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(result, expectedResult);
}

void tst_Model_Relations::chunkMap_EmptyResult_TemplatedReturnValue() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto callbackInvoked = false;

    // Ownership of a unique_ptr()
    const auto relation = Torrent::find(2)->tags();

    relation->whereEq("torrent_tags.name", QStringLiteral("dummy-NON_EXISTENT"));

    auto result = relation->chunkMap<QString>([&callbackInvoked](Tag &&/*unused*/)
                                              -> QString
    {
        callbackInvoked = true;

        return {};
    });

    QVERIFY(!callbackInvoked);
    QVERIFY((std::is_same_v<decltype (result), QVector<QString>>));
    QVERIFY(result.isEmpty());
}

void tst_Model_Relations::chunkById() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    using SizeType = ModelsCollection<Tag>::size_type;

    // <page, chunk_modelsCount>
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 2}, {2, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(4);

    auto result = Torrent::find(2)->tags()->orderBy(ID)
                  .chunkById(2, [&compareResultSize, &ids]
                                (ModelsCollection<Tag> &&models, const qint64 page)
    {
        compareResultSize(models.size(), page);

        for (auto &&tag : models) {
            ids.emplace_back(tag.getKeyCasted());

            verifyTaggedPivot(tag);
        }

        return true;
    });

    QVERIFY(result);

    std::vector<quint64> expectedIds {1, 2, 3, 4};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::chunkById_ReturnFalse() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    using SizeType = ModelsCollection<Tag>::size_type;

    // <page, chunk_modelsCount> (I leave it here also in this test, doesn't matter much
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 2}, {2, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(2);

    auto result = Torrent::find(2)->tags()->orderBy(ID)
                  .chunkById(2, [&compareResultSize, &ids]
                                (ModelsCollection<Tag> &&models, const qint64 page)
    {
        compareResultSize(models.size(), page);

        for (auto &&tag : models) {
            auto id = tag.getKeyCasted();
            ids.emplace_back(id);

            verifyTaggedPivot(tag);

            // Interrupt chunk-ing
            if (id == 2)
                return false;
        }

        return true;
    });

    QVERIFY(!result);

    std::vector<quint64> expectedIds {1, 2};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::chunkById_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto callbackInvoked = false;

    auto result = Torrent::find(2)->tags()->whereEq("torrent_tags.name",
                                                    QStringLiteral("dummy-NON_EXISTENT"))
                  .orderBy(ID)
                  .chunkById(2, [&callbackInvoked]
                                (ModelsCollection<Tag> &&/*unused*/,
                                 const qint64 /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Model_Relations::chunkById_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    using SizeType = ModelsCollection<Tag>::size_type;

    // <page, chunk_modelsCount>
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 2}, {2, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(4);

    auto result = Torrent::find(2)->tags()
                  ->select({ASTERISK, "torrent_tags.id as id_as"})
                  .orderBy(ID)
                  .chunkById(2, [&compareResultSize, &ids]
                                (ModelsCollection<Tag> &&models, const qint64 page)
    {
        compareResultSize(models.size(), page);

        for (auto &&tag : models) {
            ids.emplace_back(tag.getKeyCasted());

            verifyTaggedPivot(tag);
        }

        return true;
    },
        ID, "id_as");

    QVERIFY(result);

    std::vector<quint64> expectedIds {1, 2, 3, 4};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::chunkById_ReturnFalse_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    using SizeType = ModelsCollection<Tag>::size_type;

    // <page, chunk_modelsCount> (I leave it here also in this test, doesn't matter much
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 2}, {2, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(2);

    auto result = Torrent::find(2)->tags()
                  ->select({ASTERISK, "torrent_tags.id as id_as"})
                  .orderBy(ID)
                  .chunkById(2, [&compareResultSize, &ids]
                                (ModelsCollection<Tag> &&models, const qint64 page)
    {
        compareResultSize(models.size(), page);

        for (auto &&tag : models) {
            auto id = tag.getKeyCasted();
            ids.emplace_back(id);

            verifyTaggedPivot(tag);

            // Interrupt chunk-ing
            if (id == 2)
                return false;
        }

        return true;
    },
        ID, "id_as");

    QVERIFY(!result);

    std::vector<quint64> expectedIds {1, 2};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::chunkById_EmptyResult_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto callbackInvoked = false;

    auto result = Torrent::find(2)->tags()
                  ->select({ASTERISK, "torrent_tags.id as id_as"})
                  .whereEq("torrent_tags.name", QStringLiteral("dummy-NON_EXISTENT"))
                  .orderBy(ID)
                  .chunkById(2, [&callbackInvoked]
                                (ModelsCollection<Tag> &&/*unused*/,
                                 const qint64 /*unused*/)
    {
        callbackInvoked = true;

        return true;
    },
        ID, "id_as");

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Model_Relations::eachById() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(4);
    std::vector<quint64> ids;
    ids.reserve(4);

    auto result = Torrent::find(2)->tags()
                  ->orderBy(ID)
                  .eachById([&indexes, &ids]
                            (Tag &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        verifyTaggedPivot(model);

        return true;
    });

    QVERIFY(result);

    std::vector<qint64> expectedIndexes {0, 1, 2, 3};
    std::vector<quint64> expectedIds {1, 2, 3, 4};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::eachById_ReturnFalse() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(2);
    std::vector<quint64> ids;
    ids.reserve(2);

    auto result = Torrent::find(2)->tags()
                  ->orderBy(ID)
                  .eachById([&indexes, &ids]
                            (Tag &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        verifyTaggedPivot(model);

        return index != 1; // false/interrupt on 1
    });

    QVERIFY(!result);

    std::vector<qint64> expectedIndexes {0, 1};
    std::vector<quint64> expectedIds {1, 2};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::eachById_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto callbackInvoked = false;

    auto result = Torrent::find(2)->tags()->whereEq("torrent_tags.name",
                                                    QStringLiteral("dummy-NON_EXISTENT"))
                  .orderBy(ID)
                  .eachById([&callbackInvoked]
                            (Tag &&/*unused*/, const qint64 /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Model_Relations::eachById_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(4);
    std::vector<quint64> ids;
    ids.reserve(4);

    auto result = Torrent::find(2)->tags()
                  ->select({ASTERISK, "torrent_tags.id as id_as"})
                  .orderBy(ID)
                  .eachById([&indexes, &ids]
                            (Tag &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        verifyTaggedPivot(model);

        return true;
    },
        1000, ID, "id_as");

    QVERIFY(result);

    std::vector<qint64> expectedIndexes {0, 1, 2, 3};
    std::vector<quint64> expectedIds {1, 2, 3, 4};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::eachById_ReturnFalse_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(2);
    std::vector<quint64> ids;
    ids.reserve(2);

    auto result = Torrent::find(2)->tags()
                  ->select({ASTERISK, "torrent_tags.id as id_as"})
                  .orderBy(ID)
                  .eachById([&indexes, &ids]
                            (Tag &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        verifyTaggedPivot(model);

        return index != 1; // false/interrupt on 1
    },
        1000, ID, "id_as");

    QVERIFY(!result);

    std::vector<qint64> expectedIndexes {0, 1};
    std::vector<quint64> expectedIds {1, 2};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Relations::eachById_EmptyResult_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto callbackInvoked = false;

    auto result = Torrent::find(2)->tags()
                  ->select({ASTERISK, "torrent_tags.id as id_as"})
                  .whereEq("torrent_tags.name", QStringLiteral("dummy-NON_EXISTENT"))
                  .orderBy(ID)
                  .eachById([&callbackInvoked]
                            (Tag &&/*unused*/, const qint64 /*unused*/)
    {
        callbackInvoked = true;

        return true;
    },
        1000, ID, "id_as");

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Model_Relations::upsert() const
{
    QFETCH_GLOBAL(QString, connection);

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

        QVector<QVector<QVariant>> result;
        result.reserve(tagPropertiesSize);

        for (const auto &tagProperty : tagProperties)
            result.append({tagProperty.getAttribute("color"),
                           tagProperty.getAttribute("position"),
                           tagProperty.getAttribute("tag_id")});

        QVector<QVector<QVariant>> expextedResult {
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
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrentFile = Torrent::find(1)->torrentFiles()
                       ->withCasts({{"progress", Orm::Tiny::CastType::UInteger}})
                       .first();

    QVERIFY(torrentFile);
    QVERIFY(torrentFile->exists);
    QCOMPARE(torrentFile->getKeyCasted(), 1);

    auto attribute = torrentFile->getAttribute("progress");

    auto typeId = Helpers::qVariantTypeId(attribute);

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

    QCOMPARE(attribute.value<uint>(), 200);
}

void tst_Model_Relations::withCasts_OnRelation_ManyToMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto tag = Torrent::find(2)->tags()
               ->orderBy(ID)
               .withCasts({{ID, Orm::Tiny::CastType::UInt}})
               .first();

    QVERIFY(tag);
    QVERIFY(tag->exists);
    QCOMPARE(tag->getKeyCasted(), 1);

    auto attribute = tag->getKey();

    auto typeId = Helpers::qVariantTypeId(attribute);

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

    QCOMPARE(attribute.value<uint>(), 1);
}

void tst_Model_Relations::u_casts_OnCustomPivotModel_ManyToMany() const
{
    QFETCH_GLOBAL(QString, connection);

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

    auto typeId = Helpers::qVariantTypeId(attribute);

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
    QFETCH_GLOBAL(QString, connection);

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

    QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

    /* The time zone must be as is defined in the qt_timezone connection
       configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
       behavior of all QtSql drivers. */
    const auto timestampActual = timestampDbVariant.value<QDateTime>();
    const auto timestampExpected = QDateTime({2021, 2, 21}, {17, 31, 58}, Qt::UTC);

    QCOMPARE(timestampActual, timestampExpected);
    QCOMPARE(timestampActual, timestampExpected.toLocalTime());
    QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
}

/* QtTimeZoneType::DontConvert */

/* Server timezone UTC */

void tst_Model_Relations::
timezone_TimestampAttribute_UtcOnServer_DontConvert_OnCustomPivot_MtM() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

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

    QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

    /* The time zone must be as is defined in the qt_timezone connection
       configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
       behavior of all QtSql drivers. */
    const auto timestampActual = timestampDbVariant.value<QDateTime>();
    const auto timestampExpected = QDateTime({2021, 2, 21}, {17, 31, 58});

    QCOMPARE(timestampActual, timestampExpected);
    QCOMPARE(timestampActual, timestampExpected.toLocalTime());
    QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());

    // Restore
    DB::setQtTimeZone(QtTimeZoneConfig {QtTimeZoneType::QtTimeSpec,
                                        QVariant::fromValue(Qt::UTC)}, connection);
    QCOMPARE(DB::qtTimeZone(connection),
             (QtTimeZoneConfig {QtTimeZoneType::QtTimeSpec,
                                QVariant::fromValue(Qt::UTC)}));
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_Model_Relations)

#include "tst_model_relations.moc"
