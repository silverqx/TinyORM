#include <QCoreApplication>
#include <QtSql/QSqlDriver>
#include <QtTest>

#include "common/collection.hpp"
#include "databases.hpp"

#include "models/album.hpp"

using Orm::Constants::COMMA;
using Orm::Constants::GE;
using Orm::Constants::GT;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::NE;
using Orm::Constants::SIZE_;
using Orm::Constants::SPACE_IN;

using Orm::Exceptions::InvalidArgumentError;
using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::Exceptions::RelationNotFoundError;
using Orm::Tiny::Types::ModelsCollection;
using Orm::Utils::NullVariant;

using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

using Common = TestUtils::Common::Collection;

using Models::Album;
using Models::AlbumImage;

class tst_Collection_Relations : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase() const;

    /* Comparison operators */
    void equalComparison() const;
    void notEqualComparison() const;

    void equalComparison_WithModelsCollection() const;
    void notEqualComparison_WithModelsCollection() const;

    /* Redeclared overriden methods from the base class */
    void isEmpty() const;

    /* BaseCollection */
    void isNotEmpty() const;

    void filter() const;
    void filter_WithIndex() const;
    void filter_Remove_nullptr() const;

    void first() const;
    void first_NotFound_nullptr() const;
    void first_NotFound_DefaultModel() const;

    void last() const;
    void last_NotFound_nullptr() const;
    void last_NotFound_DefaultModel() const;

    void implode_Name() const;
    void implode_Id() const;
    void implode_Ext_ColumnWithNull() const;

    /* Collection */
    void modelKeys_QVariant() const;
    void modelKeys_quint64() const;

    void map() const;
    void map_WithIndex() const;

    void map_CustomReturnType() const;
    void map_CustomReturnType_WithIndex() const;

    void mapWithModelKeys() const;

    void mapWithKeys_IdAndName() const;
    void mapWithKeys_NameAndId() const;
    void mapWithKeys_IdAndModelPointer() const;
    void mapWithKeys_IdAndModel() const;

    void only() const;
    void only_Empty() const;
    void except() const;
    void except_Empty() const;

    void pluck() const;
    void pluck_CustomType() const;
    void pluck_KeyedById() const;
    void pluck_KeyedById_LastDuplicate() const;

    void contains_ById() const;
    void contains_ById_QVariant() const;
    void contains_Callback() const;
    void contains_Model() const;

    void doesntContain_ById() const;
    void doesntContain_ById_QVariant() const;
    void doesntContain_Callback() const;
    void doesntContain_Model() const;

    void find() const;
    void find_NotFound_nullptr() const;
    void find_NotFound_DefaultModel() const;

    void find_Model() const;
    void find_Model_NotFound_nullptr() const;
    void find_Model_NotFound_DefaultModel() const;

    void find_Ids() const;

    void toQuery() const;

    /* Collection - Relations related */
    void load_lvalue() const;
    void load_lvalue_WithSelectConstraint() const;
    void load_lvalue_WithLambdaConstraint() const;
    void load_lvalue_NonExistentRelation_Failed() const;

    void load_rvalue() const;
    void load_rvalue_WithSelectConstraint() const;
    void load_rvalue_WithLambdaConstraint() const;
    void load_rvalue_NonExistentRelation_Failed() const;

    /* EnumeratesValues */
    void reject() const;
    void reject_WithIndex() const;

    void where_QString_EQ() const;
    void where_quint64_NE() const;
    void where_quint64_GT() const;
    void where_WithNullQVariant() const;
    void where_InvalidComparisonOperator_ThrowException() const;

    void whereNull_QString() const;
    void whereNotNull_QString() const;

    void whereIn_QString() const;
    void whereIn_quint64() const;
    void whereIn_Empty() const;

    void whereNotIn_QString() const;
    void whereNotIn_quint64() const;
    void whereNotIn_Empty() const;

    void whereBetween() const;
    void whereNotBetween() const;

    void firstWhere_QString() const;
    void firstWhere_quint64_NE() const;
    void firstWhere_quint64_GE() const;
    void firstWhere_NotFound_nullptr() const;

    void value_QVariant() const;
    void value_QVariant_NonExistentAttribute() const;
    void value_QVariant_NonExistentAttribute_DefaultValue() const;
    void value_QVariant_EmptyCollection() const;

    void value_quint64() const;
    void value_quint64_NonExistentAttribute_DefaultValue() const;

    void value_QString() const;
    void value_QString_NonExistentAttribute_DefaultValue() const;

    void each_lvalue() const;
    void each_lvalue_index() const;
    void each_lvalue_bool() const;
    void each_lvalue_bool_index() const;

    void each_rvalue() const;
    void each_rvalue_index() const;
    void each_rvalue_bool() const;
    void each_rvalue_bool_index() const;

    void tap_lvalue() const;
    void tap_rvalue() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Connection name used in this test case. */
    QString m_connection {};
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Collection_Relations::initTestCase()
{
    ConnectionOverride::connection = m_connection =
            Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );
}

void tst_Collection_Relations::cleanupTestCase() const
{
    // Reset connection override
    ConnectionOverride::connection.clear();
}

/* Comparison operators */

/* In the following equality operator==() tests are real types used instead
   of the auto to clearly see what's up. */

void tst_Collection_Relations::equalComparison() const
{
    auto album2_1 = Album::find(2);
    QVERIFY(album2_1);
    QVERIFY(album2_1->exists);
    QCOMPARE(album2_1->getKey(), QVariant(2));
    QVERIFY(album2_1->relationLoaded(Common::albumImages));

    ModelsCollection<AlbumImage *>
    images2_1 = album2_1->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images2_1.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images2_1));
    QVERIFY(Common::verifyIds(images2_1, {2, 3, 4, 5, 6}));

    auto album2_2 = Album::find(2);
    QVERIFY(album2_2);
    QVERIFY(album2_2->exists);
    QCOMPARE(album2_2->getKey(), QVariant(2));
    QVERIFY(album2_2->relationLoaded(Common::albumImages));

    ModelsCollection<AlbumImage *>
    images2_2 = album2_2->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images2_2.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images2_2));
    QVERIFY(Common::verifyIds(images2_2, {2, 3, 4, 5, 6}));

    // Different collections with the same models (used Model::operator==() for comparing)
    QVERIFY(images2_1 == images2_2);

    // The same models' addresses (used Models' pointers comparison)
    const ModelsCollection<AlbumImage *> &images2_3 = images2_1;
    QVERIFY(images2_1 == images2_3);
}

void tst_Collection_Relations::notEqualComparison() const
{
    auto album1 = Album::find(1);
    QVERIFY(album1);
    QVERIFY(album1->exists);
    QCOMPARE(album1->getKey(), QVariant(1));
    QVERIFY(album1->relationLoaded(Common::albumImages));

    ModelsCollection<AlbumImage *>
    images1 = album1->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images1.size(), 1);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images1));
    QVERIFY(Common::verifyIds(images1, {1}));

    auto album2_1 = Album::find(2);
    QVERIFY(album2_1);
    QVERIFY(album2_1->exists);
    QCOMPARE(album2_1->getKey(), QVariant(2));
    QVERIFY(album2_1->relationLoaded(Common::albumImages));

    ModelsCollection<AlbumImage *>
    images2_1 = album2_1->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images2_1.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images2_1));
    QVERIFY(Common::verifyIds(images2_1, {2, 3, 4, 5, 6}));

    // Different collections size
    QVERIFY(images1 != images2_1);

    // Get a fresh images and change one model (used Model::operator==() for comparing)
    auto album2_2 = Album::find(2);
    QVERIFY(album2_2);
    QVERIFY(album2_2->exists);
    QCOMPARE(album2_2->getKey(), QVariant(2));
    QVERIFY(album2_2->relationLoaded(Common::albumImages));

    ModelsCollection<AlbumImage *>
    images2_2 = album2_2->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images2_2.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images2_2));
    QVERIFY(Common::verifyIds(images2_2, {2, 3, 4, 5, 6}));

    QVERIFY(images2_1 == images2_2);
    (*images2_2[1])[Common::ext] = "png";
    QVERIFY(images2_1 != images2_2);
}

