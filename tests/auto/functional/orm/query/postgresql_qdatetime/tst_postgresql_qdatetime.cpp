#include <QCoreApplication>
#include <QtTest>

#include "orm/db.hpp"
#include "orm/query/querybuilder.hpp"
#include "orm/utils/helpers.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"

using Orm::Constants::ID;
using Orm::Constants::UTC;

using Orm::DB;
using Orm::QtTimeZoneConfig;
using Orm::QtTimeZoneType;

using Helpers = Orm::Utils::Helpers;
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

    /* Null values QDateTime / QDate */
    /* Raw QSqlQuery */
    /* Server timezone UTC */
    void insert_Qt_QDateTime_Null_DatetimeColumn_UtcOnServer() const;
    void insert_Qt_QDate_Null_DateColumn_UtcOnServer() const;

    /* Server timezone +02:00 */
    void insert_Qt_QDateTime_Null_DatetimeColumn_0200OnServer() const;
    void insert_Qt_QDate_Null_DateColumn_0200OnServer() const;

    /* Orm::QueryBuilder */
    /* Server timezone UTC */
    void insert_QDateTime_Null_DatetimeColumn_UtcOnServer() const;
    void insert_QDate_Null_DateColumn_UtcOnServer() const;

    /* Server timezone +02:00 */
    void insert_QDateTime_Null_DatetimeColumn_0200OnServer() const;
    void insert_QDate_Null_DateColumn_0200OnServer() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /* Common */
    /*! Create QueryBuilder instance for the given connection. */
    [[nodiscard]] std::shared_ptr<QueryBuilder> createQuery() const;

    /* QDateTime with/without timezone */
    /*! Set the PostgreSQL timezone session variable to the UTC value. */
    inline void setUtcTimezone() const;
    /*! Set the PostgreSQL timezone session variable to the +02:00 value. */
    inline void setEUBratislavaTimezone() const;
    /*! Set the PostgreSQL timezone session variable to the given value. */
    void setTimezone(const QString &timeZone, QtTimeZoneConfig &&qtTimeZone) const;

    /*! Restore the database after a QDateTime-related test. */
    void restore(quint64 lastId, bool restoreTimezone = false) const;

    /* Data members */
    /*! Connection name used in this test case. */
    QString m_connection {};
};

