#include <QCoreApplication>
#include <QTest>

#include "databases.hpp"
#include "macros.hpp"

#include "models/album.hpp"
#include "models/datetime.hpp"
#include "models/datetime_serializeoverride.hpp"
#include "models/torrent.hpp"
#include "models/type.hpp"

using Orm::Constants::CREATED_AT;
using Orm::Constants::DELETED_AT;
using Orm::Constants::HASH_;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;
using Orm::Constants::UPDATED_AT;
using Orm::Constants::pivot_;

using Orm::TTimeZone;
using Orm::Utils::NullVariant;

using TypeUtils = Orm::Utils::Type;

using Orm::Tiny::AttributeItem;
using Orm::Tiny::CastType;
using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::Types::ModelsCollection;

using TestUtils::Databases;

using Models::Album;
using Models::Datetime;
using Models::Datetime_SerializeOverride;
using Models::Role;
using Models::RoleUser;
using Models::Torrent;
using Models::TorrentPeer;
using Models::TorrentPreviewableFile;
using Models::Type;
using Models::User;

class tst_Model_Serialization : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase() const;

    /* Serialization */
    void toMap() const;
    void toList() const;

    void toMap_WithCasts() const;
    void toList_WithCasts() const;

    void toMap_UDatesOnly_QDateTime_For_date_column() const;
    void toList_UDatesOnly_QDateTime_For_date_column() const;

    void toMap_UDatesOnly_QDate_For_date_column() const;
    void toList_UDatesOnly_QDate_For_date_column() const;

    void toMap_UDatesOnly_Timestamp() const;
    void toList_UDatesOnly_Timestamp() const;

    void toMap_WithDateModfiers() const;
    void toList_WithDateModfiers() const;

    void toMap_WithDateModfiers_UnixTimestamp() const;
    void toList_WithDateModfiers_UnixTimestamp() const;

    void toMap_UDatesOnly_OverrideSerializeDateTime() const;
    void toList_UDatesOnly_OverrideSerializeDateTime() const;

    void toMap_CastsOnly_OverrideSerializeDateTime() const;
    void toList_CastsOnly_OverrideSerializeDateTime() const;

    void toMap_WithUDatesAndCasts_OverrideSerializeDateTime() const;
    void toList_WithUDatesAndCasts_OverrideSerializeDateTime() const;

    void toMap_CastsOnly_WithDateModfiers_OverrideSerializeDateTime() const;
    void toList_CastsOnly_WithDateModfiers_OverrideSerializeDateTime() const;

    void toMap_UDatesOnly_DateNullVariants() const;
    void toList_UDatesOnly_DateNullVariants() const;

    void toMap_WithCasts_DateNullVariants() const;
    void toList_WithCasts_DateNullVariants() const;

    void toMap_WithRelations_HasOne_HasMany_BelongsTo() const;
    void toList_WithRelations_HasOne_HasMany_BelongsTo() const;

    void toMap_WithRelation_BelongsToMany_TorrentTags() const;
    void toList_WithRelation_BelongsToMany_TorrentTags() const;

    void toMap_WithRelation_BelongsToMany_TorrentTags_TorrentStates() const;
    void toList_WithRelation_BelongsToMany_TorrentTags_TorrentStates() const;

    void toMap_WithRelation_BelongsToMany_UserRoles() const;
    void toList_WithRelation_BelongsToMany_UserRoles() const;

    void toMap_u_snakeAttributes_false() const;
    void toList_u_snakeAttributes_false() const;

    void toMap_HasMany_EmptyRelation() const;
    void toList_HasMany_EmptyRelation() const;

    void toMap_HasOne_EmptyRelation() const;
    void toList_HasOne_EmptyRelation() const;

    void toMap_BelongsTo_EmptyRelation() const;
    void toList_BelongsTo_EmptyRelation() const;

    void toMap_BelongsToMany_EmptyRelation() const;
    void toList_BelongsToMany_EmptyRelation() const;

    void toMap_RelationOnly_HasMany() const;
    void toList_RelationOnly_HasMany() const;
    void toMap_RelationOnly_BelongsToMany() const;
    void toList_RelationOnly_BelongsToMany() const;

    void toJson_WithRelations_HasOne_HasMany_BelongsTo() const;
    void toJson_WithRelation_BelongsToMany_TorrentTags() const;
    void toJson_WithRelation_BelongsToMany_TorrentTags_TorrentStates() const;
    void toJson_WithRelation_BelongsToMany_UserRoles() const;

    void toJson_HasMany_EmptyRelation() const;
    void toJson_HasOne_EmptyRelation() const;
    void toJson_BelongsTo_EmptyRelation() const;
    void toJson_BelongsToMany_EmptyRelation() const;

    void toJson_u_snakeAttributes_false() const;
    void toJson_WithDateModfiers_UnixTimestamp() const;
    void toJson_NullQVariant() const;

    void toJson_RelationOnly_HasMany() const;
    void toJson_RelationOnly_BelongsToMany() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Connection name used in this test case. */
    QString m_connection;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Model_Serialization::initTestCase()
{
    ConnectionOverride::connection = m_connection =
            Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );
}

void tst_Model_Serialization::cleanupTestCase() const
{
    // Reset connection override
    ConnectionOverride::connection.clear();
}

/* Serialization */

void tst_Model_Serialization::toMap() const
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
        {Progress,   400},
        {SIZE_,      14},
        {UPDATED_AT, "2021-01-04T18:46:31.000Z"},
        {"user_id",  1},
    };
    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::toList() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QList<AttributeItem> serialized = torrent->toList();
    QCOMPARE(serialized.size(), 10);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {ID,         4},
        {"user_id",  1},
        {NAME,       "test4"},
        {SIZE_,      14},
        {Progress,   400},
        {"added_on", "2020-08-04T20:11:10.000Z"},
        {HASH_,      "4579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {NOTE,       "after update revert updated_at"},
        {CREATED_AT, "2019-09-04T08:11:23.000Z"},
        {UPDATED_AT, "2021-01-04T18:46:31.000Z"},
    };
    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::toMap_WithCasts() const
{
    auto datetime = Datetime::instance({
        {"datetime",  QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
        {"date",      QDateTime({2023, 05, 14}, {10, 11, 12}, TTimeZone::UTC)},
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

void tst_Model_Serialization::toList_WithCasts() const
{
    auto datetime = Datetime::instance({
        {"datetime",  QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
        {"date",      QDateTime({2023, 05, 14}, {10, 11, 12}, TTimeZone::UTC)},
        {"timestamp", static_cast<qint64>(1662712888)},
    });

    datetime.mergeCasts({{"datetime",  CastType::QDateTime},
                         {"date",      CastType::QDate},
                         {"timestamp", CastType::Timestamp}});

    QList<AttributeItem> serialized = datetime.toList();
    QCOMPARE(serialized.size(), 3);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {"datetime",  "2023-05-13T10:11:12.000Z"},
        {"date",      "2023-05-14"},
        {"timestamp", static_cast<qint64>(1662712888)},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    datetime.resetCasts();
}

void tst_Model_Serialization::toMap_UDatesOnly_QDateTime_For_date_column() const
{
    auto datetime = Datetime::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, TTimeZone::UTC)},
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
tst_Model_Serialization::toList_UDatesOnly_QDateTime_For_date_column() const
{
    auto datetime = Datetime::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, TTimeZone::UTC)},
    });

    QList<AttributeItem> serialized = datetime.toList();
    QCOMPARE(serialized.size(), 2);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {"datetime", "2023-05-13T10:11:12.000Z"},
        {"date",     "2023-05-14T10:11:12.000Z"},
    };
    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::toMap_UDatesOnly_QDate_For_date_column() const
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