void tst_Collection_Relations::equalComparison_WithModelsCollection() const
{
    auto album2_1 = Album::find(2);
    QVERIFY(album2_1);
    QVERIFY(album2_1->exists);
    QCOMPARE(album2_1->getKey(), QVariant(2));
    QVERIFY(album2_1->relationLoaded(Common::albumImages));

    ModelsCollection<AlbumImage *>
    images2_1 = album2_1->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images2_1.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images2_1));
    QVERIFY(Common::verifyIds(images2_1, {2, 3, 4, 5, 6}));

    ModelsCollection<AlbumImage>
    images2_2 = AlbumImage::whereEq(Common::album_id, 2)->get();
    QCOMPARE(images2_2.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images2_2));
    QVERIFY(Common::verifyIds(images2_2, {2, 3, 4, 5, 6}));

    // Different collections with the same models (used Model::operator==() for comparing)
    QVERIFY(images2_1 == images2_2);

    // The same models' addresses (used Models' pointers comparison)
    ModelsCollection<AlbumImage *> images2_2_Init {
        &images2_2[0], // NOLINT(readability-container-data-pointer)
        &images2_2[1],
        &images2_2[2],
        &images2_2[3],
        &images2_2[4],
    };
    /* The images2_2_Init must be on the left side to invoke the correct operator==()
       overload. */
    QVERIFY(images2_2_Init == images2_2);
}

void tst_Collection_Relations::notEqualComparison_WithModelsCollection() const
{
    auto album1 = Album::find(1);
    QVERIFY(album1);
    QVERIFY(album1->exists);
    QCOMPARE(album1->getKey(), QVariant(1));
    QVERIFY(album1->relationLoaded(Common::albumImages));

    ModelsCollection<AlbumImage *>
    images1 = album1->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images1.size(), 1);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images1));
    QVERIFY(Common::verifyIds(images1, {1}));

    ModelsCollection<AlbumImage>
    images2_1 = AlbumImage::whereEq(Common::album_id, 2)->get();
    QCOMPARE(images2_1.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images2_1));
    QVERIFY(Common::verifyIds(images2_1, {2, 3, 4, 5, 6}));

    // Different collections size
    QVERIFY(images1 != images2_1);

    // Get a fresh images and change one model (used Model::operator==() for comparing)
    auto album2_2 = Album::find(2);
    QVERIFY(album2_2);
    QVERIFY(album2_2->exists);
    QCOMPARE(album2_2->getKey(), QVariant(2));
    QVERIFY(album2_2->relationLoaded(Common::albumImages));

    ModelsCollection<AlbumImage *>
    images2_2 = album2_2->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images2_2.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images2_2));
    QVERIFY(Common::verifyIds(images2_2, {2, 3, 4, 5, 6}));

    // The images2_2 must be on the left side to invoke the correct operator==() overload
    QVERIFY(images2_2 == images2_1);
    images2_1[1][Common::ext] = "png";
    QVERIFY(images2_2 != images2_1);
}

/* Redeclared overriden methods from the base class */

void tst_Collection_Relations::isEmpty() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    QVERIFY(!images.isEmpty());

    QVERIFY(ModelsCollection<Album *>().isEmpty());
}

/* BaseCollection */

void tst_Collection_Relations::isNotEmpty() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    QVERIFY(images.isNotEmpty());

    QVERIFY(!ModelsCollection<Album *>().isNotEmpty());
}

void tst_Collection_Relations::filter() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.filter([](AlbumImage *const image)
    {
        const auto id = Common::getKeyCasted(image);
        return id == 3 || id == 5;
    });

    // Verify
    QCOMPARE(result.size(), 2);
    QVERIFY(Common::verifyIds(result, {3, 5}));
}

void tst_Collection_Relations::filter_WithIndex() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.filter([](AlbumImage *const image, const auto index)
    {
        return Common::getKeyCasted(image) == 3 || index == 2;
    });

    // Verify
    QCOMPARE(result.size(), 2);
    QVERIFY(Common::verifyIds(result, {3, 4}));
}

void tst_Collection_Relations::filter_Remove_nullptr() const
{
    auto albums = Album::all();
    QCOMPARE(albums.size(), 4);
    QCOMPARE(typeid (ModelsCollection<Album>), typeid (albums));

    // Prepare
    ModelsCollection<Album *> tmp;
    tmp.reserve(albums.size());

    for (Album &album : albums)
        tmp.push_back(&album);

    tmp[0] = nullptr;
    tmp[1] = nullptr;

    // Get result
    auto result = tmp.filter();

    // Verify
    QCOMPARE(result.size(), 2);
    QVERIFY(Common::verifyIds(result, {3, 4}));
}

void tst_Collection_Relations::first() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.first([](AlbumImage *const image)
    {
        return Common::getKeyCasted(image) == 3;
    });

    // Verify
    QVERIFY(result != nullptr);
    QCOMPARE(Common::getKeyCasted(result), 3);
}

void tst_Collection_Relations::first_NotFound_nullptr() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.first([](AlbumImage *const image)
    {
        return Common::getKeyCasted(image) == 30;
    });

    // Verify
    QVERIFY(result == nullptr);
}

void tst_Collection_Relations::first_NotFound_DefaultModel() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.first([](AlbumImage *const image)
    {
        return Common::getKeyCasted(image) == 30;
    },
        images.at(2));

    // Verify
    QVERIFY(result != nullptr);
    QCOMPARE(Common::getKeyCasted(result), 4);
}

void tst_Collection_Relations::last() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.last([](AlbumImage *const image)
    {
        return Common::getKeyCasted(image) == 4;
    });

    // Verify
    QVERIFY(result != nullptr);
    QCOMPARE(Common::getKeyCasted(result), 4);
}

void tst_Collection_Relations::last_NotFound_nullptr() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.last([](AlbumImage *const image)
    {
        return Common::getKeyCasted(image) == 30;
    });

    // Verify
    QVERIFY(result == nullptr);
}

void tst_Collection_Relations::last_NotFound_DefaultModel() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.last([](AlbumImage *const image)
    {
        return Common::getKeyCasted(image) == 30;
    },
        images.at(2));

    // Verify
    QVERIFY(result != nullptr);
    QCOMPARE(Common::getKeyCasted(result), 4);
}

void tst_Collection_Relations::implode_Name() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.implode(NAME, COMMA);

    // Verify
    QCOMPARE(result, QString("album2_image1, album2_image2, album2_image3, "
                             "album2_image4, album2_image5"));
}

void tst_Collection_Relations::implode_Id() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.implode(ID, COMMA);

    // Verify
    QCOMPARE(result, QString("2, 3, 4, 5, 6"));
}

void tst_Collection_Relations::implode_Ext_ColumnWithNull() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Prepare
    (*images.at(1))[Common::ext] = NullVariant::QString();
    (*images.at(2))[Common::ext] = NullVariant::QString();

    // Get result
    const auto result = images.implode(Common::ext, COMMA);

    // Verify
    QCOMPARE(result, QString("png, , , png, gif"));
}

/* Collection */

void tst_Collection_Relations::modelKeys_QVariant() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.modelKeys();

    // Verify
    QCOMPARE(result.size(), 5);
    QVERIFY(Common::verifyIdsQVariant<quint64>(result, {2, 3, 4, 5, 6}));
}

void tst_Collection_Relations::modelKeys_quint64() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.modelKeys<quint64>();

    // Verify
    QCOMPARE(result.size(), 5);
    QVERIFY(Common::verifyIds(result, {2, 3, 4, 5, 6}));
}

void tst_Collection_Relations::map() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.map([](AlbumImage *const image)
    {
        if (const auto id = Common::getKeyCasted(image);
            id == 3 || id == 6
        )
            (*image)[NAME] = QStringLiteral("%1_id_%2")
                             .arg(image->getAttribute<QString>(NAME))
                             .arg(id);
        return image;
    });

    // Verify
    QCOMPARE(result.size(), 5);
    QVERIFY(Common::verifyIds(result, {2, 3, 4, 5, 6}));
    QVERIFY(Common::verifyAttributeValues<QString>(
                NAME, result, {"album2_image1", "album2_image2_id_3", "album2_image3",
                               "album2_image4", "album2_image5_id_6"}));
}

void tst_Collection_Relations::map_WithIndex() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.map([](AlbumImage *const image, const auto index)
    {
        if (const auto id = Common::getKeyCasted(image);
            id == 2
        )
            (*image)[NAME] = QStringLiteral("%1_id_%2")
                             .arg(image->getAttribute<QString>(NAME))
                             .arg(id);

        else if (index == 3)
            (*image)[NAME] = QStringLiteral("%1_index_%2")
                             .arg(image->getAttribute<QString>(NAME))
                             .arg(index);

        return image;
    });

    // Verify
    QCOMPARE(result.size(), 5);
    QVERIFY(Common::verifyIds(result, {2, 3, 4, 5, 6}));
    QVERIFY(Common::verifyAttributeValues<QString>(
                NAME, result, {"album2_image1_id_2", "album2_image2", "album2_image3",
                               "album2_image4_index_3", "album2_image5"}));
}