/* private slots */

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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        qtQuery.addBindValue(QDateTime::fromString("2022-08-28 13:14:15z", Qt::ISODate));

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "datetime" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value("datetime");

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 13:14:15",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        qtQuery.addBindValue(
                    QDateTime::fromString("2022-08-28 13:14:15+02:00", Qt::ISODate));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "datetime" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value("datetime");

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 11:14:15",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::insert_Qt_QString_DatetimeColumn_UtcOnServer() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        qtQuery.addBindValue(QString("2022-08-28 13:14:15"));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "datetime" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value("datetime");

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 13:14:15",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("timestamp") values (?))"));

        qtQuery.addBindValue(QDateTime::fromString("2022-08-29 13:14:15z", Qt::ISODate));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "timestamp" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = qtQuery.value("timestamp");

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-29 13:14:15",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("timestamp") values (?))"));

        qtQuery.addBindValue(
                    QDateTime::fromString("2022-08-29 13:14:15+02:00", Qt::ISODate));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "timestamp" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = qtQuery.value("timestamp");

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-29 11:14:15",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::insert_Qt_QString_TimestampColumn_UtcOnServer() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("timestamp") values (?))"));

        qtQuery.addBindValue(QString("2022-08-29 13:14:15"));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "timestamp" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = qtQuery.value("timestamp");

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-29 13:14:15",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        qtQuery.addBindValue(QDateTime::fromString("2022-08-28 13:14:15z", Qt::ISODate));

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "datetime" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value("datetime");

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 15:14:15",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        qtQuery.addBindValue(
                    QDateTime::fromString("2022-08-28 13:14:15+02:00", Qt::ISODate));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "datetime" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value("datetime");

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 13:14:15",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        qtQuery.addBindValue(QString("2022-08-28 13:14:15"));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "datetime" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value("datetime");

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 13:14:15",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("timestamp") values (?))"));

        qtQuery.addBindValue(QDateTime::fromString("2022-08-29 13:14:15z", Qt::ISODate));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "timestamp" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = qtQuery.value("timestamp");

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-29 15:14:15",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("timestamp") values (?))"));

        qtQuery.addBindValue(
                    QDateTime::fromString("2022-08-29 13:14:15+02:00", Qt::ISODate));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "timestamp" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = qtQuery.value("timestamp");

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-29 13:14:15",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("timestamp") values (?))"));

        qtQuery.addBindValue(QString("2022-08-29 13:14:15"));

        // QPSQL driver converts the QDateTime time zone to UTC
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "timestamp" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = qtQuery.value("timestamp");

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        // QPSQL ignores database TZ and returns the QDateTime in a local timezone
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-29 13:14:15",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
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
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"datetime", QDateTime::fromString("2022-08-28 13:14:15z",
                                                             Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "datetime"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value("datetime");

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 13:14:15z",
                                                            Qt::ISODate);
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
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"datetime", QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                             Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "datetime"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value("datetime");

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 11:14:15z",
                                                            Qt::ISODate);
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
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"datetime", QString("2022-08-28 13:14:15")}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "datetime"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value("datetime");

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 13:14:15z",
                                                            Qt::ISODate);
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
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"timestamp", QDateTime::fromString("2022-08-28 13:14:15z",
                                                              Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "timestamp"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value("timestamp");

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-28 13:14:15z",
                                                             Qt::ISODate);
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
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"timestamp",
                           QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                 Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "timestamp"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value("timestamp");

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-28 11:14:15z",
                                                             Qt::ISODate);
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
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"timestamp", QString("2022-08-28 13:14:15")}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "timestamp"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value("timestamp");

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-28 13:14:15z",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

/* Server timezone Europe/Bratislava (+02:00) */

namespace
{
    /*! Time zone Europe/Bratislava. */
    Q_GLOBAL_STATIC_WITH_ARGS(QTimeZone, TimeZoneEUBratislava,
                              (QByteArray("Europe/Bratislava")));
} // namespace

void tst_PostgreSQL_QDateTime::
insert_QDateTime_UtcTimezone_DatetimeColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"datetime", QDateTime::fromString("2022-08-28 13:14:15z",
                                                             Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "datetime"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value("datetime");

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 15:14:15+02:00",
                                                            Qt::ISODate);
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
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"datetime", QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                             Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "datetime"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value("datetime");

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                            Qt::ISODate);
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
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"datetime", QString("2022-08-28 13:14:15")}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "datetime"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value("datetime");

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                            Qt::ISODate);
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
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"timestamp", QDateTime::fromString("2022-08-28 13:14:15z",
                                                              Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "timestamp"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value("timestamp");

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-28 15:14:15+02:00",
                                                             Qt::ISODate);
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
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"timestamp",
                           QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                 Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "timestamp"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value("timestamp");

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                             Qt::ISODate);
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
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"timestamp", QString("2022-08-28 13:14:15")}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "timestamp"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value("timestamp");

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QPSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                             Qt::ISODate);
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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("date") values (?))"));

        qtQuery.addBindValue(QDate::fromString("2022-08-28", Qt::ISODate));

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "date" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = qtQuery.value("date");

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate::fromString("2022-08-28", Qt::ISODate);
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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("date") values (?))"));

        qtQuery.addBindValue(QString("2022-08-28"));

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "date" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = qtQuery.value("date");

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate::fromString("2022-08-28", Qt::ISODate);
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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("date") values (?))"));

        qtQuery.addBindValue(QDate::fromString("2022-08-28", Qt::ISODate));

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "date" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = qtQuery.value("date");

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate::fromString("2022-08-28", Qt::ISODate);
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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("date") values (?))"));

        qtQuery.addBindValue(QString("2022-08-28"));

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "date" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = qtQuery.value("date");

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate::fromString("2022-08-28", Qt::ISODate);
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
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"date", QDate::fromString("2022-08-28", Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "date"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value("date");

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate::fromString("2022-08-28", Qt::ISODate);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

void tst_PostgreSQL_QDateTime::insert_QString_DateColumn_UtcOnServer() const
{
    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"date", QString("2022-08-28")}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "date"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value("date");

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate::fromString("2022-08-28", Qt::ISODate);
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
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"date", QDate::fromString("2022-08-28", Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "date"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value("date");

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate::fromString("2022-08-28", Qt::ISODate);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true);
}