void tst_Model_Serialization::toList_UDatesOnly_QDate_For_date_column() const
{
    auto datetime = Datetime::instance({
        {"date", QDate(2023, 05, 14)},
    });

    QList<AttributeItem> serialized = datetime.toList();
    QCOMPARE(serialized.size(), 1);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {"date", "2023-05-14T00:00:00.000Z"},
    };
    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::toMap_UDatesOnly_Timestamp() const
{
    auto datetime = Datetime::instance(QList<AttributeItem> {
        {"timestamp", static_cast<qint64>(1662712888)},
    });

    QVariantMap serialized = datetime.toMap();
    QCOMPARE(serialized.size(), 1);

    QVariantMap expectedAttributes {
        {"timestamp", "2022-09-09T08:41:28.000Z"},
    };
    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::toList_UDatesOnly_Timestamp() const
{
    auto datetime = Datetime::instance(QList<AttributeItem> {
        {"timestamp", static_cast<qint64>(1662712888)},
    });

    QList<AttributeItem> serialized = datetime.toList();
    QCOMPARE(serialized.size(), 1);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {"timestamp", "2022-09-09T08:41:28.000Z"},
    };
    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::toMap_WithDateModfiers() const
{
    // Prepare
    auto timeFormatOriginal = Datetime::u_timeFormat;
    Datetime::u_timeFormat = sl("HH:mm:ss.zzz");

    auto datetime = Datetime::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, TTimeZone::UTC)},
        {"time",     QTime(14, 11, 15)}, // It must also accept QTime() instances
        {"time_ms",  QTime(14, 11, 15, 123)}, // It must also accept QTime() instances
    });

    datetime.mergeCasts({
        {"datetime", {CastType::CustomQDateTime, "dd.MM.yyyy HH:mm:ss.z t"}},
        {"date",     {CastType::CustomQDate,     "dd.MM.yyyy"}},
        {"time",     {CastType::CustomQTime,     "HH:mm:ss.z"}},
        {"time_ms",  {CastType::CustomQTime,     "HH:mm:ss.zzz"}},
    });

    QVariantMap serialized = datetime.toMap();
    QCOMPARE(serialized.size(), 4);

    // The order must be the same as returned from the MySQL database
    QVariantMap expectedAttributes {
        {"date",     "14.05.2023"},
        {"datetime", "13.05.2023 10:11:12.0 UTC"},
        {"time",     "14:11:15.0"},
        {"time_ms",  "14:11:15.123"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    datetime.resetCasts();
    Datetime::u_timeFormat = std::move(timeFormatOriginal);
}

void tst_Model_Serialization::toList_WithDateModfiers() const
{
    auto datetime = Datetime::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, TTimeZone::UTC)},
        {"time",     sl("14:11:15")},
        {"time_ms",  sl("14:11:15.12")},
    });

    datetime.mergeCasts({
        {"datetime", {CastType::CustomQDateTime, "dd.MM.yyyy HH:mm:ss.z t"}},
        {"date",     {CastType::CustomQDate,     "dd.MM.yyyy"}},
        {"time",     {CastType::CustomQTime,     "HH:mm:ss"}},
        {"time_ms",  {CastType::CustomQTime,     "HH:mm:ss.zzz"}},
    });

    QList<AttributeItem> serialized = datetime.toList();
    QCOMPARE(serialized.size(), 4);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {"datetime", "13.05.2023 10:11:12.0 UTC"},
        {"date",     "14.05.2023"},
        {"time",     "14:11:15"},
        {"time_ms",  "14:11:15.120"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    datetime.resetCasts();
}

void tst_Model_Serialization::toMap_WithDateModfiers_UnixTimestamp() const
{
    auto datetime = Datetime::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
    });

    datetime.mergeCasts({
        {"datetime", {CastType::CustomQDateTime, "U"}},
    });

    QVariantMap serialized = datetime.toMap();
    QCOMPARE(serialized.size(), 1);

    // The order must be the same as returned from the MySQL database
    QVariantMap expectedAttributes {
        {"datetime", static_cast<qint64>(1683972672)},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    datetime.resetCasts();
}

void tst_Model_Serialization::toList_WithDateModfiers_UnixTimestamp() const
{
    auto datetime = Datetime::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
    });

    datetime.mergeCasts({
        {"datetime", {CastType::CustomQDateTime, "U"}},
    });

    QList<AttributeItem> serialized = datetime.toList();
    QCOMPARE(serialized.size(), 1);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {"datetime", static_cast<qint64>(1683972672)},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    datetime.resetCasts();
}

void tst_Model_Serialization::toMap_UDatesOnly_OverrideSerializeDateTime() const
{
    auto datetime = Datetime_SerializeOverride::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, TTimeZone::UTC)},
    });

    Datetime_SerializeOverride::u_dates = {"date", "datetime"};

    QVariantMap serialized = datetime.toMap();
    QCOMPARE(serialized.size(), 2);

    // The order must be the same as returned from the MySQL database
    QVariantMap expectedAttributes {
        {"date",     "14.05.2023 10:11:12.0 UTC"},
        {"datetime", "13.05.2023 10:11:12.0 UTC"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    Datetime_SerializeOverride::u_dates.clear();
}

void
tst_Model_Serialization::toList_UDatesOnly_OverrideSerializeDateTime() const
{
    auto datetime = Datetime_SerializeOverride::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, TTimeZone::UTC)},
    });

    Datetime_SerializeOverride::u_dates = {"date", "datetime"};

    QList<AttributeItem> serialized = datetime.toList();
    QCOMPARE(serialized.size(), 2);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {"datetime", "13.05.2023 10:11:12.0 UTC"},
        {"date",     "14.05.2023 10:11:12.0 UTC"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    Datetime_SerializeOverride::u_dates.clear();
}

void tst_Model_Serialization::toMap_CastsOnly_OverrideSerializeDateTime() const
{
    auto datetime = Datetime_SerializeOverride::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, TTimeZone::UTC)},
        {"time",     sl("14:11:15")},
        {"time_ms",  sl("14:11:15.123")},
    });

    datetime.mergeCasts({
        {"datetime", CastType::QDateTime},
        {"date",     CastType::QDate},
        {"time",     CastType::QTime},
        {"time_ms",  CastType::QTime},
    });

    QVariantMap serialized = datetime.toMap();
    QCOMPARE(serialized.size(), 4);

    // The order must be the same as returned from the MySQL database
    QVariantMap expectedAttributes {
        {"date",     "14.05.2023"},
        {"datetime", "13.05.2023 10:11:12.0 UTC"},
        {"time",     "14-11-15.0"},
        {"time_ms",  "14-11-15.123"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    datetime.resetCasts();
}

void
tst_Model_Serialization::toList_CastsOnly_OverrideSerializeDateTime() const
{
    // Prepare
    auto timeFormatOriginal = Datetime_SerializeOverride::u_timeFormat;
    Datetime_SerializeOverride::u_timeFormat = sl("HH:mm:ss.zzz");

    auto datetime = Datetime_SerializeOverride::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, TTimeZone::UTC)},
        {"time",     QTime(14, 11, 15)}, // It must also accept QTime() instances
        {"time_ms",  QTime(14, 11, 15, 123)}, // It must also accept QTime() instances
    });

    datetime.mergeCasts({
        {"datetime", CastType::QDateTime},
        {"date",     CastType::QDate},
        {"time",     CastType::QTime},
        {"time_ms",  CastType::QTime},
    });

    QList<AttributeItem> serialized = datetime.toList();
    QCOMPARE(serialized.size(), 4);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {"datetime", "13.05.2023 10:11:12.0 UTC"},
        {"date",     "14.05.2023"},
        {"time",     "14-11-15.0"},
        {"time_ms",  "14-11-15.123"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    datetime.resetCasts();
    Datetime_SerializeOverride::u_timeFormat = std::move(timeFormatOriginal);
}

void tst_Model_Serialization::
     toMap_WithUDatesAndCasts_OverrideSerializeDateTime() const
{
    auto datetime = Datetime_SerializeOverride::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, TTimeZone::UTC)},
    });

    Datetime_SerializeOverride::u_dates = {"date", "datetime"};

    datetime.mergeCasts({
        {"datetime", CastType::QDateTime},
        {"date",     CastType::QDate},
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
    Datetime_SerializeOverride::u_dates.clear();
    datetime.resetCasts();
}

void tst_Model_Serialization::
     toList_WithUDatesAndCasts_OverrideSerializeDateTime() const
{
    auto datetime = Datetime_SerializeOverride::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, TTimeZone::UTC)},
    });

    Datetime_SerializeOverride::u_dates = {"date", "datetime"};

    datetime.mergeCasts({
        {"datetime", CastType::QDateTime},
        {"date",     CastType::QDate},
    });

    QList<AttributeItem> serialized = datetime.toList();
    QCOMPARE(serialized.size(), 2);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {"datetime", "13.05.2023 10:11:12.0 UTC"},
        {"date",     "14.05.2023"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    Datetime_SerializeOverride::u_dates.clear();
    datetime.resetCasts();
}

