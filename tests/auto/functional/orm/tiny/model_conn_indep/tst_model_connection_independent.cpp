#include <QCoreApplication>
#include <QtSql/QSqlDriver>
#include <QtTest>

#include "orm/db.hpp"

#include "databases.hpp"

#include "models/album.hpp"
#include "models/datetime.hpp"
#include "models/filepropertyproperty.hpp"
#include "models/massassignmentmodels.hpp"
#include "models/torrent.hpp"
#include "models/torrenteager.hpp"
#include "models/torrenteager_without_qdatetime.hpp"
#include "models/type.hpp"

using Orm::Constants::ASTERISK;
using Orm::Constants::CREATED_AT;
using Orm::Constants::DELETED_AT;
using Orm::Constants::HASH_;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::SIZE_;
using Orm::Constants::UPDATED_AT;
using Orm::Constants::pivot_;

using Orm::DB;
using Orm::Exceptions::InvalidArgumentError;
using Orm::Exceptions::MultipleRecordsFoundError;
using Orm::Exceptions::RecordsNotFoundError;
using Orm::One;
using Orm::Utils::Helpers;

using NullVariant = Orm::Utils::NullVariant;
using TypeUtils = Orm::Utils::Type;

using Orm::Tiny::AttributeItem;
using Orm::Tiny::CastType;
using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::Exceptions::MassAssignmentError;
using Orm::Tiny::Types::ModelsCollection;

using AttributeUtils = Orm::Tiny::Utils::Attribute;

using TestUtils::Databases;

using Models::Album;
using Models::AlbumImage;
using Models::Datetime;
using Models::FilePropertyProperty;
using Models::Torrent;
using Models::TorrentPeer;
using Models::TorrentPreviewableFile;
using Models::Torrent_AllowedMassAssignment;
using Models::Torrent_GuardedAttribute;
using Models::Torrent_TotallyGuarded;
using Models::TorrentEager;
using Models::TorrentEager_Without_QDateTime;
using Models::TorrentPreviewableFileProperty;
using Models::Type;
using Models::User;

class tst_Model_Connection_Independent : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase() const;

    /* Model instantiation */
    void defaultAttributeValues_WithQDateTime_DefaultCtor() const;
    void defaultAttributeValues_WithQDateTime_ConvertingAttributesCtor() const;
    void defaultAttributeValues_WithQDateTime_ListInitializationCtor() const;

    void defaultAttributeValues_WithQDateTime_InstanceMethod() const;
    void defaultAttributeValues_WithQDateTime_InstanceAttributesMethod() const;
    void defaultAttributeValues_WithQDateTime_InstanceMethod_WithConnection() const;
    void defaultAttributeValues_WithQDateTime_InstanceAttributesMethod_WithConnection() const;

    void defaultAttributeValues_WithoutQDateTime_DefaultCtor() const;
    void defaultAttributeValues_WithoutQDateTime_ConvertingAttributesCtor() const;
    void defaultAttributeValues_WithoutQDateTime_ListInitializationCtor() const;

    void defaultAttributeValues_WithQDateTime_InstanceHeapMethod() const;
    void defaultAttributeValues_WithQDateTime_InstanceHeapAttributesMethod() const;
    void defaultAttributeValues_WithQDateTime_InstanceHeapMethod_WithConnection() const;
    void defaultAttributeValues_WithQDateTime_InstanceHeapAttributesMethod_WithConnection() const;

    void replicate() const;
    void replicate_WithCreate() const;
    void replicate_WithRelations() const;

    /* Model operator[] */
    void subscriptOperator() const;
    void subscriptOperator_OnLhs() const;
    void subscriptOperator_OnLhs_AssignAttributeReference() const;

    void isCleanAndIsDirty() const;

    /* Models comparison */
    void is() const;
    void isNot() const;

    void equalComparison() const;
    void notEqualComparison() const;

    /* Mass assignment */
    void massAssignment_Fillable() const;
    void massAssignment_Guarded() const;
    void massAssignment_GuardedAll_NonExistentAttribute() const;
    void massAssignment_GuardedDisabled_ExistentAttribute() const;
    void massAssignment_GuardedDisabled_NonExistentAttribute() const;
    void massAssignment_TotallyGuarded_Exception() const;
    void massAssignment_CantMassFillAttributesWithTableNamesWhenUsingGuarded() const;

    void massAssignment_forceFill_OnTotallyGuardedModel() const;
    void massAssignment_forceFill_OnGuardedAttribute() const;
    void massAssignment_forceFill_NonExistentAttribute() const;

    /* Eager loading */
    void with_WithSelectConstraint_WithoutQualifiedColumnsForRelatedTable() const;
    void with_BelongsToMany_WithSelectConstraint_QualifiedColumnsForRelatedTable() const;

    /* Retrieving results */
    void pluck() const;
    void pluck_EmptyResult() const;
    void pluck_QualifiedColumnOrKey() const;

    void pluck_With_u_dates() const;
    void pluck_EmptyResult_With_u_dates() const;
    void pluck_QualifiedColumnOrKey_With_u_dates() const;

    void whereRowValues() const;

    void only() const;
    void only_NonExistentAttribute() const;

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

    void tap() const;

    void sole() const;
    void sole_RecordsNotFoundError() const;
    void sole_MultipleRecordsFoundError() const;
    void sole_Pretending() const;

    void soleValue() const;
    void soleValue_RecordsNotFoundError() const;
    void soleValue_MultipleRecordsFoundError() const;
    void soleValue_Pretending() const;

    /* Serialization */
    void toMap() const;
    void toVector() const;

    void toMap_WithCasts() const;
    void toVector_WithCasts() const;

    void toMap_UDatesOnly_QDateTime_For_date_column() const;
    void toVector_UDatesOnly_QDateTime_For_date_column() const;

    void toMap_UDatesOnly_QDate_For_date_column() const;
    void toVector_UDatesOnly_QDate_For_date_column() const;

    void toMap_UDatesOnly_Timestamp() const;
    void toVector_UDatesOnly_Timestamp() const;

    void toMap_WithDateModfiers() const;
    void toVector_WithDateModfiers() const;

    void toMap_UDatesOnly_DateNullVariants() const;
    void toVector_UDatesOnly_DateNullVariants() const;

    void toMap_WithCasts_DateNullVariants() const;
    void toVector_WithCasts_DateNullVariants() const;

    void toMap_WithRelations_HasOne_HasMany_BelongsTo() const;
    void toVector_WithRelations_HasOne_HasMany_BelongsTo() const;

    void toMap_WithRelation_BelongsToMany_TorrentTags() const;
    void toVector_WithRelation_BelongsToMany_TorrentTags() const;

    void toMap_WithRelation_BelongsToMany_TorrentTags_TorrentStates() const;
    void toVector_WithRelation_BelongsToMany_TorrentTags_TorrentStates() const;

    void toMap_WithRelation_BelongsToMany_UserRoles() const;
    void toVector_WithRelation_BelongsToMany_UserRoles() const;

    void toMap_u_snakeAttributes_false() const;
    void toVector_u_snakeAttributes_false() const;

    void toMap_HasMany_EmptyRelation() const;
    void toVector_HasMany_EmptyRelation() const;

    void toMap_HasOne_EmptyRelation() const;
    void toVector_HasOne_EmptyRelation() const;

    void toMap_BelongsTo_EmptyRelation() const;
    void toVector_BelongsTo_EmptyRelation() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Connection name used in this test case. */
    QString m_connection {};
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Model_Connection_Independent::initTestCase()
{
    ConnectionOverride::connection = m_connection =
            Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );
}

void tst_Model_Connection_Independent::cleanupTestCase() const
{
    // Reset connection override
    ConnectionOverride::connection.clear();
}

/* Model instantiation */

void tst_Model_Connection_Independent::
defaultAttributeValues_WithQDateTime_DefaultCtor() const
{
    // Default ctor must throw because of the QDateTime
    QVERIFY_EXCEPTION_THROWN(TorrentEager(), InvalidArgumentError);
}

void tst_Model_Connection_Independent::
defaultAttributeValues_WithQDateTime_ConvertingAttributesCtor() const
{
    // Attributes converting ctor must throw because of the QDateTime
    QVERIFY_EXCEPTION_THROWN(TorrentEager({
                                 {NAME, "test22"},
                                 {NOTE, "Torrent::instance()"},
                             }),
                             InvalidArgumentError);
}

void tst_Model_Connection_Independent::
defaultAttributeValues_WithQDateTime_ListInitializationCtor() const
{
    /* List initialization using the std::initializer_list<AttributeItem> must throw
       because of the QDateTime. */
    QVERIFY_EXCEPTION_THROWN((TorrentEager {
                                 {NAME, "test22"},
                                 {NOTE, "Torrent::instance()"},
                             }),
                             InvalidArgumentError);
}

void tst_Model_Connection_Independent::
defaultAttributeValues_WithQDateTime_InstanceMethod() const
{
    // The Model::instance() method must work well
    auto torrent = TorrentEager::instance();

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent[SIZE_], QVariant(0));
    QCOMPARE(torrent["progress"], QVariant(0));
    QCOMPARE(torrent["added_on"],
             QVariant(QDateTime({2021, 4, 1}, {15, 10, 10}, Qt::UTC)));
    QCOMPARE(torrent.getAttributes().size(), 3);

    // Check also the connection name
    QCOMPARE(torrent.getConnectionName(), m_connection);
}

void tst_Model_Connection_Independent::
defaultAttributeValues_WithQDateTime_InstanceAttributesMethod() const
{
    const auto name = QStringLiteral("test22");
    const auto note = QStringLiteral("Torrent::instance()");

    // The Model::instance(attributes) must work well
    auto torrent = TorrentEager::instance({
        {NAME, name},
        {NOTE, note},
    });

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent[SIZE_], QVariant(0));
    QCOMPARE(torrent["progress"], QVariant(0));
    QCOMPARE(torrent["added_on"],
             QVariant(QDateTime({2021, 4, 1}, {15, 10, 10}, Qt::UTC)));
    QCOMPARE(torrent[NAME], QVariant(name));
    QCOMPARE(torrent[NOTE], QVariant(note));
    QCOMPARE(torrent.getAttributes().size(), 5);

    // Check also the connection name
    QCOMPARE(torrent.getConnectionName(), m_connection);
}

void tst_Model_Connection_Independent::
defaultAttributeValues_WithQDateTime_InstanceMethod_WithConnection() const
{
    const auto connectionForInstance =
            QStringLiteral(
                "tinyorm_pgsql_tests-tst_Model_Connection_Independent-"
                "defaultAttributeValues_WithQDateTime");

    // The Model::instance() method must work well (with connection override)
    auto torrent = TorrentEager::instance(connectionForInstance);

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent[SIZE_], QVariant(0));
    QCOMPARE(torrent["progress"], QVariant(0));
    QCOMPARE(torrent["added_on"],
             QVariant(QDateTime({2021, 4, 1}, {15, 10, 10}, Qt::UTC)));
    QCOMPARE(torrent.getAttributes().size(), 3);

    // Check also the connection name
    /* Reset the global connection override to be able test the Model::instance()
       connection override. */
    ConnectionOverride::connection.clear();
    QCOMPARE(torrent.getConnectionName(), connectionForInstance);
    // Restore
    ConnectionOverride::connection = m_connection;
}

void tst_Model_Connection_Independent::
defaultAttributeValues_WithQDateTime_InstanceAttributesMethod_WithConnection() const
{
    const auto connectionForInstance =
            QStringLiteral(
                "tinyorm_pgsql_tests-tst_Model_Connection_Independent-"
                "defaultAttributeValues_WithQDateTime");

    const auto name = QStringLiteral("test22");
    const auto note = QStringLiteral("Torrent::instance()");

    // The Model::instance(attributes) must work well (with connection override)
    auto torrent = TorrentEager::instance({
        {NAME, name},
        {NOTE, note},
    },
        connectionForInstance);

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent[SIZE_], QVariant(0));
    QCOMPARE(torrent["progress"], QVariant(0));
    QCOMPARE(torrent["added_on"],
             QVariant(QDateTime({2021, 4, 1}, {15, 10, 10}, Qt::UTC)));
    QCOMPARE(torrent[NAME], QVariant(name));
    QCOMPARE(torrent[NOTE], QVariant(note));
    QCOMPARE(torrent.getAttributes().size(), 5);

    /* Reset the global connection override to be able test the Model::instance()
       connection override. */
    ConnectionOverride::connection.clear();
    QCOMPARE(torrent.getConnectionName(), connectionForInstance);
    // Restore
    ConnectionOverride::connection = m_connection;
}

void tst_Model_Connection_Independent::
defaultAttributeValues_WithoutQDateTime_DefaultCtor() const
{
    // The default ctor without QDateTime must work well
    TorrentEager_Without_QDateTime torrent;

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent[SIZE_], QVariant(0));
    QCOMPARE(torrent["progress"], QVariant(0));
    QCOMPARE(torrent.getAttributes().size(), 2);

    // Check also the connection name
    QCOMPARE(torrent.getConnectionName(), m_connection);
}

void tst_Model_Connection_Independent::
defaultAttributeValues_WithoutQDateTime_ConvertingAttributesCtor() const
{
    const auto name = QStringLiteral("test22");
    const auto note = QStringLiteral("Torrent::instance()");

    // The converting attributes ctor without QDateTime must work well
    TorrentEager_Without_QDateTime torrent({
        {NAME, name},
        {NOTE, note},
    });

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent[SIZE_], QVariant(0));
    QCOMPARE(torrent["progress"], QVariant(0));
    QCOMPARE(torrent[NAME], QVariant(name));
    QCOMPARE(torrent[NOTE], QVariant(note));
    QCOMPARE(torrent.getAttributes().size(), 4);

    // Check also the connection name
    QCOMPARE(torrent.getConnectionName(), m_connection);
}

void tst_Model_Connection_Independent::
defaultAttributeValues_WithoutQDateTime_ListInitializationCtor() const
{
    const auto name = QStringLiteral("test22");
    const auto note = QStringLiteral("Torrent::instance()");

    /* The list initialization ctor without QDateTime
       using the std::initializer_list<AttributeItem> must work well. */
    TorrentEager_Without_QDateTime torrent {
        {NAME, name},
        {NOTE, note},
    };

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent[SIZE_], QVariant(0));
    QCOMPARE(torrent["progress"], QVariant(0));
    QCOMPARE(torrent[NAME], QVariant(name));
    QCOMPARE(torrent[NOTE], QVariant(note));
    QCOMPARE(torrent.getAttributes().size(), 4);

    // Check also the connection name
    QCOMPARE(torrent.getConnectionName(), m_connection);
}