void tst_Collection_Relations::map_CustomReturnType() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.map<QString>([](AlbumImage *const image)
    {
        const auto nameRef = (*image)[NAME];

        if (const auto id = Common::getKeyCasted(image);
            id == 3 || id == 6
        )
            nameRef = QStringLiteral("%1_id_%2")
                      .arg(image->getAttribute<QString>(NAME))
                      .arg(id);

        return nameRef->value<QString>();
    });

    // Verify
    QCOMPARE(result.size(), 5);
    QVector<QString> expected {"album2_image1", "album2_image2_id_3", "album2_image3",
                               "album2_image4", "album2_image5_id_6"};
    QCOMPARE(result, expected);
}

void tst_Collection_Relations::map_CustomReturnType_WithIndex() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.map<QString>([](AlbumImage *const image, const auto index)
    {
        const auto nameRef = (*image)[NAME];

        if (const auto id = Common::getKeyCasted(image);
            id == 2
        )
            nameRef = QStringLiteral("%1_id_%2")
                      .arg(image->getAttribute<QString>(NAME))
                      .arg(id);

        else if (index == 3)
            nameRef = QStringLiteral("%1_index_%2")
                      .arg(image->getAttribute<QString>(NAME))
                      .arg(index);

        return nameRef->value<QString>();
    });

    // Verify
    QCOMPARE(result.size(), 5);
    QVector<QString> expected {"album2_image1_id_2", "album2_image2", "album2_image3",
                               "album2_image4_index_3", "album2_image5"};
    QCOMPARE(result, expected);
}

void tst_Collection_Relations::mapWithModelKeys() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.mapWithModelKeys();

    // Verify
    QCOMPARE(result.size(), 5);

    std::unordered_map<quint64, AlbumImage *> expected {
        {2, images.at(0)},
        {3, images.at(1)},
        {4, images.at(2)},
        {5, images.at(3)},
        {6, images.at(4)},
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Relations::mapWithKeys_IdAndName() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result =
            images.mapWithKeys<quint64, QString>(
                [](AlbumImage *const image) -> std::pair<quint64, QString>
    {
        return {Common::getKeyCasted(image), image->getAttribute<QString>(NAME)};
    });

    // Verify
    QCOMPARE(result.size(), 5);

    std::unordered_map<quint64, QString> expected {
        {2, images.at(0)->getAttribute<QString>(NAME)},
        {3, images.at(1)->getAttribute<QString>(NAME)},
        {4, images.at(2)->getAttribute<QString>(NAME)},
        {5, images.at(3)->getAttribute<QString>(NAME)},
        {6, images.at(4)->getAttribute<QString>(NAME)},
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Relations::mapWithKeys_NameAndId() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result =
            images.mapWithKeys<QString, quint64>(
                [](AlbumImage *const image) -> std::pair<QString, quint64>
    {
        return {image->getAttribute<QString>(NAME), Common::getKeyCasted(image)};
    });

    // Verify
    QCOMPARE(result.size(), 5);

    std::unordered_map<QString, quint64> expected {
        {images.at(0)->getAttribute<QString>(NAME), 2},
        {images.at(1)->getAttribute<QString>(NAME), 3},
        {images.at(2)->getAttribute<QString>(NAME), 4},
        {images.at(3)->getAttribute<QString>(NAME), 5},
        {images.at(4)->getAttribute<QString>(NAME), 6},
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Relations::mapWithKeys_IdAndModelPointer() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result =
            images.mapWithKeys<quint64, AlbumImage *>(
                [](AlbumImage *const image) -> std::pair<quint64, AlbumImage *>
    {
        return {Common::getKeyCasted(image), image};
    });

    // Verify
    QCOMPARE(result.size(), 5);

    std::unordered_map<quint64, AlbumImage *> expected {
        {2, images.at(0)},
        {3, images.at(1)},
        {4, images.at(2)},
        {5, images.at(3)},
        {6, images.at(4)},
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Relations::mapWithKeys_IdAndModel() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result =
            images.mapWithKeys<quint64, AlbumImage>(
                [](AlbumImage *const image) -> std::pair<quint64, AlbumImage>
    {
        return {Common::getKeyCasted(image), *image};
    });

    // Verify
    QCOMPARE(result.size(), 5);

    std::unordered_map<quint64, AlbumImage> expected {
        {2, *images.at(0)},
        {3, *images.at(1)},
        {4, *images.at(2)},
        {5, *images.at(3)},
        {6, *images.at(4)},
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Relations::only() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.only({6, 2, 5, 10, 2});

    // Verify
    QCOMPARE(result.size(), 3);

    ModelsCollection<AlbumImage *> expected {
        images.at(0),
        images.at(3),
        images.at(4),
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Relations::only_Empty() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.only({});

    // Verify
    QVERIFY(result.isEmpty());
}

void tst_Collection_Relations::except() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.except({5, 2, 10, 2});

    // Verify
    QCOMPARE(result.size(), 3);

    ModelsCollection<AlbumImage *> expected {
        images.at(1),
        images.at(2),
        images.at(4),
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Relations::except_Empty() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.except({});

    // Verify
    QCOMPARE(result.size(), 5);

    ModelsCollection<AlbumImage *> expected {
        images.at(0),
        images.at(1),
        images.at(2),
        images.at(3),
        images.at(4),
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Relations::pluck() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.pluck(NAME);

    // Verify
    QCOMPARE(result.size(), 5);

    QVector<QVariant> expected {
        QString("album2_image1"),
        QString("album2_image2"),
        QString("album2_image3"),
        QString("album2_image4"),
        QString("album2_image5"),
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Relations::pluck_CustomType() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.pluck<QString>(NAME);

    // Verify
    QCOMPARE(result.size(), 5);
    QCOMPARE(typeid (result), typeid (QVector<QString>));

    QVector<QString> expected {
        QString("album2_image1"),
        QString("album2_image2"),
        QString("album2_image3"),
        QString("album2_image4"),
        QString("album2_image5"),
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Relations::pluck_KeyedById() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.pluck<quint64>(NAME, ID);

    // Verify
    QCOMPARE(result.size(), 5);

    std::map<quint64, QVariant> expected {
        {2, QString("album2_image1")},
        {3, QString("album2_image2")},
        {4, QString("album2_image3")},
        {5, QString("album2_image4")},
        {6, QString("album2_image5")},
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Relations::pluck_KeyedById_LastDuplicate() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Prepare
    /* Even if this is not technically right because of the duplicate primary key, it
       tests what we need to test, so no reason to rewrite it with a better example. */
    images << images.at(2);
    (*images.last())[NAME] = "album2_image6";
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6, 4}));

    // Get result
    const auto result = images.pluck<quint64>(NAME, ID);

    // Verify
    QCOMPARE(result.size(), 5);

    std::map<quint64, QVariant> expected {
        {2, QString("album2_image1")},
        {3, QString("album2_image2")},
        {4, QString("album2_image6")}, // Must be assigned again
        {5, QString("album2_image4")},
        {6, QString("album2_image5")},
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Relations::contains_ById() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY(images.contains(2));
    QVERIFY(images.contains(6));
    QVERIFY(!images.contains(1));
    QVERIFY(!images.contains(7));
    QVERIFY(!images.contains(20));
}

void tst_Collection_Relations::contains_ById_QVariant() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY(images.contains(QVariant(2)));
    QVERIFY(images.contains(QVariant(6)));
    QVERIFY(!images.contains(QVariant(1)));
    QVERIFY(!images.contains(QVariant(7)));
    QVERIFY(!images.contains(QVariant(20)));
}

void tst_Collection_Relations::contains_Callback() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY(images.contains([](AlbumImage *const image)
    {
        return image->getKeyCasted() == 2;
    }));
    QVERIFY(images.contains([](AlbumImage *const image)
    {
        return image->getKeyCasted() == 6;
    }));
    QVERIFY(images.contains([](AlbumImage *const image)
    {
        return image->getKeyCasted() != 1;
    }));
    QVERIFY(images.contains([](AlbumImage *const image)
    {
        return image->getKeyCasted() != 7;
    }));
    QVERIFY(images.contains([](AlbumImage *const image)
    {
        return image->getKeyCasted() != 20;
    }));
}

void tst_Collection_Relations::contains_Model() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY(images.contains(*images.at(0)));
    QVERIFY(images.contains(*images.at(4)));
    // Make a copy so I can modify the ID
    AlbumImage image = *images.at(0);
    image[ID] = 1;
    QVERIFY(!images.contains(image));
    image[ID] = 7;
    QVERIFY(!images.contains(image));
    image[ID] = 20;
    QVERIFY(!images.contains(image));
    QVERIFY(!images.contains(std::nullopt));
}

void tst_Collection_Relations::doesntContain_ById() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY(images.doesntContain(1));
    QVERIFY(images.doesntContain(7));
    QVERIFY(images.doesntContain(20));
    QVERIFY(!images.doesntContain(2));
    QVERIFY(!images.doesntContain(6));
}

void tst_Collection_Relations::doesntContain_ById_QVariant() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY(images.doesntContain(QVariant(1)));
    QVERIFY(images.doesntContain(QVariant(7)));
    QVERIFY(images.doesntContain(QVariant(20)));
    QVERIFY(!images.doesntContain(QVariant(2)));
    QVERIFY(!images.doesntContain(QVariant(6)));
}

void tst_Collection_Relations::doesntContain_Callback() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY(images.doesntContain([](AlbumImage *const image)
    {
        return image->getKeyCasted() == 1;
    }));
    QVERIFY(images.doesntContain([](AlbumImage *const image)
    {
        return image->getKeyCasted() == 7;
    }));
    QVERIFY(images.doesntContain([](AlbumImage *const image)
    {
        return image->getKeyCasted() == 20;
    }));
    /* All the following conditions are correct, I have investigated it and even if
       it's weird they are correct; simply this is how the mathematic works.
       So the conclusion is that this doesntContain(callback) is weird if the != is
       used inside the callback. */
    QVERIFY(!images.doesntContain([](AlbumImage *const image)
    {
        return image->getKeyCasted() == 2;
    }));
    QVERIFY(!images.doesntContain([](AlbumImage *const image)
    {
        return image->getKeyCasted() != 2;
    }));
    QVERIFY(!images.doesntContain([](AlbumImage *const image)
    {
        return image->getKeyCasted() == 6;
    }));
    QVERIFY(!images.doesntContain([](AlbumImage *const image)
    {
        return image->getKeyCasted() != 6;
    }));
}

void tst_Collection_Relations::doesntContain_Model() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY(!images.doesntContain(*images.at(0)));
    QVERIFY(!images.doesntContain(*images.at(4)));
    // Make a copy so I can modify the ID
    AlbumImage image = *images.at(0);
    image[ID] = 1;
    QVERIFY(images.doesntContain(image));
    image[ID] = 7;
    QVERIFY(images.doesntContain(image));
    image[ID] = 20;
    QVERIFY(images.doesntContain(image));
    QVERIFY(images.doesntContain(std::nullopt));
}

