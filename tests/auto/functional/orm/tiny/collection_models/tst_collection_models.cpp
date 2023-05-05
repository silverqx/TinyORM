#include <QCoreApplication>
#include <QtSql/QSqlDriver>
#include <QtTest>

#include "databases.hpp"

#include "models/albumimage.hpp"

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
using Orm::Tiny::Types::ModelsCollection;

using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

using Models::Album;
using Models::AlbumImage;

class tst_Collection_Models : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase() const;

    /* BaseCollection */
    void filter() const;
    void filter_WithIndex() const;

    void first() const;
    void first_NotFound_nullptr() const;
    void first_NotFound_DefaultModel() const;

    void implode_Name() const;
    void implode_Id() const;
    void implode_Note_ColumnWithNull() const;

    /* Collection */
    void modelKeys_QVariant() const;
    void modelKeys_quint64() const;

    void map() const;
    void map_WithIndex() const;

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
    void pluck_KeyedById() const;

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

    /* EnumeratesValues */
    void reject() const;
    void reject_WithIndex() const;

    void where_QString_EQ() const;
    void where_quint64_NE() const;
    void where_quint64_GT() const;
    void where_InvalidComparisonOperator_ThrowException() const;

    void whereNull_QString() const;
    void whereNotNull_QString() const;
    void whereNull_quint64() const;
    void whereNotNull_quint64() const;

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
    /*! Verify primary key IDs in the given collection. */
    template<typename M>
    static bool
    verifyIds(const ModelsCollection<M *> &actual,
              const std::unordered_set<quint64> &expected);
    /*! Verify primary key IDs in the given collection. */
    template<typename M>
    static bool
    verifyIds(const ModelsCollection<M> &actual,
              const std::unordered_set<quint64> &expected);
    /*! Verify primary key IDs in the given vector. */
    inline static bool
    verifyIds(const QVector<quint64> &actual,
              const std::unordered_set<quint64> &expected);
    /*! Verify primary key IDs in the given vector. */
    template<typename T>
    static bool
    verifyIdsQVariant(const QVector<QVariant> &actual,
                      const std::unordered_set<T> &expected);

    /*! Verify attribute values in the given vector. */
    template<typename T, typename M>
    static bool
    verifyAttributeValues(const QString &attribute, const ModelsCollection<M *> &actual,
                          const std::unordered_set<T> &expected);

    /*! Get the value of the model's primary key casted to the Derived::KeyType. */
    template<typename M>
    static typename M::KeyType getKeyCasted(M *model);
    /*! Get the value of the model's primary key casted to the Derived::KeyType. */
    template<typename M>
    static typename M::KeyType getKeyCasted(const M &model);

    /*! Connection name used in this test case. */
    QString m_connection {};
};

/*! Orm::Column constant "album_id" (perf. reason, one time initialization). */
Q_GLOBAL_STATIC_WITH_ARGS(Orm::Column, album_id, ("album_id")) // NOLINT(misc-use-anonymous-namespace)

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Collection_Models::initTestCase()
{
    ConnectionOverride::connection = m_connection =
            Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );
}

void tst_Collection_Models::cleanupTestCase() const
{
    // Reset connection override
    ConnectionOverride::connection.clear();
}

/* BaseCollection */

void tst_Collection_Models::filter() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.filter([](AlbumImage *const image)
    {
        const auto id = getKeyCasted(image);
        return id == 3 || id == 5;
    });

    // Verify
    QCOMPARE(result.size(), 2);
    QVERIFY(verifyIds(result, {3, 5}));
}

void tst_Collection_Models::filter_WithIndex() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.filter([](AlbumImage *const image, const auto index)
    {
        return getKeyCasted(image) == 3 || index == 2;
    });

    // Verify
    QCOMPARE(result.size(), 2);
    QVERIFY(verifyIds(result, {3, 4}));
}

void tst_Collection_Models::first() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.first([](AlbumImage *const image)
    {
        return getKeyCasted(image) == 3;
    });

    // Verify
    QCOMPARE(getKeyCasted(result), 3);
}

void tst_Collection_Models::first_NotFound_nullptr() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.first([](AlbumImage *const image)
    {
        return getKeyCasted(image) == 30;
    });

    // Verify
    QVERIFY(result == nullptr);
}

void tst_Collection_Models::first_NotFound_DefaultModel() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.first([](AlbumImage *const image)
    {
        return getKeyCasted(image) == 30;
    },
        &images[2]);

    // Verify
    QVERIFY(result != nullptr);
    QCOMPARE(getKeyCasted(result), 4);
}

void tst_Collection_Models::implode_Name() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.implode(NAME, COMMA);

    // Verify
    QCOMPARE(result, QString("album2_image1, album2_image2, album2_image3, "
                             "album2_image4, album2_image5"));
}

