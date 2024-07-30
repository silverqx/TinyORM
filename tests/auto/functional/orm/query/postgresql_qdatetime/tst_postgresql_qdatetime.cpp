#include <QCoreApplication>
#include <QTest>

#ifdef TINYORM_USING_TINYDRIVERS
#  include "orm/drivers/dummysqlerror.hpp"
#endif

#include "orm/db.hpp"
#include "orm/utils/nullvariant.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"
#include "macros.hpp"

using Orm::Constants::ID;
using Orm::Constants::UTC;

using Orm::DB;
using Orm::QtTimeZoneConfig;
using Orm::QtTimeZoneType;
using Orm::TTimeZone;
using Orm::Utils::NullVariant;

using QueryBuilder = Orm::Query::Builder;
using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

class tst_PostgreSQL_QDateTime : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    /* QDateTime with/without timezone */
    /* Raw QSqlQuery */
    /* Server timezone UTC */
    void insert_Qt_QDateTime_UtcTimezone_DatetimeColumn_UtcOnServer() const;
    void insert_Qt_QDateTime_0200Timezone_DatetimeColumn_UtcOnServer() const;
    void insert_Qt_QString_DatetimeColumn_UtcOnServer() const;
    void insert_Qt_QDateTime_UtcTimezone_TimestampColumn_UtcOnServer() const;
    void insert_Qt_QDateTime_0200Timezone_TimestampColumn_UtcOnServer() const;
    void insert_Qt_QString_TimestampColumn_UtcOnServer() const;

    /* Server timezone Europe/Bratislava (+02:00) */
    void insert_Qt_QDateTime_UtcTimezone_DatetimeColumn_0200OnServer() const;
    void insert_Qt_QDateTime_0200Timezone_DatetimeColumn_0200OnServer() const;
    void insert_Qt_QString_DatetimeColumn_0200OnServer() const;
    void insert_Qt_QDateTime_UtcTimezone_TimestampColumn_0200OnServer() const;
    void insert_Qt_QDateTime_0200Timezone_TimestampColumn_0200OnServer() const;
    void insert_Qt_QString_TimestampColumn_0200OnServer() const;

    /* Orm::QueryBuilder */
    /* Server timezone UTC */
    void insert_QDateTime_UtcTimezone_DatetimeColumn_UtcOnServer() const;
    void insert_QDateTime_0200Timezone_DatetimeColumn_UtcOnServer() const;
    void insert_QString_DatetimeColumn_UtcOnServer() const;
    void insert_QDateTime_UtcTimezone_TimestampColumn_UtcOnServer() const;
    void insert_QDateTime_0200Timezone_TimestampColumn_UtcOnServer() const;
    void insert_QString_TimestampColumn_UtcOnServer() const;

    /* Server timezone Europe/Bratislava (+02:00) */
    void insert_QDateTime_UtcTimezone_DatetimeColumn_0200OnServer() const;
    void insert_QDateTime_0200Timezone_DatetimeColumn_0200OnServer() const;
    void insert_QString_DatetimeColumn_0200OnServer() const;
    void insert_QDateTime_UtcTimezone_TimestampColumn_0200OnServer() const;
    void insert_QDateTime_0200Timezone_TimestampColumn_0200OnServer() const;
    void insert_QString_TimestampColumn_0200OnServer() const;

    /* QDate */
    /* Raw QSqlQuery */
    /* Server timezone UTC */
    void insert_Qt_QDate_UtcTimezone_DateColumn_UtcOnServer() const;
    void insert_Qt_QString_DateColumn_UtcOnServer() const;

    /* Server timezone Europe/Bratislava (+02:00) */
    void insert_Qt_QDate_UtcTimezone_DateColumn_0200OnServer() const;
    void insert_Qt_QString_DateColumn_0200OnServer() const;

    /* Orm::QueryBuilder */
    /* Server timezone UTC */
    void insert_QDate_UtcTimezone_DateColumn_UtcOnServer() const;
    void insert_QString_DateColumn_UtcOnServer() const;

    /* Server timezone Europe/Bratislava (+02:00) */
    void insert_QDate_UtcTimezone_DateColumn_0200OnServer() const;
    void insert_QString_DateColumn_0200OnServer() const;

    /* QTime */
    /* Raw QSqlQuery */
    /* Server timezone UTC */
    void insert_Qt_QTime_UtcTimezone_TimeColumn_UtcOnServer() const;
    void insert_Qt_QString_TimeColumn_UtcOnServer() const;

    /* Server timezone Europe/Bratislava (+02:00) */
    void insert_Qt_QTime_UtcTimezone_TimeColumn_0200OnServer() const;
    void insert_Qt_QString_TimeColumn_0200OnServer() const;

    /* Orm::QueryBuilder */
    /* Server timezone UTC */
    void insert_QTime_UtcTimezone_TimeColumn_UtcOnServer() const;
    void insert_QString_TimeColumn_UtcOnServer() const;

    /* Server timezone Europe/Bratislava (+02:00) */
    void insert_QTime_UtcTimezone_TimeColumn_0200OnServer() const;
    void insert_QString_TimeColumn_0200OnServer() const;

    /* Null values QDateTime / QDate */
    /* Raw QSqlQuery */
    /* Server timezone UTC */
    void insert_Qt_QDateTime_Null_DatetimeColumn_UtcOnServer() const;
    void insert_Qt_QDate_Null_DateColumn_UtcOnServer() const;
    void insert_Qt_QTime_Null_TimeColumn_UtcOnServer() const;

    /* Server timezone +02:00 */
    void insert_Qt_QDateTime_Null_DatetimeColumn_0200OnServer() const;
    void insert_Qt_QDate_Null_DateColumn_0200OnServer() const;
    void insert_Qt_QTime_Null_TimeColumn_0200OnServer() const;

    /* Orm::QueryBuilder */
    /* Server timezone UTC */
    void insert_QDateTime_Null_DatetimeColumn_UtcOnServer() const;
    void insert_QDate_Null_DateColumn_UtcOnServer() const;
    void insert_QTime_Null_TimeColumn_UtcOnServer() const;

    /* Server timezone +02:00 */
    void insert_QDateTime_Null_DatetimeColumn_0200OnServer() const;
    void insert_QDate_Null_DateColumn_0200OnServer() const;
    void insert_QTime_Null_TimeColumn_0200OnServer() const;

    /* QtTimeZoneType::DontConvert */
    /* Orm::QueryBuilder */
    /* Server timezone UTC */
    void insert_QDateTime_0300Timezone_DatetimeColumn_UtcOnServer_DontConvert() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /* Common */
    /*! Create a QueryBuilder instance for the given connection. */
    [[nodiscard]] inline std::shared_ptr<QueryBuilder> createQuery() const;
    /*! Create a raw QSqlQuery instance for the given connection. */
    [[nodiscard]] inline TSqlQuery createSqlQuery() const;

    /* QDateTime with/without timezone */
    /*! Set the PostgreSQL timezone session variable to the UTC value. */
    inline void setUtcTimezone() const;
    /*! Set the PostgreSQL timezone session variable to the +02:00 value. */
    inline void setEUBratislavaTimezone() const;
    /*! Set the qt_timezone to the QtTimeZoneType::DontConvert value. */
    inline void setDontConvertTimezone() const;
    /*! Set the PostgreSQL timezone session variable to the given value. */
    void setTimezone(const QString &timeZone, QtTimeZoneConfig &&qtTimeZone) const;

    /*! Restore the database after a QDateTime-related test. */
    void restore(quint64 lastId, bool restoreTimezone = false) const;

    /* Data members */
    /*! Connection name used in this test case. */
    QString m_connection;
};