void tst_Model_Connection_Independent::
defaultAttributeValues_WithQDateTime_InstanceHeapMethod() const
{
    // The Model::instance() method must work well
    auto torrent = TorrentEager::instanceHeap();

    QVERIFY(!torrent->exists);
    QCOMPARE((*torrent)[SIZE_], QVariant(0));
    QCOMPARE((*torrent)["progress"], QVariant(0));
    QCOMPARE((*torrent)["added_on"],
             QVariant(QDateTime({2021, 4, 1}, {15, 10, 10}, Qt::UTC)));
    QCOMPARE((*torrent).getAttributes().size(), 3);

    // Check also the connection name
    QCOMPARE(torrent->getConnectionName(), m_connection);
}

void tst_Model_Connection_Independent::
defaultAttributeValues_WithQDateTime_InstanceHeapAttributesMethod() const
{
    const auto name = QStringLiteral("test22");
    const auto note = QStringLiteral("Torrent::instance()");

    // The Model::instance(attributes) must work well
    auto torrent = TorrentEager::instanceHeap({
        {NAME, name},
        {NOTE, note},
    });

    QVERIFY(!torrent->exists);
    QCOMPARE((*torrent)[SIZE_], QVariant(0));
    QCOMPARE((*torrent)["progress"], QVariant(0));
    QCOMPARE((*torrent)["added_on"],
             QVariant(QDateTime({2021, 4, 1}, {15, 10, 10}, Qt::UTC)));
    QCOMPARE((*torrent)[NAME], QVariant(name));
    QCOMPARE((*torrent)[NOTE], QVariant(note));
    QCOMPARE(torrent->getAttributes().size(), 5);

    // Check also the connection name
    QCOMPARE(torrent->getConnectionName(), m_connection);
}

void tst_Model_Connection_Independent::
defaultAttributeValues_WithQDateTime_InstanceHeapMethod_WithConnection() const
{
    const auto connectionForInstance =
            QStringLiteral(
                "tinyorm_pgsql_tests-tst_Model_Connection_Independent-"
                "defaultAttributeValues_WithQDateTime");

    // The Model::instance() method must work well (with connection override)
    auto torrent = TorrentEager::instanceHeap(connectionForInstance);

    QVERIFY(!torrent->exists);
    QCOMPARE((*torrent)[SIZE_], QVariant(0));
    QCOMPARE((*torrent)["progress"], QVariant(0));
    QCOMPARE((*torrent)["added_on"],
             QVariant(QDateTime({2021, 4, 1}, {15, 10, 10}, Qt::UTC)));
    QCOMPARE(torrent->getAttributes().size(), 3);

    // Check also the connection name
    /* Reset the global connection override to be able test the Model::instance()
       connection override. */
    ConnectionOverride::connection.clear();
    QCOMPARE(torrent->getConnectionName(), connectionForInstance);
    // Restore
    ConnectionOverride::connection = m_connection;
}

void tst_Model_Connection_Independent::
defaultAttributeValues_WithQDateTime_InstanceHeapAttributesMethod_WithConnection() const
{
    const auto connectionForInstance =
            QStringLiteral(
                "tinyorm_pgsql_tests-tst_Model_Connection_Independent-"
                "defaultAttributeValues_WithQDateTime");

    const auto name = QStringLiteral("test22");
    const auto note = QStringLiteral("Torrent::instance()");

    // The Model::instance(attributes) must work well (with connection override)
    auto torrent = TorrentEager::instanceHeap({
        {NAME, name},
        {NOTE, note},
    },
        connectionForInstance);

    QVERIFY(!torrent->exists);
    QCOMPARE((*torrent)[SIZE_], QVariant(0));
    QCOMPARE((*torrent)["progress"], QVariant(0));
    QCOMPARE((*torrent)["added_on"],
             QVariant(QDateTime({2021, 4, 1}, {15, 10, 10}, Qt::UTC)));
    QCOMPARE((*torrent)[NAME], QVariant(name));
    QCOMPARE((*torrent)[NOTE], QVariant(note));
    QCOMPARE(torrent->getAttributes().size(), 5);

    /* Reset the global connection override to be able test the Model::instance()
       connection override. */
    ConnectionOverride::connection.clear();
    QCOMPARE(torrent->getConnectionName(), connectionForInstance);
    // Restore
    ConnectionOverride::connection = m_connection;
}

void tst_Model_Connection_Independent::replicate() const
{
    auto user1 = User::find(1);
    QVERIFY(user1);
    QVERIFY(user1->exists);

    auto user1Replicated = user1->replicate();

    auto user1Attributes = AttributeUtils::exceptAttributesForReplicate(*user1);
    auto user1ReplicatedAttributes = AttributeUtils::exceptAttributesForReplicate(
                                         user1Replicated);

    QVERIFY(!user1Replicated.exists);
    QVERIFY(user1Attributes == user1ReplicatedAttributes);
    QVERIFY(user1->getRelations().empty());
    QVERIFY(user1Replicated.getRelations().empty());
}

void tst_Model_Connection_Independent::replicate_WithCreate() const
{
    // Following is the most used case for the replicate method so I will test it
    auto user = User::create({{NAME, "xyz"},
                              {"is_banned", true},
                              {NOTE, "test replicate"}});
    QVERIFY(user.exists);

    std::unordered_set except {NAME};

    auto userReplicated = user.replicate(except);

    auto userAttributes = AttributeUtils::exceptAttributesForReplicate(user, except);
    auto userReplicatedAttributes = AttributeUtils::exceptAttributesForReplicate(
                                         userReplicated);

    QVERIFY(!userReplicated.exists);
    QVERIFY(userAttributes == userReplicatedAttributes);

    QCOMPARE(user.getAttribute(NAME), QVariant("xyz"));
    QVERIFY(!userReplicated.getAttribute(NAME).isValid());

    QVERIFY(user.getRelations().empty());
    QVERIFY(userReplicated.getRelations().empty());

    // Restore db
    QVERIFY(user.forceDelete());
}

void tst_Model_Connection_Independent::replicate_WithRelations() const
{
    auto torrent2 = Torrent::with("torrentFiles.fileProperty")->find(2);
    QVERIFY(torrent2);
    QVERIFY(torrent2->exists);

    auto torrent2Replicated = torrent2->replicate();

    auto torrent2Attributes = AttributeUtils::exceptAttributesForReplicate(*torrent2);
    auto torrent2ReplicatedAttributes = AttributeUtils::exceptAttributesForReplicate(
                                            torrent2Replicated);

    const auto &torrent2Relations = torrent2->getRelations();
    const auto &torrent2ReplicatedRelations = torrent2Replicated.getRelations();

    /* Crazy, but I'm going to check all relations 😮😎, yeah it's definitely crazy and
       overmotivated, but it checks almost everything I wanted. 🙃🤙 */
    // Torrent
    QVERIFY(!torrent2Replicated.exists);
    QVERIFY(torrent2Attributes == torrent2ReplicatedAttributes);
    QVERIFY(torrent2Relations.size() == torrent2ReplicatedRelations.size());
    QVERIFY(torrent2->relationLoaded("torrentFiles"));
    QVERIFY(torrent2Replicated.relationLoaded("torrentFiles"));
    // The Model::operator== was needed to make this real
    QVERIFY(torrent2Relations == torrent2ReplicatedRelations);

    // TorrentPreviewableFile
    const auto torrentFiles2 =
            torrent2->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    const auto torrentFiles2Replicated =
            torrent2Replicated.getRelationValue<TorrentPreviewableFile>("torrentFiles");

    QVERIFY(torrentFiles2.size() == torrentFiles2Replicated.size());

    for (std::remove_cvref_t<decltype (torrentFiles2)>::size_type i = 0;
         torrentFiles2.size() < i; ++i
    ) {
        auto *torrentFile = torrentFiles2.value(i);
        QVERIFY(torrentFile->exists);
        QVERIFY(torrentFile->relationLoaded("fileProperty"));

        auto *torrentFileReplicated = torrentFiles2Replicated.value(i);
        QVERIFY(torrentFileReplicated->exists);
        QVERIFY(torrentFileReplicated->relationLoaded("fileProperty"));

        QVERIFY(torrentFile->getAttributes() == torrentFileReplicated->getAttributes());
        // The Model::operator== was needed to make this real
        QVERIFY(torrentFile->getRelations() == torrentFileReplicated->getRelations());

        // TorrentPreviewableFileProperty
        auto *fileProperty =
                torrentFile->getRelationValue<TorrentPreviewableFileProperty, One>(
                    "fileProperty");
        auto *filePropertyReplicated =
                torrentFileReplicated->getRelationValue<TorrentPreviewableFileProperty,
                                                        One>("fileProperty");

        QVERIFY(fileProperty->exists);
        QVERIFY(fileProperty->getRelations().empty());
        QVERIFY(filePropertyReplicated->exists);
        QVERIFY(filePropertyReplicated->getRelations().empty());
        QVERIFY(fileProperty->getAttributes() ==
                filePropertyReplicated->getAttributes());
    }
}

/* Model operator[] */

void tst_Model_Connection_Independent::subscriptOperator() const
{
    auto torrent = Torrent::find(2);
    QVERIFY(torrent->exists);

    QCOMPARE((*torrent)[ID], QVariant(2));
    QCOMPARE((*torrent)[NAME], QVariant("test2"));

    QCOMPARE((*torrent)["added_on"],
            QVariant(QDateTime({2020, 8, 2}, {20, 11, 10}, Qt::UTC)));
}

void tst_Model_Connection_Independent::subscriptOperator_OnLhs() const
{
    auto torrent = Torrent::find(2);
    QVERIFY(torrent->exists);

    QCOMPARE(torrent->getKey(), QVariant(2));
    QCOMPARE(torrent->getAttribute(NAME), QVariant("test2"));
    QCOMPARE(torrent->getAttribute(SIZE_), QVariant(12));

    const auto name = QStringLiteral("test2 operator[]");
    const auto size = 112;
    (*torrent)[NAME] = name;
    (*torrent)[SIZE_] = size;

    QCOMPARE(torrent->getAttribute(NAME), QVariant(name));
    QCOMPARE(torrent->getAttribute(SIZE_), QVariant(size));
}

void tst_Model_Connection_Independent::
     subscriptOperator_OnLhs_AssignAttributeReference() const
{
    auto torrent2 = Torrent::find(2);
    QVERIFY(torrent2->exists);

    QCOMPARE(torrent2->getKey(), QVariant(2));
    QCOMPARE(torrent2->getAttribute(NAME), QVariant("test2"));

    auto attributeReference = (*torrent2)[NAME];

    // Fetch fresh torrent to assign an attribute reference to its 'name' attribute
    auto torrent3 = Torrent::find(3);
    QVERIFY(torrent3->exists);

    QCOMPARE(torrent3->getKey(), QVariant(3));
    QCOMPARE(torrent3->getAttribute(NAME), QVariant("test3"));

    (*torrent3)[NAME] = attributeReference;

    QCOMPARE(torrent3->getAttribute(NAME), torrent2->getAttribute(NAME));

    // Some more testing
    const auto name = QStringLiteral("test2 operator[]");

    attributeReference = name;
    (*torrent3)[NAME] = attributeReference;

    const auto torrent2Name = torrent2->getAttribute(NAME);
    QCOMPARE(torrent2Name, QVariant(name));
    QCOMPARE(torrent3->getAttribute(NAME), torrent2Name);
}

void tst_Model_Connection_Independent::isCleanAndIsDirty() const
{
    auto torrent = Torrent::find(3);

    QVERIFY(torrent->isClean());
    QVERIFY(!torrent->isDirty());
    QVERIFY(torrent->isClean(NAME));
    QVERIFY(!torrent->isDirty(NAME));

    torrent->setAttribute(NAME, "test3 dirty");

    QVERIFY(!torrent->isClean());
    QVERIFY(torrent->isDirty());
    QVERIFY(!torrent->isClean(NAME));
    QVERIFY(torrent->isDirty(NAME));
    QVERIFY(torrent->isClean(SIZE_));
    QVERIFY(!torrent->isDirty(SIZE_));

    torrent->save();

    QVERIFY(torrent->isClean());
    QVERIFY(!torrent->isDirty());
    QVERIFY(torrent->isClean(NAME));
    QVERIFY(!torrent->isDirty(NAME));
    QVERIFY(torrent->isClean(SIZE_));
    QVERIFY(!torrent->isDirty(SIZE_));

    // Restore the name
    torrent->setAttribute(NAME, "test3");
    torrent->save();
}

/* Models comparison */

void tst_Model_Connection_Independent::is() const
{
    auto torrent2_1 = Torrent::find(2);
    auto torrent2_2 = Torrent::find(2);

    // The same primary key, table name and connection name
    QVERIFY(torrent2_1->is(torrent2_2));
}

void tst_Model_Connection_Independent::isNot() const
{
    auto torrent2_1 = Torrent::find(2);

    // Different primary key
    {
        auto torrent3 = Torrent::find(3);

        QVERIFY(torrent2_1->isNot(torrent3));
    }
    // Different table name (also different type)
    {
        auto file4 = TorrentPreviewableFile::find(4);

        QVERIFY(torrent2_1->isNot(file4));
    }
    // Different connection name
    {
        auto torrent2_2 = Torrent::find(2);
        // Check before the connection changed
        QVERIFY(torrent2_1->is(torrent2_2));

        torrent2_2->setConnection("dummy_connection");
        /* Disable connection override, so the isNot() can pickup a connection from
           the model itself (don't pickup an overridden connection). */
        ConnectionOverride::connection.clear();

        QVERIFY(torrent2_1->isNot(torrent2_2));
    }

    // Restore
    ConnectionOverride::connection = m_connection;
}

void tst_Model_Connection_Independent::equalComparison() const
{
    // Is equal without relations
    {
        auto torrent1_1 = Torrent::find(1);
        auto torrent1_2 = Torrent::find(1);

        QVERIFY(torrent1_1);
        QVERIFY(torrent1_2);
        QVERIFY(*torrent1_1 == *torrent1_2);
    }
    // Is equal with relations
    {
        auto torrent1_1 = Torrent::with("torrentFiles")->find(1);
        auto torrent1_2 = Torrent::with("torrentFiles")->find(1);

        QVERIFY(torrent1_1);
        QVERIFY(torrent1_2);
        QVERIFY(*torrent1_1 == *torrent1_2);
    }
    // Is equal with sub-relations
    {
        auto torrent1_1 = Torrent::with("torrentFiles.fileProperty")->find(1);
        auto torrent1_2 = Torrent::with("torrentFiles.fileProperty")->find(1);

        QVERIFY(torrent1_1);
        QVERIFY(torrent1_2);
        QVERIFY(*torrent1_1 == *torrent1_2);
    }
}