void tst_Collection_Models::implode_Id() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.implode(ID, COMMA);

    // Verify
    QCOMPARE(result, QString("2, 3, 4, 5, 6"));
}

void tst_Collection_Models::implode_Note_ColumnWithNull() const
{
    auto albums = Album::all();
    QCOMPARE(albums.size(), 4);
    QCOMPARE(typeid (ModelsCollection<Album>), typeid (albums));

    // Get result
    const auto result = albums.implode("note", COMMA);

    // Verify
    QCOMPARE(result, QString(", , album3 note, no images"));
}

/* Collection */

void tst_Collection_Models::modelKeys_QVariant() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.modelKeys();

    // Verify
    QCOMPARE(result.size(), 5);
    QVERIFY(verifyIdsQVariant<quint64>(result, {2, 3, 4, 5, 6}));
}

void tst_Collection_Models::modelKeys_quint64() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.modelKeys<quint64>();

    // Verify
    QCOMPARE(result.size(), 5);
    QVERIFY(verifyIds(result, {2, 3, 4, 5, 6}));
}

void tst_Collection_Models::map() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.map([](AlbumImage *const image)
    {
        if (const auto id = getKeyCasted(image);
            id == 3 || id == 6
        )
            (*image)[NAME] = QStringLiteral("%1_id_%2")
                             .arg(image->getAttribute(NAME).template value<QString>())
                             .arg(id);
        return image;
    });

    // Verify
    QCOMPARE(result.size(), 5);
    QVERIFY(verifyIds(result, {2, 3, 4, 5, 6}));
    QVERIFY(verifyAttributeValues<QString>(
                NAME, result, {"album2_image1", "album2_image2_id_3", "album2_image3",
                               "album2_image4", "album2_image5_id_6"}));
}

void tst_Collection_Models::map_WithIndex() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.map([](AlbumImage *const image, const auto index)
    {
        if (const auto id = getKeyCasted(image);
            id == 2
        )
            (*image)[NAME] = QStringLiteral("%1_id_%2")
                             .arg(image->getAttribute(NAME).template value<QString>())
                             .arg(id);

        else if (index == 3)
            (*image)[NAME] = QStringLiteral("%1_index_%2")
                             .arg(image->getAttribute(NAME).template value<QString>())
                             .arg(index);

        return image;
    });

    // Verify
    QCOMPARE(result.size(), 5);
    QVERIFY(verifyIds(result, {2, 3, 4, 5, 6}));
    QVERIFY(verifyAttributeValues<QString>(
                NAME, result, {"album2_image1_id_2", "album2_image2", "album2_image3",
                               "album2_image4_index_3", "album2_image5"}));
}