/*! Time zone Europe/Bratislava. */
Q_GLOBAL_STATIC_WITH_ARGS(const QTimeZone, TimeZoneEUBratislava, // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
                          (QByteArray("Europe/Bratislava")))

/*! QString constant "datetimes" (perf. reason, one time initialization). */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, datetimes, ("datetimes")) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
/*! QString constant "datetime" (perf. reason, one time initialization). */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, datetime, ("datetime")) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
/*! QString constant "timestamp" (perf. reason, one time initialization). */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, timestamp, ("timestamp")) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
/*! QString constant "date" (perf. reason, one time initialization). */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, date, ("date")) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
/*! QString constant "time" (perf. reason, one time initialization). */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, time_, ("time")) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_PostgreSQL_QDateTime::initTestCase()
{
    m_connection = Databases::createConnection(Databases::POSTGRESQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );
}

/* QDateTime with/without timezone */

/* Raw QSqlQuery */

/* Server timezone UTC */
void tst_PostgreSQL_QDateTime::
insert_Qt_QDateTime_UtcTimezone_DatetimeColumn_UtcOnServer() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        sqlQuery.addBindValue(QDateTime({2022, 8, 28}, {13, 14, 15}, TTimeZone::UTC));

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "datetime" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = sqlQuery.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                QTimeZone::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
#else
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15});

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
#endif
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::
insert_Qt_QDateTime_0200Timezone_DatetimeColumn_UtcOnServer() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        sqlQuery.addBindValue(
                    QDateTime({2022, 8, 28}, {13, 14, 15}, *TimeZoneEUBratislava));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "datetime" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = sqlQuery.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {11, 14, 15},
                                                QTimeZone::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
