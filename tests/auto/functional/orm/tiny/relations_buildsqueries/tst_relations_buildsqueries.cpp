#include <QCoreApplication>
#include <QTest>

#include "databases.hpp"
#include "macros.hpp"

#include "models/torrent.hpp"

using Orm::Constants::ASTERISK;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::dummy_NONEXISTENT;

using Orm::One;

using TypeUtils = Orm::Utils::Type;

using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::Types::ModelsCollection;

using TestUtils::Databases;

using Models::FilePropertyProperty;
using Models::Tag;
using Models::Tagged;
using Models::Torrent;
using Models::TorrentPreviewableFileProperty;

class tst_Relations_BuildsQueries : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase_data() const;

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
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Relations_BuildsQueries::initTestCase_data() const
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

/* Relation related */

/* Builds Queries */

void tst_Relations_BuildsQueries::chunk_Relation() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
                            (ModelsCollection<FilePropertyProperty> &&models, // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::each_Relation() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(3);
    std::vector<quint64> ids;
    ids.reserve(3);

    auto result = TorrentPreviewableFileProperty::find(5)->filePropertyProperty()
                  ->orderBy(ID)
                  .each([&indexes, &ids]
                        (FilePropertyProperty &&model, const qint64 index) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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
        bool operator==(const IdAndName &right) const noexcept
        {
            return id == right.id && name == right.name;
        }
    };
} // namespace

void tst_Relations_BuildsQueries::chunkMap_Relation() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto result = TorrentPreviewableFileProperty::find(5)->filePropertyProperty()
                  ->orderBy(ID)
                  .chunkMap([](FilePropertyProperty &&model)
    {
        auto nameRef = model[NAME];

        // Modify the name attribute
        nameRef = sl("%1_mapped").arg(nameRef->template value<QString>());

        return std::move(model);
    });

    QList<IdAndName> expectedResult {
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
            | ranges::to<QList<IdAndName>>();

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(resultTransformed, expectedResult);
}

void tst_Relations_BuildsQueries::chunkMap_TemplatedReturnValue_Relation() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto result = TorrentPreviewableFileProperty::find(5)->filePropertyProperty()
                  ->chunkMap<QString>([](FilePropertyProperty &&model) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        // Return the modify name directly
        return sl("%1_mapped").arg(model[NAME]->template value<QString>());
    });

    QList<QString> expectedResult {
        {"test5_file1_property1_mapped"},
        {"test5_file1_property2_mapped"},
        {"test5_file1_property3_mapped"},
    };

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(result, expectedResult);
}