void tst_Collection_Relations::find() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.find(4);

    // Verify
    QVERIFY(result);
    QVERIFY(result->exists);
    QCOMPARE(result->getKey(), QVariant(4));
}

void tst_Collection_Relations::find_NotFound_nullptr() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.find(40);

    // Verify
    QVERIFY(result == nullptr);
}

void tst_Collection_Relations::find_NotFound_DefaultModel() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.find(40, images.at(3));

    // Verify
    QVERIFY(result);
    QVERIFY(result->exists);
    QCOMPARE(result->getKey(), QVariant(5));
}

void tst_Collection_Relations::find_Model() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.find(*images.at(2));

    // Verify
    QVERIFY(result);
    QVERIFY(result->exists);
    QCOMPARE(result->getKey(), QVariant(4));
}

void tst_Collection_Relations::find_Model_NotFound_nullptr() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Prepare
    AlbumImage image40 = *images.at(0);
    image40[ID] = 40;

    // Get result
    AlbumImage *const result = images.find(image40);

    // Verify
    QVERIFY(result == nullptr);
}

void tst_Collection_Relations::find_Model_NotFound_DefaultModel() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Prepare
    AlbumImage image40 = *images.at(0);
    image40[ID] = 40;

    // Get result
    AlbumImage *const result = images.find(image40, images.at(3));

    // Verify
    QVERIFY(result);
    QVERIFY(result->exists);
    QCOMPARE(result->getKey(), QVariant(5));
}

void tst_Collection_Relations::find_Ids() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.find({6, 3, 5, 6});

    // Verify
    QCOMPARE(result.size(), 3);

    ModelsCollection<AlbumImage *> expected {
        images.at(1),
        images.at(3),
        images.at(4),
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Relations::toQuery() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.toQuery()->get();

    // Verify
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));
    QCOMPARE(result.constFirst().getAttributes().size(), 7);
}

/* Collection - Relations related */

/*! Expected album images many type relation after the load() method invoked. */
struct ExpectedImages
{
    /*! Number of images. */
    std::size_t imagesSize;
    /*! Images ID. */
    std::unordered_set<AlbumImage::KeyType> imagesId;
};

void tst_Collection_Relations::load_lvalue() const
{
    auto albums = Album::without(Common::albumImages)->findMany({1, 2, 4});
    QCOMPARE(albums.size(), 3);
    QCOMPARE(typeid (ModelsCollection<Album>), typeid (albums));
    QVERIFY(Common::verifyIds(albums, {1, 2, 4}));

    // Prepare
    ModelsCollection<Album *> albumsInit {
        &albums[0], // NOLINT(readability-container-data-pointer)
        &albums[1],
    };

    // Verify before
    QVERIFY(std::ranges::all_of(albumsInit, [](Album *const album)
    {
        return album->getRelations().empty();
    }));

    // Load the albumImages hasMany relation
    ModelsCollection<Album *> &result = albumsInit.load(Common::albumImages);
    // Both must be lvalue references because of that the decltype ((images)) is used
    QVERIFY((std::is_same_v<decltype (result), decltype ((albumsInit))>));
    // It must be the same ModelsCollection (the same memory address)
    QVERIFY(reinterpret_cast<uintptr_t>(&result) ==
            reinterpret_cast<uintptr_t>(&albumsInit));

    // Prepare - AlbumImage::KeyType is Album ID
    std::unordered_map<AlbumImage::KeyType, ExpectedImages> expectedImages {
        {1, {1, {1}}},
        {2, {5, {2, 3, 4, 5, 6}}},
        {4, {0, {}}},
    };

    // Verify after, both the result and also original albums vector
    // Original albums collection
    for (Album *const album : albumsInit) {
        const auto albumId = album->getKey();
        const auto &expectedImage = expectedImages.at(albumId.value<Album::KeyType>());

        QVERIFY(album->relationLoaded(Common::albumImages));
        QVERIFY(!album->getRelations().empty());
        const auto &relations = album->getRelations();
        QCOMPARE(relations.size(), 1);
        QVERIFY(relations.contains(Common::albumImages));

        // AlbumImage has-many relation
        auto images = album->getRelation<AlbumImage>(Common::albumImages);
        QCOMPARE(images.size(), expectedImage.imagesSize);
        QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));

        // Expected image IDs
        for (AlbumImage *const image : images) {
            QVERIFY(image);
            QVERIFY(image->exists);
            QCOMPARE(image->getAttribute(Common::album_id_s), albumId);
            QVERIFY(expectedImage.imagesId.contains(
                        image->getKey().value<AlbumImage::KeyType>()));
            QCOMPARE(typeid (AlbumImage *), typeid (image));
        }
    }
}

void tst_Collection_Relations::load_lvalue_WithSelectConstraint() const
{
    auto albums = Album::without(Common::albumImages)->findMany({1, 2, 4});
    QCOMPARE(albums.size(), 3);
    QCOMPARE(typeid (ModelsCollection<Album>), typeid (albums));
    QVERIFY(Common::verifyIds(albums, {1, 2, 4}));

    // Prepare
    ModelsCollection<Album *> albumsInit {
        &albums[0], // NOLINT(readability-container-data-pointer)
        &albums[1],
    };

    // Verify before
    QVERIFY(std::ranges::all_of(albumsInit, [](Album *const album)
    {
        return album->getRelations().empty();
    }));

    // Load the albumImages hasMany relation
    ModelsCollection<Album *> &result = albumsInit.load("albumImages:id,album_id,ext");
    // Both must be lvalue references because of that the decltype ((images)) is used
    QVERIFY((std::is_same_v<decltype (result), decltype ((albumsInit))>));
    // It must be the same ModelsCollection (the same memory address)
    QVERIFY(reinterpret_cast<uintptr_t>(&result) ==
            reinterpret_cast<uintptr_t>(&albumsInit));

    // Prepare - AlbumImage::KeyType is Album ID
    std::unordered_map<AlbumImage::KeyType, ExpectedImages> expectedImages {
        {1, {1, {1}}},
        {2, {5, {2, 3, 4, 5, 6}}},
        {4, {0, {}}},
    };

    // Verify after, both the result and also original albums vector
    // Original albums collection
    for (Album *const album : albumsInit) {
        const auto albumId = album->getKey();
        const auto &expectedImage = expectedImages.at(albumId.value<Album::KeyType>());

        QVERIFY(album->relationLoaded(Common::albumImages));
        QVERIFY(!album->getRelations().empty());
        const auto &relations = album->getRelations();
        QCOMPARE(relations.size(), 1);
        QVERIFY(relations.contains(Common::albumImages));

        // AlbumImage has-many relation
        auto images = album->getRelation<AlbumImage>(Common::albumImages);
        QCOMPARE(images.size(), expectedImage.imagesSize);
        QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));

        // Expected image IDs
        for (AlbumImage *const image : images) {
            QVERIFY(image);
            QVERIFY(image->exists);

            // Check whether constraints was correctly applied
            const auto &attributes = image->getAttributes();
            QCOMPARE(attributes.size(), 3);

            std::unordered_set<QString> expectedAttributes {
                ID, Common::album_id_s, Common::ext,
            };
            for (const auto &attribute : attributes)
                QVERIFY(expectedAttributes.contains(attribute.key));

            QCOMPARE(image->getAttribute(Common::album_id_s), albumId);
            QVERIFY(expectedImage.imagesId.contains(
                        image->getKey().value<AlbumImage::KeyType>()));
            QCOMPARE(typeid (AlbumImage *), typeid (image));
        }
    }
}