#else
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {11, 14, 15});

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
#endif
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::insert_Qt_QString_DatetimeColumn_UtcOnServer() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        sqlQuery.addBindValue(QString("2022-08-28 13:14:15"));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "datetime" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = sqlQuery.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                QTimeZone::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
#else
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15});

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
#endif
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::
insert_Qt_QDateTime_UtcTimezone_TimestampColumn_UtcOnServer() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("timestamp") values (?))"));

        sqlQuery.addBindValue(QDateTime({2022, 8, 29}, {13, 14, 15}, TTimeZone::UTC));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "timestamp" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = sqlQuery.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        const auto timestampExpected = QDateTime({2022, 8, 29}, {13, 14, 15},
                                                 QTimeZone::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
#else
        const auto timestampExpected = QDateTime({2022, 8, 29}, {13, 14, 15});

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
#endif
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::
insert_Qt_QDateTime_0200Timezone_TimestampColumn_UtcOnServer() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("timestamp") values (?))"));

        sqlQuery.addBindValue(
                    QDateTime({2022, 8, 29}, {13, 14, 15}, *TimeZoneEUBratislava));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "timestamp" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = sqlQuery.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        const auto timestampExpected = QDateTime({2022, 8, 29}, {11, 14, 15},
                                                 QTimeZone::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
#else
        const auto timestampExpected = QDateTime({2022, 8, 29}, {11, 14, 15});

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
#endif
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::insert_Qt_QString_TimestampColumn_UtcOnServer() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("timestamp") values (?))"));

        sqlQuery.addBindValue(QString("2022-08-29 13:14:15"));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "timestamp" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = sqlQuery.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        const auto timestampExpected = QDateTime({2022, 8, 29}, {13, 14, 15},
                                                 QTimeZone::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
#else
        const auto timestampExpected = QDateTime({2022, 8, 29}, {13, 14, 15});

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
#endif
    }

    // Restore
    restore(lastId);
}

/* Server timezone Europe/Bratislava (+02:00) */