void tst_Relations_BuildsQueries::chunkById_Relation() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
                                (ModelsCollection<FilePropertyProperty> &&models, // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::chunkById_WithAlias_Relation() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
                                (ModelsCollection<FilePropertyProperty> &&models, // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::eachById_Relation() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(3);
    std::vector<quint64> ids;
    ids.reserve(3);

    auto result = TorrentPreviewableFileProperty::find(5)->filePropertyProperty()
                  ->orderBy(ID)
                  .eachById([&indexes, &ids]
                            (FilePropertyProperty &&model, const qint64 index) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::eachById_WithAlias_Relation() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(3);
    std::vector<quint64> ids;
    ids.reserve(3);

    auto result = TorrentPreviewableFileProperty::find(5)->filePropertyProperty()
                  ->select({ASTERISK, "id as id_as"})
                  .orderBy(ID)
                  .eachById([&indexes, &ids]
                            (FilePropertyProperty &&model, const qint64 index) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::chunk() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
                            (ModelsCollection<Tag> &&models, const qint64 page) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::chunk_ReturnFalse() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
                            (ModelsCollection<Tag> &&models, const qint64 page) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::chunk_EnforceOrderBy() const
{
    /* The TinBuilder version doesn't throws exception if the 'order by' clause is not
       specified, instead it adds a generic 'order by' clause
       on the Model::getQualifiedKeyName() (it sorts by the primary key by default). */
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
                             (ModelsCollection<Tag> &&models, const qint64 page) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::chunk_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto callbackInvoked = false;

    auto result = Torrent::find(2)->tags()->whereEq("torrent_tags.name",
                                                    dummy_NONEXISTENT)
                  .orderBy(ID)
                  .chunk(2, [&callbackInvoked]
                            (ModelsCollection<Tag> &&/*unused*/, const qint64 /*unused*/) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Relations_BuildsQueries::each() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(4);
    std::vector<quint64> ids;
    ids.reserve(4);

    auto result = Torrent::find(2)->tags()->orderBy(ID)
                  .each([&indexes, &ids]
                        (Tag &&model, const qint64 index) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::each_ReturnFalse() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(2);
    std::vector<quint64> ids;
    ids.reserve(2);

    auto result = Torrent::find(2)->tags()->orderBy(ID)
                  .each([&indexes, &ids]
                        (Tag &&model, const qint64 index) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::each_EnforceOrderBy() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
                         (Tag &&model, const qint64 index) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::each_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto callbackInvoked = false;

    auto result = Torrent::find(2)->tags()->whereEq("torrent_tags.name",
                                                    dummy_NONEXISTENT)
                  .orderBy(ID)
                  .each([&callbackInvoked]
                        (Tag &&/*unused*/, const qint64 /*unused*/) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Relations_BuildsQueries::chunkMap() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto result = Torrent::find(2)->tags()->orderBy(ID)
                  .chunkMap([](Tag &&model)
    {
        auto nameRef = model[NAME];

        // Modify the name attribute
        nameRef = sl("%1_mapped").arg(nameRef->template value<QString>());

        verifyTaggedPivot(model);

        return std::move(model);
    });

    QList<IdAndName> expectedResult {
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
            | ranges::to<QList<IdAndName>>();

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(resultTransformed, expectedResult);
}

void tst_Relations_BuildsQueries::chunkMap_EnforceOrderBy() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    /* The TinBuilder version doesn't throws exception if the 'order by' clause is not
       specified, instead it adds a generic 'order by' clause
       on the Model::getQualifiedKeyName() (it sorts by the primary key by default). */
    auto result = Torrent::find(2)->tags()->chunkMap([](Tag &&model)
    {
        auto nameRef = model[NAME];

        // Modify the name attribute
        nameRef = sl("%1_mapped").arg(nameRef->template value<QString>());

        verifyTaggedPivot(model);

        return std::move(model);
    });

    QList<IdAndName> expectedResult {
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
            | ranges::to<QList<IdAndName>>();

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(resultTransformed, expectedResult);
}

void tst_Relations_BuildsQueries::chunkMap_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto callbackInvoked = false;

    auto result = Torrent::find(2)->tags()
                  ->whereEq("torrent_tags.name", dummy_NONEXISTENT)
                  .chunkMap([&callbackInvoked](Tag &&model)
    {
        callbackInvoked = true;

        return std::move(model);
    });

    QVERIFY(!callbackInvoked);
    QVERIFY((std::is_same_v<decltype (result), ModelsCollection<Tag>>));
    QVERIFY(result.isEmpty());
}

void tst_Relations_BuildsQueries::chunkMap_TemplatedReturnValue() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Ownership of a unique_ptr()
    const auto relation = Torrent::find(2)->tags();

    relation->orderBy(ID);

    auto result = relation->chunkMap<QString>([](Tag &&model) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        verifyTaggedPivot(model);

        // Return the modify name directly
        return sl("%1_mapped").arg(model[NAME]->template value<QString>());
    });

    QList<QString> expectedResult {
        {"tag1_mapped"},
        {"tag2_mapped"},
        {"tag3_mapped"},
        {"tag4_mapped"},
    };

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(result, expectedResult);
}

void tst_Relations_BuildsQueries::chunkMap_TemplatedReturnValue_OnRelationRef() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    /* Even if the chunkMap<> is called on the Relation & it can't crash or fail, it
       should normally works but the pivot table will not be hydrated. */
    auto result = Torrent::find(2)->tags()->orderBy(ID)
                  .chunkMap<QString>([](Tag &&model) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        // Pivot table is not hydrated
        Q_ASSERT(!model.relationLoaded("tagged"));

        // Return the modify name directly
        return sl("%1_mapped").arg(model[NAME]->template value<QString>());
    });

    QList<QString> expectedResult {
        {"tag1_mapped"},
        {"tag2_mapped"},
        {"tag3_mapped"},
        {"tag4_mapped"},
    };

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(result, expectedResult);
}

void
tst_Relations_BuildsQueries::chunkMap_EnforceOrderBy_TemplatedReturnValue() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    /* The TinBuilder version doesn't throws exception if the 'order by' clause is not
       specified, instead it adds a generic 'order by' clause
       on the Model::getQualifiedKeyName() (it sorts by the primary key by default). */

    // Ownership of a unique_ptr()
    const auto relation = Torrent::find(2)->tags();

    auto result = relation->chunkMap<QString>([](Tag &&model) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        verifyTaggedPivot(model);

        // Return the modify name directly
        return sl("%1_mapped").arg(model[NAME]->template value<QString>());
    });

    QList<QString> expectedResult {
        {"tag1_mapped"},
        {"tag2_mapped"},
        {"tag3_mapped"},
        {"tag4_mapped"},
    };

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(result, expectedResult);
}

void tst_Relations_BuildsQueries::chunkMap_EmptyResult_TemplatedReturnValue() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto callbackInvoked = false;

    // Ownership of a unique_ptr()
    const auto relation = Torrent::find(2)->tags();

    relation->whereEq("torrent_tags.name", dummy_NONEXISTENT);

    auto result = relation->chunkMap<QString>([&callbackInvoked](Tag &&/*unused*/) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
                                              -> QString
    {
        callbackInvoked = true;

        return {};
    });

    QVERIFY(!callbackInvoked);
    QVERIFY((std::is_same_v<decltype (result), QList<QString>>));
    QVERIFY(result.isEmpty());
}

void tst_Relations_BuildsQueries::chunkById() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
                                (ModelsCollection<Tag> &&models, const qint64 page) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::chunkById_ReturnFalse() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
                                (ModelsCollection<Tag> &&models, const qint64 page) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::chunkById_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto callbackInvoked = false;

    auto result = Torrent::find(2)->tags()->whereEq("torrent_tags.name",
                                                    dummy_NONEXISTENT)
                  .orderBy(ID)
                  .chunkById(2, [&callbackInvoked]
                                (ModelsCollection<Tag> &&/*unused*/, // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
                                 const qint64 /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Relations_BuildsQueries::chunkById_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
                                (ModelsCollection<Tag> &&models, const qint64 page) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::chunkById_ReturnFalse_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
                                (ModelsCollection<Tag> &&models, const qint64 page) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::chunkById_EmptyResult_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto callbackInvoked = false;

    auto result = Torrent::find(2)->tags()
                  ->select({ASTERISK, "torrent_tags.id as id_as"})
                  .whereEq("torrent_tags.name", dummy_NONEXISTENT)
                  .orderBy(ID)
                  .chunkById(2, [&callbackInvoked]
                                (ModelsCollection<Tag> &&/*unused*/, // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
                                 const qint64 /*unused*/)
    {
        callbackInvoked = true;

        return true;
    },
        ID, "id_as");

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Relations_BuildsQueries::eachById() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(4);
    std::vector<quint64> ids;
    ids.reserve(4);

    auto result = Torrent::find(2)->tags()
                  ->orderBy(ID)
                  .eachById([&indexes, &ids]
                            (Tag &&model, const qint64 index) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::eachById_ReturnFalse() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(2);
    std::vector<quint64> ids;
    ids.reserve(2);

    auto result = Torrent::find(2)->tags()
                  ->orderBy(ID)
                  .eachById([&indexes, &ids]
                            (Tag &&model, const qint64 index) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::eachById_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto callbackInvoked = false;

    auto result = Torrent::find(2)->tags()->whereEq("torrent_tags.name",
                                                    dummy_NONEXISTENT)
                  .orderBy(ID)
                  .eachById([&callbackInvoked]
                            (Tag &&/*unused*/, const qint64 /*unused*/) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Relations_BuildsQueries::eachById_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(4);
    std::vector<quint64> ids;
    ids.reserve(4);

    auto result = Torrent::find(2)->tags()
                  ->select({ASTERISK, "torrent_tags.id as id_as"})
                  .orderBy(ID)
                  .eachById([&indexes, &ids]
                            (Tag &&model, const qint64 index) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::eachById_ReturnFalse_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    std::vector<qint64> indexes;
    indexes.reserve(2);
    std::vector<quint64> ids;
    ids.reserve(2);

    auto result = Torrent::find(2)->tags()
                  ->select({ASTERISK, "torrent_tags.id as id_as"})
                  .orderBy(ID)
                  .eachById([&indexes, &ids]
                            (Tag &&model, const qint64 index) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

void tst_Relations_BuildsQueries::eachById_EmptyResult_WithAlias() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto callbackInvoked = false;

    auto result = Torrent::find(2)->tags()
                  ->select({ASTERISK, "torrent_tags.id as id_as"})
                  .whereEq("torrent_tags.name", dummy_NONEXISTENT)
                  .orderBy(ID)
                  .eachById([&callbackInvoked]
                            (Tag &&/*unused*/, const qint64 /*unused*/) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        callbackInvoked = true;

        return true;
    },
        1000, ID, "id_as");

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_Relations_BuildsQueries)

#include "tst_relations_buildsqueries.moc"