void tst_Collection_Relations::load_lvalue_WithLambdaConstraint() const
{
    auto albums = Album::without(Common::albumImages)->findMany({1, 2, 4});
    QCOMPARE(albums.size(), 3);
    QCOMPARE(typeid (ModelsCollection<Album>), typeid (albums));
    QVERIFY(Common::verifyIds(albums, {1, 2, 4}));

    // Prepare
    ModelsCollection<Album *> albumsInit {
        &albums[0], // NOLINT(readability-container-data-pointer)
        &albums[1],
    };

    // Verify before
    QVERIFY(std::ranges::all_of(albumsInit, [](Album *const album)
    {
        return album->getRelations().empty();
    }));

    // Load the albumImages hasMany relation
    ModelsCollection<Album *> &result =
            albumsInit.load({{Common::albumImages, [](auto &query)
                              {
                                  query.select({ID, Common::album_id_s, SIZE_});
                              }}});
    // Both must be lvalue references because of that the decltype ((images)) is used
    QVERIFY((std::is_same_v<decltype (result), decltype ((albumsInit))>));
    // It must be the same ModelsCollection (the same memory address)
    QVERIFY(reinterpret_cast<uintptr_t>(&result) ==
            reinterpret_cast<uintptr_t>(&albumsInit));

    // Prepare - AlbumImage::KeyType is Album ID
    std::unordered_map<AlbumImage::KeyType, ExpectedImages> expectedImages {
        {1, {1, {1}}},
        {2, {5, {2, 3, 4, 5, 6}}},
        {4, {0, {}}},
    };

    // Verify after, both the result and also original albums vector
    // Original albums collection
    for (Album *const album : albumsInit) {
        const auto albumId = album->getKey();
        const auto &expectedImage = expectedImages.at(albumId.value<Album::KeyType>());

        QVERIFY(album->relationLoaded(Common::albumImages));
        QVERIFY(!album->getRelations().empty());
        const auto &relations = album->getRelations();
        QCOMPARE(relations.size(), 1);
        QVERIFY(relations.contains(Common::albumImages));

        // AlbumImage has-many relation
        auto images = album->getRelation<AlbumImage>(Common::albumImages);
        QCOMPARE(images.size(), expectedImage.imagesSize);
        QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));

        // Expected image IDs
        for (AlbumImage *const image : images) {
            QVERIFY(image);
            QVERIFY(image->exists);

            // Check whether constraints was correctly applied
            const auto &attributes = image->getAttributes();
            QCOMPARE(attributes.size(), 3);

            std::unordered_set<QString> expectedAttributes {
                ID, Common::album_id_s, SIZE_,
            };
            for (const auto &attribute : attributes)
                QVERIFY(expectedAttributes.contains(attribute.key));

            QCOMPARE(image->getAttribute(Common::album_id_s), albumId);
            QVERIFY(expectedImage.imagesId.contains(
                        image->getKey().value<AlbumImage::KeyType>()));
            QCOMPARE(typeid (AlbumImage *), typeid (image));
        }
    }
}

void tst_Collection_Relations::load_lvalue_NonExistentRelation_Failed() const
{
    auto albums = Album::without(Common::albumImages)->findMany({1, 2});
    QCOMPARE(albums.size(), 2);
    QCOMPARE(typeid (ModelsCollection<Album>), typeid (albums));
    QVERIFY(Common::verifyIds(albums, {1, 2}));

    // Prepare
    ModelsCollection<Album *> albumsInit {
        &albums[0], // NOLINT(readability-container-data-pointer)
        &albums[1],
    };

    const auto verify = [&albumsInit]
    {
        QVERIFY(std::ranges::all_of(albumsInit, [](Album *const album)
        {
            return album->getRelations().empty();
        }));
    };

    // Verify before
    verify();

    QVERIFY_EXCEPTION_THROWN(albumsInit.load("albumImages-NON_EXISTENT"),
                             RelationNotFoundError);

    // Verify after
    verify();
}

void tst_Collection_Relations::load_rvalue() const
{
    auto albums = Album::without(Common::albumImages)->findMany({1, 2, 4});
    QCOMPARE(albums.size(), 3);
    QCOMPARE(typeid (ModelsCollection<Album>), typeid (albums));
    QVERIFY(Common::verifyIds(albums, {1, 2, 4}));

    // Prepare
    ModelsCollection<Album *> albumsInit {
        &albums[0], // NOLINT(readability-container-data-pointer)
        &albums[1],
    };

    // Verify before
    QVERIFY(std::ranges::all_of(albumsInit, [](Album *const album)
    {
        return album->getRelations().empty();
    }));

    // Load the albumImages hasMany relation
    ModelsCollection<Album *> &&result = std::move(albumsInit).load(Common::albumImages);
    /* In 99% cases it must be the same ModelsCollection (the same memory address) but
       I'm disabling the QVERIFY check because this is compiler specific, if the result
       is rvalue then the albums will be constructed in-place and no moves will be done,
       the compiler optimizes it out, if the result would be just only result without
       the rvalue reference then the move constructor would be called.
       Another thing is that I need to call std::move(albums).load() because I need to
       verify the albums, the real world scenario would be:
       Album::without(Common::albumImages)->findMany({1, 2, 4}).load("albumImages")
       At the end of the day both command statements are the same. */
//    QVERIFY(reinterpret_cast<uintptr_t>(&result) ==
//            reinterpret_cast<uintptr_t>(&albumsInit));

    // Prepare - AlbumImage::KeyType is Album ID
    std::unordered_map<AlbumImage::KeyType, ExpectedImages> expectedImages {
        {1, {1, {1}}},
        {2, {5, {2, 3, 4, 5, 6}}},
        {4, {0, {}}},
    };

    // Verify after, both the result and also original albums vector
    // Original albums collection
    for (Album *const album : result) {
        const auto albumId = album->getKey();
        const auto &expectedImage = expectedImages.at(albumId.value<Album::KeyType>());

        QVERIFY(album->relationLoaded(Common::albumImages));
        QVERIFY(!album->getRelations().empty());
        const auto &relations = album->getRelations();
        QCOMPARE(relations.size(), 1);
        QVERIFY(relations.contains(Common::albumImages));

        // AlbumImage has-many relation
        auto images = album->getRelation<AlbumImage>(Common::albumImages);
        QCOMPARE(images.size(), expectedImage.imagesSize);
        QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));

        // Expected image IDs
        for (AlbumImage *const image : images) {
            QVERIFY(image);
            QVERIFY(image->exists);
            QCOMPARE(image->getAttribute(Common::album_id_s), albumId);
            QVERIFY(expectedImage.imagesId.contains(
                        image->getKey().value<AlbumImage::KeyType>()));
            QCOMPARE(typeid (AlbumImage *), typeid (image));
        }
    }
}