void tst_PostgreSQL_QDateTime::
insert_Qt_QDateTime_UtcTimezone_DatetimeColumn_0200OnServer() const
{
    setEUBratislavaTimezone();
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        sqlQuery.addBindValue(QDateTime({2022, 8, 28}, {13, 14, 15}, TTimeZone::UTC));

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "datetime" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = sqlQuery.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {15, 14, 15},
                                                QTimeZone::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
#else
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {15, 14, 15});

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
#endif
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::
insert_Qt_QDateTime_0200Timezone_DatetimeColumn_0200OnServer() const
{
    setEUBratislavaTimezone();
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        sqlQuery.addBindValue(
                    QDateTime({2022, 8, 28}, {13, 14, 15}, *TimeZoneEUBratislava));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "datetime" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = sqlQuery.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                QTimeZone::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
#else
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15});

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
#endif
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::insert_Qt_QString_DatetimeColumn_0200OnServer() const
{
    setEUBratislavaTimezone();
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        sqlQuery.addBindValue(QString("2022-08-28 13:14:15"));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "datetime" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = sqlQuery.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                QTimeZone::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
#else
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15});

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
#endif
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::
insert_Qt_QDateTime_UtcTimezone_TimestampColumn_0200OnServer() const
{
    setEUBratislavaTimezone();
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("timestamp") values (?))"));

        sqlQuery.addBindValue(QDateTime({2022, 8, 29}, {13, 14, 15}, TTimeZone::UTC));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "timestamp" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = sqlQuery.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        const auto timestampExpected = QDateTime({2022, 8, 29}, {15, 14, 15},
                                                 QTimeZone::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
#else
        const auto timestampExpected = QDateTime({2022, 8, 29}, {15, 14, 15});

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
#endif
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::
insert_Qt_QDateTime_0200Timezone_TimestampColumn_0200OnServer() const
{
    setEUBratislavaTimezone();
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("timestamp") values (?))"));

        sqlQuery.addBindValue(
                    QDateTime({2022, 8, 29}, {13, 14, 15}, *TimeZoneEUBratislava));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "timestamp" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = sqlQuery.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        const auto timestampExpected = QDateTime({2022, 8, 29}, {13, 14, 15},
                                                 QTimeZone::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
#else
        const auto timestampExpected = QDateTime({2022, 8, 29}, {13, 14, 15});

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
#endif
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::insert_Qt_QString_TimestampColumn_0200OnServer() const
{
    setEUBratislavaTimezone();
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("timestamp") values (?))"));

        sqlQuery.addBindValue(QString("2022-08-29 13:14:15"));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "timestamp" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = sqlQuery.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        const auto timestampExpected = QDateTime({2022, 8, 29}, {13, 14, 15},
                                                 QTimeZone::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
#else
        const auto timestampExpected = QDateTime({2022, 8, 29}, {13, 14, 15});

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
#endif
    }

    // Restore
    restore(lastId, true);
}

/* Orm::QueryBuilder */

/* Server timezone UTC */

void tst_PostgreSQL_QDateTime::
insert_QDateTime_UtcTimezone_DatetimeColumn_UtcOnServer() const
{
    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*datetime,
                           QDateTime({2022, 8, 28}, {13, 14, 15}, TTimeZone::UTC)}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                TTimeZone::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::
insert_QDateTime_0200Timezone_DatetimeColumn_UtcOnServer() const
{
    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*datetime,
                           QDateTime({2022, 8, 28}, {13, 14, 15},
                                     *TimeZoneEUBratislava)}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {11, 14, 15},
                                                TTimeZone::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::insert_QString_DatetimeColumn_UtcOnServer() const
{
    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*datetime, QString("2022-08-28 13:14:15")}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                TTimeZone::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::
insert_QDateTime_UtcTimezone_TimestampColumn_UtcOnServer() const
{
    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*timestamp,
                           QDateTime({2022, 8, 28}, {13, 14, 15}, TTimeZone::UTC)}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *timestamp});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                 TTimeZone::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::
insert_QDateTime_0200Timezone_TimestampColumn_UtcOnServer() const
{
    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*timestamp,
                           QDateTime({2022, 8, 28}, {13, 14, 15},
                                     *TimeZoneEUBratislava)}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *timestamp});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {11, 14, 15},
                                                 TTimeZone::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::insert_QString_TimestampColumn_UtcOnServer() const
{
    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*timestamp, QString("2022-08-28 13:14:15")}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *timestamp});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                 TTimeZone::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

/* Server timezone Europe/Bratislava (+02:00) */

void tst_PostgreSQL_QDateTime::
insert_QDateTime_UtcTimezone_DatetimeColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*datetime,
                           QDateTime({2022, 8, 28}, {13, 14, 15}, TTimeZone::UTC)}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {15, 14, 15},
                                                *TimeZoneEUBratislava);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), *TimeZoneEUBratislava);
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::
insert_QDateTime_0200Timezone_DatetimeColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*datetime,
                           QDateTime({2022, 8, 28}, {13, 14, 15},
                                     *TimeZoneEUBratislava)}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                *TimeZoneEUBratislava);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), *TimeZoneEUBratislava);
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::insert_QString_DatetimeColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*datetime, QString("2022-08-28 13:14:15")}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                *TimeZoneEUBratislava);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), *TimeZoneEUBratislava);
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::
insert_QDateTime_UtcTimezone_TimestampColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*timestamp,
                           QDateTime({2022, 8, 28}, {13, 14, 15}, TTimeZone::UTC)}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *timestamp});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {15, 14, 15},
                                                 *TimeZoneEUBratislava);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), *TimeZoneEUBratislava);
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::
insert_QDateTime_0200Timezone_TimestampColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*timestamp,
                           QDateTime({2022, 8, 28}, {13, 14, 15},
                                     *TimeZoneEUBratislava)}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *timestamp});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                 *TimeZoneEUBratislava);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), *TimeZoneEUBratislava);
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::insert_QString_TimestampColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*timestamp, QString("2022-08-28 13:14:15")}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *timestamp});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                 *TimeZoneEUBratislava);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), *TimeZoneEUBratislava);
    }

    // Restore
    restore(lastId, true);
}

/* QDate */

/* Raw QSqlQuery */