void tst_Collection_Models::mapWithModelKeys() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.mapWithModelKeys();

    // Verify
    QCOMPARE(result.size(), 5);

    std::unordered_map<quint64, AlbumImage *> expected {
        {2, images.data()},
        {3, &images[1]},
        {4, &images[2]},
        {5, &images[3]},
        {6, &images[4]},
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Models::mapWithKeys_IdAndName() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result =
            images.mapWithKeys<quint64, QString>(
                [](AlbumImage *const image) -> std::pair<quint64, QString>
    {
        return {getKeyCasted(image),
                image->getAttribute(NAME).template value<QString>()};
    });

    // Verify
    QCOMPARE(result.size(), 5);

    std::unordered_map<quint64, QString> expected {
        {2, images[0].getAttribute(NAME).template value<QString>()},
        {3, images[1].getAttribute(NAME).template value<QString>()},
        {4, images[2].getAttribute(NAME).template value<QString>()},
        {5, images[3].getAttribute(NAME).template value<QString>()},
        {6, images[4].getAttribute(NAME).template value<QString>()},
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Models::mapWithKeys_NameAndId() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result =
            images.mapWithKeys<QString, quint64>(
                [](AlbumImage *const image) -> std::pair<QString, quint64>
    {
        return {image->getAttribute(NAME).template value<QString>(),
                getKeyCasted(image)};
    });

    // Verify
    QCOMPARE(result.size(), 5);

    std::unordered_map<QString, quint64> expected {
        {images[0].getAttribute(NAME).template value<QString>(), 2},
        {images[1].getAttribute(NAME).template value<QString>(), 3},
        {images[2].getAttribute(NAME).template value<QString>(), 4},
        {images[3].getAttribute(NAME).template value<QString>(), 5},
        {images[4].getAttribute(NAME).template value<QString>(), 6},
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Models::mapWithKeys_IdAndModelPointer() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result =
            images.mapWithKeys<quint64, AlbumImage *>(
                [](AlbumImage *const image) -> std::pair<quint64, AlbumImage *>
    {
        return {getKeyCasted(image), image};
    });

    // Verify
    QCOMPARE(result.size(), 5);

    std::unordered_map<quint64, AlbumImage *> expected {
        {2, &images[0]},
        {3, &images[1]},
        {4, &images[2]},
        {5, &images[3]},
        {6, &images[4]},
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Models::mapWithKeys_IdAndModel() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result =
            images.mapWithKeys<quint64, AlbumImage>(
                [](AlbumImage *const image) -> std::pair<quint64, AlbumImage>
    {
        return {getKeyCasted(image), *image};
    });

    // Verify
    QCOMPARE(result.size(), 5);

    std::unordered_map<quint64, AlbumImage> expected {
        {2, images.at(0)},
        {3, images.at(1)},
        {4, images.at(2)},
        {5, images.at(3)},
        {6, images.at(4)},
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Models::only() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.only({6, 2, 5, 10, 2});

    // Verify
    QCOMPARE(result.size(), 3);

    ModelsCollection<AlbumImage *> expected {
        images.data(),
        &images[3],
        &images[4],
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Models::only_Empty() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.only({});

    // Verify
    QVERIFY(result.isEmpty());
}

void tst_Collection_Models::except() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.except({5, 2, 10, 2});

    // Verify
    QCOMPARE(result.size(), 3);

    ModelsCollection<AlbumImage *> expected {
        &images[1],
        &images[2],
        &images[4],
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Models::except_Empty() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.except({});

    // Verify
    QCOMPARE(result.size(), 5);

    ModelsCollection<AlbumImage *> expected {
        images.data(),
        &images[1],
        &images[2],
        &images[3],
        &images[4],
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Models::pluck() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

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

void tst_Collection_Models::pluck_KeyedById() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

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

void tst_Collection_Models::contains_ById() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY(images.contains(2));
    QVERIFY(images.contains(6));
    QVERIFY(!images.contains(1));
    QVERIFY(!images.contains(7));
    QVERIFY(!images.contains(20));
}

void tst_Collection_Models::contains_ById_QVariant() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY(images.contains(QVariant(2)));
    QVERIFY(images.contains(QVariant(6)));
    QVERIFY(!images.contains(QVariant(1)));
    QVERIFY(!images.contains(QVariant(7)));
    QVERIFY(!images.contains(QVariant(20)));
}

void tst_Collection_Models::contains_Callback() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY(images.contains([](AlbumImage *const image)
    {
        return image->getAttribute(ID).template value<quint64>() == 2;
    }));
    QVERIFY(images.contains([](AlbumImage *const image)
    {
        return image->getAttribute(ID).template value<quint64>() == 6;
    }));
    QVERIFY(images.contains([](AlbumImage *const image)
    {
        return image->getAttribute(ID).template value<quint64>() != 1;
    }));
    QVERIFY(images.contains([](AlbumImage *const image)
    {
        return image->getAttribute(ID).template value<quint64>() != 7;
    }));
    QVERIFY(images.contains([](AlbumImage *const image)
    {
        return image->getAttribute(ID).template value<quint64>() != 20;
    }));
}

void tst_Collection_Models::contains_Model() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY(images.contains(images[0]));
    QVERIFY(images.contains(images[4]));
    // Make a copy so I can modify the ID
    AlbumImage image = images[0];
    image[ID] = 1;
    QVERIFY(!images.contains(image));
    image[ID] = 7;
    QVERIFY(!images.contains(image));
    image[ID] = 20;
    QVERIFY(!images.contains(image));
    QVERIFY(!images.contains(std::nullopt));
}

void tst_Collection_Models::doesntContain_ById() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY(images.doesntContain(1));
    QVERIFY(images.doesntContain(7));
    QVERIFY(images.doesntContain(20));
    QVERIFY(!images.doesntContain(2));
    QVERIFY(!images.doesntContain(6));
}

void tst_Collection_Models::doesntContain_ById_QVariant() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY(images.doesntContain(QVariant(1)));
    QVERIFY(images.doesntContain(QVariant(7)));
    QVERIFY(images.doesntContain(QVariant(20)));
    QVERIFY(!images.doesntContain(QVariant(2)));
    QVERIFY(!images.doesntContain(QVariant(6)));
}

void tst_Collection_Models::doesntContain_Callback() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY(images.doesntContain([](AlbumImage *const image)
    {
        return image->getAttribute(ID).template value<quint64>() == 1;
    }));
    QVERIFY(images.doesntContain([](AlbumImage *const image)
    {
        return image->getAttribute(ID).template value<quint64>() == 7;
    }));
    QVERIFY(images.doesntContain([](AlbumImage *const image)
    {
        return image->getAttribute(ID).template value<quint64>() == 20;
    }));
    /* All the following conditions are correct, I have investigated it and even if
       it's weird they are correct; simply this is how the mathematic works.
       So the conclusion is that this doesntContain(callback) is weird if the != is
       used inside the callback. */
    QVERIFY(!images.doesntContain([](AlbumImage *const image)
    {
        return image->getAttribute(ID).template value<quint64>() == 2;
    }));
    QVERIFY(!images.doesntContain([](AlbumImage *const image)
    {
        return image->getAttribute(ID).template value<quint64>() != 2;
    }));
    QVERIFY(!images.doesntContain([](AlbumImage *const image)
    {
        return image->getAttribute(ID).template value<quint64>() == 6;
    }));
    QVERIFY(!images.doesntContain([](AlbumImage *const image)
    {
        return image->getAttribute(ID).template value<quint64>() != 6;
    }));
}