void tst_Collection_Relations::load_rvalue_WithSelectConstraint() const
{
    auto albums = Album::without(Common::albumImages)->findMany({1, 2, 4});
    QCOMPARE(albums.size(), 3);
    QCOMPARE(typeid (ModelsCollection<Album>), typeid (albums));
    QVERIFY(Common::verifyIds(albums, {1, 2, 4}));

    // Prepare
    ModelsCollection<Album *> albumsInit {
        &albums[0], // NOLINT(readability-container-data-pointer)
        &albums[1],
    };

    // Verify before
    QVERIFY(std::ranges::all_of(albumsInit, [](Album *const album)
    {
        return album->getRelations().empty();
    }));

    // Load the albumImages hasMany relation
    ModelsCollection<Album *> &&result = std::move(albumsInit)
                                         .load("albumImages:id,album_id,ext");
    /* In 99% cases it must be the same ModelsCollection (the same memory address) but
       I'm disabling the QVERIFY check because this is compiler specific, if the result
       is rvalue then the albums will be constructed in-place and no moves will be done,
       the compiler optimizes it out, if the result would be just only result without
       the rvalue reference then the move constructor would be called.
       Another thing is that I need to call std::move(albums).load() because I need to
       verify the albums, the real world scenario would be:
       Album::without(Common::albumImages)->findMany({1, 2, 4}).load("albumImages")
       At the end of the day both command statements are the same. */
//    QVERIFY(reinterpret_cast<uintptr_t>(&result) ==
//            reinterpret_cast<uintptr_t>(&albumsInit));

    // Prepare - AlbumImage::KeyType is Album ID
    std::unordered_map<AlbumImage::KeyType, ExpectedImages> expectedImages {
        {1, {1, {1}}},
        {2, {5, {2, 3, 4, 5, 6}}},
        {4, {0, {}}},
    };

    // Verify after, both the result and also original albums vector
    // Original albums collection
    for (Album *const album : result) {
        const auto albumId = album->getKey();
        const auto &expectedImage = expectedImages.at(albumId.value<Album::KeyType>());

        QVERIFY(album->relationLoaded(Common::albumImages));
        QVERIFY(!album->getRelations().empty());
        const auto &relations = album->getRelations();
        QCOMPARE(relations.size(), 1);
        QVERIFY(relations.contains(Common::albumImages));

        // AlbumImage has-many relation
        auto images = album->getRelation<AlbumImage>(Common::albumImages);
        QCOMPARE(images.size(), expectedImage.imagesSize);
        QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));

        // Expected image IDs
        for (AlbumImage *const image : images) {
            QVERIFY(image);
            QVERIFY(image->exists);

            // Check whether constraints was correctly applied
            const auto &attributes = image->getAttributes();
            QCOMPARE(attributes.size(), 3);

            std::unordered_set<QString> expectedAttributes {
                ID, Common::album_id_s, Common::ext,
            };
            for (const auto &attribute : attributes)
                QVERIFY(expectedAttributes.contains(attribute.key));

            QCOMPARE(image->getAttribute(Common::album_id_s), albumId);
            QVERIFY(expectedImage.imagesId.contains(
                        image->getKey().value<AlbumImage::KeyType>()));
            QCOMPARE(typeid (AlbumImage *), typeid (image));
        }
    }
}

void tst_Collection_Relations::load_rvalue_WithLambdaConstraint() const
{
    auto albums = Album::without(Common::albumImages)->findMany({1, 2, 4});
    QCOMPARE(albums.size(), 3);
    QCOMPARE(typeid (ModelsCollection<Album>), typeid (albums));
    QVERIFY(Common::verifyIds(albums, {1, 2, 4}));

    // Prepare
    ModelsCollection<Album *> albumsInit {
        &albums[0], // NOLINT(readability-container-data-pointer)
        &albums[1],
    };

    // Verify before
    QVERIFY(std::ranges::all_of(albumsInit, [](Album *const album)
    {
        return album->getRelations().empty();
    }));

    // Load the albumImages hasMany relation
    ModelsCollection<Album *> &&result =
            std::move(albumsInit)
            .load({{"albumImages", [](auto &query)
                    {
                        query.select({ID, Common::album_id_s, SIZE_});
                    }}});

    /* In 99% cases it must be the same ModelsCollection (the same memory address) but
       I'm disabling the QVERIFY check because this is compiler specific, if the result
       is rvalue then the albums will be constructed in-place and no moves will be done,
       the compiler optimizes it out, if the result would be just only result without
       the rvalue reference then the move constructor would be called.
       Another thing is that I need to call std::move(albums).load() because I need to
       verify the albums, the real world scenario would be:
       Album::without(Common::albumImages)->findMany({1, 2, 4}).load("albumImages")
       At the end of the day both command statements are the same. */
//    QVERIFY(reinterpret_cast<uintptr_t>(&result) ==
//            reinterpret_cast<uintptr_t>(&albumsInit));

    // Prepare - AlbumImage::KeyType is Album ID
    std::unordered_map<AlbumImage::KeyType, ExpectedImages> expectedImages {
        {1, {1, {1}}},
        {2, {5, {2, 3, 4, 5, 6}}},
        {4, {0, {}}},
    };

    // Verify after, both the result and also original albums vector
    // Original albums collection
    for (Album *const album : result) {
        const auto albumId = album->getKey();
        const auto &expectedImage = expectedImages.at(albumId.value<Album::KeyType>());

        QVERIFY(album->relationLoaded(Common::albumImages));
        QVERIFY(!album->getRelations().empty());
        const auto &relations = album->getRelations();
        QCOMPARE(relations.size(), 1);
        QVERIFY(relations.contains(Common::albumImages));

        // AlbumImage has-many relation
        auto images = album->getRelation<AlbumImage>(Common::albumImages);
        QCOMPARE(images.size(), expectedImage.imagesSize);
        QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));

        // Expected image IDs
        for (AlbumImage *const image : images) {
            QVERIFY(image);
            QVERIFY(image->exists);

            // Check whether constraints was correctly applied
            const auto &attributes = image->getAttributes();
            QCOMPARE(attributes.size(), 3);

            std::unordered_set<QString> expectedAttributes {
                ID, Common::album_id_s, SIZE_,
            };
            for (const auto &attribute : attributes)
                QVERIFY(expectedAttributes.contains(attribute.key));

            QCOMPARE(image->getAttribute(Common::album_id_s), albumId);
            QVERIFY(expectedImage.imagesId.contains(
                        image->getKey().value<AlbumImage::KeyType>()));
            QCOMPARE(typeid (AlbumImage *), typeid (image));
        }
    }
}

void tst_Collection_Relations::load_rvalue_NonExistentRelation_Failed() const
{
    auto albums = Album::without(Common::albumImages)->findMany({1, 2});
    QCOMPARE(albums.size(), 2);
    QCOMPARE(typeid (ModelsCollection<Album>), typeid (albums));
    QVERIFY(Common::verifyIds(albums, {1, 2}));

    // Prepare
    ModelsCollection<Album *> albumsInit {
        &albums[0], // NOLINT(readability-container-data-pointer)
        &albums[1],
    };

    const auto verify = [&albumsInit]
    {
        QVERIFY(std::ranges::all_of(albumsInit, [](Album *const album)
        {
            return album->getRelations().empty();
        }));
    };

    // Verify before
    verify();

    QVERIFY_EXCEPTION_THROWN(std::move(albumsInit).load("albumImages-NON_EXISTENT"),
                             RelationNotFoundError);

    // Verify after
    verify();
}

/* EnumeratesValues */

void tst_Collection_Relations::reject() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.reject([](AlbumImage *const image)
    {
        const auto id = Common::getKeyCasted(image);
        return id == 3 || id == 5;
    });

    // Verify
    QCOMPARE(result.size(), 3);
    QVERIFY(Common::verifyIds(result, {2, 4, 6}));
}

void tst_Collection_Relations::reject_WithIndex() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.reject([](AlbumImage *const image, const auto index)
    {
        return Common::getKeyCasted(image) == 3 || index == 2;
    });

    // Verify
    QCOMPARE(result.size(), 3);
    QVERIFY(Common::verifyIds(result, {2, 5, 6}));
}

void tst_Collection_Relations::where_QString_EQ() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.whereEq<QString>(NAME, "album2_image3");

    // Verify
    QCOMPARE(result.size(), 1);
    QVERIFY(Common::verifyIds(result, {4}));
}

void tst_Collection_Relations::where_quint64_NE() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.where(ID, NE, 4);

    // Verify
    QCOMPARE(result.size(), 4);
    QVERIFY(Common::verifyIds(result, {2, 3, 5, 6}));
}

void tst_Collection_Relations::where_quint64_GT() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.where(ID, GT, 3);

    // Verify
    QCOMPARE(result.size(), 3);
    QVERIFY(Common::verifyIds(result, {4, 5, 6}));
}

void tst_Collection_Relations::where_WithNullQVariant() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Prepare
    (*images[1])[NAME] = NullVariant::QString();

    // Get result
    const auto result = images.whereEq<QString>(NAME, "album2_image3");

    // Verify
    QCOMPARE(result.size(), 1);
    QVERIFY(Common::verifyIds(result, {4}));
}

void tst_Collection_Relations::where_InvalidComparisonOperator_ThrowException() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY_EXCEPTION_THROWN(images.where(ID, "dummy-NON_EXISTENT", 10),
                             InvalidArgumentError);
}

void tst_Collection_Relations::whereNull_QString() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Prepare
    (*images.at(1))[Common::ext] = NullVariant::QString();
    (*images.at(2))[Common::ext] = NullVariant::QString();

    // Get result
    const auto result = images.whereNull(Common::ext);

    // Verify
    QCOMPARE(result.size(), 2);
    QVERIFY(Common::verifyIds(result, {3, 4}));
}