/* Server timezone UTC */

void
tst_PostgreSQL_QDateTime::insert_Qt_QDate_UtcTimezone_DateColumn_UtcOnServer() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("date") values (?))"));

        sqlQuery.addBindValue(QDate(2022, 8, 28));

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "date" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = sqlQuery.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(dateDbVariant.typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::insert_Qt_QString_DateColumn_UtcOnServer() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("date") values (?))"));

        sqlQuery.addBindValue(QString("2022-08-28"));

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "date" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = sqlQuery.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(dateDbVariant.typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

/* Server timezone Europe/Bratislava (+02:00) */

void tst_PostgreSQL_QDateTime::
insert_Qt_QDate_UtcTimezone_DateColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("date") values (?))"));

        sqlQuery.addBindValue(QDate(2022, 8, 28));

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "date" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = sqlQuery.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(dateDbVariant.typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::insert_Qt_QString_DateColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("date") values (?))"));

        sqlQuery.addBindValue(QString("2022-08-28"));

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "date" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = sqlQuery.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(dateDbVariant.typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true);
}

/* Orm::QueryBuilder */

/* Server timezone UTC */

void tst_PostgreSQL_QDateTime::insert_QDate_UtcTimezone_DateColumn_UtcOnServer() const
{
    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*date, QDate(2022, 8, 28)}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *date});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(dateDbVariant.typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::insert_QString_DateColumn_UtcOnServer() const
{
    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*date, QString("2022-08-28")}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *date});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(dateDbVariant.typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

/* Server timezone Europe/Bratislava (+02:00) */

void tst_PostgreSQL_QDateTime::
insert_QDate_UtcTimezone_DateColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*date, QDate(2022, 8, 28)}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *date});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(dateDbVariant.typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::insert_QString_DateColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*date, QString("2022-08-28")}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *date});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(dateDbVariant.typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true);
}

/* QTime */

/* Raw QSqlQuery */

/* Server timezone UTC */

void
tst_PostgreSQL_QDateTime::insert_Qt_QTime_UtcTimezone_TimeColumn_UtcOnServer() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("time") values (?))"));

        sqlQuery.addBindValue(QTime(17, 2, 59));

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "time" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = sqlQuery.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(!timeDbVariant.isNull());

        QCOMPARE(timeDbVariant.typeId(), QMetaType::QTime);

        const auto timeActual = timeDbVariant.value<QTime>();
        const auto timeExpected = QTime(17, 2, 59);
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::insert_Qt_QString_TimeColumn_UtcOnServer() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("time") values (?))"));

        sqlQuery.addBindValue(QString("17:02:59"));

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "time" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = sqlQuery.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(!timeDbVariant.isNull());

        QCOMPARE(timeDbVariant.typeId(), QMetaType::QTime);

        const auto timeActual = timeDbVariant.value<QTime>();
        const auto timeExpected = QTime(17, 2, 59);
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(lastId);
}

/* Server timezone Europe/Bratislava (+02:00) */

void tst_PostgreSQL_QDateTime::
insert_Qt_QTime_UtcTimezone_TimeColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("time") values (?))"));

        sqlQuery.addBindValue(QTime(17, 2, 59));

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "time" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = sqlQuery.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(!timeDbVariant.isNull());

        QCOMPARE(timeDbVariant.typeId(), QMetaType::QTime);

        const auto timeActual = timeDbVariant.value<QTime>();
        const auto timeExpected = QTime(17, 2, 59);
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::insert_Qt_QString_TimeColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("time") values (?))"));

        sqlQuery.addBindValue(QString("17:02:59"));

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "time" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = sqlQuery.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(!timeDbVariant.isNull());

        QCOMPARE(timeDbVariant.typeId(), QMetaType::QTime);

        const auto timeActual = timeDbVariant.value<QTime>();
        const auto timeExpected = QTime(17, 2, 59);
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(lastId, true);
}

/* Orm::QueryBuilder */

/* Server timezone UTC */

void tst_PostgreSQL_QDateTime::insert_QTime_UtcTimezone_TimeColumn_UtcOnServer() const
{
    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*time_, QTime(17, 2, 59)}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *time_});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = query.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(!timeDbVariant.isNull());

        QCOMPARE(timeDbVariant.typeId(), QMetaType::QTime);

        const auto timeActual = timeDbVariant.value<QTime>();
        const auto timeExpected = QTime(17, 2, 59);
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::insert_QString_TimeColumn_UtcOnServer() const
{
    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*time_, QString("17:02:59")}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *time_});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = query.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(!timeDbVariant.isNull());

        QCOMPARE(timeDbVariant.typeId(), QMetaType::QTime);

        const auto timeActual = timeDbVariant.value<QTime>();
        const auto timeExpected = QTime(17, 2, 59);
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(lastId);
}