void tst_Model_Connection_Independent::notEqualComparison() const
{
    auto torrent1_1 = Torrent::with("torrentFiles.fileProperty")->find(1);
    auto torrent2 = Torrent::with("torrentFiles.fileProperty")->find(2);
    QVERIFY(torrent1_1);
    QVERIFY(torrent2);

    // Different torrent
    QVERIFY(*torrent1_1 != *torrent2);

    // Different attribute
    {
        auto torrent1_2 = Torrent::with("torrentFiles.fileProperty")->find(1);
        QVERIFY(torrent1_2);

        // Check before change
        QVERIFY(*torrent1_1 == *torrent1_2);

        torrent1_2->setAttribute(NAME, "test1 changed");

        QVERIFY(*torrent1_1 != *torrent1_2);
    }
    // Different relations
    {
        auto torrent1_2 = Torrent::with("torrentFiles.fileProperty")->find(1);
        QVERIFY(torrent1_2);

        // Check before change
        QVERIFY(*torrent1_1 == *torrent1_2);

        torrent1_1->load("user");

        QVERIFY(*torrent1_1 != *torrent1_2);
    }
    // Different sub-relation
    {
        auto torrent1_2 = Torrent::with("torrentFiles")->find(1);
        QVERIFY(torrent1_2);

        QVERIFY(*torrent1_1 != *torrent1_2);
    }
}

/* Mass assignment */

void tst_Model_Connection_Independent::massAssignment_Fillable() const
{
    Torrent torrent;

    torrent.fill({{NAME, "test150"}, {SIZE_, 10}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent[NAME], QVariant("test150"));
    QCOMPARE(torrent[SIZE_], QVariant(10));
    QCOMPARE(torrent.getAttributes().size(), 2);
}

void tst_Model_Connection_Independent::massAssignment_Guarded() const
{
    Torrent_GuardedAttribute torrent;

    torrent.fill({{CREATED_AT, QDateTime::currentDateTimeUtc()}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent.getAttributes().size(), 0);
}

void tst_Model_Connection_Independent::
     massAssignment_GuardedAll_NonExistentAttribute() const
{
    Torrent torrent;

    torrent.fill({{"dummy-NON_EXISTENT", "foo"}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent.getAttributes().size(), 0);
}

void tst_Model_Connection_Independent::
     massAssignment_GuardedDisabled_ExistentAttribute() const
{
    Torrent_AllowedMassAssignment torrent;

    torrent.fill({{"dummy-NON_EXISTENT", "foo"}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent["dummy-NON_EXISTENT"], QVariant("foo"));
    QCOMPARE(torrent.getAttributes().size(), 1);
}

void tst_Model_Connection_Independent::
     massAssignment_GuardedDisabled_NonExistentAttribute() const
{
    Torrent_AllowedMassAssignment torrent;

    torrent.fill({{"dummy-NON_EXISTENT", "foo"}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent["dummy-NON_EXISTENT"], QVariant("foo"));
    QCOMPARE(torrent.getAttributes().size(), 1);
}

void
tst_Model_Connection_Independent::massAssignment_TotallyGuarded_Exception() const
{
    Torrent_TotallyGuarded torrent;

    QVERIFY(!torrent.exists);
    QVERIFY_EXCEPTION_THROWN(torrent.fill({{NAME, "test150"}}),
                             MassAssignmentError);
}

void tst_Model_Connection_Independent::
     massAssignment_CantMassFillAttributesWithTableNamesWhenUsingGuarded() const
{
    Torrent torrent;

    torrent.fill({{"foo.bar", 123}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent.getAttributes().size(), 0);
}

void tst_Model_Connection_Independent::
     massAssignment_forceFill_OnTotallyGuardedModel() const
{
    Torrent_TotallyGuarded torrent;
    torrent.forceFill({{NAME, "foo"}, {SIZE_, 12}, {"progress", 20}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent[NAME], QVariant("foo"));
    QCOMPARE(torrent[SIZE_], QVariant(12));
    QCOMPARE(torrent["progress"], QVariant(20));
    QCOMPARE(torrent.getAttributes().size(), 3);
}

void tst_Model_Connection_Independent::
     massAssignment_forceFill_OnGuardedAttribute() const
{
    Torrent_GuardedAttribute torrent;

    const auto createdAt = QDateTime({2021, 2, 2}, {10, 11, 12}, Qt::UTC);

    torrent.forceFill({{CREATED_AT, createdAt}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent[CREATED_AT], QVariant(createdAt));
    QCOMPARE(torrent.getAttributes().size(), 1);
}

void tst_Model_Connection_Independent::
     massAssignment_forceFill_NonExistentAttribute() const
{
    Torrent_TotallyGuarded torrent;
    torrent.forceFill({{"dummy-NON_EXISTENT", "foo"}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent["dummy-NON_EXISTENT"], QVariant("foo"));
    QCOMPARE(torrent.getAttributes().size(), 1);
}

/* Eager loading */

/* Eager or lazy loading all other relations with select constraints don't have and
   also don't need qualified column names for the related table. */

void tst_Model_Connection_Independent::
     with_WithSelectConstraint_WithoutQualifiedColumnsForRelatedTable() const
{
    DB::flushQueryLog(m_connection);
    DB::enableQueryLog(m_connection);
    auto torrent = Torrent::with({"torrentFiles:id,torrent_id,filepath"})->find(2);
    DB::disableQueryLog(m_connection);

    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    const auto queryLog = DB::getQueryLog(m_connection);
    QCOMPARE(queryLog->size(), 2);
    QCOMPARE(queryLog->at(1).query,
             QString("select `id`, `torrent_id`, `filepath` "
                     "from `torrent_previewable_files` "
                     "where `torrent_previewable_files`.`torrent_id` in (?)"));
}

/* Eager or lazy loading the belongs-to-many relation with select constraints must have
   qualified column names on the related and pivot tables. The qualified column names
   on the pivot table is done through the BelongsToManyRelatedTableStore. */

void tst_Model_Connection_Independent::
     with_BelongsToMany_WithSelectConstraint_QualifiedColumnsForRelatedTable() const
{
    DB::flushQueryLog(m_connection);
    DB::enableQueryLog(m_connection);
    auto torrent = Torrent::with({"tags:id,name"})->find(3);
    DB::disableQueryLog(m_connection);

    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    const auto queryLog = DB::getQueryLog(m_connection);

    QCOMPARE(queryLog->size(), 3);
    QCOMPARE(queryLog->at(1).query,
             QString(
                 "select `torrent_tags`.`id`, `torrent_tags`.`name`, "
                     "`tag_torrent`.`torrent_id` as `pivot_torrent_id`, "
                     "`tag_torrent`.`tag_id` as `pivot_tag_id`, "
                     "`tag_torrent`.`active` as `pivot_active`, "
                     "`tag_torrent`.`created_at` as `pivot_created_at`, "
                     "`tag_torrent`.`updated_at` as `pivot_updated_at` "
                 "from `torrent_tags` "
                     "inner join `tag_torrent` "
                         "on `torrent_tags`.`id` = `tag_torrent`.`tag_id` "
                 "where `tag_torrent`.`torrent_id` in (?)"));
}

/* Retrieving results */

void tst_Model_Connection_Independent::pluck() const
{
    // Simple pluck without keying
    {
        auto result = Torrent::orderBy(NAME)->pluck(NAME);

        QVector<QVariant> expected {
            "test1", "test2", "test3", "test4", "test5", "test6", "test7",
        };
        QCOMPARE(result, expected);
    }
    // Templated pluck keyed by id<quint64>
    {
        auto result = Torrent::pluck<quint64>(NAME, ID);

        std::map<quint64, QVariant> expected {
            {1, "test1"}, {2, "test2"}, {3, "test3"}, {4, "test4"},
            {5, "test5"}, {6, "test6"}, {7, "test7"},
        };
        QCOMPARE(result, expected);
    }
    // Templated pluck keyed by name<QString>
    {
        auto result = Torrent::pluck<QString>(SIZE_, NAME);

        std::map<QString, QVariant> expected {
            {"test1", 11}, {"test2", 12}, {"test3", 13}, {"test4", 14},
            {"test5", 15}, {"test6", 16}, {"test7", 17},
        };
        QCOMPARE(result, expected);
    }
    // Templated pluck keyed by file_index<bool>, bool type is used intentionally 😎
    {
        auto result = TorrentPreviewableFile::orderBy(ID)
                      ->pluck<bool>("filepath", "file_index");

        std::map<bool, QVariant> expected {
            {false, "test1_file1.mkv"}, {true, "test2_file2.mkv"},
        };
        QCOMPARE(result, expected);
    }
}

void tst_Model_Connection_Independent::pluck_EmptyResult() const
{
    {
        auto result = Torrent::whereEq(NAME, "dummy-NON_EXISTENT")->pluck(NAME);

        QCOMPARE(result, QVector<QVariant>());
    }
    {
        auto result = Torrent::whereEq(NAME, "dummy-NON_EXISTENT")
                      ->pluck<quint64>(NAME, ID);

        std::map<quint64, QVariant> expected;
        QCOMPARE(result, expected);
    }
}

void tst_Model_Connection_Independent::pluck_QualifiedColumnOrKey() const
{
    // Strip table name
    {
        auto result = Torrent::orderBy(NAME)->pluck("torrents.name");

        QVector<QVariant> expected {
            "test1", "test2", "test3", "test4", "test5", "test6", "test7",
        };
        QCOMPARE(result, expected);
    }
    // Strip table name
    {
        auto result = Torrent::pluck<quint64>("torrents.name", "torrents.id");

        std::map<quint64, QVariant> expected {
            {1, "test1"}, {2, "test2"}, {3, "test3"}, {4, "test4"},
            {5, "test5"}, {6, "test6"}, {7, "test7"},
        };
        QCOMPARE(result, expected);
    }
    // Strip column alias
    {
        auto result = Torrent::orderBy("name_alt")->pluck("name as name_alt");

        QVector<QVariant> expected {
            "test1", "test2", "test3", "test4", "test5", "test6", "test7",
        };
        QCOMPARE(result, expected);
    }
    // Strip column alias
    {
        auto result = Torrent::pluck<quint64>("name as name_alt", "id as id_alt");

        std::map<quint64, QVariant> expected {
            {1, "test1"}, {2, "test2"}, {3, "test3"}, {4, "test4"},
            {5, "test5"}, {6, "test6"}, {7, "test7"},
        };
        QCOMPARE(result, expected);
    }
    // Strip column alias and table name
    {
        auto result = Torrent::pluck<quint64>("torrents.name", "id as id_alt");

        std::map<quint64, QVariant> expected {
            {1, "test1"}, {2, "test2"}, {3, "test3"}, {4, "test4"},
            {5, "test5"}, {6, "test6"}, {7, "test7"},
        };
        QCOMPARE(result, expected);
    }
}

void tst_Model_Connection_Independent::pluck_With_u_dates() const
{
    // Simple pluck without keying
    {
        auto result = Torrent::pluck("added_on");

        QVector<QVariant> expected {
            QDateTime({2020, 8, 1}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 2}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 3}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 4}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 5}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 6}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 7}, {20, 11, 10}, Qt::UTC),
        };
        QCOMPARE(result, expected);
    }
    // Templated pluck keyed by id<quint64> and QVariant<QDateTime> as the value
    {
        auto result = Torrent::pluck<quint64>("added_on", ID);

        std::map<quint64, QVariant> expected {
            {1, QDateTime({2020, 8, 1}, {20, 11, 10}, Qt::UTC)},
            {2, QDateTime({2020, 8, 2}, {20, 11, 10}, Qt::UTC)},
            {3, QDateTime({2020, 8, 3}, {20, 11, 10}, Qt::UTC)},
            {4, QDateTime({2020, 8, 4}, {20, 11, 10}, Qt::UTC)},
            {5, QDateTime({2020, 8, 5}, {20, 11, 10}, Qt::UTC)},
            {6, QDateTime({2020, 8, 6}, {20, 11, 10}, Qt::UTC)},
            {7, QDateTime({2020, 8, 7}, {20, 11, 10}, Qt::UTC)},
        };
        QCOMPARE(result, expected);
    }
    // Templated pluck keyed by added_on<QDateTime>
    {
        auto result = Torrent::pluck<QDateTime>(ID, "added_on");

        std::map<QDateTime, QVariant> expected {
            {QDateTime({2020, 8, 1}, {20, 11, 10}, Qt::UTC), 1},
            {QDateTime({2020, 8, 2}, {20, 11, 10}, Qt::UTC), 2},
            {QDateTime({2020, 8, 3}, {20, 11, 10}, Qt::UTC), 3},
            {QDateTime({2020, 8, 4}, {20, 11, 10}, Qt::UTC), 4},
            {QDateTime({2020, 8, 5}, {20, 11, 10}, Qt::UTC), 5},
            {QDateTime({2020, 8, 6}, {20, 11, 10}, Qt::UTC), 6},
            {QDateTime({2020, 8, 7}, {20, 11, 10}, Qt::UTC), 7},
        };
        QCOMPARE(result, expected);
    }
}

void tst_Model_Connection_Independent::pluck_EmptyResult_With_u_dates() const
{
    {
        auto result = Torrent::whereEq(NAME, "dummy-NON_EXISTENT")->pluck("added_on");

        QCOMPARE(result, QVector<QVariant>());
    }
    {
        auto result = Torrent::whereEq(NAME, "dummy-NON_EXISTENT")
                      ->pluck<QDateTime>(ID, "added_on");

        std::map<QDateTime, QVariant> expected;
        QCOMPARE(result, expected);
    }
}

void tst_Model_Connection_Independent::pluck_QualifiedColumnOrKey_With_u_dates() const
{
    // Strip table name
    {
        auto result = Torrent::pluck("torrents.added_on");

        QVector<QVariant> expected {
            QDateTime({2020, 8, 1}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 2}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 3}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 4}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 5}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 6}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 7}, {20, 11, 10}, Qt::UTC),
        };
        QCOMPARE(result, expected);
    }
    // Strip table name
    {
        auto result = Torrent::pluck<quint64>("torrents.added_on", ID);

        std::map<quint64, QVariant> expected {
            {1, QDateTime({2020, 8, 1}, {20, 11, 10}, Qt::UTC)},
            {2, QDateTime({2020, 8, 2}, {20, 11, 10}, Qt::UTC)},
            {3, QDateTime({2020, 8, 3}, {20, 11, 10}, Qt::UTC)},
            {4, QDateTime({2020, 8, 4}, {20, 11, 10}, Qt::UTC)},
            {5, QDateTime({2020, 8, 5}, {20, 11, 10}, Qt::UTC)},
            {6, QDateTime({2020, 8, 6}, {20, 11, 10}, Qt::UTC)},
            {7, QDateTime({2020, 8, 7}, {20, 11, 10}, Qt::UTC)},
        };
        QCOMPARE(result, expected);
    }
    // Strip column alias
    {
        auto result = Torrent::orderBy(ID)->pluck("added_on as added_on_alt");

        QVector<QVariant> expected {
            QDateTime({2020, 8, 1}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 2}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 3}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 4}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 5}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 6}, {20, 11, 10}, Qt::UTC),
            QDateTime({2020, 8, 7}, {20, 11, 10}, Qt::UTC),
        };
        QCOMPARE(result, expected);
    }
    // Strip column alias
    {
        auto result = Torrent::pluck<quint64>("added_on as added_on_alt", "id as id_alt");

        std::map<quint64, QVariant> expected {
            {1, QDateTime({2020, 8, 1}, {20, 11, 10}, Qt::UTC)},
            {2, QDateTime({2020, 8, 2}, {20, 11, 10}, Qt::UTC)},
            {3, QDateTime({2020, 8, 3}, {20, 11, 10}, Qt::UTC)},
            {4, QDateTime({2020, 8, 4}, {20, 11, 10}, Qt::UTC)},
            {5, QDateTime({2020, 8, 5}, {20, 11, 10}, Qt::UTC)},
            {6, QDateTime({2020, 8, 6}, {20, 11, 10}, Qt::UTC)},
            {7, QDateTime({2020, 8, 7}, {20, 11, 10}, Qt::UTC)},
        };
        QCOMPARE(result, expected);
    }
    // Strip column alias and table name
    {
        auto result = Torrent::pluck<quint64>("torrents.added_on", "id as id_alt");

        std::map<quint64, QVariant> expected {
            {1, QDateTime({2020, 8, 1}, {20, 11, 10}, Qt::UTC)},
            {2, QDateTime({2020, 8, 2}, {20, 11, 10}, Qt::UTC)},
            {3, QDateTime({2020, 8, 3}, {20, 11, 10}, Qt::UTC)},
            {4, QDateTime({2020, 8, 4}, {20, 11, 10}, Qt::UTC)},
            {5, QDateTime({2020, 8, 5}, {20, 11, 10}, Qt::UTC)},
            {6, QDateTime({2020, 8, 6}, {20, 11, 10}, Qt::UTC)},
            {7, QDateTime({2020, 8, 7}, {20, 11, 10}, Qt::UTC)},
        };
        QCOMPARE(result, expected);
    }
}