void tst_Collection_Models::doesntContain_Model() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY(!images.doesntContain(images[0]));
    QVERIFY(!images.doesntContain(images[4]));
    // Make a copy so I can modify the ID
    AlbumImage image = images[0];
    image[ID] = 1;
    QVERIFY(images.doesntContain(image));
    image[ID] = 7;
    QVERIFY(images.doesntContain(image));
    image[ID] = 20;
    QVERIFY(images.doesntContain(image));
    QVERIFY(images.doesntContain(std::nullopt));
}

void tst_Collection_Models::find() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.find(4);

    // Verify
    QVERIFY(result);
    QVERIFY(result->exists);
    QCOMPARE(result->getAttribute(ID), QVariant(4));
}

void tst_Collection_Models::find_NotFound_nullptr() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.find(40);

    // Verify
    QVERIFY(result == nullptr);
}

void tst_Collection_Models::find_NotFound_DefaultModel() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.find(40, &images[3]);

    // Verify
    QVERIFY(result);
    QVERIFY(result->exists);
    QCOMPARE(result->getAttribute(ID), QVariant(5));
}

void tst_Collection_Models::find_Model() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.find(images.at(2));

    // Verify
    QVERIFY(result);
    QVERIFY(result->exists);
    QCOMPARE(result->getAttribute(ID), QVariant(4));
}

void tst_Collection_Models::find_Model_NotFound_nullptr() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Prepare
    AlbumImage image40 = images.at(0);
    image40[ID] = 40;

    // Get result
    AlbumImage *const result = images.find(image40);

    // Verify
    QVERIFY(result == nullptr);
}

void tst_Collection_Models::find_Model_NotFound_DefaultModel() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Prepare
    AlbumImage image40 = images.at(0);
    image40[ID] = 40;

    // Get result
    AlbumImage *const result = images.find(image40, &images[3]);

    // Verify
    QVERIFY(result);
    QVERIFY(result->exists);
    QCOMPARE(result->getAttribute(ID), QVariant(5));
}

void tst_Collection_Models::find_Ids() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.find({6, 3, 5, 6});

    // Verify
    QCOMPARE(result.size(), 3);

    ModelsCollection<AlbumImage *> expected {
        &images[1],
        &images[3],
        &images[4],
    };
    QCOMPARE(result, expected);
}

void tst_Collection_Models::toQuery() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.toQuery()->get();

    // Verify
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));
    QCOMPARE(result.constFirst().getAttributes().size(), 7);
}

/* EnumeratesValues */

void tst_Collection_Models::reject() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.reject([](AlbumImage *const image)
    {
        const auto id = getKeyCasted(image);
        return id == 3 || id == 5;
    });

    // Verify
    QCOMPARE(result.size(), 3);
    QVERIFY(verifyIds(result, {2, 4, 6}));
}

void tst_Collection_Models::reject_WithIndex() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.reject([](AlbumImage *const image, const auto index)
    {
        return getKeyCasted(image) == 3 || index == 2;
    });

    // Verify
    QCOMPARE(result.size(), 3);
    QVERIFY(verifyIds(result, {2, 5, 6}));
}

void tst_Collection_Models::where_QString_EQ() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.whereEq<QString>(NAME, "album2_image3");

    // Verify
    QCOMPARE(result.size(), 1);
    QVERIFY(verifyIds(result, {4}));
}

void tst_Collection_Models::where_quint64_NE() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.where(ID, NE, 4);

    // Verify
    QCOMPARE(result.size(), 4);
    QVERIFY(verifyIds(result, {2, 3, 5, 6}));
}

void tst_Collection_Models::where_quint64_GT() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.where(ID, GT, 3);

    // Verify
    QCOMPARE(result.size(), 3);
    QVERIFY(verifyIds(result, {4, 5, 6}));
}