/* Server timezone Europe/Bratislava (+02:00) */

void tst_PostgreSQL_QDateTime::
insert_QTime_UtcTimezone_TimeColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*time_, QTime(17, 2, 59)}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *time_});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = query.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(!timeDbVariant.isNull());

        QCOMPARE(timeDbVariant.typeId(), QMetaType::QTime);

        const auto timeActual = timeDbVariant.value<QTime>();
        const auto timeExpected = QTime(17, 2, 59);
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::insert_QString_TimeColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*time_, QString("17:02:59")}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *time_});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = query.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(!timeDbVariant.isNull());

        QCOMPARE(timeDbVariant.typeId(), QMetaType::QTime);

        const auto timeActual = timeDbVariant.value<QTime>();
        const auto timeExpected = QTime(17, 2, 59);
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(lastId, true);
}

/* Null values QDateTime / QDate */

/* Raw QSqlQuery */

/* Server timezone UTC */

void tst_PostgreSQL_QDateTime::
insert_Qt_QDateTime_Null_DatetimeColumn_UtcOnServer() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        sqlQuery.addBindValue(NullVariant::QDateTime());

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "datetime" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = sqlQuery.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* TZ is irrelevant for null values, but I will check them anyway, if something
           weird happens and TZ changes then test fail, so I will know about that. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime();

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::insert_Qt_QDate_Null_DateColumn_UtcOnServer() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("date") values (?))"));

        sqlQuery.addBindValue(NullVariant::QDate());

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "date" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = sqlQuery.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(dateDbVariant.isNull());

        QCOMPARE(dateDbVariant.typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::insert_Qt_QTime_Null_TimeColumn_UtcOnServer() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("time") values (?))"));

        sqlQuery.addBindValue(NullVariant::QTime());

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "time" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = sqlQuery.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(timeDbVariant.isNull());

        QCOMPARE(timeDbVariant.typeId(), QMetaType::QTime);

        const auto timeActual = timeDbVariant.value<QTime>();
        const auto timeExpected = QTime();
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(lastId);
}

/* Server timezone +02:00 */

void tst_PostgreSQL_QDateTime::
insert_Qt_QDateTime_Null_DatetimeColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        sqlQuery.addBindValue(NullVariant::QDateTime());

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "datetime" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = sqlQuery.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* TZ is irrelevant for null values, but I will check them anyway, if something
           weird happens and TZ changes then test fail, so I will know about that. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime();

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::insert_Qt_QDate_Null_DateColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("date") values (?))"));

        sqlQuery.addBindValue(NullVariant::QDate());

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "date" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = sqlQuery.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(dateDbVariant.isNull());

        QCOMPARE(dateDbVariant.typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::insert_Qt_QTime_Null_TimeColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(R"(insert into "datetimes" ("time") values (?))"));

        sqlQuery.addBindValue(NullVariant::QTime());

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery();

        QVERIFY(sqlQuery.prepare(
                    R"(select "id", "time" from "datetimes" where "id" = ?)"));

        sqlQuery.addBindValue(lastId);

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && sqlQuery.isSelect());
        QCOMPARE(sqlQuery.size(), 1);

        QVERIFY(sqlQuery.first());

        QCOMPARE(sqlQuery.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = sqlQuery.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(timeDbVariant.isNull());

        QCOMPARE(timeDbVariant.typeId(), QMetaType::QTime);

        const auto timeActual = timeDbVariant.value<QTime>();
        const auto timeExpected = QTime();
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(lastId, true);
}

/* Orm::QueryBuilder */

/* Server timezone UTC */