void tst_PostgreSQL_QDateTime::insert_QString_DateColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"date", QString("2022-08-28")}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "date"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value("date");

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate::fromString("2022-08-28", Qt::ISODate);
        QCOMPARE(dateActual, dateExpected);
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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        qtQuery.addBindValue(QVariant(QMetaType(QMetaType::QDateTime)));

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "datetime" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value("datetime");

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("date") values (?))"));

        qtQuery.addBindValue(QVariant(QMetaType(QMetaType::QDate)));

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "date" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = qtQuery.value("date");

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();

        QCOMPARE(dateActual, dateExpected);
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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        qtQuery.addBindValue(QVariant(QMetaType(QMetaType::QDateTime)));

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "datetime" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value("datetime");

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

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
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("date") values (?))"));

        qtQuery.addBindValue(QVariant(QMetaType(QMetaType::QDate)));

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(
                    R"(select "id", "date" from "datetimes" where "id" = ?)"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = qtQuery.value("date");

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();

        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true);
}

/* Orm::QueryBuilder */

/* Server timezone UTC */

void tst_PostgreSQL_QDateTime::insert_QDateTime_Null_DatetimeColumn_UtcOnServer() const
{
    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"datetime", QVariant(QMetaType(QMetaType::QDateTime))}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "datetime"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value("datetime");

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

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
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"date", QVariant(QMetaType(QMetaType::QDate))}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "date"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value("date");

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();

        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

/* Server timezone +02:00 */

void tst_PostgreSQL_QDateTime::insert_QDateTime_Null_DatetimeColumn_0200OnServer() const
{
    setEUBratislavaTimezone();

    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"datetime", QVariant(QMetaType(QMetaType::QDateTime))}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "datetime"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value("datetime");

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

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
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"date", QVariant(QMetaType(QMetaType::QDate))}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "date"});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value("date");

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();

        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true);
}

/* private */

/* Common */

std::shared_ptr<QueryBuilder>
tst_PostgreSQL_QDateTime::createQuery() const
{
    return DB::connection(m_connection).query();
}

/* QDateTime with/without timezone */

void tst_PostgreSQL_QDateTime::setUtcTimezone() const
{
    setTimezone(UTC, {QtTimeZoneType::QtTimeSpec, QVariant::fromValue(Qt::UTC)});
}

void tst_PostgreSQL_QDateTime::setEUBratislavaTimezone() const
{
    setTimezone(QStringLiteral("Europe/Bratislava"),
                {QtTimeZoneType::QTimeZone, QVariant::fromValue(*TimeZoneEUBratislava)});
}

void tst_PostgreSQL_QDateTime::setTimezone(const QString &timeZone,
                                           QtTimeZoneConfig &&qtTimeZone) const
{
    const auto qtQuery = DB::unprepared(
                             QStringLiteral("set time zone '%1';").arg(timeZone),
                             m_connection);

    QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());

    DB::connection(m_connection).setQtTimeZone(std::move(qtTimeZone));
}

void tst_PostgreSQL_QDateTime::restore(const quint64 lastId,
                                       const bool restoreTimezone) const
{
    const auto [affected, query] = createQuery()->from("datetimes").remove(lastId);

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