void tst_Model_Connection_Independent::whereRowValues() const
{
    auto tag = Torrent::whereRowValuesEq({ID, NAME}, {3, "test3"})->first();

    QVERIFY(tag);
    QCOMPARE(tag->getKey(), QVariant(3));
    QCOMPARE(tag->getAttribute(NAME), QVariant("test3"));
}

void tst_Model_Connection_Independent::only() const
{
    auto torrent = Torrent::find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto attributes = torrent->only({ID, NAME, SIZE_, CREATED_AT});
    QCOMPARE(attributes.size(), 4);

    /* The order must be the same as the requested order during only() method call and
       as it internally calls getAttribute() all the casts will be applied. */
    QVector<AttributeItem> expectedAttributes {
        {ID,         1},
        {NAME,       QString("test1")},
        {SIZE_,      11},
        {CREATED_AT, QDateTime({2016, 6, 1}, {8, 8, 23}, Qt::UTC)},
    };
    QCOMPARE(attributes, expectedAttributes);
}

void tst_Model_Connection_Independent::only_NonExistentAttribute() const
{
    auto torrent = Torrent::find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto attributes = torrent->only({ID, NAME, "dummy-NON_EXISTENT"});
    QCOMPARE(attributes.size(), 3);

    /* The order must be the same as the requested order during only() method call and
       as it internally calls getAttribute() all the casts will be applied. */
    QVector<AttributeItem> expectedAttributes {
        {ID,   1},
        {NAME, QString("test1")},
        // Must return invalid QVariant for non-existent attribute
        {"dummy-NON_EXISTENT", QVariant()},
    };
    QCOMPARE(attributes, expectedAttributes);

    // Verify an invalid attribute 🤓
    const auto &dummyAttribute = expectedAttributes.at(2);
    QCOMPARE(dummyAttribute.key, QString("dummy-NON_EXISTENT"));
    QVERIFY(!dummyAttribute.value.isValid());
    QVERIFY(dummyAttribute.value.isNull());
}

/* Builds Queries */