void tst_Collection_Models::where_InvalidComparisonOperator_ThrowException() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Verify
    QVERIFY_EXCEPTION_THROWN(images.where(ID, "dummy-NON_EXISTENT", 10),
                             InvalidArgumentError);
}

void tst_Collection_Models::whereNull_QString() const
{
    auto albums = Album::all();
    QCOMPARE(albums.size(), 4);
    QCOMPARE(typeid (ModelsCollection<Album>), typeid (albums));

    // Get result
    const auto result = albums.whereNull("note");

    // Verify
    QCOMPARE(result.size(), 2);
    QVERIFY(verifyIds(result, {1, 2}));
}

void tst_Collection_Models::whereNotNull_QString() const
{
    auto albums = Album::all();
    QCOMPARE(albums.size(), 4);
    QCOMPARE(typeid (ModelsCollection<Album>), typeid (albums));

    // Get result
    const auto result = albums.whereNotNull("note");

    // Verify
    QCOMPARE(result.size(), 2);
    QVERIFY(verifyIds(result, {3, 4}));
}

void tst_Collection_Models::whereNull_quint64() const
{
    auto images = AlbumImage::where(ID, GE, 6)->get();
    QCOMPARE(images.size(), 4);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {6, 7, 8, 9}));

    // Get result
    const auto result = images.whereNull("album_id");

    // Verify
    QCOMPARE(result.size(), 2);
    QVERIFY(verifyIds(result, {8, 9}));
}

void tst_Collection_Models::whereNotNull_quint64() const
{
    auto images = AlbumImage::where(ID, GE, 6)->get();
    QCOMPARE(images.size(), 4);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {6, 7, 8, 9}));

    // Get result
    const auto result = images.whereNotNull("album_id");

    // Verify
    QCOMPARE(result.size(), 2);
    QVERIFY(verifyIds(result, {6, 7}));
}

void tst_Collection_Models::whereIn_QString() const
{
    auto images = AlbumImage::all();
    QCOMPARE(images.size(), 9);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {1, 2, 3, 4, 5, 6, 7, 8, 9}));

    // Get result
    const auto result = images.whereIn<QString>("ext", {"png", "gif"});

    // Verify
    QCOMPARE(result.size(), 4);
    QVERIFY(verifyIds(result, {1, 2, 5, 6}));
}

void tst_Collection_Models::whereIn_quint64() const
{
    auto images = AlbumImage::all();
    QCOMPARE(images.size(), 9);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {1, 2, 3, 4, 5, 6, 7, 8, 9}));

    // Get result
    const auto result = images.whereIn<quint64>("album_id", {2, 3});

    // Verify
    QCOMPARE(result.size(), 6);
    QVERIFY(verifyIds(result, {2, 3, 4, 5, 6, 7}));
}

void tst_Collection_Models::whereIn_Empty() const
{
    auto albums = Album::all();
    QCOMPARE(albums.size(), 4);
    QCOMPARE(typeid (ModelsCollection<Album>), typeid (albums));
    QVERIFY(verifyIds(albums, {1, 2, 3, 4}));

    // Get result
    const auto result = albums.whereIn<quint64>(ID, {});

    // Verify
    QVERIFY(result.isEmpty());
}

void tst_Collection_Models::whereNotIn_QString() const
{
    auto images = AlbumImage::all();
    QCOMPARE(images.size(), 9);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {1, 2, 3, 4, 5, 6, 7, 8, 9}));

    // Get result
    const auto result = images.whereNotIn<QString>("ext", {"png", "gif"});

    // Verify
    QCOMPARE(result.size(), 5);
    QVERIFY(verifyIds(result, {3, 4, 7, 8, 9}));
}

void tst_Collection_Models::whereNotIn_quint64() const
{
    auto images = AlbumImage::all();
    QCOMPARE(images.size(), 9);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {1, 2, 3, 4, 5, 6, 7, 8, 9}));

    // Get result
    const auto result = images.whereNotIn<quint64>("album_id", {2, 3});

    // Verify
    QCOMPARE(result.size(), 3);
    QVERIFY(verifyIds(result, {1, 8, 9}));
}

void tst_Collection_Models::whereNotIn_Empty() const
{
    auto albums = Album::all();
    QCOMPARE(albums.size(), 4);
    QCOMPARE(typeid (ModelsCollection<Album>), typeid (albums));
    QVERIFY(verifyIds(albums, {1, 2, 3, 4}));

    // Get result
    const auto result = albums.whereNotIn<quint64>(ID, {});

    // Verify
    QCOMPARE(result.size(), 4);
    QVERIFY(verifyIds(result, {1, 2, 3, 4}));
}

void tst_Collection_Models::whereBetween() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.whereBetween<quint64>(ID, {3, 5});

    // Verify
    QCOMPARE(result.size(), 3);
    QVERIFY(verifyIds(result, {3, 4, 5}));
}