void tst_Collection_Relations::whereNotNull_QString() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Prepare
    (*images.at(1))[Common::ext] = NullVariant::QString();
    (*images.at(2))[Common::ext] = NullVariant::QString();

    // Get result
    const auto result = images.whereNotNull(Common::ext);

    // Verify
    QCOMPARE(result.size(), 3);
    QVERIFY(Common::verifyIds(result, {2, 5, 6}));
}

void tst_Collection_Relations::whereIn_QString() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.whereIn<QString>(Common::ext, {"png", "gif"});

    // Verify
    QCOMPARE(result.size(), 3);
    QVERIFY(Common::verifyIds(result, {2, 5, 6}));
}

void tst_Collection_Relations::whereIn_quint64() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.whereIn<quint64>(ID, {3, 5, 6});

    // Verify
    QCOMPARE(result.size(), 3);
    QVERIFY(Common::verifyIds(result, {3, 5, 6}));
}

void tst_Collection_Relations::whereIn_Empty() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.whereIn<quint64>(ID, {});

    // Verify
    QVERIFY(result.isEmpty());
}

void tst_Collection_Relations::whereNotIn_QString() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.whereNotIn<QString>(Common::ext, {"png", "gif"});

    // Verify
    QCOMPARE(result.size(), 2);
    QVERIFY(Common::verifyIds(result, {3, 4}));
}

void tst_Collection_Relations::whereNotIn_quint64() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.whereNotIn<quint64>(ID, {3, 5, 6});

    // Verify
    QCOMPARE(result.size(), 2);
    QVERIFY(Common::verifyIds(result, {2, 4}));
}

void tst_Collection_Relations::whereNotIn_Empty() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.whereNotIn<quint64>(ID, {});

    // Verify
    QCOMPARE(result.size(), 5);
    QVERIFY(Common::verifyIds(result, {2, 3, 4, 5, 6}));
}

void tst_Collection_Relations::whereBetween() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.whereBetween<quint64>(ID, {3, 5});

    // Verify
    QCOMPARE(result.size(), 3);
    QVERIFY(Common::verifyIds(result, {3, 4, 5}));
}

void tst_Collection_Relations::whereNotBetween() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.whereNotBetween<quint64>(ID, {3, 5});

    // Verify
    QCOMPARE(result.size(), 2);
    QVERIFY(Common::verifyIds(result, {2, 6}));
}

void tst_Collection_Relations::firstWhere_QString() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.firstWhereEq<QString>(NAME, "album2_image3");

    // Verify
    QVERIFY(result != nullptr);
    QCOMPARE(Common::getKeyCasted(result), 4);
}

void tst_Collection_Relations::firstWhere_quint64_NE() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.firstWhere<quint64>(ID, NE, 2);

    // Verify
    QVERIFY(result != nullptr);
    QCOMPARE(Common::getKeyCasted(result), 3);
}

void tst_Collection_Relations::firstWhere_quint64_GE() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.firstWhere<quint64>(ID, GE, 4);

    // Verify
    QVERIFY(result != nullptr);
    QCOMPARE(Common::getKeyCasted(result), 4);
}

void tst_Collection_Relations::firstWhere_NotFound_nullptr() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.firstWhere<quint64>(ID, GE, 40);

    // Verify
    QVERIFY(result == nullptr);
}

void tst_Collection_Relations::value_QVariant() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.value(NAME);

    // Verify
    QVERIFY(!result.isNull() && result.isValid());
    QCOMPARE(result, QVariant("album2_image1"));
}

void tst_Collection_Relations::value_QVariant_NonExistentAttribute() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.value("dummy-NON_EXISTENT");

    // Verify
    QVERIFY(!result.isValid());
    QCOMPARE(result, QVariant());
}

void tst_Collection_Relations::value_QVariant_NonExistentAttribute_DefaultValue() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.value("dummy-NON_EXISTENT", "default-value");

    // Verify
    QVERIFY(!result.isNull() && result.isValid());
    QCOMPARE(result, QVariant("default-value"));
}

void tst_Collection_Relations::value_QVariant_EmptyCollection() const
{
    // Get result
    const auto result = ModelsCollection<AlbumImage *>().value("dummy-NON_EXISTENT");

    // Verify
    QVERIFY(!result.isValid());
    QCOMPARE(result, QVariant());
}

void tst_Collection_Relations::value_quint64() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.value<quint64>(SIZE_);

    // Verify
    QVERIFY((std::is_same_v<std::remove_const_t<decltype (result)>, quint64>));
    QCOMPARE(result, 424);
}

void tst_Collection_Relations::value_quint64_NonExistentAttribute_DefaultValue() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.value<quint64>("dummy-NON_EXISTENT", 0);

    // Verify
    QVERIFY((std::is_same_v<std::remove_const_t<decltype (result)>, quint64>));
    QCOMPARE(result, 0);
}

void tst_Collection_Relations::value_QString() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.value<QString>(NAME);

    // Verify
    QVERIFY((std::is_same_v<std::remove_const_t<decltype (result)>, QString>));
    QCOMPARE(result, QString("album2_image1"));
}

void tst_Collection_Relations::value_QString_NonExistentAttribute_DefaultValue() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.value<QString>("dummy-NON_EXISTENT", "default-value");

    // Verify
    QVERIFY((std::is_same_v<std::remove_const_t<decltype (result)>, QString>));
    QCOMPARE(result, QVariant("default-value"));
}

void tst_Collection_Relations::each_lvalue() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    QVector<quint64> expectedIds;
    expectedIds.reserve(images.size());

    ModelsCollection<AlbumImage *> &result = images.each([&expectedIds]
                                                         (AlbumImage *const image)
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        expectedIds.emplace_back(Common::getKeyCasted(image));
#else
        expectedIds.append(Common::getKeyCasted(image));
#endif
    });

    // Verify
    // Both must be lvalue references because of that the decltype ((images)) is used
    QVERIFY((std::is_same_v<decltype (result), decltype ((images))>));
    // It must be the same ModelsCollection (the same memory address)
    QVERIFY(reinterpret_cast<uintptr_t>(&result) ==
            reinterpret_cast<uintptr_t>(&images));
    QCOMPARE(expectedIds.size(), 5);
    QCOMPARE(QVector<quint64>({2, 3, 4, 5, 6}), expectedIds);
}

/*! ExpectedItem used in the each() related tests. */
struct ExpectedItem
{
    /*! Model ID. */
    quint64 id;
    /*! Looping index. */
    ModelsCollection<AlbumImage *>::size_type index;

    /*! Equality comparison operator for the ExpectedItem. */
    inline bool operator==(const ExpectedItem &) const = default; // clazy:exclude=function-args-by-value
};

void tst_Collection_Relations::each_lvalue_index() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    QVector<ExpectedItem> expectedIds;
    expectedIds.reserve(images.size());

    ModelsCollection<AlbumImage *> &result =
            images.each([&expectedIds]
                        (AlbumImage *const image, const auto index)
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        expectedIds.emplace_back(Common::getKeyCasted(image), index);
#else
        expectedIds.append({Common::getKeyCasted(image), index});
#endif
    });

    // Verify
    // Both must be lvalue references because of that the decltype ((images)) is used
    QVERIFY((std::is_same_v<decltype (result), decltype ((images))>));
    // It must be the same ModelsCollection (the same memory address)
    QVERIFY(reinterpret_cast<uintptr_t>(&result) ==
            reinterpret_cast<uintptr_t>(&images));
    QCOMPARE(expectedIds.size(), 5);
    QCOMPARE(QVector<ExpectedItem>({{2, 0}, {3, 1}, {4, 2}, {5, 3}, {6, 4}}),
             expectedIds);
}

void tst_Collection_Relations::each_lvalue_bool() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    QVector<quint64> expectedIds;
    expectedIds.reserve(images.size());

    ModelsCollection<AlbumImage *> &result =
            images.each(Orm::EachBoolCallback,
                        [&expectedIds](AlbumImage *const image) -> bool
    {
        // Exit/break the each() looping
        if (Common::getKeyCasted(image) == 5)
            return false;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        expectedIds.emplace_back(Common::getKeyCasted(image));
#else
        expectedIds.append(Common::getKeyCasted(image));
#endif

        return true;
    });

    // Verify
    // Both must be lvalue references because of that the decltype ((images)) is used
    QVERIFY((std::is_same_v<decltype (result), decltype ((images))>));
    // It must be the same ModelsCollection (the same memory address)
    QVERIFY(reinterpret_cast<uintptr_t>(&result) ==
            reinterpret_cast<uintptr_t>(&images));
    QCOMPARE(expectedIds.size(), 3);
    QCOMPARE(QVector<quint64>({2, 3, 4}), expectedIds);
}