void tst_PostgreSQL_QDateTime::insert_QDateTime_Null_DatetimeColumn_UtcOnServer() const
{
    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*datetime, NullVariant::QDateTime()}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* TZ is irrelevant for null values, but I will check them anyway, if something
           weird happens and TZ changes then test fail, so I will know about that. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime();

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::insert_QDate_Null_DateColumn_UtcOnServer() const
{
    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*date, NullVariant::QDate()}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *date});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(dateDbVariant.isNull());

        QCOMPARE(dateDbVariant.typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::insert_QTime_Null_TimeColumn_UtcOnServer() const
{
    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*time_, NullVariant::QTime()}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *time_});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = query.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(timeDbVariant.isNull());

        QCOMPARE(timeDbVariant.typeId(), QMetaType::QTime);

        const auto timeActual = timeDbVariant.value<QTime>();
        const auto timeExpected = QTime();
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(lastId);
}

/* Server timezone +02:00 */

void tst_PostgreSQL_QDateTime::insert_QDateTime_Null_DatetimeColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*datetime, NullVariant::QDateTime()}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* TZ is irrelevant for null values, but I will check them anyway, if something
           weird happens and TZ changes then test fail, so I will know about that. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime();

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::insert_QDate_Null_DateColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*date, NullVariant::QDate()}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *date});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(dateDbVariant.isNull());

        QCOMPARE(dateDbVariant.typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::insert_QTime_Null_TimeColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*time_, NullVariant::QTime()}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *time_});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = query.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(timeDbVariant.isNull());

        QCOMPARE(timeDbVariant.typeId(), QMetaType::QTime);

        const auto timeActual = timeDbVariant.value<QTime>();
        const auto timeExpected = QTime();
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(lastId, true);
}

/* QtTimeZoneType::DontConvert */

/* Orm::QueryBuilder */

/* Server timezone UTC */

void tst_PostgreSQL_QDateTime::
insert_QDateTime_0300Timezone_DatetimeColumn_UtcOnServer_DontConvert() const
{
    setDontConvertTimezone();

    QCOMPARE(DB::qtTimeZone(m_connection),
             QtTimeZoneConfig {QtTimeZoneType::DontConvert});

    /* The QDateTime's time zone is ignored with the QtTimeZoneType::DontConvert
       connection option, only toString(m_queryGrammar->getDateFormat()) is applied. */

    // Insert
    quint64 lastId = createQuery()->from(*datetimes).insertGetId(
                         {{*datetime,
                           QDateTime({2022, 8, 28}, {13, 14, 15},
                                     *TimeZoneEUBratislava)}});

    // Verify
    {
        auto query = createQuery()->from(*datetimes).find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        // Practically it should behave as is the default QPSQL driver behavior
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                QTimeZone::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
#else
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15});

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
#endif
    }

    // Restore
    restore(lastId, true);
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

/* Common */

std::shared_ptr<QueryBuilder>
tst_PostgreSQL_QDateTime::createQuery() const
{
    return DB::query(m_connection);
}

TSqlQuery tst_PostgreSQL_QDateTime::createSqlQuery() const
{
    return DB::sqlQuery(m_connection);
}

/* QDateTime with/without timezone */

void tst_PostgreSQL_QDateTime::setUtcTimezone() const
{
    setTimezone(UTC, QtTimeZoneConfig::utc());
}

void tst_PostgreSQL_QDateTime::setEUBratislavaTimezone() const
{
    setTimezone(sl("Europe/Bratislava"),
                {QtTimeZoneType::QTimeZone, QVariant::fromValue(*TimeZoneEUBratislava)});
}

void tst_PostgreSQL_QDateTime::setDontConvertTimezone() const
{
    setTimezone(UTC, {QtTimeZoneType::DontConvert});
}

void tst_PostgreSQL_QDateTime::setTimezone(const QString &timeZone,
                                           QtTimeZoneConfig &&qtTimeZone) const
{
    const auto sqlQuery = DB::unprepared(sl("set time zone '%1';").arg(timeZone),
                                        m_connection);

    QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());

    DB::connection(m_connection).setQtTimeZone(std::move(qtTimeZone));
}

void tst_PostgreSQL_QDateTime::restore(const quint64 lastId,
                                       const bool restoreTimezone) const
{
    const auto [affected, query] = createQuery()->from(*datetimes).remove(lastId);

    QVERIFY(!query.lastError().isValid());
    QVERIFY(!query.isValid() && query.isActive() && !query.isSelect());
    QCOMPARE(affected, 1);

    if (!restoreTimezone)
        return;

    /* Restore also the PostgreSQL timezone session variable to auto tests default UTC
       value. */
    setUtcTimezone();
}

QTEST_MAIN(tst_PostgreSQL_QDateTime)

#include "tst_postgresql_qdatetime.moc"