void tst_Collection_Models::whereNotBetween() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.whereNotBetween<quint64>(ID, {3, 5});

    // Verify
    QCOMPARE(result.size(), 2);
    QVERIFY(verifyIds(result, {2, 6}));
}

void tst_Collection_Models::firstWhere_QString() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.firstWhereEq<QString>(NAME, "album2_image3");

    // Verify
    QVERIFY(result != nullptr);
    QCOMPARE(getKeyCasted(result), 4);
}

void tst_Collection_Models::firstWhere_quint64_NE() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.firstWhere<quint64>(ID, NE, 2);

    // Verify
    QVERIFY(result != nullptr);
    QCOMPARE(getKeyCasted(result), 3);
}

void tst_Collection_Models::firstWhere_quint64_GE() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.firstWhere<quint64>(ID, GE, 4);

    // Verify
    QVERIFY(result != nullptr);
    QCOMPARE(getKeyCasted(result), 4);
}

void tst_Collection_Models::firstWhere_NotFound_nullptr() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    AlbumImage *const result = images.firstWhere<quint64>(ID, GE, 40);

    // Verify
    QVERIFY(result == nullptr);
}

void tst_Collection_Models::value_QVariant() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.value(NAME);

    // Verify
    QVERIFY(!result.isNull() && result.isValid());
    QCOMPARE(result, QVariant("album2_image1"));
}

void tst_Collection_Models::value_QVariant_NonExistentAttribute() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.value("dummy-NON_EXISTENT");

    // Verify
    QVERIFY(!result.isValid());
    QCOMPARE(result, QVariant());
}

void tst_Collection_Models::value_QVariant_NonExistentAttribute_DefaultValue() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.value("dummy-NON_EXISTENT", "default-value");

    // Verify
    QVERIFY(!result.isNull() && result.isValid());
    QCOMPARE(result, QVariant("default-value"));
}

void tst_Collection_Models::value_QVariant_EmptyCollection() const
{
    // Get result
    const auto result = ModelsCollection<AlbumImage>().value("dummy-NON_EXISTENT");

    // Verify
    QVERIFY(!result.isValid());
    QCOMPARE(result, QVariant());
}

void tst_Collection_Models::value_quint64() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.value<quint64>(SIZE_);

    // Verify
    QVERIFY((std::is_same_v<std::remove_const_t<decltype (result)>, quint64>));
    QCOMPARE(result, 424);
}

void tst_Collection_Models::value_quint64_NonExistentAttribute_DefaultValue() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.value<quint64>("dummy-NON_EXISTENT", 0);

    // Verify
    QVERIFY((std::is_same_v<std::remove_const_t<decltype (result)>, quint64>));
    QCOMPARE(result, 0);
}

void tst_Collection_Models::value_QString() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.value<QString>(NAME);

    // Verify
    QVERIFY((std::is_same_v<std::remove_const_t<decltype (result)>, QString>));
    QCOMPARE(result, QString("album2_image1"));
}

void tst_Collection_Models::value_QString_NonExistentAttribute_DefaultValue() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    const auto result = images.value<QString>("dummy-NON_EXISTENT", "default-value");

    // Verify
    QVERIFY((std::is_same_v<std::remove_const_t<decltype (result)>, QString>));
    QCOMPARE(result, QVariant("default-value"));
}

void tst_Collection_Models::each_lvalue() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    std::vector<quint64> expectedIds;
    expectedIds.reserve(images.size());

    ModelsCollection<AlbumImage> &result = images.each([&expectedIds]
                                                       (AlbumImage *const image)
    {
        expectedIds.emplace_back(getKeyCasted(image));
    });

    // Verify
    // Both must be lvalue references because of that the decltype ((images)) is used
    QVERIFY((std::is_same_v<decltype (result), decltype ((images))>));
    // It must be the same ModelsCollection (the same memory address)
    QVERIFY(reinterpret_cast<uintptr_t>(&result) ==
            reinterpret_cast<uintptr_t>(&images));
    QCOMPARE(expectedIds.size(), 5);
    QCOMPARE(std::vector<quint64>({2, 3, 4, 5, 6}), expectedIds);
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

void tst_Collection_Models::each_lvalue_index() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    std::vector<ExpectedItem> expectedIds;
    expectedIds.reserve(images.size());

    ModelsCollection<AlbumImage> &result =
            images.each([&expectedIds]
                        (AlbumImage *const image, const auto index)
    {
        expectedIds.emplace_back(getKeyCasted(image), index);
    });

    // Verify
    // Both must be lvalue references because of that the decltype ((images)) is used
    QVERIFY((std::is_same_v<decltype (result), decltype ((images))>));
    // It must be the same ModelsCollection (the same memory address)
    QVERIFY(reinterpret_cast<uintptr_t>(&result) ==
            reinterpret_cast<uintptr_t>(&images));
    QCOMPARE(expectedIds.size(), 5);
    QCOMPARE(std::vector<ExpectedItem>({{2, 0}, {3, 1}, {4, 2}, {5, 3}, {6, 4}}),
             expectedIds);
}