void tst_Collection_Relations::each_lvalue_bool_index() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    QVector<ExpectedItem> expectedIds;
    expectedIds.reserve(images.size());

    ModelsCollection<AlbumImage *> &result =
            images.each(Orm::EachBoolCallback,
                        [&expectedIds]
                        (AlbumImage *const image, const auto index) -> bool
    {
        // Exit/break the each() looping
        if (index == 3)
            return false;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        expectedIds.emplace_back(Common::getKeyCasted(image), index);
#else
        expectedIds.append({Common::getKeyCasted(image), index});
#endif

        return true;
    });

    // Verify
    // Both must be lvalue references because of that the decltype ((images)) is used
    QVERIFY((std::is_same_v<decltype (result), decltype ((images))>));
    // It must be the same ModelsCollection (the same memory address)
    QVERIFY(reinterpret_cast<uintptr_t>(&result) ==
            reinterpret_cast<uintptr_t>(&images));
    QCOMPARE(expectedIds.size(), 3);
    QCOMPARE(QVector<ExpectedItem>({{2, 0}, {3, 1}, {4, 2}}),
             expectedIds);
}

void tst_Collection_Relations::each_rvalue() const
{
    auto images = AlbumImage::whereEq(Common::album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Prepare
    ModelsCollection<AlbumImage *> imagesInit {
        &images[0], // NOLINT(readability-container-data-pointer)
        &images[1],
        &images[2],
        &images[3],
        &images[4],
    };

    // Get result
    QVector<quint64> expectedIds;
    expectedIds.reserve(imagesInit.size());

    ModelsCollection<AlbumImage *> result =
            std::move(imagesInit).each([&expectedIds]
                                       (AlbumImage *const image)
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        expectedIds.emplace_back(Common::getKeyCasted(image));
#else
        expectedIds.append(Common::getKeyCasted(image));
#endif
    });

    // Verify
    QVERIFY((std::is_same_v<decltype (result), decltype (imagesInit)>));
    QCOMPARE(expectedIds.size(), 5);
    QCOMPARE(QVector<quint64>({2, 3, 4, 5, 6}), expectedIds);
}

void tst_Collection_Relations::each_rvalue_index() const
{
    auto images = AlbumImage::whereEq(Common::album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Prepare
    ModelsCollection<AlbumImage *> imagesInit {
        &images[0], // NOLINT(readability-container-data-pointer)
        &images[1],
        &images[2],
        &images[3],
        &images[4],
    };

    // Get result
    QVector<ExpectedItem> expectedIds;
    expectedIds.reserve(imagesInit.size());

    ModelsCollection<AlbumImage *> result =
            std::move(imagesInit).each([&expectedIds]
                                       (AlbumImage *const image, const auto index)
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        expectedIds.emplace_back(Common::getKeyCasted(image), index);
#else
        expectedIds.append({Common::getKeyCasted(image), index});
#endif
    });

    // Verify
    QVERIFY((std::is_same_v<decltype (result), decltype (imagesInit)>));
    QCOMPARE(expectedIds.size(), 5);
    QCOMPARE(QVector<ExpectedItem>({{2, 0}, {3, 1}, {4, 2}, {5, 3}, {6, 4}}),
             expectedIds);
}

void tst_Collection_Relations::each_rvalue_bool() const
{
    auto images = AlbumImage::whereEq(Common::album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Prepare
    ModelsCollection<AlbumImage *> imagesInit {
        &images[0], // NOLINT(readability-container-data-pointer)
        &images[1],
        &images[2],
        &images[3],
        &images[4],
    };

    // Get result
    QVector<quint64> expectedIds;
    expectedIds.reserve(imagesInit.size());

    ModelsCollection<AlbumImage *> result =
            std::move(imagesInit).each(Orm::EachBoolCallback,
                                       [&expectedIds](AlbumImage *const image) -> bool
    {
        // Exit/break the each() looping
        if (Common::getKeyCasted(image) == 5)
            return false;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        expectedIds.emplace_back(Common::getKeyCasted(image));
#else
        expectedIds.append(Common::getKeyCasted(image));
#endif

        return true;
    });

    // Verify
    QVERIFY((std::is_same_v<decltype (result), decltype (imagesInit)>));
    QCOMPARE(expectedIds.size(), 3);
    QCOMPARE(QVector<quint64>({2, 3, 4}), expectedIds);
}

void tst_Collection_Relations::each_rvalue_bool_index() const
{
    auto images = AlbumImage::whereEq(Common::album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));

    // Prepare
    ModelsCollection<AlbumImage *> imagesInit {
        &images[0], // NOLINT(readability-container-data-pointer)
        &images[1],
        &images[2],
        &images[3],
        &images[4],
    };

    // Get result
    QVector<ExpectedItem> expectedIds;
    expectedIds.reserve(imagesInit.size());

    ModelsCollection<AlbumImage *> result =
            std::move(imagesInit)
            .each(Orm::EachBoolCallback,
                  [&expectedIds](AlbumImage *const image, const auto index) -> bool
    {
        // Exit/break the each() looping
        if (index == 3)
            return false;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        expectedIds.emplace_back(Common::getKeyCasted(image), index);
#else
        expectedIds.append({Common::getKeyCasted(image), index});
#endif

        return true;
    });

    // Verify
    QVERIFY((std::is_same_v<decltype (result), decltype (imagesInit)>));
    QCOMPARE(expectedIds.size(), 3);
    QCOMPARE(QVector<ExpectedItem>({{2, 0}, {3, 1}, {4, 2}}),
             expectedIds);
}

void tst_Collection_Relations::tap_lvalue() const
{
    auto album = Album::find(2);
    QVERIFY(album);
    QVERIFY(album->exists);
    QCOMPARE(album->getKey(), QVariant(2));
    QVERIFY(album->relationLoaded(Common::albumImages));

    auto images = album->getRelation<AlbumImage>(Common::albumImages);
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage *>), typeid (images));
    QVERIFY(Common::verifyIds(images, {2, 3, 4, 5, 6}));
    QCOMPARE(images.at(2)->getAttribute(NAME), QVariant("album2_image3"));

    // Get result
    auto callbackInvoked = false;
    uintptr_t imagesInCallbackAddress = 0;

    ModelsCollection<AlbumImage *> &result =
            images.tap([&callbackInvoked, &imagesInCallbackAddress]
                       (ModelsCollection<AlbumImage *> &imagesRef)
    {
        callbackInvoked = true;
        imagesInCallbackAddress = reinterpret_cast<uintptr_t>(&imagesRef);

        // Change the name, append " NEW"
        auto &image4 = *imagesRef.at(2);
        image4[NAME] = SPACE_IN.arg(image4.getAttribute<QString>(NAME),
                                    QStringLiteral("NEW"));
    });

    // Verify
    // Both must be lvalue references because of that the decltype ((images)) is used
    QVERIFY((std::is_same_v<decltype (result), decltype ((images))>));
    QVERIFY(callbackInvoked);
    // It must be the same ModelsCollection (the same memory address)
    const auto imagesAddress = reinterpret_cast<uintptr_t>(&images);
    QVERIFY(reinterpret_cast<uintptr_t>(&result) == imagesAddress);
    QVERIFY(imagesInCallbackAddress == imagesAddress);
    // Collection content
    QCOMPARE(result.size(), 5);
    QVERIFY(Common::verifyIds(result, {2, 3, 4, 5, 6}));
    // Verify the changed name
    QCOMPARE(result.at(2)->getAttribute(NAME), QVariant("album2_image3 NEW"));
}

void tst_Collection_Relations::tap_rvalue() const
{
    // Prepare
    AlbumImage image2 {{ID, 2}, {NAME, QStringLiteral("image2")}};
    ModelsCollection<AlbumImage *> imagesInit {&image2};

    // Get result
    auto callbackInvoked = false;

    ModelsCollection<AlbumImage *> result =
            std::move(imagesInit).tap([&callbackInvoked]
                                      (ModelsCollection<AlbumImage *> &imagesRef)
    {
        callbackInvoked = true;

        // Change the name, append " NEW"
        auto &image2_ = *imagesRef.first();
        image2_[NAME] = SPACE_IN.arg(image2_.getAttribute<QString>(NAME),
                                     QStringLiteral("NEW"));
    });

    // Verify
    QVERIFY((std::is_same_v<decltype (result), decltype (imagesInit)>));
    QVERIFY(callbackInvoked);
    // Collection content
    QCOMPARE(result.size(), 1);
    QVERIFY(Common::verifyIds(result, {2}));
    // Verify the changed name
    QCOMPARE(result.first()->getAttribute(NAME), QVariant("image2 NEW"));
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_Collection_Relations)

#include "tst_collection_relations.moc"