void tst_Model_Serialization::
     toMap_CastsOnly_WithDateModfiers_OverrideSerializeDateTime() const
{
    // Prepare
    auto timeFormatOriginal = Datetime_SerializeOverride::u_timeFormat;
    Datetime_SerializeOverride::u_timeFormat = sl("HH:mm:ss.zzz");

    auto datetime = Datetime_SerializeOverride::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, TTimeZone::UTC)},
        {"time",     QTime(14, 11, 15)}, // It must also accept QTime() instances
        {"time_ms",  QTime(14, 11, 15, 12)}, // It must also accept QTime() instances
    });

    /* The MM and dd are switched here, in this case the user serialize override methods
       must be skipped and the custom modifiers must be used. */
    datetime.mergeCasts({
        {"datetime", {CastType::CustomQDateTime, "MM.dd.yyyy HH:mm:ss.z t"}},
        {"date",     {CastType::CustomQDate,     "MM.dd.yyyy"}},
        {"time",     {CastType::CustomQTime,     "HH_mm_ss"}},
        {"time_ms",  {CastType::CustomQTime,     "HH_mm_ss.zzz"}},
    });

    QVariantMap serialized = datetime.toMap();
    QCOMPARE(serialized.size(), 4);

    // The order must be the same as returned from the MySQL database
    QVariantMap expectedAttributes {
        {"date",     "05.14.2023"},
        {"datetime", "05.13.2023 10:11:12.0 UTC"},
        {"time",     "14_11_15"},
        {"time_ms",  "14_11_15.012"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    datetime.resetCasts();
    Datetime_SerializeOverride::u_timeFormat = std::move(timeFormatOriginal);
}

void tst_Model_Serialization::
     toList_CastsOnly_WithDateModfiers_OverrideSerializeDateTime() const
{
    // Prepare
    auto timeFormatOriginal = Datetime_SerializeOverride::u_timeFormat;
    Datetime_SerializeOverride::u_timeFormat = sl("HH:mm:ss.zzz");

    auto datetime = Datetime_SerializeOverride::instance({
        {"datetime", QDateTime({2023, 05, 13}, {10, 11, 12}, TTimeZone::UTC)},
        {"date",     QDateTime({2023, 05, 14}, {10, 11, 12}, TTimeZone::UTC)},
        {"time",     sl("14:11:15")},
        {"time_ms",  sl("14:11:15.123")},
    });

    /* The MM and dd are switched here, in this case the user serialize override methods
       must be skipped and the custom modifiers must be used. */
    datetime.mergeCasts({
        {"datetime", {CastType::CustomQDateTime, "MM.dd.yyyy HH:mm:ss.z t"}},
        {"date",     {CastType::CustomQDate,     "MM.dd.yyyy"}},
        {"time",     {CastType::CustomQTime,     "HH_mm_ss.zzz"}},
        {"time_ms",  {CastType::CustomQTime,     "HH_mm_ss.zzz"}},
    });

    QList<AttributeItem> serialized = datetime.toList();
    QCOMPARE(serialized.size(), 4);

    // The order must be the same as returned from the MySQL database
    QList<AttributeItem> expectedAttributes {
        {"datetime", "05.13.2023 10:11:12.0 UTC"},
        {"date",     "05.14.2023"},
        {"time",     "14_11_15.000"},
        {"time_ms",  "14_11_15.123"},
    };
    QCOMPARE(serialized, expectedAttributes);

    // Restore
    datetime.resetCasts();
    Datetime_SerializeOverride::u_timeFormat = std::move(timeFormatOriginal);
}

void tst_Model_Serialization::toMap_UDatesOnly_DateNullVariants() const
{
    Type::u_dates = {"date", "datetime", "timestamp"};

    auto type = Type::find(3, {ID, "date", "datetime", "timestamp"});
    QVERIFY(type);
    QVERIFY(type->exists);

    QVariantMap serialized = type->toMap();
    QCOMPARE(serialized.size(), 4);

    QCOMPARE(serialized[ID].template value<typename Type::KeyType>(), 3);

    QCOMPARE(serialized["date"].typeId(),      QMetaType::QDateTime);
    QCOMPARE(serialized["datetime"].typeId(),  QMetaType::QDateTime);
    QCOMPARE(serialized["timestamp"].typeId(), QMetaType::QDateTime);

    // Restore
    Type::u_dates.clear();
}

void tst_Model_Serialization::toList_UDatesOnly_DateNullVariants() const
{
    Type::u_dates = {"date", "datetime", "timestamp"};

    auto type = Type::find(3, {ID, "date", "datetime", "timestamp"});
    QVERIFY(type);
    QVERIFY(type->exists);

    QList<AttributeItem> serialized = type->toList();
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

    QCOMPARE(serialized1.value.typeId(), QMetaType::QDateTime);
    QCOMPARE(serialized2.value.typeId(), QMetaType::QDateTime);
    QCOMPARE(serialized3.value.typeId(), QMetaType::QDateTime);

    // Restore
    Type::u_dates.clear();
}

void tst_Model_Serialization::toMap_WithCasts_DateNullVariants() const
{
    auto type = Type::find(3, {ID, "date", "datetime", "timestamp", "time"});
    QVERIFY(type);
    QVERIFY(type->exists);

    type->mergeCasts({
        {"date",      CastType::QDate},
        {"datetime",  CastType::QDateTime},
        {"timestamp", CastType::Timestamp},
        {"time",      CastType::QTime},
    });

    QVariantMap serialized = type->toMap();
    QCOMPARE(serialized.size(), 5);

    QCOMPARE(serialized[ID].template value<typename Type::KeyType>(), 3);

    QVERIFY(serialized.contains("date"));
    QVERIFY(serialized.contains("datetime"));
    QVERIFY(serialized.contains("timestamp"));
    QVERIFY(serialized.contains("time"));

    const auto &serialized1 = serialized["date"];
    const auto &serialized2 = serialized["datetime"];
    const auto &serialized3 = serialized["timestamp"];
    const auto &serialized4 = serialized["time"];

    QVERIFY(serialized1.isValid() && serialized1.isNull());
    QVERIFY(serialized2.isValid() && serialized2.isNull());
    QVERIFY(serialized3.isValid() && serialized3.isNull());
    QVERIFY(serialized4.isValid() && serialized4.isNull());

    QCOMPARE(serialized1.typeId(), QMetaType::QDate);
    QCOMPARE(serialized2.typeId(), QMetaType::QDateTime);
    QCOMPARE(serialized3.typeId(), QMetaType::LongLong);
    QCOMPARE(serialized4.typeId(), QMetaType::QTime);

    // Restore
    type->resetCasts();
}

void tst_Model_Serialization::toList_WithCasts_DateNullVariants() const
{
    auto type = Type::find(3, {ID, "date", "datetime", "timestamp", "time"});
    QVERIFY(type);
    QVERIFY(type->exists);

    type->mergeCasts({
        {"date",      CastType::QDate},
        {"datetime",  CastType::QDateTime},
        {"timestamp", CastType::Timestamp},
        {"time",      CastType::QTime},
    });

    QList<AttributeItem> serialized = type->toList();
    QCOMPARE(serialized.size(), 5);

    const auto &serialized0 = serialized.at(0);
    QCOMPARE(serialized0.key, ID);
    QCOMPARE(serialized0.value.template value<typename Type::KeyType>(), 3);

    const auto &serialized1 = serialized.at(1);
    const auto &serialized2 = serialized.at(2);
    const auto &serialized3 = serialized.at(3);
    const auto &serialized4 = serialized.at(4);

    QCOMPARE(serialized1.key, sl("date"));
    QCOMPARE(serialized2.key, sl("datetime"));
    QCOMPARE(serialized3.key, sl("timestamp"));
    QCOMPARE(serialized4.key, sl("time"));

    QVERIFY(serialized1.value.isValid() && serialized1.value.isNull());
    QVERIFY(serialized2.value.isValid() && serialized2.value.isNull());
    QVERIFY(serialized3.value.isValid() && serialized3.value.isNull());
    QVERIFY(serialized4.value.isValid() && serialized4.value.isNull());

    QCOMPARE(serialized1.value.typeId(), QMetaType::QDate);
    QCOMPARE(serialized2.value.typeId(), QMetaType::QDateTime);
    QCOMPARE(serialized3.value.typeId(), QMetaType::LongLong);
    QCOMPARE(serialized4.value.typeId(), QMetaType::QTime);

    // Restore
    type->resetCasts();
}

void
tst_Model_Serialization::toMap_WithRelations_HasOne_HasMany_BelongsTo() const
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
        {Progress,        700},
        {SIZE_,           17},
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
tst_Model_Serialization::toList_WithRelations_HasOne_HasMany_BelongsTo() const
{
    auto torrent = Torrent::with({"torrentPeer", "user", "torrentFiles"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

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
    // user
    {
        const auto it = ranges::find(serialized, "user", keyProj);
        if (it == serialized.end())
            QFAIL("The \"user\" key not found in the \"serialized\" result.");

        auto actualAttributes = serialized.takeAt(std::distance(serialized.begin(), it))
                                    .value.value<QList<AttributeItem>>();

        QList<AttributeItem> expectedAttributes {
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
            {ID,              7},
            {"user_id",       2},
            {NAME,            "test7"},
            {SIZE_,           17},
            {Progress,        700},
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
tst_Model_Serialization::toMap_WithRelation_BelongsToMany_TorrentTags() const
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
        {Progress,   700},
        {SIZE_,      17},
        {"tags",     QVariantList {QVariantMap {
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
                     }, QVariantMap {
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
                     }, QVariantMap {
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
                     }}},
        {UPDATED_AT, "2021-01-07T18:46:31.000Z"},
        {"user_id",  2},
    };

    QCOMPARE(serialized, expectedAttributes);
}

void
tst_Model_Serialization::toList_WithRelation_BelongsToMany_TorrentTags() const
{
    auto torrent = Torrent::with("tags")->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QList<AttributeItem> serialized = torrent->toList();

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
        auto itTags = ranges::find(serialized, "tags", keyProj);
        if (itTags == serialized.end())
            QFAIL("The \"tags\" key not found in the \"serialized\" result.");

        // Then extract the tag_property vectors from all tags
        /* I'm going to directly modify the tag vector using the QVariant::data(),
           it helps to avoid a lot of a junky code. */
        auto &tags = *reinterpret_cast<QVariantList *>(itTags->value.data()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

        QList<QList<AttributeItem>> actualTagProperties;
        actualTagProperties.reserve(tags.size());

        for (auto &tagVariant : tags) {
            auto &tag = *reinterpret_cast<QList<AttributeItem> *>(tagVariant.data());

            const auto it = ranges::find(tag, "tag_property", keyProj);
            if (it == tag.end())
                QFAIL("The \"tag_property\" key not found in the \"tag\" result.");

            actualTagProperties << tag.takeAt(std::distance(tag.begin(), it))
                                       .value.value<QList<AttributeItem>>();
        }

        QList<QList<AttributeItem>> expectedTagProperties {{
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
    QList<AttributeItem> expectedAttributes {
        {ID,         7},
        {"user_id",  2},
        {NAME,       "test7"},
        {SIZE_,      17},
        {Progress,   700},
        {"added_on", "2020-08-07T20:11:10.000Z"},
        {HASH_,      "7579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {NOTE,       "for serialization"},
        {CREATED_AT, "2021-11-07T08:13:23.000Z"},
        {UPDATED_AT, "2021-01-07T18:46:31.000Z"},
        {"tags",     QVariantList {QVariant::fromValue(QList<AttributeItem> {
                         {"id",           1},
                         {NAME,           "tag1"},
                         {NOTE,           NullVariant::QString()},
                         {CREATED_AT,     "2021-01-11T11:51:28.000Z"},
                         {UPDATED_AT,     "2021-01-11T23:47:11.000Z"},
                         {"tagged",       QVariant::fromValue(QList<AttributeItem> {
                                              {"torrent_id", 7},
                                              {"tag_id",     1},
                                              {"active",     true},
                                              {CREATED_AT,   "2021-03-01T17:31:58.000Z"},
                                              {UPDATED_AT,   "2021-03-01T18:49:22.000Z"},
                                          })},
                     }), QVariant::fromValue(QList<AttributeItem> {
                         {"id",           2},
                         {NAME,           "tag2"},
                         {NOTE,           NullVariant::QString()},
                         {CREATED_AT,     "2021-01-12T11:51:28.000Z"},
                         {UPDATED_AT,     "2021-01-12T23:47:11.000Z"},
                         {"tagged",       QVariant::fromValue(QList<AttributeItem> {
                                              {"torrent_id", 7},
                                              {"tag_id",     2},
                                              {"active",     true},
                                              {CREATED_AT,   "2021-03-02T17:31:58.000Z"},
                                              {UPDATED_AT,   "2021-03-02T18:49:22.000Z"},
                                          })},
                     }), QVariant::fromValue(QList<AttributeItem> {
                         {"id",           3},
                         {NAME,           "tag3"},
                         {NOTE,           NullVariant::QString()},
                         {CREATED_AT,     "2021-01-13T11:51:28.000Z"},
                         {UPDATED_AT,     "2021-01-13T23:47:11.000Z"},
                         {"tagged",       QVariant::fromValue(QList<AttributeItem> {
                                              {"torrent_id", 7},
                                              {"tag_id",     3},
                                              {"active",     false},
                                              {CREATED_AT,   "2021-03-03T17:31:58.000Z"},
                                              {UPDATED_AT,   "2021-03-03T18:49:22.000Z"},
                                          })},
                     })}},
    };

    QCOMPARE(serialized, expectedAttributes);
}

// Following two unit tests test two belongs-to-many relations on one model

void tst_Model_Serialization::
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
        {Progress,   700},
        {SIZE_,      17},
        {"tags",     QVariantList {QVariantMap {
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
                     }, QVariantMap {
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
                     }, QVariantMap {
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
                     }}},

        {"torrent_states", QVariantList {QVariantMap {
                               {ID,     1},
                               {NAME,   "Active"},
                               {pivot_, QVariantMap {
                                            {"active",     1},
                                            {"state_id",   1},
                                            {"torrent_id", 7},
                                        }},
                           }, QVariantMap {
                               {ID,     4},
                               {NAME,   "Downloading"},
                               {pivot_, QVariantMap {
                                            {"active",     0},
                                            {"state_id",   4},
                                            {"torrent_id", 7},
                                        }},
                           }}},

        {UPDATED_AT,      "2021-01-07T18:46:31.000Z"},
        {"user_id",       2},
    };

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::
     toList_WithRelation_BelongsToMany_TorrentTags_TorrentStates() const
{
    auto torrent = Torrent::with({"tags", "torrentStates"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QList<AttributeItem> serialized = torrent->toList();

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
        auto itTags = ranges::find(serialized, "tags", keyProj);
        if (itTags == serialized.end())
            QFAIL("The \"tags\" key not found in the \"serialized\" result.");

        // Then extract the tag_property vectors from all tags
        /* I'm going to directly modify the tag vector using the QVariant::data(),
           it helps to avoid a lot of a junky code. */
        auto &tags = *reinterpret_cast<QVariantList *>(itTags->value.data());

        QList<QList<AttributeItem>> actualTagProperties;
        actualTagProperties.reserve(tags.size());

        for (auto &tagVariant : tags) {
            auto &tag = *reinterpret_cast<QList<AttributeItem> *>(tagVariant.data());

            const auto it = ranges::find(tag, "tag_property", keyProj);
            if (it == tag.end())
                QFAIL("The \"tag_property\" key not found in the \"tag\" result.");

            actualTagProperties << tag.takeAt(std::distance(tag.begin(), it))
                                       .value.value<QList<AttributeItem>>();
        }

        QList<QList<AttributeItem>> expectedTagProperties {{
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
        const auto it = ranges::find(serialized, "torrent_states", keyProj);
        if (it == serialized.end())
            QFAIL("The \"torrent_states\" key not found in the \"serialized\" result.");

        auto actualStates = serialized.takeAt(std::distance(serialized.begin(), it))
                                .value.value<QVariantList>();

        QVariantList expectedStates {QVariant::fromValue(QList<AttributeItem> {
            {ID,     1},
            {NAME,   "Active"},
            {pivot_, QVariant::fromValue(QList<AttributeItem> {
                         {"torrent_id", 7},
                         {"state_id",   1},
                         {"active",     1},
                     })},
        }), QVariant::fromValue(QList<AttributeItem> {
            {ID,     4},
            {NAME,   "Downloading"},
            {pivot_, QVariant::fromValue(QList<AttributeItem> {
                         {"torrent_id", 7},
                         {"state_id",   4},
                         {"active",     0},
                     })}
        })};

        QCOMPARE(actualStates, expectedStates);
    }

    QList<AttributeItem> expectedAttributes {
        {ID,         7},
        {"user_id",  2},
        {NAME,       "test7"},
        {SIZE_,      17},
        {Progress,   700},
        {"added_on", "2020-08-07T20:11:10.000Z"},
        {HASH_,      "7579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {NOTE,       "for serialization"},
        {CREATED_AT, "2021-11-07T08:13:23.000Z"},
        {UPDATED_AT, "2021-01-07T18:46:31.000Z"},
        {"tags",     QVariantList {QVariant::fromValue(QList<AttributeItem> {
                         {"id",           1},
                         {NAME,           "tag1"},
                         {NOTE,           NullVariant::QString()},
                         {CREATED_AT,     "2021-01-11T11:51:28.000Z"},
                         {UPDATED_AT,     "2021-01-11T23:47:11.000Z"},
                         {"tagged",       QVariant::fromValue(QList<AttributeItem> {
                                              {"torrent_id", 7},
                                              {"tag_id",     1},
                                              {"active",     true},
                                              {CREATED_AT,   "2021-03-01T17:31:58.000Z"},
                                              {UPDATED_AT,   "2021-03-01T18:49:22.000Z"},
                                          })},
                     }), QVariant::fromValue(QList<AttributeItem> {
                         {"id",           2},
                         {NAME,           "tag2"},
                         {NOTE,           NullVariant::QString()},
                         {CREATED_AT,     "2021-01-12T11:51:28.000Z"},
                         {UPDATED_AT,     "2021-01-12T23:47:11.000Z"},
                         {"tagged",       QVariant::fromValue(QList<AttributeItem> {
                                              {"torrent_id", 7},
                                              {"tag_id",     2},
                                              {"active",     true},
                                              {CREATED_AT,   "2021-03-02T17:31:58.000Z"},
                                              {UPDATED_AT,   "2021-03-02T18:49:22.000Z"},
                                          })},
                     }), QVariant::fromValue(QList<AttributeItem> {
                         {"id",           3},
                         {NAME,           "tag3"},
                         {NOTE,           NullVariant::QString()},
                         {CREATED_AT,     "2021-01-13T11:51:28.000Z"},
                         {UPDATED_AT,     "2021-01-13T23:47:11.000Z"},
                         {"tagged",       QVariant::fromValue(QList<AttributeItem> {
                                              {"torrent_id", 7},
                                              {"tag_id",     3},
                                              {"active",     false},
                                              {CREATED_AT,   "2021-03-03T17:31:58.000Z"},
                                              {UPDATED_AT,   "2021-03-03T18:49:22.000Z"},
                                          })},
                     })}},

    };

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::toMap_WithRelation_BelongsToMany_UserRoles() const
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
        {"roles",     QVariantList {QVariantMap {
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
        {UPDATED_AT,  "2022-01-01T17:46:31.000Z"},
    };

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::
     toList_WithRelation_BelongsToMany_UserRoles() const
{
    auto user = User::with("roles")->find(1);
    QVERIFY(user);
    QVERIFY(user->exists);

    QList<AttributeItem> serialized = user->toList();

    QList<AttributeItem> expectedAttributes {
        {ID,          1},
        {NAME,        "andrej"},
        {"is_banned", false},
        {NOTE,        NullVariant::QString()},
        {CREATED_AT,  "2022-01-01T14:51:23.000Z"},
        {UPDATED_AT,  "2022-01-01T17:46:31.000Z"},
        {DELETED_AT,  NullVariant::QDateTime()},
        {"roles",     QVariantList {QVariant::fromValue(QList<AttributeItem> {
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
}

void tst_Model_Serialization::toMap_u_snakeAttributes_false() const
{
    auto album = Album::find(1);
    QVERIFY(album);
    QVERIFY(album->exists);

    QVariantMap serialized = album->toMap();

    QVariantMap expectedAttributes {
        {"albumImages", QVariantList {QVariantMap {
                            {"album_id", 1},
                            {CREATED_AT, "2023-03-01T15:24:37.000Z"},
                            {"ext",      "png"},
                            {ID,         1},
                            {NAME,       "album1_image1"},
                            {SIZE_,      726},
                            {UPDATED_AT, "2023-04-01T14:35:47.000Z"},
                        }}},
        {CREATED_AT,    "2023-01-01T12:21:14.000Z"},
        {ID,            1},
        {NAME,          "album1"},
        {NOTE,          NullVariant::QString()},
        {UPDATED_AT,    "2023-02-01T16:54:28.000Z"},
    };

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::toList_u_snakeAttributes_false() const
{
    auto album = Album::find(1);
    QVERIFY(album);
    QVERIFY(album->exists);

    QList<AttributeItem> serialized = album->toList();

    QList<AttributeItem> expectedAttributes {
        {ID,            1},
        {NAME,          "album1"},
        {NOTE,          NullVariant::QString()},
        {CREATED_AT,    "2023-01-01T12:21:14.000Z"},
        {UPDATED_AT,    "2023-02-01T16:54:28.000Z"},
        {"albumImages", QVariantList {QVariant::fromValue(QList<AttributeItem> {
                            {ID,         1},
                            {"album_id", 1},
                            {NAME,       "album1_image1"},
                            {"ext",      "png"},
                            {SIZE_,      726},
                            {CREATED_AT, "2023-03-01T15:24:37.000Z"},
                            {UPDATED_AT, "2023-04-01T14:35:47.000Z"},
                        })}},
    };

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::toMap_HasMany_EmptyRelation() const
{
    auto albums = Album::findMany({3, 4});
    QCOMPARE(albums.size(), 2);
    QCOMPARE(typeid (albums), typeid (ModelsCollection<Album>));

    QList<QVariantMap> serialized = albums.toMap();

    QList<QVariantMap> expectedAttributes {{
        {"albumImages", QVariantList {QVariantMap {
                            {"album_id", 3},
                            {CREATED_AT, "2023-03-07T15:24:37.000Z"},
                            {"ext",      "jpg"},
                            {ID,         7},
                            {NAME,       "album3_image1"},
                            {SIZE_,      718},
                            {UPDATED_AT, "2023-04-07T14:35:47.000Z"},
                        }}},
        {CREATED_AT,    "2023-01-03T12:21:14.000Z"},
        {ID,            3},
        {NAME,          "album3"},
        {NOTE,          "album3 note"},
        {UPDATED_AT,    "2023-02-03T16:54:28.000Z"},
    }, {
        {"albumImages", QVariantList {}},
        {CREATED_AT,    "2023-01-04T12:21:14.000Z"},
        {ID,            4},
        {NAME,          "album4"},
        {NOTE,          "no images"},
        {UPDATED_AT,    "2023-02-04T16:54:28.000Z"},
    }};

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::toList_HasMany_EmptyRelation() const
{
    auto albums = Album::findMany({3, 4});
    QCOMPARE(albums.size(), 2);
    QCOMPARE(typeid (albums), typeid (ModelsCollection<Album>));

    QList<QList<AttributeItem>> serialized = albums.toList();

    QList<QList<AttributeItem>> expectedAttributes {{
        {ID,            3},
        {NAME,          "album3"},
        {NOTE,          "album3 note"},
        {CREATED_AT,    "2023-01-03T12:21:14.000Z"},
        {UPDATED_AT,    "2023-02-03T16:54:28.000Z"},
        {"albumImages", QVariantList {QVariant::fromValue(QList<AttributeItem> {
                            {ID,         7},
                            {"album_id", 3},
                            {NAME,       "album3_image1"},
                            {"ext",      "jpg"},
                            {SIZE_,      718},
                            {CREATED_AT, "2023-03-07T15:24:37.000Z"},
                            {UPDATED_AT, "2023-04-07T14:35:47.000Z"},
                        })}},
    }, {
        {ID,            4},
        {NAME,          "album4"},
        {NOTE,          "no images"},
        {CREATED_AT,    "2023-01-04T12:21:14.000Z"},
        {UPDATED_AT,    "2023-02-04T16:54:28.000Z"},
        {"albumImages", QVariantList {}},
    }};

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::toMap_HasOne_EmptyRelation() const
{
    auto torrents = Torrent::with("torrentPeer")->findMany({6, 7});
    QCOMPARE(torrents.size(), 2);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent>));

    QList<QVariantMap> serialized = torrents.toMap();

    QList<QVariantMap> expectedAttributes {{
        {"added_on",     "2020-08-06T20:11:10.000Z"},
        {CREATED_AT,     "2021-11-06T08:13:23.000Z"},
        {HASH_,          "6579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {ID,             6},
        {NAME,           "test6"},
        {NOTE,           "no files no peers"},
        {Progress,       600},
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
        {Progress,       700},
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

void tst_Model_Serialization::toList_HasOne_EmptyRelation() const
{
    auto torrents = Torrent::with("torrentPeer")->findMany({6, 7});
    QCOMPARE(torrents.size(), 2);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent>));

    QList<QList<AttributeItem>> serialized = torrents.toList();

    QList<QList<AttributeItem>> expectedAttributes {{
        {ID,             6},
        {"user_id",      2},
        {NAME,           "test6"},
        {SIZE_,          16},
        {Progress,       600},
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
        {Progress,       700},
        {"added_on",     "2020-08-07T20:11:10.000Z"},
        {HASH_,          "7579e3af2768cdf52ec84c1f320333f68401dc6e"},
        {NOTE,           "for serialization"},
        {CREATED_AT,     "2021-11-07T08:13:23.000Z"},
        {UPDATED_AT,     "2021-01-07T18:46:31.000Z"},
        {"torrent_peer", QVariant::fromValue(QList<AttributeItem> {
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

void tst_Model_Serialization::toMap_BelongsTo_EmptyRelation() const
{
    auto torrentPeers = TorrentPeer::with("torrent")->findMany({5, 6});
    QCOMPARE(torrentPeers.size(), 2);
    QCOMPARE(typeid (torrentPeers), typeid (ModelsCollection<TorrentPeer>));

    QList<QVariantMap> serialized = torrentPeers.toMap();

    QList<QVariantMap> expectedAttributes {{
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
                               {Progress,   700},
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

void tst_Model_Serialization::toList_BelongsTo_EmptyRelation() const
{
    auto torrentPeers = TorrentPeer::with("torrent")->findMany({5, 6});
    QCOMPARE(torrentPeers.size(), 2);
    QCOMPARE(typeid (torrentPeers), typeid (ModelsCollection<TorrentPeer>));

    QList<QList<AttributeItem>> serialized = torrentPeers.toList();

    QList<QList<AttributeItem>> expectedAttributes {{
        {ID,               5},
        {"torrent_id",     7},
        {"seeds",          NullVariant::Int()},
        {"total_seeds",    7},
        {"leechers",       7},
        {"total_leechers", 7},
        {CREATED_AT,       "2021-01-07T14:51:23.000Z"},
        {UPDATED_AT,       "2021-01-07T17:46:31.000Z"},
        {"torrent",        QVariant::fromValue(QList<AttributeItem> {
                               {ID,         7},
                               {"user_id",  2},
                               {NAME,       "test7"},
                               {SIZE_,      17},
                               {Progress,   700},
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

void tst_Model_Serialization::toMap_BelongsToMany_EmptyRelation() const
{
    auto users = User::with("roles")->findMany({2, 3});
    QCOMPARE(users.size(), 2);
    QCOMPARE(typeid (users), typeid (ModelsCollection<User>));

    QList<QVariantMap> serialized = users.toMap();

    QList<QVariantMap> expectedAttributes {{
        {CREATED_AT,  "2022-01-02T14:51:23.000Z"},
        {DELETED_AT,  NullVariant::QDateTime()},
        {ID,          2},
        {"is_banned", false},
        {NAME,        "silver"},
        {NOTE,        NullVariant::QString()},
        {"roles",     QVariantList {QVariantMap {
                          {"added_on",     "2022-08-02T13:36:56.000Z"},
                          {ID,             2},
                          {NAME,           "role two"},
                          {"subscription", QVariantMap {
                                               {"active",  true},
                                               {"role_id", 2},
                                               {"user_id", 2},
                                           }},
                      }}},
        {UPDATED_AT,  "2022-01-02T17:46:31.000Z"},
    }, {
        {CREATED_AT,  "2022-01-03T14:51:23.000Z"},
        {DELETED_AT,  NullVariant::QDateTime()},
        {ID,          3},
        {"is_banned", true},
        {NAME,        "peter"},
        {NOTE,        "no torrents no roles"},
        {"roles",     QVariantList {}},
        {UPDATED_AT,  "2022-01-03T17:46:31.000Z"},
    }};

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::toList_BelongsToMany_EmptyRelation() const
{
    auto users = User::with("roles")->findMany({2, 3});
    QCOMPARE(users.size(), 2);
    QCOMPARE(typeid (users), typeid (ModelsCollection<User>));

    QList<QList<AttributeItem>> serialized = users.toList();

    QList<QList<AttributeItem>> expectedAttributes {{
        {ID,          2},
        {NAME,        "silver"},
        {"is_banned", false},
        {NOTE,        NullVariant::QString()},
        {CREATED_AT,  "2022-01-02T14:51:23.000Z"},
        {UPDATED_AT,  "2022-01-02T17:46:31.000Z"},
        {DELETED_AT,  NullVariant::QDateTime()},
        {"roles",     QVariantList {QVariant::fromValue(QList<AttributeItem> {
                          {ID,             2},
                          {NAME,           "role two"},
                          {"added_on",     "2022-08-02T13:36:56.000Z"},
                          {"subscription", QVariant::fromValue(QList<AttributeItem> {
                                               {"user_id", 2},
                                               {"role_id", 2},
                                               {"active",  true}
                                           })},
                      })}},
    }, {
        {ID,          3},
        {NAME,        "peter"},
        {"is_banned", true},
        {NOTE,        "no torrents no roles"},
        {CREATED_AT,  "2022-01-03T14:51:23.000Z"},
        {UPDATED_AT,  "2022-01-03T17:46:31.000Z"},
        {DELETED_AT,  NullVariant::QDateTime()},
        {"roles",     QVariantList {}},
    }};

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::toMap_RelationOnly_HasMany() const
{
    auto torrent = Torrent::with("torrentFiles")->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    ModelsCollection<TorrentPreviewableFile *>
    torrentFiles = torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(torrentFiles.size(), 3);

    QList<QVariantMap> serialized = torrentFiles.toMap();

    QList<QVariantMap> expectedAttributes {QVariantMap {
        {CREATED_AT,   "2021-01-10T14:51:23.000Z"},
        {"file_index", 0},
        {"filepath",   "test7_file1.mkv"},
        {ID,           10},
        {NOTE,         "for serialization"},
        {Progress,     512},
        {SIZE_,        4562},
        {"torrent_id", 7},
        {UPDATED_AT,   "2021-01-10T17:46:31.000Z"},
    }, QVariantMap {
        {CREATED_AT,   "2021-01-11T14:51:23.000Z"},
        {"file_index", 1},
        {"filepath",   "test7_file2.mkv"},
        {ID,           11},
        {NOTE,         "for serialization"},
        {Progress,     256},
        {SIZE_,        2567},
        {"torrent_id", 7},
        {UPDATED_AT,   "2021-01-11T17:46:31.000Z"},
    }, QVariantMap {
        {CREATED_AT,   "2021-01-12T14:51:23.000Z"},
        {"file_index", 2},
        {"filepath",   "test7_file3.mkv"},
        {ID,           12},
        {NOTE,         "for serialization"},
        {Progress,     768},
        {SIZE_,        4279},
        {"torrent_id", 7},
        {UPDATED_AT,   "2021-01-12T17:46:31.000Z"},
    }};

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::toList_RelationOnly_HasMany() const
{
    auto torrent = Torrent::with("torrentFiles")->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    ModelsCollection<TorrentPreviewableFile *>
    torrentFiles = torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(torrentFiles.size(), 3);

    QList<QList<AttributeItem>> serialized = torrentFiles.toList();

    QList<QList<AttributeItem>> expectedAttributes {QList<AttributeItem> {
        {ID,           10},
        {"torrent_id", 7},
        {"file_index", 0},
        {"filepath",   "test7_file1.mkv"},
        {SIZE_,        4562},
        {Progress,     512},
        {NOTE,         "for serialization"},
        {CREATED_AT,   "2021-01-10T14:51:23.000Z"},
        {UPDATED_AT,   "2021-01-10T17:46:31.000Z"},
    }, QList<AttributeItem> {
        {ID,           11},
        {"torrent_id", 7},
        {"file_index", 1},
        {"filepath",   "test7_file2.mkv"},
        {SIZE_,        2567},
        {Progress,     256},
        {NOTE,         "for serialization"},
        {CREATED_AT,   "2021-01-11T14:51:23.000Z"},
        {UPDATED_AT,   "2021-01-11T17:46:31.000Z"},
    }, QList<AttributeItem> {
        {ID,           12},
        {"torrent_id", 7},
        {"file_index", 2},
        {"filepath",   "test7_file3.mkv"},
        {SIZE_,        4279},
        {Progress,     768},
        {NOTE,         "for serialization"},
        {CREATED_AT,   "2021-01-12T14:51:23.000Z"},
        {UPDATED_AT,   "2021-01-12T17:46:31.000Z"},
    }};

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::toMap_RelationOnly_BelongsToMany() const
{
    auto user = User::with("roles")->find(1);
    QVERIFY(user);
    QVERIFY(user->exists);

    ModelsCollection<Role *> roles = user->template getRelationValue<Role>("roles");
    QCOMPARE(roles.size(), 3);

    QList<QVariantMap> serialized = roles.toMap<RoleUser>();

    QList<QVariantMap> expectedAttributes {QVariantMap {
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
    }};

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::toList_RelationOnly_BelongsToMany() const
{
    auto user = User::with("roles")->find(1);
    QVERIFY(user);
    QVERIFY(user->exists);

    ModelsCollection<Role *> roles = user->template getRelationValue<Role>("roles");
    QCOMPARE(roles.size(), 3);

    QList<QList<AttributeItem>> serialized = roles.toList<RoleUser>();

    QList<QList<AttributeItem>> expectedAttributes {QList<AttributeItem> {
        {ID,             1},
        {NAME,           "role one"},
        {"added_on",     "2022-08-01T13:36:56.000Z"},
        {"subscription", QVariant::fromValue(QList<AttributeItem> {
                             {"user_id", 1},
                             {"role_id", 1},
                             {"active",  true},
                         })},
    }, QList<AttributeItem> {
        {ID,             2},
        {NAME,           "role two"},
        {"added_on",     "2022-08-02T13:36:56.000Z"},
        {"subscription", QVariant::fromValue(QList<AttributeItem> {
                             {"user_id", 1},
                             {"role_id", 2},
                             {"active",  false},
                         })},
    }, QList<AttributeItem> {
        {ID,             3},
        {NAME,           "role three"},
        {"added_on",     NullVariant::QDateTime()},
        {"subscription", QVariant::fromValue(QList<AttributeItem> {
                             {"user_id", 1},
                             {"role_id", 3},
                             {"active",  true},
                         })},
    }};

    QCOMPARE(serialized, expectedAttributes);
}

void tst_Model_Serialization::
     toJson_WithRelations_HasOne_HasMany_BelongsTo() const
{
    auto torrent = Torrent::with({"torrentPeer", "user", "torrentFiles"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QByteArray json = torrent->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"({
    "added_on": "2020-08-07T20:11:10.000Z",
    "created_at": "2021-11-07T08:13:23.000Z",
    "hash": "7579e3af2768cdf52ec84c1f320333f68401dc6e",
    "id": 7,
    "name": "test7",
    "note": "for serialization",
    "progress": 700,
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
    "torrent_peer": {
        "created_at": "2021-01-07T14:51:23.000Z",
        "id": 5,
        "leechers": 7,
        "seeds": null,
        "torrent_id": 7,
        "total_leechers": 7,
        "total_seeds": 7,
        "updated_at": "2021-01-07T17:46:31.000Z"
    },
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
}

void
tst_Model_Serialization::toJson_WithRelation_BelongsToMany_TorrentTags() const
{
    auto torrent = Torrent::with("tags")->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QByteArray json = torrent->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"({
    "added_on": "2020-08-07T20:11:10.000Z",
    "created_at": "2021-11-07T08:13:23.000Z",
    "hash": "7579e3af2768cdf52ec84c1f320333f68401dc6e",
    "id": 7,
    "name": "test7",
    "note": "for serialization",
    "progress": 700,
    "size": 17,
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
    "updated_at": "2021-01-07T18:46:31.000Z",
    "user_id": 2
}
)");

    QCOMPARE(json, expectedJson);
}

void tst_Model_Serialization::
     toJson_WithRelation_BelongsToMany_TorrentTags_TorrentStates() const
{
    auto torrent = Torrent::with({"tags", "torrentStates"})->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QByteArray json = torrent->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"({
    "added_on": "2020-08-07T20:11:10.000Z",
    "created_at": "2021-11-07T08:13:23.000Z",
    "hash": "7579e3af2768cdf52ec84c1f320333f68401dc6e",
    "id": 7,
    "name": "test7",
    "note": "for serialization",
    "progress": 700,
    "size": 17,
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
}

void tst_Model_Serialization::toJson_WithRelation_BelongsToMany_UserRoles() const
{
    auto user = User::with("roles")->find(1);
    QVERIFY(user);
    QVERIFY(user->exists);

    QByteArray json = user->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"({
    "created_at": "2022-01-01T14:51:23.000Z",
    "deleted_at": null,
    "id": 1,
    "is_banned": false,
    "name": "andrej",
    "note": null,
    "roles": [
        {
            "added_on": "2022-08-01T13:36:56.000Z",
            "id": 1,
            "name": "role one",
            "subscription": {
                "active": true,
                "role_id": 1,
                "user_id": 1
            }
        },
        {
            "added_on": "2022-08-02T13:36:56.000Z",
            "id": 2,
            "name": "role two",
            "subscription": {
                "active": false,
                "role_id": 2,
                "user_id": 1
            }
        },
        {
            "added_on": null,
            "id": 3,
            "name": "role three",
            "subscription": {
                "active": true,
                "role_id": 3,
                "user_id": 1
            }
        }
    ],
    "updated_at": "2022-01-01T17:46:31.000Z"
}
)");

    QCOMPARE(json, expectedJson);
}

void tst_Model_Serialization::toJson_HasMany_EmptyRelation() const
{
    auto albums = Album::findMany({3, 4});
    QCOMPARE(albums.size(), 2);
    QCOMPARE(typeid (albums), typeid (ModelsCollection<Album>));

    QByteArray json = albums.toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"([
    {
        "albumImages": [
            {
                "album_id": 3,
                "created_at": "2023-03-07T15:24:37.000Z",
                "ext": "jpg",
                "id": 7,
                "name": "album3_image1",
                "size": 718,
                "updated_at": "2023-04-07T14:35:47.000Z"
            }
        ],
        "created_at": "2023-01-03T12:21:14.000Z",
        "id": 3,
        "name": "album3",
        "note": "album3 note",
        "updated_at": "2023-02-03T16:54:28.000Z"
    },
    {
        "albumImages": [
        ],
        "created_at": "2023-01-04T12:21:14.000Z",
        "id": 4,
        "name": "album4",
        "note": "no images",
        "updated_at": "2023-02-04T16:54:28.000Z"
    }
]
)");

    QCOMPARE(json, expectedJson);
}

void tst_Model_Serialization::toJson_HasOne_EmptyRelation() const
{
    auto torrents = Torrent::with("torrentPeer")->findMany({6, 7});
    QCOMPARE(torrents.size(), 2);
    QCOMPARE(typeid (torrents), typeid (ModelsCollection<Torrent>));

    QByteArray json = torrents.toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"([
    {
        "added_on": "2020-08-06T20:11:10.000Z",
        "created_at": "2021-11-06T08:13:23.000Z",
        "hash": "6579e3af2768cdf52ec84c1f320333f68401dc6e",
        "id": 6,
        "name": "test6",
        "note": "no files no peers",
        "progress": 600,
        "size": 16,
        "torrent_peer": null,
        "updated_at": "2021-01-06T18:46:31.000Z",
        "user_id": 2
    },
    {
        "added_on": "2020-08-07T20:11:10.000Z",
        "created_at": "2021-11-07T08:13:23.000Z",
        "hash": "7579e3af2768cdf52ec84c1f320333f68401dc6e",
        "id": 7,
        "name": "test7",
        "note": "for serialization",
        "progress": 700,
        "size": 17,
        "torrent_peer": {
            "created_at": "2021-01-07T14:51:23.000Z",
            "id": 5,
            "leechers": 7,
            "seeds": null,
            "torrent_id": 7,
            "total_leechers": 7,
            "total_seeds": 7,
            "updated_at": "2021-01-07T17:46:31.000Z"
        },
        "updated_at": "2021-01-07T18:46:31.000Z",
        "user_id": 2
    }
]
)");

    QCOMPARE(json, expectedJson);
}

void tst_Model_Serialization::toJson_BelongsTo_EmptyRelation() const
{
    auto torrentPeers = TorrentPeer::with("torrent")->findMany({5, 6});
    QCOMPARE(torrentPeers.size(), 2);
    QCOMPARE(typeid (torrentPeers), typeid (ModelsCollection<TorrentPeer>));

    QByteArray json = torrentPeers.toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"([
    {
        "created_at": "2021-01-07T14:51:23.000Z",
        "id": 5,
        "leechers": 7,
        "seeds": null,
        "torrent": {
            "added_on": "2020-08-07T20:11:10.000Z",
            "created_at": "2021-11-07T08:13:23.000Z",
            "hash": "7579e3af2768cdf52ec84c1f320333f68401dc6e",
            "id": 7,
            "name": "test7",
            "note": "for serialization",
            "progress": 700,
            "size": 17,
            "updated_at": "2021-01-07T18:46:31.000Z",
            "user_id": 2
        },
        "torrent_id": 7,
        "total_leechers": 7,
        "total_seeds": 7,
        "updated_at": "2021-01-07T17:46:31.000Z"
    },
    {
        "created_at": "2021-01-06T14:51:23.000Z",
        "id": 6,
        "leechers": 6,
        "seeds": null,
        "torrent": null,
        "torrent_id": null,
        "total_leechers": 6,
        "total_seeds": 6,
        "updated_at": "2021-01-06T17:46:31.000Z"
    }
]
)");

    QCOMPARE(json, expectedJson);
}

void tst_Model_Serialization::toJson_BelongsToMany_EmptyRelation() const
{
    auto users = User::with("roles")->findMany({2, 3});
    QCOMPARE(users.size(), 2);
    QCOMPARE(typeid (users), typeid (ModelsCollection<User>));

    QByteArray json = users.toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"([
    {
        "created_at": "2022-01-02T14:51:23.000Z",
        "deleted_at": null,
        "id": 2,
        "is_banned": false,
        "name": "silver",
        "note": null,
        "roles": [
            {
                "added_on": "2022-08-02T13:36:56.000Z",
                "id": 2,
                "name": "role two",
                "subscription": {
                    "active": true,
                    "role_id": 2,
                    "user_id": 2
                }
            }
        ],
        "updated_at": "2022-01-02T17:46:31.000Z"
    },
    {
        "created_at": "2022-01-03T14:51:23.000Z",
        "deleted_at": null,
        "id": 3,
        "is_banned": true,
        "name": "peter",
        "note": "no torrents no roles",
        "roles": [
        ],
        "updated_at": "2022-01-03T17:46:31.000Z"
    }
]
)");

    QCOMPARE(json, expectedJson);
}

void tst_Model_Serialization::toJson_u_snakeAttributes_false() const
{
    auto album = Album::find(1);
    QVERIFY(album);
    QVERIFY(album->exists);

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
    "created_at": "2023-01-01T12:21:14.000Z",
    "id": 1,
    "name": "album1",
    "note": null,
    "updated_at": "2023-02-01T16:54:28.000Z"
}
)");

    QCOMPARE(json, expectedJson);
}

void tst_Model_Serialization::toJson_WithDateModfiers_UnixTimestamp() const
{
    auto torrent = Torrent::find(4);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    torrent->mergeCasts({
        {"added_on", {CastType::CustomQDateTime, "U"}},
        // These two attributes have also the u_dates defined
        {CREATED_AT, {CastType::CustomQDateTime, "U"}},
        {UPDATED_AT, {CastType::CustomQDateTime, "U"}},
    });

    QByteArray json = torrent->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"({
    "added_on": 1596571870,
    "created_at": 1567584683,
    "hash": "4579e3af2768cdf52ec84c1f320333f68401dc6e",
    "id": 4,
    "name": "test4",
    "note": "after update revert updated_at",
    "progress": 400,
    "size": 14,
    "updated_at": 1609785991,
    "user_id": 1
}
)");

    QCOMPARE(json, expectedJson);

    // Restore
    torrent->resetCasts();
}

void tst_Model_Serialization::toJson_NullQVariant() const
{
    auto type = Type::find(3);
    QVERIFY(type);
    QVERIFY(type->exists);

    QByteArray json = type->toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"({
    "bigint": null,
    "bigint_u": null,
    "binary": null,
    "bool_false": null,
    "bool_true": null,
    "date": null,
    "datetime": null,
    "decimal": null,
    "decimal_down": null,
    "decimal_infinity": null,
    "decimal_nan": null,
    "decimal_up": null,
    "double": null,
    "double_infinity": null,
    "double_nan": null,
    "id": 3,
    "int": null,
    "int_u": null,
    "medium_binary": null,
    "medium_text": null,
    "smallint": null,
    "smallint_u": null,
    "string": null,
    "text": null,
    "time": null,
    "timestamp": null
}
)");

    QCOMPARE(json, expectedJson);
}

void tst_Model_Serialization::toJson_RelationOnly_HasMany() const
{
    auto torrent = Torrent::with("torrentFiles")->find(7);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    ModelsCollection<TorrentPreviewableFile *>
    torrentFiles = torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(torrentFiles.size(), 3);

    QByteArray json = torrentFiles.toJson(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"([
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
]
)");

    QCOMPARE(json, expectedJson);
}

void tst_Model_Serialization::toJson_RelationOnly_BelongsToMany() const
{
    auto user = User::with("roles")->find(1);
    QVERIFY(user);
    QVERIFY(user->exists);

    ModelsCollection<Role *> roles = user->getRelationValue<Role>("roles");
    QCOMPARE(roles.size(), 3);

    QByteArray json = roles.template toJson<RoleUser>(QJsonDocument::Indented);

    auto expectedJson = QByteArrayLiteral(
R"([
    {
        "added_on": "2022-08-01T13:36:56.000Z",
        "id": 1,
        "name": "role one",
        "subscription": {
            "active": true,
            "role_id": 1,
            "user_id": 1
        }
    },
    {
        "added_on": "2022-08-02T13:36:56.000Z",
        "id": 2,
        "name": "role two",
        "subscription": {
            "active": false,
            "role_id": 2,
            "user_id": 1
        }
    },
    {
        "added_on": null,
        "id": 3,
        "name": "role three",
        "subscription": {
            "active": true,
            "role_id": 3,
            "user_id": 1
        }
    }
]
)");

    QCOMPARE(json, expectedJson);
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_Model_Serialization)

#include "tst_model_serialization.moc"