void tst_Model_Connection_Independent::chunk() const
{
    using SizeType = ModelsCollection<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount>
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = FilePropertyProperty::orderBy(ID)
                  ->chunk(3, [&compareResultSize, &ids]
                             (ModelsCollection<FilePropertyProperty> &&models,
                              const qint64 page)
    {
        compareResultSize(models.size(), page);

        for (auto &&fileProperty : models)
            ids.emplace_back(fileProperty[ID]->template value<quint64>());

        return true;
    });

    QVERIFY(result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::chunk_ReturnFalse() const
{
    using SizeType = ModelsCollection<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount> (I leave it here also in this test, doesn't matter much)
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = FilePropertyProperty::orderBy(ID)
                  ->chunk(3, [&compareResultSize, &ids]
                             (ModelsCollection<FilePropertyProperty> &&models,
                              const qint64 page)
    {
        compareResultSize(models.size(), page);

        for (auto &&fileProperty : models) {
            auto id = fileProperty[ID]->template value<quint64>();
            ids.emplace_back(id);

            // Interrupt chunk-ing
            if (id == 5)
                return false;
        }

        return true;
    });

    QVERIFY(!result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::chunk_EnforceOrderBy() const
{
    /* The TinBuilder version doesn't throws exception if the 'order by' clause is not
       specified, instead it adds a generic 'order by' clause
       on the Model::getQualifiedKeyName() (it sorts by the primary key by default). */
    using SizeType = ModelsCollection<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount>
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = FilePropertyProperty::chunk(
                      3, [&compareResultSize, &ids]
                         (ModelsCollection<FilePropertyProperty> &&models,
                          const qint64 page)
    {
        compareResultSize(models.size(), page);

        for (auto &&fileProperty : models)
            ids.emplace_back(fileProperty[ID]->template value<quint64>());

        return true;
    });

    QVERIFY(result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::chunk_EmptyResult() const
{
    auto callbackInvoked = false;

    auto result = FilePropertyProperty::whereEq(NAME,
                                                QStringLiteral("dummy-NON_EXISTENT"))
                  ->orderBy(ID)
                  .chunk(3, [&callbackInvoked]
                            (ModelsCollection<FilePropertyProperty> &&/*unused*/,
                             const qint64 /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Model_Connection_Independent::each() const
{
    std::vector<qint64> indexes;
    indexes.reserve(8);
    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = FilePropertyProperty::orderBy(ID)
                  ->each([&indexes, &ids]
                         (FilePropertyProperty &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        return true;
    });

    QVERIFY(result);

    std::vector<qint64> expectedIndexes {0, 1, 2, 3, 4, 5, 6, 7};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::each_ReturnFalse() const
{
    std::vector<qint64> indexes;
    indexes.reserve(5);
    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = FilePropertyProperty::orderBy(ID)
                  ->each([&indexes, &ids]
                         (FilePropertyProperty &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        return index != 4; // false/interrupt on 4
    });

    QVERIFY(!result);

    std::vector<qint64> expectedIndexes {0, 1, 2, 3, 4};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::each_EnforceOrderBy() const
{
    /* The TinBuilder version doesn't throws exception if the 'order by' clause is not
       specified, instead it adds a generic 'order by' clause
       on the Model::getQualifiedKeyName() (it sorts by the primary key by default). */
    std::vector<qint64> indexes;
    indexes.reserve(8);
    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = FilePropertyProperty::each(
                      [&indexes, &ids]
                      (FilePropertyProperty &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        return true;
    });

    QVERIFY(result);

    std::vector<qint64> expectedIndexes {0, 1, 2, 3, 4, 5, 6, 7};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::each_EmptyResult() const
{
    auto callbackInvoked = false;

    auto result = FilePropertyProperty::whereEq(NAME,
                                                QStringLiteral("dummy-NON_EXISTENT"))
                  ->orderBy(ID)
                  .each([&callbackInvoked]
                        (FilePropertyProperty &&/*unused*/, const qint64 /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
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

void tst_Model_Connection_Independent::chunkMap() const
{
    auto result = FilePropertyProperty::orderBy(ID)
                  ->chunkMap([](FilePropertyProperty &&model)
    {
        auto nameRef = model[NAME];

        // Modify the name attribute
        nameRef = QStringLiteral("%1_mapped").arg(nameRef->template value<QString>());

        return std::move(model);
    });

    QVector<IdAndName> expectedResult {
        {1, "test2_file1_property1_mapped"},
        {2, "test2_file2_property1_mapped"},
        {3, "test3_file1_property1_mapped"},
        {4, "test3_file1_property2_mapped"},
        {5, "test4_file1_property1_mapped"},
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

void tst_Model_Connection_Independent::chunkMap_EnforceOrderBy() const
{
    /* The TinBuilder version doesn't throws exception if the 'order by' clause is not
       specified, instead it adds a generic 'order by' clause
       on the Model::getQualifiedKeyName() (it sorts by the primary key by default). */
    auto result = FilePropertyProperty::chunkMap([](FilePropertyProperty &&model)
    {
        auto nameRef = model[NAME];

        // Modify the name attribute
        nameRef = QStringLiteral("%1_mapped").arg(nameRef->template value<QString>());

        return std::move(model);
    });

    QVector<IdAndName> expectedResult {
        {1, "test2_file1_property1_mapped"},
        {2, "test2_file2_property1_mapped"},
        {3, "test3_file1_property1_mapped"},
        {4, "test3_file1_property2_mapped"},
        {5, "test4_file1_property1_mapped"},
        {6, "test5_file1_property1_mapped"},
        {7, "test5_file1_property2_mapped"},
        {8, "test5_file1_property3_mapped"},
    };

    // Transform the result so I can compare it
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

void tst_Model_Connection_Independent::chunkMap_EmptyResult() const
{
    auto callbackInvoked = false;

    auto result = FilePropertyProperty::whereEq(NAME,
                                                QStringLiteral("dummy-NON_EXISTENT"))
                  ->chunkMap([&callbackInvoked]
                             (FilePropertyProperty &&model)
    {
        callbackInvoked = true;

        return std::move(model);
    });

    QVERIFY(!callbackInvoked);
    QVERIFY((std::is_same_v<decltype (result), ModelsCollection<FilePropertyProperty>>));
    QVERIFY(result.isEmpty());
}

void tst_Model_Connection_Independent::chunkMap_TemplatedReturnValue() const
{
    auto result = FilePropertyProperty::orderBy(ID)
                  ->chunkMap<QString>([](FilePropertyProperty &&model)
    {
        // Return the modify name directly
        return QStringLiteral("%1_mapped").arg(model[NAME]->template value<QString>());
    });

    QVector<QString> expectedResult {
        {"test2_file1_property1_mapped"},
        {"test2_file2_property1_mapped"},
        {"test3_file1_property1_mapped"},
        {"test3_file1_property2_mapped"},
        {"test4_file1_property1_mapped"},
        {"test5_file1_property1_mapped"},
        {"test5_file1_property2_mapped"},
        {"test5_file1_property3_mapped"},
    };

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(result, expectedResult);
}

void
tst_Model_Connection_Independent::chunkMap_EnforceOrderBy_TemplatedReturnValue() const
{
    /* The TinBuilder version doesn't throws exception if the 'order by' clause is not
       specified, instead it adds a generic 'order by' clause
       on the Model::getQualifiedKeyName() (it sorts by the primary key by default). */
    auto result = FilePropertyProperty::chunkMap<QString>(
                      [](FilePropertyProperty &&model)
    {
        // Return the modify name directly
        return QStringLiteral("%1_mapped").arg(model[NAME]->template value<QString>());
    });

    QVector<QString> expectedResult {
        {"test2_file1_property1_mapped"},
        {"test2_file2_property1_mapped"},
        {"test3_file1_property1_mapped"},
        {"test3_file1_property2_mapped"},
        {"test4_file1_property1_mapped"},
        {"test5_file1_property1_mapped"},
        {"test5_file1_property2_mapped"},
        {"test5_file1_property3_mapped"},
    };

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(result, expectedResult);
}

void tst_Model_Connection_Independent::chunkMap_EmptyResult_TemplatedReturnValue() const
{
    auto callbackInvoked = false;

    auto result = FilePropertyProperty::whereEq(NAME,
                                                QStringLiteral("dummy-NON_EXISTENT"))
                  ->chunkMap<QString>([&callbackInvoked]
                                      (FilePropertyProperty &&/*unused*/)
                                      -> QString
    {
        callbackInvoked = true;

        return {};
    });

    QVERIFY(!callbackInvoked);
    QVERIFY((std::is_same_v<decltype (result), QVector<QString>>));
    QVERIFY(result.isEmpty());
}

void tst_Model_Connection_Independent::chunkById() const
{
    using SizeType = ModelsCollection<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount>
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = FilePropertyProperty::orderBy(ID)
                  ->chunkById(3, [&compareResultSize, &ids]
                                 (ModelsCollection<FilePropertyProperty> &&models,
                                  const qint64 page)
    {
        compareResultSize(models.size(), page);

        for (auto &&tag : models)
            ids.emplace_back(tag.getKeyCasted());

        return true;
    });

    QVERIFY(result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::chunkById_ReturnFalse() const
{
    using SizeType = ModelsCollection<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount> (I leave it here also in this test, doesn't matter much
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = FilePropertyProperty::orderBy(ID)
                  ->chunkById(3, [&compareResultSize, &ids]
                                 (ModelsCollection<FilePropertyProperty> &&models,
                                  const qint64 page)
    {
        compareResultSize(models.size(), page);

        for (auto &&tag : models) {
            auto id = tag.getKeyCasted();
            ids.emplace_back(id);

            // Interrupt chunk-ing
            if (id == 5)
                return false;
        }

        return true;
    });

    QVERIFY(!result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::chunkById_EmptyResult() const
{
    auto callbackInvoked = false;

    auto result = FilePropertyProperty::whereEq(NAME,
                                                QStringLiteral("dummy-NON_EXISTENT"))
                  ->orderBy(ID)
                  .chunkById(3, [&callbackInvoked]
                                (ModelsCollection<FilePropertyProperty> &&/*unused*/,
                                 const qint64 /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Model_Connection_Independent::chunkById_WithAlias() const
{
    using SizeType = ModelsCollection<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount>
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = FilePropertyProperty::select({ASTERISK, "id as id_as"})
                  ->orderBy(ID)
                  .chunkById(3, [&compareResultSize, &ids]
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

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::chunkById_ReturnFalse_WithAlias() const
{
    using SizeType = ModelsCollection<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount> (I leave it here also in this test, doesn't matter much
    const std::unordered_map<qint64, SizeType> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows]
                                   (const SizeType size, const qint64 page)
    {
        QCOMPARE(static_cast<qint64>(size), expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = FilePropertyProperty::select({ASTERISK, "id as id_as"})
                  ->orderBy(ID)
                  .chunkById(3, [&compareResultSize, &ids]
                                (ModelsCollection<FilePropertyProperty> &&models,
                                 const qint64 page)
    {
        compareResultSize(models.size(), page);

        for (auto &&tag : models) {
            auto id = tag.getKeyCasted();
            ids.emplace_back(id);

            // Interrupt chunk-ing
            if (id == 5)
                return false;
        }

        return true;
    },
        ID, "id_as");

    QVERIFY(!result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::chunkById_EmptyResult_WithAlias() const
{
    auto callbackInvoked = false;

    auto result = FilePropertyProperty::select({ASTERISK, "id as id_as"})
                  ->whereEq(NAME, QStringLiteral("dummy-NON_EXISTENT"))
                  .orderBy(ID)
                  .chunkById(3, [&callbackInvoked]
                                (ModelsCollection<FilePropertyProperty> &&/*unused*/,
                                 const qint64 /*unused*/)
    {
        callbackInvoked = true;

        return true;
    },
        ID, "id_as");

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Model_Connection_Independent::eachById() const
{
    std::vector<qint64> indexes;
    indexes.reserve(8);
    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = FilePropertyProperty::orderBy(ID)
                  ->eachById([&indexes, &ids]
                             (FilePropertyProperty &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        return true;
    });

    QVERIFY(result);

    std::vector<qint64> expectedIndexes {0, 1, 2, 3, 4, 5, 6, 7};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::eachById_ReturnFalse() const
{
    std::vector<qint64> indexes;
    indexes.reserve(5);
    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = FilePropertyProperty::orderBy(ID)
                  ->eachById([&indexes, &ids]
                             (FilePropertyProperty &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        return index != 4; // false/interrupt on 4
    });

    QVERIFY(!result);

    std::vector<qint64> expectedIndexes {0, 1, 2, 3, 4};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::eachById_EmptyResult() const
{
    auto callbackInvoked = false;

    auto result = FilePropertyProperty::whereEq(NAME,
                                                QStringLiteral("dummy-NON_EXISTENT"))
                  ->orderBy(ID)
                  .eachById([&callbackInvoked]
                            (FilePropertyProperty &&/*unused*/, const qint64 /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Model_Connection_Independent::eachById_WithAlias() const
{
    std::vector<qint64> indexes;
    indexes.reserve(8);
    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = FilePropertyProperty::select({ASTERISK, "id as id_as"})
                  ->orderBy(ID)
                  .eachById([&indexes, &ids]
                            (FilePropertyProperty &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        return true;
    },
        1000, ID, "id_as");

    QVERIFY(result);

    std::vector<qint64> expectedIndexes {0, 1, 2, 3, 4, 5, 6, 7};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::eachById_ReturnFalse_WithAlias() const
{
    std::vector<qint64> indexes;
    indexes.reserve(5);
    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = FilePropertyProperty::select({ASTERISK, "id as id_as"})
                  ->orderBy(ID)
                  .eachById([&indexes, &ids]
                            (FilePropertyProperty &&model, const qint64 index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getKeyCasted());

        return index != 4; // false/interrupt on 4
    },
        1000, ID, "id_as");

    QVERIFY(!result);

    std::vector<qint64> expectedIndexes {0, 1, 2, 3, 4};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::eachById_EmptyResult_WithAlias() const
{
    auto callbackInvoked = false;

    auto result = FilePropertyProperty::select({ASTERISK, "id as id_as"})
                  ->whereEq(NAME, QStringLiteral("dummy-NON_EXISTENT"))
                  .orderBy(ID)
                  .eachById([&callbackInvoked]
                            (FilePropertyProperty &&/*unused*/, const qint64 /*unused*/)
    {
        callbackInvoked = true;

        return true;
    },
        1000, ID, "id_as");

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Model_Connection_Independent::tap() const
{
    auto builder = FilePropertyProperty::query();

    auto callbackInvoked = false;
    auto &tappedBuilder = builder->tap([&callbackInvoked]
                                       (Orm::Tiny::Builder<FilePropertyProperty> &query)
    {
        callbackInvoked = true;

        return query;
    });

    QVERIFY((std::is_same_v<decltype (tappedBuilder), decltype (*builder)>));
    QVERIFY(callbackInvoked);
    // It must be the same TinyBuilder (the same memory address)
    QVERIFY(reinterpret_cast<uintptr_t>(&*builder) ==
            reinterpret_cast<uintptr_t>(&tappedBuilder));
}

void tst_Model_Connection_Independent::sole() const
{
    auto model = FilePropertyProperty::whereEq(ID, 1)->sole();

    QCOMPARE(model[ID]->value<quint64>(), static_cast<quint64>(1));
    QCOMPARE(model[NAME]->value<QString>(), QString("test2_file1_property1"));
}

void tst_Model_Connection_Independent::sole_RecordsNotFoundError() const
{
    QVERIFY_EXCEPTION_THROWN(
            FilePropertyProperty::whereEq(NAME, "dummy-NON_EXISTENT")->sole(),
            RecordsNotFoundError);
}

void tst_Model_Connection_Independent::sole_MultipleRecordsFoundError() const
{
    QVERIFY_EXCEPTION_THROWN(
            FilePropertyProperty::whereEq("file_property_id", 5)->sole(),
            MultipleRecordsFoundError);
}

void tst_Model_Connection_Independent::sole_Pretending() const
{
    auto log = DB::connection(m_connection).pretend([]()
    {
        FilePropertyProperty::whereEq(NAME, "dummy-NON_EXISTENT")->sole();
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select * from `file_property_properties` where `name` = ? limit 2");
    QCOMPARE(firstLog.boundValues,
             QVector<QVariant>({QVariant(QString("dummy-NON_EXISTENT"))}));
}

void tst_Model_Connection_Independent::soleValue() const
{
    auto value = FilePropertyProperty::whereEq(ID, 1)->soleValue(NAME);

    QVERIFY((std::is_same_v<decltype (value), QVariant>));
    QVERIFY(value.isValid() && !value.isNull());
    QCOMPARE(value, QVariant(QString("test2_file1_property1")));
}

void tst_Model_Connection_Independent::soleValue_RecordsNotFoundError() const
{
    QVERIFY_EXCEPTION_THROWN(
            FilePropertyProperty::whereEq(NAME, "dummy-NON_EXISTENT")->soleValue(NAME),
            RecordsNotFoundError);
}

void tst_Model_Connection_Independent::soleValue_MultipleRecordsFoundError() const
{
    QVERIFY_EXCEPTION_THROWN(
            FilePropertyProperty::soleValue(NAME),
            MultipleRecordsFoundError);
}

void tst_Model_Connection_Independent::soleValue_Pretending() const
{
    auto log = DB::connection(m_connection).pretend([]()
    {
        FilePropertyProperty::whereEq(NAME, "dummy-NON_EXISTENT")->soleValue(NAME);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select `name` from `file_property_properties` where `name` = ? limit 2");
    QCOMPARE(firstLog.boundValues,
             QVector<QVariant>({QVariant(QString("dummy-NON_EXISTENT"))}));
}

/* Serialization */

void tst_Model_Connection_Independent::toMap() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVariantMap serialized = torrent->toMap();
    QCOMPARE(serialized.size(), 10);

    QVariantMap expectedAttributes {
        {"added_on", "2020-08-04T20:11:10.000Z"},
        {CREATED_AT, "2019-09-04T08:11:23.000Z"},
        {HASH_,      "4579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {ID,         4},
        {NAME,       "test4"},
        {NOTE,       "after update revert updated_at"},
        {"progress", 400},
        {SIZE_,      14},
        {UPDATED_AT, "2021-01-04T18:46:31.000Z"},
        {"user_id",  1},
    };
    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toVector() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVector<AttributeItem> serialized = torrent->toVector();
    QCOMPARE(serialized.size(), 10);

    // The order must be the same as returned from the MySQL database
    QVector<AttributeItem> expectedAttributes {
        {ID,         4},
        {"user_id",  1},
        {NAME,       "test4"},
        {SIZE_,      14},
        {"progress", 400},
        {"added_on", "2020-08-04T20:11:10.000Z"},
        {HASH_,      "4579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {NOTE,       "after update revert updated_at"},
        {CREATED_AT, "2019-09-04T08:11:23.000Z"},
        {UPDATED_AT, "2021-01-04T18:46:31.000Z"},
    };
    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toMap_WithCasts() const
{
    auto datetime = Datetime::instance({
        {"datetime",  QDateTime({2023, 05, 13}, {10, 11, 12}, Qt::UTC)},
        {"date",      QDateTime({2023, 05, 14}, {10, 11, 12}, Qt::UTC)},
        {"timestamp", static_cast<qint64>(1662712888)},
    });

    datetime.mergeCasts({{"datetime",  CastType::QDateTime},
                         {"date",      CastType::QDate},
                         {"timestamp", CastType::Timestamp}});

    QVariantMap serialized = datetime.toMap();
    QCOMPARE(serialized.size(), 3);

    // The order must be the same as returned from the MySQL database
    QVariantMap expectedAttributes {
        {"date",      "2023-05-14"},
        {"datetime",  "2023-05-13T10:11:12.000Z"},
        {"timestamp", static_cast<qint64>(1662712888)},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    datetime.resetCasts();
}

void tst_Model_Connection_Independent::toVector_WithCasts() const
{
    auto datetime = Datetime::instance({
        {"datetime",  QDateTime({2023, 05, 13}, {10, 11, 12}, Qt::UTC)},
        {"date",      QDateTime({2023, 05, 14}, {10, 11, 12}, Qt::UTC)},
        {"timestamp", static_cast<qint64>(1662712888)},
    });

    datetime.mergeCasts({{"datetime",  CastType::QDateTime},
                         {"date",      CastType::QDate},
                         {"timestamp", CastType::Timestamp}});

    QVector<AttributeItem> serialized = datetime.toVector();
    QCOMPARE(serialized.size(), 3);

    // The order must be the same as returned from the MySQL database
    QVector<AttributeItem> expectedAttributes {
        {"datetime",  "2023-05-13T10:11:12.000Z"},
        {"date",      "2023-05-14"},
        {"timestamp", static_cast<qint64>(1662712888)},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    datetime.resetCasts();
}

void tst_Model_Connection_Independent::toMap_UDatesOnly_QDateTime_For_date_column() const
{
    auto datetime = Datetime::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, Qt::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, Qt::UTC)},
    });

    QVariantMap serialized = datetime.toMap();
    QCOMPARE(serialized.size(), 2);

    // The order must be the same as returned from the MySQL database
    QVariantMap expectedAttributes {
        {"date",     "2023-05-14T10:11:12.000Z"},
        {"datetime", "2023-05-13T10:11:12.000Z"},
    };
    QCOMPARE(serialized, expectedAttributes);
}

void
tst_Model_Connection_Independent::toVector_UDatesOnly_QDateTime_For_date_column() const
{
    auto datetime = Datetime::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, Qt::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, Qt::UTC)},
    });

    QVector<AttributeItem> serialized = datetime.toVector();
    QCOMPARE(serialized.size(), 2);

    // The order must be the same as returned from the MySQL database
    QVector<AttributeItem> expectedAttributes {
        {"datetime", "2023-05-13T10:11:12.000Z"},
        {"date",     "2023-05-14T10:11:12.000Z"},
    };
    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toMap_UDatesOnly_QDate_For_date_column() const
{
    auto datetime = Datetime::instance({
        {"date", QDate(2023, 05, 14)},
    });

    QVariantMap serialized = datetime.toMap();
    QCOMPARE(serialized.size(), 1);

    // The order must be the same as returned from the MySQL database
    QVariantMap expectedAttributes {
        {"date", "2023-05-14T00:00:00.000Z"},
    };
    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toVector_UDatesOnly_QDate_For_date_column() const
{
    auto datetime = Datetime::instance({
        {"date", QDate(2023, 05, 14)},
    });

    QVector<AttributeItem> serialized = datetime.toVector();
    QCOMPARE(serialized.size(), 1);

    // The order must be the same as returned from the MySQL database
    QVector<AttributeItem> expectedAttributes {
        {"date", "2023-05-14T00:00:00.000Z"},
    };
    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toMap_UDatesOnly_Timestamp() const
{
    auto datetime = Datetime::instance(QVector<AttributeItem> {
        {"timestamp", static_cast<qint64>(1662712888)},
    });

    QVariantMap serialized = datetime.toMap();
    QCOMPARE(serialized.size(), 1);

    QVariantMap expectedAttributes {
        {"timestamp", "2022-09-09T08:41:28.000Z"},
    };
    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toVector_UDatesOnly_Timestamp() const
{
    auto datetime = Datetime::instance(QVector<AttributeItem> {
        {"timestamp", static_cast<qint64>(1662712888)},
    });

    QVector<AttributeItem> serialized = datetime.toVector();
    QCOMPARE(serialized.size(), 1);

    // The order must be the same as returned from the MySQL database
    QVector<AttributeItem> expectedAttributes {
        {"timestamp", "2022-09-09T08:41:28.000Z"},
    };
    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toMap_WithDateModfiers() const
{
    auto datetime = Datetime::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, Qt::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, Qt::UTC)},
    });

    datetime.mergeCasts({
        {"datetime", {CastType::CustomQDateTime, "dd.MM.yyyy HH:mm:ss.z t"}},
        {"date",     {CastType::CustomQDate,     "dd.MM.yyyy"}},
    });

    QVariantMap serialized = datetime.toMap();
    QCOMPARE(serialized.size(), 2);

    // The order must be the same as returned from the MySQL database
    QVariantMap expectedAttributes {
        {"date",     "14.05.2023"},
        {"datetime", "13.05.2023 10:11:12.0 UTC"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    datetime.resetCasts();
}

void tst_Model_Connection_Independent::toVector_WithDateModfiers() const
{
    auto datetime = Datetime::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, Qt::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, Qt::UTC)},
    });

    datetime.mergeCasts({
        {"datetime", {CastType::CustomQDateTime, "dd.MM.yyyy HH:mm:ss.z t"}},
        {"date",     {CastType::CustomQDate,     "dd.MM.yyyy"}},
    });

    QVector<AttributeItem> serialized = datetime.toVector();
    QCOMPARE(serialized.size(), 2);

    // The order must be the same as returned from the MySQL database
    QVector<AttributeItem> expectedAttributes {
        {"datetime", "13.05.2023 10:11:12.0 UTC"},
        {"date",     "14.05.2023"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    datetime.resetCasts();
}

void tst_Model_Connection_Independent::toMap_UDatesOnly_DateNullVariants() const
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    Type::u_dates = {"date", "datetime", "timestamp"};
#else
    Type::u_dates = QStringList {"date", "datetime", "timestamp"};
#endif

    auto type = Type::find(3, {ID, "date", "datetime", "timestamp"});
    QVERIFY(type);
    QVERIFY(type->exists);

    QVariantMap serialized = type->toMap();
    QCOMPARE(serialized.size(), 4);

    QCOMPARE(serialized[ID].template value<typename Type::KeyType>(), 3);

    QCOMPARE(Helpers::qVariantTypeId(serialized["date"]),      QMetaType::QDateTime);
    QCOMPARE(Helpers::qVariantTypeId(serialized["datetime"]),  QMetaType::QDateTime);
    QCOMPARE(Helpers::qVariantTypeId(serialized["timestamp"]), QMetaType::QDateTime);

    // Restore
    Type::u_dates.clear();
}

void tst_Model_Connection_Independent::toVector_UDatesOnly_DateNullVariants() const
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    Type::u_dates = {"date", "datetime", "timestamp"};
#else
    Type::u_dates = QStringList {"date", "datetime", "timestamp"};
#endif

    auto type = Type::find(3, {ID, "date", "datetime", "timestamp"});
    QVERIFY(type);
    QVERIFY(type->exists);

    QVector<AttributeItem> serialized = type->toVector();
    QCOMPARE(serialized.size(), 4);

    const auto &serialized0 = serialized.at(0);
    QCOMPARE(serialized0.key, ID);
    QCOMPARE(serialized0.value.template value<typename Type::KeyType>(), 3);

    const auto &serialized1 = serialized.at(1);
    const auto &serialized2 = serialized.at(2);
    const auto &serialized3 = serialized.at(3);

    QCOMPARE(serialized1.key, "date");
    QCOMPARE(serialized2.key, "datetime");
    QCOMPARE(serialized3.key, "timestamp");

    QCOMPARE(Helpers::qVariantTypeId(serialized1.value), QMetaType::QDateTime);
    QCOMPARE(Helpers::qVariantTypeId(serialized2.value), QMetaType::QDateTime);
    QCOMPARE(Helpers::qVariantTypeId(serialized3.value), QMetaType::QDateTime);

    // Restore
    Type::u_dates.clear();
}

void tst_Model_Connection_Independent::toMap_WithCasts_DateNullVariants() const
{
    auto type = Type::find(3, {ID, "date", "datetime", "timestamp"});
    QVERIFY(type);
    QVERIFY(type->exists);

    type->mergeCasts({
        {"date",      CastType::QDate},
        {"datetime",  CastType::QDateTime},
        {"timestamp", CastType::Timestamp},
    });

    QVariantMap serialized = type->toMap();
    QCOMPARE(serialized.size(), 4);

    QCOMPARE(serialized[ID].template value<typename Type::KeyType>(), 3);

    QCOMPARE(Helpers::qVariantTypeId(serialized["date"]),      QMetaType::QDate);
    QCOMPARE(Helpers::qVariantTypeId(serialized["datetime"]),  QMetaType::QDateTime);
    QCOMPARE(Helpers::qVariantTypeId(serialized["timestamp"]), QMetaType::LongLong);

    // Restore
    type->resetCasts();
}

void tst_Model_Connection_Independent::toVector_WithCasts_DateNullVariants() const
{
    auto type = Type::find(3, {ID, "date", "datetime", "timestamp"});
    QVERIFY(type);
    QVERIFY(type->exists);

    type->mergeCasts({
        {"date",      CastType::QDate},
        {"datetime",  CastType::QDateTime},
        {"timestamp", CastType::Timestamp},
    });

    QVector<AttributeItem> serialized = type->toVector();
    QCOMPARE(serialized.size(), 4);

    const auto &serialized0 = serialized.at(0);
    QCOMPARE(serialized0.key, ID);
    QCOMPARE(serialized0.value.template value<typename Type::KeyType>(), 3);

    const auto &serialized1 = serialized.at(1);
    const auto &serialized2 = serialized.at(2);
    const auto &serialized3 = serialized.at(3);

    QCOMPARE(serialized1.key, "date");
    QCOMPARE(serialized2.key, "datetime");
    QCOMPARE(serialized3.key, "timestamp");

    QCOMPARE(Helpers::qVariantTypeId(serialized1.value), QMetaType::QDate);
    QCOMPARE(Helpers::qVariantTypeId(serialized2.value), QMetaType::QDateTime);
    QCOMPARE(Helpers::qVariantTypeId(serialized3.value), QMetaType::LongLong);

    // Restore
    type->resetCasts();
}

void
tst_Model_Connection_Independent::toMap_WithRelations_HasOne_HasMany_BelongsTo() const
{
    auto torrent = Torrent::with({"torrentPeer", "user", "torrentFiles"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVariantMap serialized = torrent->toMap();

    QVariantMap expectedAttributes {
        {"added_on",      "2020-08-07T20:11:10.000Z"},
        {CREATED_AT,      "2021-11-07T08:13:23.000Z"},
        {HASH_,           "7579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {ID,              7},
        {NAME,            "test7"},
        {NOTE,            "for serialization"},
        {"progress",      700},
        {SIZE_,           17},
        {"torrent_files", QVariant::fromValue(QVector<QVariantMap> {{
                              {CREATED_AT,    "2021-01-10T14:51:23.000Z"},
                              {"file_index",  0},
                              {"filepath",    "test7_file1.mkv"},
                              {ID,            10},
                              {NOTE,          "for serialization"},
                              {"progress",    512},
                              {SIZE_,         4562},
                              {"torrent_id",  7},
                              {UPDATED_AT,    "2021-01-10T17:46:31.000Z"},
                          }, {
                              {CREATED_AT,    "2021-01-11T14:51:23.000Z"},
                              {"file_index",  1},
                              {"filepath",    "test7_file2.mkv"},
                              {ID,            11},
                              {NOTE,          "for serialization"},
                              {"progress",    256},
                              {SIZE_,         2567},
                              {"torrent_id",  7},
                              {UPDATED_AT,    "2021-01-11T17:46:31.000Z"},
                          }, {
                              {CREATED_AT,    "2021-01-12T14:51:23.000Z"},
                              {"file_index",  2},
                              {"filepath",    "test7_file3.mkv"},
                              {ID,            12},
                              {NOTE,          "for serialization"},
                              {"progress",    768},
                              {SIZE_,         4279},
                              {"torrent_id",  7},
                              {UPDATED_AT,    "2021-01-12T17:46:31.000Z"},
                          }})},
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
        {UPDATED_AT,      "2021-01-07T18:46:31.000Z"},
        {"user",          QVariantMap {
                              {CREATED_AT,  "2022-01-02T14:51:23.000Z"},
                              {DELETED_AT,  NullVariant::QDateTime()},
                              {ID,          2},
                              {"is_banned", false},
                              {NAME,        "silver"},
                              {NOTE,        NullVariant::QString()},
                              {UPDATED_AT,  "2022-01-02T17:46:31.000Z"},
                          }},
        {"user_id",       2},
    };

    QCOMPARE(serialized, expectedAttributes);
}

void
tst_Model_Connection_Independent::toVector_WithRelations_HasOne_HasMany_BelongsTo() const
{
    auto torrent = Torrent::with({"torrentPeer", "user", "torrentFiles"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVector<AttributeItem> serialized = torrent->toVector();

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
        const auto it = std::ranges::find(serialized, "torrent_peer", keyProj);
        if (it == serialized.end())
            QFAIL("The \"torrent_peer\" key not found in the \"serialized\" result.");

        auto actualAttributes = serialized.takeAt(std::distance(serialized.begin(), it))
                                .value.value<QVector<AttributeItem>>();

        QVector<AttributeItem> expectedAttributes {
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
    // user
    {
        const auto it = std::ranges::find(serialized, "user", keyProj);
        if (it == serialized.end())
            QFAIL("The \"user\" key not found in the \"serialized\" result.");

        auto actualAttributes = serialized.takeAt(std::distance(serialized.begin(), it))
                                .value.value<QVector<AttributeItem>>();

        QVector<AttributeItem> expectedAttributes {
            {ID,          2},
            {NAME,        "silver"},
            {"is_banned", false},
            {NOTE,        NullVariant::QString()},
            {CREATED_AT,  "2022-01-02T14:51:23.000Z"},
            {UPDATED_AT,  "2022-01-02T17:46:31.000Z"},
            {DELETED_AT,  NullVariant::QDateTime()},
        };

        QCOMPARE(actualAttributes, expectedAttributes);
    }
    // torrent_files
    {
        const auto it = std::ranges::find(serialized, "torrent_files", keyProj);
        if (it == serialized.end())
            QFAIL("The \"torrent_files\" key not found in the \"serialized\" result.");

        auto actualAttributes = serialized.takeAt(std::distance(serialized.begin(), it))
                                .value.value<QVector<QVector<AttributeItem>>>();

        QVector<QVector<AttributeItem>> expectedAttributes {{
            {ID,            10},
            {"torrent_id",  7},
            {"file_index",  0},
            {"filepath",    "test7_file1.mkv"},
            {SIZE_,         4562},
            {"progress",    512},
            {NOTE,          "for serialization"},
            {CREATED_AT,    "2021-01-10T14:51:23.000Z"},
            {UPDATED_AT,    "2021-01-10T17:46:31.000Z"},
        }, {
            {ID,            11},
            {"torrent_id",  7},
            {"file_index",  1},
            {"filepath",    "test7_file2.mkv"},
            {SIZE_,         2567},
            {"progress",    256},
            {NOTE,          "for serialization"},
            {CREATED_AT,    "2021-01-11T14:51:23.000Z"},
            {UPDATED_AT,    "2021-01-11T17:46:31.000Z"},
        }, {
            {ID,            12},
            {"torrent_id",  7},
            {"file_index",  2},
            {"filepath",    "test7_file3.mkv"},
            {SIZE_,         4279},
            {"progress",    768},
            {NOTE,          "for serialization"},
            {CREATED_AT,    "2021-01-12T14:51:23.000Z"},
            {UPDATED_AT,    "2021-01-12T17:46:31.000Z"},
        }};

        QCOMPARE(actualAttributes, expectedAttributes);
    }
    // Compare the rest of attributes - torrent itself
    {
        QVector<AttributeItem> expectedAttributes {
            {ID,              7},
            {"user_id",       2},
            {NAME,            "test7"},
            {SIZE_,           17},
            {"progress",      700},
            {"added_on",      "2020-08-07T20:11:10.000Z"},
            {HASH_,           "7579e3af2768cdf52ec84c1f320333f68401dc6e"},
            {NOTE,            "for serialization"},
            {CREATED_AT,      "2021-11-07T08:13:23.000Z"},
            {UPDATED_AT,      "2021-01-07T18:46:31.000Z"},
        };

        QCOMPARE(serialized, expectedAttributes);
    }
}

void
tst_Model_Connection_Independent::toMap_WithRelation_BelongsToMany_TorrentTags() const
{
    auto torrent = Torrent::with("tags")->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVariantMap serialized = torrent->toMap();

    QVariantMap expectedAttributes {
        {"added_on", "2020-08-07T20:11:10.000Z"},
        {CREATED_AT, "2021-11-07T08:13:23.000Z"},
        {HASH_,      "7579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {ID,         7},
        {NAME,       "test7"},
        {NOTE,       "for serialization"},
        {"progress", 700},
        {SIZE_,      17},
        {"tags",     QVariant::fromValue(QVector<QVariantMap> {{
                         {CREATED_AT,     "2021-01-11T11:51:28.000Z"},
                         {"id",           1},
                         {NAME,           "tag1"},
                         {NOTE,           NullVariant::QString()},
                         {"tag_property", QVariantMap {
                                              {"color",    "white"},
                                              {CREATED_AT, "2021-02-11T12:41:28.000Z"},
                                              {ID,         1},
                                              {"position", 0},
                                              {"tag_id",   1},
                                              {UPDATED_AT, "2021-02-11T22:17:11.000Z"},
                                          }},
                         {"tagged",       QVariantMap {
                                              {"active",     true},
                                              {CREATED_AT,   "2021-03-01T17:31:58.000Z"},
                                              {"tag_id",     1},
                                              {"torrent_id", 7},
                                              {UPDATED_AT,   "2021-03-01T18:49:22.000Z"},
                                          }},
                         {UPDATED_AT,     "2021-01-11T23:47:11.000Z"},
                     }, {
                         {CREATED_AT,     "2021-01-12T11:51:28.000Z"},
                         {"id",           2},
                         {NAME,           "tag2"},
                         {NOTE,           NullVariant::QString()},
                         {"tag_property", QVariantMap {
                                              {"color",    "blue"},
                                              {CREATED_AT, "2021-02-12T12:41:28.000Z"},
                                              {ID,         2},
                                              {"position", 1},
                                              {"tag_id",   2},
                                              {UPDATED_AT, "2021-02-12T22:17:11.000Z"},
                                          }},
                         {"tagged",       QVariantMap {
                                              {"active",     true},
                                              {CREATED_AT,   "2021-03-02T17:31:58.000Z"},
                                              {"tag_id",     2},
                                              {"torrent_id", 7},
                                              {UPDATED_AT,   "2021-03-02T18:49:22.000Z"},
                                          }},
                         {UPDATED_AT,     "2021-01-12T23:47:11.000Z"},
                     }, {
                         {CREATED_AT,     "2021-01-13T11:51:28.000Z"},
                         {"id",           3},
                         {NAME,           "tag3"},
                         {NOTE,           NullVariant::QString()},
                         {"tag_property", QVariantMap {
                                              {"color",    "red"},
                                              {CREATED_AT, "2021-02-13T12:41:28.000Z"},
                                              {ID,         3},
                                              {"position", 2},
                                              {"tag_id",   3},
                                              {UPDATED_AT, "2021-02-13T22:17:11.000Z"},
                                          }},
                         {"tagged",       QVariantMap {
                                              {"active",     false},
                                              {CREATED_AT,   "2021-03-03T17:31:58.000Z"},
                                              {"tag_id",     3},
                                              {"torrent_id", 7},
                                              {UPDATED_AT,   "2021-03-03T18:49:22.000Z"},
                                          }},
                         {UPDATED_AT,     "2021-01-13T23:47:11.000Z"},
                     }})},
        {UPDATED_AT, "2021-01-07T18:46:31.000Z"},
        {"user_id",  2},
    };

    QCOMPARE(serialized, expectedAttributes);
}

void
tst_Model_Connection_Independent::toVector_WithRelation_BelongsToMany_TorrentTags() const
{
    auto torrent = Torrent::with("tags")->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVector<AttributeItem> serialized = torrent->toVector();

    // Verify
    /* Here we will have to compare all serialized relation attributes separately
       because the Model::m_relations is the std::unordered_map so the relations are
       serialized in random order. */
    // tag_property
    {
        const auto keyProj = [](const auto &attribute)
        {
            return attribute.key;
        };

        // First find the tags relationship vector
        auto itTags = std::ranges::find(serialized, "tags", keyProj);
        if (itTags == serialized.end())
            QFAIL("The \"tags\" key not found in the \"serialized\" result.");

        // Then extract the tag_property vectors from all tags
        /* I'm going to directly modify the tag vector using the QVariant::data(),
           it helps to avoid a lot of a junky code. */
        auto &tags = *reinterpret_cast<QVector<QVector<AttributeItem>> *>(
                             itTags->value.data());

        QVector<QVector<AttributeItem>> actualTagProperties;
        actualTagProperties.reserve(tags.size());

        for (auto &tag : tags) {
            const auto it = std::ranges::find(tag, "tag_property", keyProj);
            if (it == tag.end())
                QFAIL("The \"tag_property\" key not found in the \"tag\" result.");

            actualTagProperties << tag.takeAt(std::distance(tag.begin(), it))
                                .value.value<QVector<AttributeItem>>();
        }

        QVector<QVector<AttributeItem>> expectedTagProperties {{
            {ID,         1},
            {"tag_id",   1},
            {"color",    "white"},
            {"position", 0},
            {CREATED_AT, "2021-02-11T12:41:28.000Z"},
            {UPDATED_AT, "2021-02-11T22:17:11.000Z"},
        }, {
            {ID,         2},
            {"tag_id",   2},
            {"color",    "blue"},
            {"position", 1},
            {CREATED_AT, "2021-02-12T12:41:28.000Z"},
            {UPDATED_AT, "2021-02-12T22:17:11.000Z"},
        }, {
            {ID,         3},
            {"tag_id",   3},
            {"color",    "red"},
            {"position", 2},
            {CREATED_AT, "2021-02-13T12:41:28.000Z"},
            {UPDATED_AT, "2021-02-13T22:17:11.000Z"},
        }};

        QCOMPARE(actualTagProperties, expectedTagProperties);
    }

    // Compare the rest of attributes - they will already be in the right order
    QVector<AttributeItem> expectedAttributes {
        {ID,         7},
        {"user_id",  2},
        {NAME,       "test7"},
        {SIZE_,      17},
        {"progress", 700},
        {"added_on", "2020-08-07T20:11:10.000Z"},
        {HASH_,      "7579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {NOTE,       "for serialization"},
        {CREATED_AT, "2021-11-07T08:13:23.000Z"},
        {UPDATED_AT, "2021-01-07T18:46:31.000Z"},
        {"tags",     QVariant::fromValue(QVector<QVector<AttributeItem>> {{
                         {"id",           1},
                         {NAME,           "tag1"},
                         {NOTE,           NullVariant::QString()},
                         {CREATED_AT,     "2021-01-11T11:51:28.000Z"},
                         {UPDATED_AT,     "2021-01-11T23:47:11.000Z"},
                         {"tagged",       QVariant::fromValue(QVector<AttributeItem> {
                                              {"torrent_id", 7},
                                              {"tag_id",     1},
                                              {"active",     true},
                                              {CREATED_AT,   "2021-03-01T17:31:58.000Z"},
                                              {UPDATED_AT,   "2021-03-01T18:49:22.000Z"},
                                          })},
                     }, {
                         {"id",           2},
                         {NAME,           "tag2"},
                         {NOTE,           NullVariant::QString()},
                         {CREATED_AT,     "2021-01-12T11:51:28.000Z"},
                         {UPDATED_AT,     "2021-01-12T23:47:11.000Z"},
                         {"tagged",       QVariant::fromValue(QVector<AttributeItem> {
                                              {"torrent_id", 7},
                                              {"tag_id",     2},
                                              {"active",     true},
                                              {CREATED_AT,   "2021-03-02T17:31:58.000Z"},
                                              {UPDATED_AT,   "2021-03-02T18:49:22.000Z"},
                                          })},
                     }, {
                         {"id",           3},
                         {NAME,           "tag3"},
                         {NOTE,           NullVariant::QString()},
                         {CREATED_AT,     "2021-01-13T11:51:28.000Z"},
                         {UPDATED_AT,     "2021-01-13T23:47:11.000Z"},
                         {"tagged",       QVariant::fromValue(QVector<AttributeItem> {
                                              {"torrent_id", 7},
                                              {"tag_id",     3},
                                              {"active",     false},
                                              {CREATED_AT,   "2021-03-03T17:31:58.000Z"},
                                              {UPDATED_AT,   "2021-03-03T18:49:22.000Z"},
                                          })},
                     }})},
    };

    QCOMPARE(serialized, expectedAttributes);
}

// Following two unit tests test two belongs-to-many relations on one model

void tst_Model_Connection_Independent::
     toMap_WithRelation_BelongsToMany_TorrentTags_TorrentStates() const
{
    auto torrent = Torrent::with({"tags", "torrentStates"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVariantMap serialized = torrent->toMap();

    QVariantMap expectedAttributes {
        {"added_on", "2020-08-07T20:11:10.000Z"},
        {CREATED_AT, "2021-11-07T08:13:23.000Z"},
        {HASH_,      "7579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {ID,         7},
        {NAME,       "test7"},
        {NOTE,       "for serialization"},
        {"progress", 700},
        {SIZE_,      17},
        {"tags",     QVariant::fromValue(QVector<QVariantMap> {{
                         {CREATED_AT,     "2021-01-11T11:51:28.000Z"},
                         {"id",           1},
                         {NAME,           "tag1"},
                         {NOTE,           NullVariant::QString()},
                         {"tag_property", QVariantMap {
                                              {"color",    "white"},
                                              {CREATED_AT, "2021-02-11T12:41:28.000Z"},
                                              {ID,         1},
                                              {"position", 0},
                                              {"tag_id",   1},
                                              {UPDATED_AT, "2021-02-11T22:17:11.000Z"},
                                          }},
                         {"tagged",       QVariantMap {
                                              {"active",     true},
                                              {CREATED_AT,   "2021-03-01T17:31:58.000Z"},
                                              {"tag_id",     1},
                                              {"torrent_id", 7},
                                              {UPDATED_AT,   "2021-03-01T18:49:22.000Z"},
                                          }},
                         {UPDATED_AT,     "2021-01-11T23:47:11.000Z"},
                     },{
                         {CREATED_AT,     "2021-01-12T11:51:28.000Z"},
                         {"id",           2},
                         {NAME,           "tag2"},
                         {NOTE,           NullVariant::QString()},
                         {"tag_property", QVariantMap {
                                              {"color",    "blue"},
                                              {CREATED_AT, "2021-02-12T12:41:28.000Z"},
                                              {ID,         2},
                                              {"position", 1},
                                              {"tag_id",   2},
                                              {UPDATED_AT, "2021-02-12T22:17:11.000Z"},
                                          }},
                         {"tagged",       QVariantMap {
                                              {"active",     true},
                                              {CREATED_AT,   "2021-03-02T17:31:58.000Z"},
                                              {"tag_id",     2},
                                              {"torrent_id", 7},
                                              {UPDATED_AT,   "2021-03-02T18:49:22.000Z"},
                                          }},
                         {UPDATED_AT,     "2021-01-12T23:47:11.000Z"},
                     }, {
                         {CREATED_AT,     "2021-01-13T11:51:28.000Z"},
                         {"id",           3},
                         {NAME,           "tag3"},
                         {NOTE,           NullVariant::QString()},
                         {"tag_property", QVariantMap {
                                              {"color",    "red"},
                                              {CREATED_AT, "2021-02-13T12:41:28.000Z"},
                                              {ID,         3},
                                              {"position", 2},
                                              {"tag_id",   3},
                                              {UPDATED_AT, "2021-02-13T22:17:11.000Z"},
                                          }},
                         {"tagged",       QVariantMap {
                                              {"active",     false},
                                              {CREATED_AT,   "2021-03-03T17:31:58.000Z"},
                                              {"tag_id",     3},
                                              {"torrent_id", 7},
                                              {UPDATED_AT,   "2021-03-03T18:49:22.000Z"},
                                          }},
                         {UPDATED_AT,     "2021-01-13T23:47:11.000Z"},
                     }})},

        {"torrent_states", QVariant::fromValue(QVector<QVariantMap> {{
                               {ID,     1},
                               {NAME,   "Active"},
                               {pivot_, QVariantMap {
                                            {"active",     1},
                                            {"state_id",   1},
                                            {"torrent_id", 7},
                                        }},
                           }, {
                               {ID,     4},
                               {NAME,   "Downloading"},
                               {pivot_, QVariantMap {
                                            {"active",     0},
                                            {"state_id",   4},
                                            {"torrent_id", 7},
                                        }},
                           }})},

        {UPDATED_AT,      "2021-01-07T18:46:31.000Z"},
        {"user_id",       2},
    };

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::
     toVector_WithRelation_BelongsToMany_TorrentTags_TorrentStates() const
{
    auto torrent = Torrent::with({"tags", "torrentStates"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVector<AttributeItem> serialized = torrent->toVector();

    // Verify
    /* Here we will have to compare all serialized relation attributes separately
       because the Model::m_relations is the std::unordered_map so the relations are
       serialized in random order. */

    const auto keyProj = [](const auto &attribute)
    {
        return attribute.key;
    };

    // tag_property
    {
        // First find the tags relationship vector
        auto itTags = std::ranges::find(serialized, "tags", keyProj);
        if (itTags == serialized.end())
            QFAIL("The \"tags\" key not found in the \"serialized\" result.");

        // Then extract the tag_property vectors from all tags
        /* I'm going to directly modify the tag vector using the QVariant::data(),
           it helps to avoid a lot of a junky code. */
        auto &tags = *reinterpret_cast<QVector<QVector<AttributeItem>> *>(
                             itTags->value.data());

        QVector<QVector<AttributeItem>> actualTagProperties;
        actualTagProperties.reserve(tags.size());

        for (auto &tag : tags) {
            const auto it = std::ranges::find(tag, "tag_property", keyProj);
            if (it == tag.end())
                QFAIL("The \"tag_property\" key not found in the \"tag\" result.");

            actualTagProperties << tag.takeAt(std::distance(tag.begin(), it))
                                .value.value<QVector<AttributeItem>>();
        }

        QVector<QVector<AttributeItem>> expectedTagProperties {{
            {ID,         1},
            {"tag_id",   1},
            {"color",    "white"},
            {"position", 0},
            {CREATED_AT, "2021-02-11T12:41:28.000Z"},
            {UPDATED_AT, "2021-02-11T22:17:11.000Z"},
        }, {
            {ID,         2},
            {"tag_id",   2},
            {"color",    "blue"},
            {"position", 1},
            {CREATED_AT, "2021-02-12T12:41:28.000Z"},
            {UPDATED_AT, "2021-02-12T22:17:11.000Z"},
        }, {
            {ID,         3},
            {"tag_id",   3},
            {"color",    "red"},
            {"position", 2},
            {CREATED_AT, "2021-02-13T12:41:28.000Z"},
            {UPDATED_AT, "2021-02-13T22:17:11.000Z"},
        }};
        QCOMPARE(actualTagProperties, expectedTagProperties);
    }

    // torrent_states
    {
        const auto it = std::ranges::find(serialized, "torrent_states", keyProj);
        if (it == serialized.end())
            QFAIL("The \"torrent_states\" key not found in the \"serialized\" result.");

        auto actualStates = serialized.takeAt(std::distance(serialized.begin(), it))
                            .value.value<QVector<QVector<AttributeItem>>>();

        QVector<QVector<AttributeItem>> expectedStates {{
            {ID,     1},
            {NAME,   "Active"},
            {pivot_, QVariant::fromValue(QVector<AttributeItem> {
                         {"torrent_id", 7},
                         {"state_id",   1},
                         {"active",     1},
                     })},
        }, {
            {ID,     4},
            {NAME,   "Downloading"},
            {pivot_, QVariant::fromValue(QVector<AttributeItem> {
                         {"torrent_id", 7},
                         {"state_id",   4},
                         {"active",     0},
                     })}
        }};

        QCOMPARE(actualStates, expectedStates);
    }

    QVector<AttributeItem> expectedAttributes {
        {ID,         7},
        {"user_id",  2},
        {NAME,       "test7"},
        {SIZE_,      17},
        {"progress", 700},
        {"added_on", "2020-08-07T20:11:10.000Z"},
        {HASH_,      "7579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {NOTE,       "for serialization"},
        {CREATED_AT, "2021-11-07T08:13:23.000Z"},
        {UPDATED_AT, "2021-01-07T18:46:31.000Z"},
        {"tags",     QVariant::fromValue(QVector<QVector<AttributeItem>> {{
                         {"id",           1},
                         {NAME,           "tag1"},
                         {NOTE,           NullVariant::QString()},
                         {CREATED_AT,     "2021-01-11T11:51:28.000Z"},
                         {UPDATED_AT,     "2021-01-11T23:47:11.000Z"},
                         {"tagged",       QVariant::fromValue(QVector<AttributeItem> {
                                              {"torrent_id", 7},
                                              {"tag_id",     1},
                                              {"active",     true},
                                              {CREATED_AT,   "2021-03-01T17:31:58.000Z"},
                                              {UPDATED_AT,   "2021-03-01T18:49:22.000Z"},
                                          })},
                     }, {
                         {"id",           2},
                         {NAME,           "tag2"},
                         {NOTE,           NullVariant::QString()},
                         {CREATED_AT,     "2021-01-12T11:51:28.000Z"},
                         {UPDATED_AT,     "2021-01-12T23:47:11.000Z"},
                         {"tagged",       QVariant::fromValue(QVector<AttributeItem> {
                                              {"torrent_id", 7},
                                              {"tag_id",     2},
                                              {"active",     true},
                                              {CREATED_AT,   "2021-03-02T17:31:58.000Z"},
                                              {UPDATED_AT,   "2021-03-02T18:49:22.000Z"},
                                          })},
                     }, {
                         {"id",           3},
                         {NAME,           "tag3"},
                         {NOTE,           NullVariant::QString()},
                         {CREATED_AT,     "2021-01-13T11:51:28.000Z"},
                         {UPDATED_AT,     "2021-01-13T23:47:11.000Z"},
                         {"tagged",       QVariant::fromValue(QVector<AttributeItem> {
                                              {"torrent_id", 7},
                                              {"tag_id",     3},
                                              {"active",     false},
                                              {CREATED_AT,   "2021-03-03T17:31:58.000Z"},
                                              {UPDATED_AT,   "2021-03-03T18:49:22.000Z"},
                                          })},
                     }})},

    };

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toMap_WithRelation_BelongsToMany_UserRoles() const
{
    auto user = User::with("roles")->find(1);
    QVERIFY(user);
    QVERIFY(user->exists);

    QVariantMap serialized = user->toMap();

    QVariantMap expectedAttributes {
        {CREATED_AT,  "2022-01-01T14:51:23.000Z"},
        {DELETED_AT,  NullVariant::QDateTime()},
        {ID,          1},
        {"is_banned", false},
        {NAME,        "andrej"},
        {NOTE,        NullVariant::QString()},
        {"roles",     QVariant::fromValue(QVector<QVariantMap> {{
                          {ID,             1},
                          {NAME,           "role one"},
                          {"added_on",     "2022-08-01T13:36:56.000Z"},
                          {"subscription", QVariantMap {
                                               {"active",  true},
                                               {"role_id", 1},
                                               {"user_id", 1},
                                           }},
                      }, {
                          {ID,             2},
                          {NAME,           "role two"},
                          {"added_on",     "2022-08-02T13:36:56.000Z"},
                          {"subscription", QVariantMap {
                                               {"active",  false},
                                               {"role_id", 2},
                                               {"user_id", 1},
                                           }},
                      }, {
                          {ID,             3},
                          {NAME,           "role three"},
                          {"added_on",     NullVariant::QDateTime()},
                          {"subscription", QVariantMap {
                                               {"active",  true},
                                               {"role_id", 3},
                                               {"user_id", 1},
                                           }},
                      }})},
        {UPDATED_AT,  "2022-01-01T17:46:31.000Z"},
    };

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toVector_WithRelation_BelongsToMany_UserRoles() const
{
    auto user = User::with("roles")->find(1);
    QVERIFY(user);
    QVERIFY(user->exists);

    QVector<AttributeItem> serialized = user->toVector();

    QVector<AttributeItem> expectedAttributes {
        {ID,          1},
        {NAME,        "andrej"},
        {"is_banned", false},
        {NOTE,        NullVariant::QString()},
        {CREATED_AT,  "2022-01-01T14:51:23.000Z"},
        {UPDATED_AT,  "2022-01-01T17:46:31.000Z"},
        {DELETED_AT,  NullVariant::QDateTime()},
        {"roles",     QVariant::fromValue(QVector<QVector<AttributeItem>> {{
                          {ID,             1},
                          {NAME,           "role one"},
                          {"added_on",     "2022-08-01T13:36:56.000Z"},
                          {"subscription", QVariant::fromValue(QVector<AttributeItem> {
                                               {"user_id", 1},
                                               {"role_id", 1},
                                               {"active",  true},
                                           })},
                      }, {
                          {ID,             2},
                          {NAME,           "role two"},
                          {"added_on",     "2022-08-02T13:36:56.000Z"},
                          {"subscription", QVariant::fromValue(QVector<AttributeItem> {
                                               {"user_id", 1},
                                               {"role_id", 2},
                                               {"active",  false},
                                           })},
                      }, {
                          {ID,             3},
                          {NAME,           "role three"},
                          {"added_on",     NullVariant::QDateTime()},
                          {"subscription", QVariant::fromValue(QVector<AttributeItem> {
                                               {"user_id", 1},
                                               {"role_id", 3},
                                               {"active",  true},
                                           })},
                      }})},
    };

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toMap_u_snakeAttributes_false() const
{
    auto album = Album::find(1);
    QVERIFY(album);
    QVERIFY(album->exists);

    QVariantMap serialized = album->toMap();

    QVariantMap expectedAttributes {
        {"albumImages", QVariant::fromValue(QVector<QVariantMap> {{
                            {"album_id", 1},
                            {CREATED_AT, "2023-03-01T15:24:37.000Z"},
                            {"ext",      "png"},
                            {ID,         1},
                            {NAME,       "album1_image1"},
                            {SIZE_,      726},
                            {UPDATED_AT, "2023-04-01T14:35:47.000Z"},
                        }})},
        {CREATED_AT,    "2023-01-01T12:21:14.000Z"},
        {ID,            1},
        {NAME,          "album1"},
        {NOTE,          NullVariant::QString()},
        {UPDATED_AT,    "2023-02-01T16:54:28.000Z"},
    };

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toVector_u_snakeAttributes_false() const
{
    auto album = Album::find(1);
    QVERIFY(album);
    QVERIFY(album->exists);

    QVector<AttributeItem> serialized = album->toVector();

    QVector<AttributeItem> expectedAttributes {
        {ID,            1},
        {NAME,          "album1"},
        {NOTE,          NullVariant::QString()},
        {CREATED_AT,    "2023-01-01T12:21:14.000Z"},
        {UPDATED_AT,    "2023-02-01T16:54:28.000Z"},
        {"albumImages", QVariant::fromValue(QVector<QVector<AttributeItem>> {{
                            {ID,         1},
                            {"album_id", 1},
                            {NAME,       "album1_image1"},
                            {"ext",      "png"},
                            {SIZE_,      726},
                            {CREATED_AT, "2023-03-01T15:24:37.000Z"},
                            {UPDATED_AT, "2023-04-01T14:35:47.000Z"},
                        }})},
    };

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toMap_HasMany_EmptyRelation() const
{
    auto albums = Album::findMany({3, 4});
    QCOMPARE(albums.size(), 2);
    QCOMPARE(typeid (albums), typeid (ModelsCollection<Album>));

    QVector<QVariantMap> serialized = albums.toMap();

    QVector<QVariantMap> expectedAttributes {{
        {"albumImages", QVariant::fromValue(QVector<QVariantMap> {{
                            {"album_id", 3},
                            {CREATED_AT, "2023-03-07T15:24:37.000Z"},
                            {"ext",      "jpg"},
                            {ID,         7},
                            {NAME,       "album3_image1"},
                            {SIZE_,      718},
                            {UPDATED_AT, "2023-04-07T14:35:47.000Z"},
                        }})},
        {CREATED_AT,    "2023-01-03T12:21:14.000Z"},
        {ID,            3},
        {NAME,          "album3"},
        {NOTE,          "album3 note"},
        {UPDATED_AT,    "2023-02-03T16:54:28.000Z"},
    }, {
        {"albumImages", QVariant::fromValue(QVector<QVariantMap> {})},
        {CREATED_AT,    "2023-01-04T12:21:14.000Z"},
        {ID,            4},
        {NAME,          "album4"},
        {NOTE,          "no images"},
        {UPDATED_AT,    "2023-02-04T16:54:28.000Z"},
    }};

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toVector_HasMany_EmptyRelation() const
{
    auto albums = Album::findMany({3, 4});
    QCOMPARE(albums.size(), 2);
    QCOMPARE(typeid (albums), typeid (ModelsCollection<Album>));

    QVector<QVector<AttributeItem>> serialized = albums.toVector();

    QVector<QVector<AttributeItem>> expectedAttributes {{
        {ID,            3},
        {NAME,          "album3"},
        {NOTE,          "album3 note"},
        {CREATED_AT,    "2023-01-03T12:21:14.000Z"},
        {UPDATED_AT,    "2023-02-03T16:54:28.000Z"},
        {"albumImages", QVariant::fromValue(QVector<QVector<AttributeItem>> {{
                            {ID,         7},
                            {"album_id", 3},
                            {NAME,       "album3_image1"},
                            {"ext",      "jpg"},
                            {SIZE_,      718},
                            {CREATED_AT, "2023-03-07T15:24:37.000Z"},
                            {UPDATED_AT, "2023-04-07T14:35:47.000Z"},
                        }})},
    }, {
        {ID,            4},
        {NAME,          "album4"},
        {NOTE,          "no images"},
        {CREATED_AT,    "2023-01-04T12:21:14.000Z"},
        {UPDATED_AT,    "2023-02-04T16:54:28.000Z"},
        {"albumImages", QVariant::fromValue(QVector<QVector<AttributeItem>> {})},
    }};

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toMap_HasOne_EmptyRelation() const
{
    auto torrents = Torrent::with("torrentPeer")->findMany({6, 7});
    QCOMPARE(torrents.size(), 2);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent>));

    QVector<QVariantMap> serialized = torrents.toMap();

    QVector<QVariantMap> expectedAttributes {{
        {"added_on",     "2020-08-06T20:11:10.000Z"},
        {CREATED_AT,     "2021-11-06T08:13:23.000Z"},
        {HASH_,          "6579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {ID,             6},
        {NAME,           "test6"},
        {NOTE,           "no files no peers"},
        {"progress",     600},
        {SIZE_,          16},
        {"torrent_peer", QVariant::fromValue(nullptr)},
        {UPDATED_AT,     "2021-01-06T18:46:31.000Z"},
        {"user_id",      2},
    }, {
        {"added_on",     "2020-08-07T20:11:10.000Z"},
        {CREATED_AT,     "2021-11-07T08:13:23.000Z"},
        {HASH_,          "7579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {ID,             7},
        {NAME,           "test7"},
        {NOTE,           "for serialization"},
        {"progress",     700},
        {SIZE_,          17},
        {"torrent_peer", QVariantMap {
                             {CREATED_AT,       "2021-01-07T14:51:23.000Z"},
                             {ID,               5},
                             {"leechers",       7},
                             {"seeds",          NullVariant::Int()},
                             {"torrent_id",     7},
                             {"total_leechers", 7},
                             {"total_seeds",    7},
                             {UPDATED_AT,       "2021-01-07T17:46:31.000Z"},
                         }},
        {UPDATED_AT,     "2021-01-07T18:46:31.000Z"},
        {"user_id",      2},
    }};

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toVector_HasOne_EmptyRelation() const
{
    auto torrents = Torrent::with("torrentPeer")->findMany({6, 7});
    QCOMPARE(torrents.size(), 2);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent>));

    QVector<QVector<AttributeItem>> serialized = torrents.toVector();

    QVector<QVector<AttributeItem>> expectedAttributes {{
        {ID,             6},
        {"user_id",      2},
        {NAME,           "test6"},
        {SIZE_,          16},
        {"progress",     600},
        {"added_on",     "2020-08-06T20:11:10.000Z"},
        {HASH_,          "6579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {NOTE,           "no files no peers"},
        {CREATED_AT,     "2021-11-06T08:13:23.000Z"},
        {UPDATED_AT,     "2021-01-06T18:46:31.000Z"},
        {"torrent_peer", QVariant::fromValue(nullptr)},
    }, {
        {ID,             7},
        {"user_id",      2},
        {NAME,           "test7"},
        {SIZE_,          17},
        {"progress",     700},
        {"added_on",     "2020-08-07T20:11:10.000Z"},
        {HASH_,          "7579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {NOTE,           "for serialization"},
        {CREATED_AT,     "2021-11-07T08:13:23.000Z"},
        {UPDATED_AT,     "2021-01-07T18:46:31.000Z"},
        {"torrent_peer", QVariant::fromValue(QVector<AttributeItem> {
                             {ID,               5},
                             {"torrent_id",     7},
                             {"seeds",          NullVariant::Int()},
                             {"total_seeds",    7},
                             {"leechers",       7},
                             {"total_leechers", 7},
                             {CREATED_AT,       "2021-01-07T14:51:23.000Z"},
                             {UPDATED_AT,       "2021-01-07T17:46:31.000Z"},
                         })},
    }};

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toMap_BelongsTo_EmptyRelation() const
{
    auto torrentPeers = TorrentPeer::with("torrent")->findMany({5, 6});
    QCOMPARE(torrentPeers.size(), 2);
    QCOMPARE(typeid (torrentPeers), typeid (ModelsCollection<TorrentPeer>));

    QVector<QVariantMap> serialized = torrentPeers.toMap();

    QVector<QVariantMap> expectedAttributes {{
        {CREATED_AT,       "2021-01-07T14:51:23.000Z"},
        {ID,               5},
        {"leechers",       7},
        {"seeds",          NullVariant::Int()},
        {"torrent",        QVariantMap {
                               {"added_on", "2020-08-07T20:11:10.000Z"},
                               {CREATED_AT, "2021-11-07T08:13:23.000Z"},
                               {HASH_,      "7579e3af2768cdf52ec84c1f320333f68401dc6e"},
                               {ID,         7},
                               {NAME,       "test7"},
                               {NOTE,       "for serialization"},
                               {"progress", 700},
                               {SIZE_,      17},
                               {UPDATED_AT, "2021-01-07T18:46:31.000Z"},
                               {"user_id",  2},
                           }},
        {"torrent_id",     7},
        {"total_leechers", 7},
        {"total_seeds",    7},
        {UPDATED_AT,       "2021-01-07T17:46:31.000Z"},
    }, {
        {CREATED_AT,       "2021-01-06T14:51:23.000Z"},
        {ID,               6},
        {"leechers",       6},
        {"seeds",          NullVariant::Int()},
        {"torrent",        QVariant::fromValue(nullptr)},
        {"torrent_id",     NullVariant::ULongLong()},
        {"total_leechers", 6},
        {"total_seeds",    6},
        {UPDATED_AT,       "2021-01-06T17:46:31.000Z"},
    }};

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Connection_Independent::toVector_BelongsTo_EmptyRelation() const
{
    auto torrentPeers = TorrentPeer::with("torrent")->findMany({5, 6});
    QCOMPARE(torrentPeers.size(), 2);
    QCOMPARE(typeid (torrentPeers), typeid (ModelsCollection<TorrentPeer>));

    QVector<QVector<AttributeItem>> serialized = torrentPeers.toVector();

    QVector<QVector<AttributeItem>> expectedAttributes {{
        {ID,               5},
        {"torrent_id",     7},
        {"seeds",          NullVariant::Int()},
        {"total_seeds",    7},
        {"leechers",       7},
        {"total_leechers", 7},
        {CREATED_AT,       "2021-01-07T14:51:23.000Z"},
        {UPDATED_AT,       "2021-01-07T17:46:31.000Z"},
        {"torrent",        QVariant::fromValue(QVector<AttributeItem> {
                               {ID,         7},
                               {"user_id",  2},
                               {NAME,       "test7"},
                               {SIZE_,      17},
                               {"progress", 700},
                               {"added_on", "2020-08-07T20:11:10.000Z"},
                               {HASH_,      "7579e3af2768cdf52ec84c1f320333f68401dc6e"},
                               {NOTE,       "for serialization"},
                               {CREATED_AT, "2021-11-07T08:13:23.000Z"},
                               {UPDATED_AT, "2021-01-07T18:46:31.000Z"},
                           })},
    }, {
        {ID,               6},
        {"torrent_id",     NullVariant::ULongLong()},
        {"seeds",          NullVariant::Int()},
        {"total_seeds",    6},
        {"leechers",       6},
        {"total_leechers", 6},
        {CREATED_AT,       "2021-01-06T14:51:23.000Z"},
        {UPDATED_AT,       "2021-01-06T17:46:31.000Z"},
        {"torrent",        QVariant::fromValue(nullptr)},
    }};

    QCOMPARE(serialized, expectedAttributes);
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_Model_Connection_Independent)

#include "tst_model_connection_independent.moc"