void tst_Collection_Models::each_lvalue_bool() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    std::vector<quint64> expectedIds;
    expectedIds.reserve(images.size());

    ModelsCollection<AlbumImage> &result =
            images.each(Orm::EachBoolCallback,
                        [&expectedIds]
                        (AlbumImage *const image) -> bool
    {
        // Exit/break the each() looping
        if (getKeyCasted(image) == 5)
            return false;

        expectedIds.emplace_back(getKeyCasted(image));

        return true;
    });

    // Verify
    // Both must be lvalue references because of that the decltype ((images)) is used
    QVERIFY((std::is_same_v<decltype (result), decltype ((images))>));
    // It must be the same ModelsCollection (the same memory address)
    QVERIFY(reinterpret_cast<uintptr_t>(&result) ==
            reinterpret_cast<uintptr_t>(&images));
    QCOMPARE(expectedIds.size(), 3);
    QCOMPARE(std::vector<quint64>({2, 3, 4}), expectedIds);
}

void tst_Collection_Models::each_lvalue_bool_index() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    std::vector<ExpectedItem> expectedIds;
    expectedIds.reserve(images.size());

    ModelsCollection<AlbumImage> &result =
            images.each(Orm::EachBoolCallback, [&expectedIds]
                        (AlbumImage *const image, const auto index) -> bool
    {
        // Exit/break the each() looping
        if (index == 3)
            return false;

        expectedIds.emplace_back(getKeyCasted(image), index);

        return true;
    });

    // Verify
    // Both must be lvalue references because of that the decltype ((images)) is used
    QVERIFY((std::is_same_v<decltype (result), decltype ((images))>));
    // It must be the same ModelsCollection (the same memory address)
    QVERIFY(reinterpret_cast<uintptr_t>(&result) ==
            reinterpret_cast<uintptr_t>(&images));
    QCOMPARE(expectedIds.size(), 3);
    QCOMPARE(std::vector<ExpectedItem>({{2, 0}, {3, 1}, {4, 2}}),
             expectedIds);
}

void tst_Collection_Models::each_rvalue() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    std::vector<quint64> expectedIds;
    expectedIds.reserve(images.size());

    ModelsCollection<AlbumImage> result =
            std::move(images).each([&expectedIds]
                                   (AlbumImage *const image)
    {
        expectedIds.emplace_back(getKeyCasted(image));
    });

    // Verify
    QVERIFY((std::is_same_v<decltype (result), decltype (images)>));
    QCOMPARE(expectedIds.size(), 5);
    QCOMPARE(std::vector<quint64>({2, 3, 4, 5, 6}), expectedIds);
}

void tst_Collection_Models::each_rvalue_index() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    std::vector<ExpectedItem> expectedIds;
    expectedIds.reserve(images.size());

    ModelsCollection<AlbumImage> result =
            std::move(images).each([&expectedIds]
                                   (AlbumImage *const image, const auto index)
    {
        expectedIds.emplace_back(getKeyCasted(image), index);
    });

    // Verify
    QVERIFY((std::is_same_v<decltype (result), decltype (images)>));
    QCOMPARE(expectedIds.size(), 5);
    QCOMPARE(std::vector<ExpectedItem>({{2, 0}, {3, 1}, {4, 2}, {5, 3}, {6, 4}}),
             expectedIds);
}

void tst_Collection_Models::each_rvalue_bool() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    std::vector<quint64> expectedIds;
    expectedIds.reserve(images.size());

    ModelsCollection<AlbumImage> result =
            std::move(images).each(Orm::EachBoolCallback,
                                   [&expectedIds]
                                   (AlbumImage *const image) -> bool
    {
        // Exit/break the each() looping
        if (getKeyCasted(image) == 5)
            return false;

        expectedIds.emplace_back(getKeyCasted(image));

        return true;
    });

    // Verify
    QVERIFY((std::is_same_v<decltype (result), decltype (images)>));
    QCOMPARE(expectedIds.size(), 3);
    QCOMPARE(std::vector<quint64>({2, 3, 4}), expectedIds);
}

void tst_Collection_Models::each_rvalue_bool_index() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    std::vector<ExpectedItem> expectedIds;
    expectedIds.reserve(images.size());

    ModelsCollection<AlbumImage> result =
            std::move(images).each(Orm::EachBoolCallback, [&expectedIds]
                                   (AlbumImage *const image, const auto index) -> bool
    {
        // Exit/break the each() looping
        if (index == 3)
            return false;

        expectedIds.emplace_back(getKeyCasted(image), index);

        return true;
    });

    // Verify
    QVERIFY((std::is_same_v<decltype (result), decltype (images)>));
    QCOMPARE(expectedIds.size(), 3);
    QCOMPARE(std::vector<ExpectedItem>({{2, 0}, {3, 1}, {4, 2}}),
             expectedIds);
}

void tst_Collection_Models::tap_lvalue() const
{
    auto images = AlbumImage::whereEq(*album_id, 2)->get();
    QCOMPARE(images.size(), 5);
    QCOMPARE(typeid (ModelsCollection<AlbumImage>), typeid (images));
    QVERIFY(verifyIds(images, {2, 3, 4, 5, 6}));

    // Get result
    auto callbackInvoked = false;
    uintptr_t imagesInCallbackAddress = 0;

    ModelsCollection<AlbumImage> &result =
            images.tap([&callbackInvoked, &imagesInCallbackAddress]
                       (ModelsCollection<AlbumImage> &images)
    {
        callbackInvoked = true;
        imagesInCallbackAddress = reinterpret_cast<uintptr_t>(&images);

        // Change the name, append " NEW"
        auto &image4 = images[2];
        image4[NAME] = SPACE_IN.arg(image4.getAttribute(NAME).template value<QString>(),
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
    QVERIFY(verifyIds(result, {2, 3, 4, 5, 6}));
    // Verify the changed name
    QCOMPARE(result.at(2).getAttribute(NAME), QVariant("album2_image3 NEW"));
}

void tst_Collection_Models::tap_rvalue() const
{
    // Prepare
    ModelsCollection<AlbumImage> imagesInit {
        {{ID, 2}, {NAME, QStringLiteral("image2")}},
    };

    // Get result
    auto callbackInvoked = false;

    ModelsCollection<AlbumImage> result =
            std::move(imagesInit).tap([&callbackInvoked]
                                      (ModelsCollection<AlbumImage> &images)
    {
        callbackInvoked = true;

        // Change the name, append " NEW"
        auto &image2 = images.first();
        image2[NAME] = SPACE_IN.arg(image2.getAttribute(NAME).template value<QString>(),
                                    QStringLiteral("NEW"));
    });

    // Verify
    QVERIFY((std::is_same_v<decltype (result), decltype (imagesInit)>));
    QVERIFY(callbackInvoked);
    // Collection content
    QCOMPARE(result.size(), 1);
    QVERIFY(verifyIds(result, {2}));
    // Verify the changed name
    QCOMPARE(result.first().getAttribute(NAME), QVariant("image2 NEW"));
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

template<typename M>
bool tst_Collection_Models::verifyIds(const ModelsCollection<M *> &actual,
                                      const std::unordered_set<quint64> &expected)
{
    return std::ranges::all_of(actual, [&expected](M *const model)
    {
        return expected.contains(getKeyCasted(model));
    });
}

template<typename M>
bool tst_Collection_Models::verifyIds(const ModelsCollection<M> &actual,
                                      const std::unordered_set<quint64> &expected)
{
    return std::ranges::all_of(actual, [&expected](const M &model)
    {
        return expected.contains(getKeyCasted(model));
    });
}

bool tst_Collection_Models::verifyIds(const QVector<quint64> &actual,
                                      const std::unordered_set<quint64> &expected)
{
    return std::ranges::all_of(actual, [&expected](const quint64 id)
    {
        return expected.contains(id);
    });
}

template<typename T>
bool tst_Collection_Models::verifyIdsQVariant(const QVector<QVariant> &actual,
                                              const std::unordered_set<T> &expected)
{
    return std::ranges::all_of(actual, [&expected](const QVariant &id)
    {
        return expected.contains(id.template value<T>());
    });
}

template<typename T, typename M>
bool tst_Collection_Models::verifyAttributeValues(
        const QString &attribute, const ModelsCollection<M *> &actual,
        const std::unordered_set<T> &expected)
{
    return std::ranges::all_of(actual,
                               [&attribute, &expected](M *const model)
    {
        return expected.contains(model->getAttribute(attribute).template value<T>());
    });
}

template<typename M>
typename M::KeyType
tst_Collection_Models::getKeyCasted(M *const model)
{
    return model->getKey().template value<typename M::KeyType>();
}

template<typename M>
typename M::KeyType
tst_Collection_Models::getKeyCasted(const M &model)
{
    return model.getKey().template value<typename M::KeyType>();
}

QTEST_MAIN(tst_Collection_Models)

#include "tst_collection_models.moc"
