#include <QCoreApplication>
#include <QtTest>

#include "orm/db.hpp"
#include "orm/query/querybuilder.hpp"
#include "orm/sqliteconnection.hpp"
#include "orm/utils/helpers.hpp"
#include "orm/utils/nullvariant.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"

using Orm::Constants::ID;

using Orm::DB;
using Orm::QtTimeZoneConfig;
using Orm::QtTimeZoneType;
using Orm::SQLiteConnection;
using Orm::Utils::Helpers;
using Orm::Utils::NullVariant;

using QueryBuilder = Orm::Query::Builder;
using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

class tst_SQLite_QDateTime : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    /* QDateTime with/without timezone */
    /* Raw QSqlQuery */
    /* Server timezone None */
    void insert_Qt_QDateTime_UtcTimezone_DatetimeColumn() const;
    void insert_Qt_QDateTime_0200Timezone_DatetimeColumn() const;
    void insert_Qt_QString_DatetimeColumn() const;
    // SQLite column type : timestamp == datetime == string
    void insert_Qt_QDateTime_UtcTimezone_TimestampColumn() const;
    void insert_Qt_QDateTime_0200Timezone_TimestampColumn() const;
    void insert_Qt_QString_TimestampColumn() const;

    /* Server timezone +02:00 - not supported by the SQLite database */

    /* Orm::QueryBuilder */
    /* Server timezone None and enabled return_qdatetime */
    void insert_QDateTime_UtcTimezone_DatetimeColumn_OnReturnQDateTime() const;
    void insert_QDateTime_0200Timezone_DatetimeColumn_OnReturnQDateTime() const;
    void insert_QString_DatetimeColumn_OnReturnQDateTime() const;
    // SQLite column type : timestamp == datetime == string
    void insert_QDateTime_UtcTimezone_TimestampColumn_OnReturnQDateTime() const;
    void insert_QDateTime_0200Timezone_TimestampColumn_OnReturnQDateTime() const;
    void insert_QString_TimestampColumn_OnReturnQDateTime() const;

    /* Server timezone None and disabled return_qdatetime */
    void insert_QDateTime_UtcTimezone_DatetimeColumn_OffReturnQDateTime() const;
    void insert_QDateTime_0200Timezone_DatetimeColumn_OffReturnQDateTime() const;
    void insert_QString_DatetimeColumn_OffReturnQDateTime() const;
    // SQLite column type : timestamp == datetime == string
    void insert_QDateTime_UtcTimezone_TimestampColumn_OffReturnQDateTime() const;
    void insert_QDateTime_0200Timezone_TimestampColumn_OffReturnQDateTime() const;
    void insert_QString_TimestampColumn_OffReturnQDateTime() const;

    /* Server timezone +02:00 - not supported by the SQLite database */

    /* QDate */
    /* Raw QSqlQuery */
    /* Server timezone None */
    void insert_Qt_QDate_UtcTimezone_DateColumn() const;
    void insert_Qt_QString_DateColumn() const;

    /* Server timezone +02:00 - not supported by the SQLite database */

    /* Orm::QueryBuilder */
    /* Server timezone None and enabled return_qdatetime */
    void insert_QDate_UtcTimezone_DateColumn_OnReturnQDateTime() const;
    void insert_QString_DateColumn_OnReturnQDateTime() const;

    /* Server timezone None and disabled return_qdatetime */
    void insert_QDate_UtcTimezone_DateColumn_OffReturnQDateTime() const;
    void insert_QString_DateColumn_OffReturnQDateTime() const;

    /* Server timezone +02:00 - not supported by the SQLite database */

    /* Null values QDateTime / QDate */
    /* Raw QSqlQuery */
    /* Server timezone None */
    void insert_Qt_QDateTime_Null_DatetimeColumn() const;
    void insert_Qt_QDate_Null_DateColumn() const;

    /* Server timezone +02:00 - not supported by the SQLite database */

    /* Orm::QueryBuilder */
    /* Server timezone None */
    void insert_QDateTime_Null_DatetimeColumn() const;
    void insert_QDate_Null_DateColumn() const;

    /* Server timezone +02:00 - not supported by the SQLite database */

    /* QtTimeZoneType::DontConvert */
    /* Orm::QueryBuilder */
    /* Server timezone None */
    void insert_QDateTime_0300Timezone_DatetimeColumn_UtcOnServer_DontConvert() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /* Common */
    /*! Create QueryBuilder instance for the given connection. */
    [[nodiscard]] std::shared_ptr<QueryBuilder> createQuery() const;

    /* QDateTime with/without timezone */
    /*! Restore the database after a QDateTime-related test. */
    void restore(quint64 lastId) const;
    /*! Disable the return_qdatetime for the current connection. */
    void disableReturnQDateTime() const;
    /*! Enable the return_qdatetime for the current connection. */
    void enableReturnQDateTime() const;

    /* Data members */
    /*! Connection name used in this test case. */
    QString m_connection {};
};

/* private slots */

void tst_SQLite_QDateTime::initTestCase()
{
    m_connection = Databases::createConnection(Databases::SQLITE);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::SQLITE)
              .toUtf8().constData(), );
}

namespace
{
    /* UTC+02:00 doesn't work here, because I'm not fixing TZ for QSQLITE driver and
       the QDateTime returns the UTC+02 TZ from QDateTime created using
       the QVariant(QString).value<QDateTime>() and
       QTimeZone(UTC+02) != QTimeZone(UTC+02:00). */
    /*! Time zone +02:00. */
    Q_GLOBAL_STATIC_WITH_ARGS(QTimeZone, TimeZone0200, (QByteArray("UTC+02")));
} // namespace

/* QDateTime with/without timezone */

/* Raw QSqlQuery */

/* Server timezone None */
void tst_SQLite_QDateTime::
insert_Qt_QDateTime_UtcTimezone_DatetimeColumn() const
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

        /* QSQLITE driver takes the QDateTime timezone into account, it internally calls
           .toString(Qt::ISODateWithMs). */
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value("datetime");
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QString);

        /* QSQLITE driver returns a string of a datetime with a timezone that was sent
           to the database in unchanged form. */
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

void tst_SQLite_QDateTime::
insert_Qt_QDateTime_0200Timezone_DatetimeColumn() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        qtQuery.addBindValue(
                    QDateTime::fromString("2022-08-28 13:14:15+02:00", Qt::ISODate));

        /* QSQLITE driver takes the QDateTime timezone into account, it internally calls
           .toString(Qt::ISODateWithMs). */
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

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value("datetime");
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QString);

        /* QSQLITE driver returns a string of a datetime with a timezone that was sent
           to the database in unchanged form. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 13:14:15+02",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), *TimeZone0200);
    }

    // Restore
    restore(lastId);
}

void tst_SQLite_QDateTime::insert_Qt_QString_DatetimeColumn() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        qtQuery.addBindValue(QString("2022-08-28 13:14:15"));

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

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value("datetime");
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QString);

        /* QSQLITE driver returns a string of a datetime that was sent to the database
           in unchanged form. */
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

void tst_SQLite_QDateTime::
insert_Qt_QDateTime_UtcTimezone_TimestampColumn() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("timestamp") values (?))"));

        qtQuery.addBindValue(QDateTime::fromString("2022-08-29 13:14:15z", Qt::ISODate));

        /* QSQLITE driver takes the QDateTime timezone into account, it internally calls
           .toString(Qt::ISODateWithMs). */
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

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = qtQuery.value("timestamp");
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QString);

        /* QSQLITE driver returns a string of a datetime with a timezone that was sent
           to the database in unchanged form. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-29 13:14:15z",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

void tst_SQLite_QDateTime::
insert_Qt_QDateTime_0200Timezone_TimestampColumn() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("timestamp") values (?))"));

        qtQuery.addBindValue(
                    QDateTime::fromString("2022-08-29 13:14:15+02:00", Qt::ISODate));

        /* QSQLITE driver takes the QDateTime timezone into account, it internally calls
           .toString(Qt::ISODateWithMs). */
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

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = qtQuery.value("timestamp");
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QString);

        /* QSQLITE driver returns a string of a datetime with a timezone that was sent
           to the database in unchanged form. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-29 13:14:15+02:00",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), *TimeZone0200);
    }

    // Restore
    restore(lastId);
}

void tst_SQLite_QDateTime::insert_Qt_QString_TimestampColumn() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("timestamp") values (?))"));

        qtQuery.addBindValue(QString("2022-08-29 13:14:15"));

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

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = qtQuery.value("timestamp");
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QString);

        /* QSQLITE driver returns a string of a datetime that was sent to the database
           in unchanged form. */
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

/* Orm::QueryBuilder */

/* Server timezone None and enabled return_qdatetime */

void tst_SQLite_QDateTime::
insert_QDateTime_UtcTimezone_DatetimeColumn_OnReturnQDateTime() const
{
    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"datetime", QDateTime::fromString("2022-08-28 13:14:15z",
                                                             Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "datetime"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value("datetime");
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        // TinyORM QueryBuilder fixes this and returns the QDateTime instead of QString
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

void tst_SQLite_QDateTime::
insert_QDateTime_0200Timezone_DatetimeColumn_OnReturnQDateTime() const
{
    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"datetime", QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                             Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "datetime"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value("datetime");
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        // TinyORM QueryBuilder fixes this and returns the QDateTime instead of QString
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

void tst_SQLite_QDateTime::insert_QString_DatetimeColumn_OnReturnQDateTime() const
{
    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"datetime", QString("2022-08-28 13:14:15")}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "datetime"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value("datetime");
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        // TinyORM QueryBuilder fixes this and returns the QDateTime instead of QString
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

void tst_SQLite_QDateTime::
insert_QDateTime_UtcTimezone_TimestampColumn_OnReturnQDateTime() const
{
    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"timestamp", QDateTime::fromString("2022-08-28 13:14:15z",
                                                              Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "timestamp"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value("timestamp");
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        // TinyORM QueryBuilder fixes this and returns the QDateTime instead of QString
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

void tst_SQLite_QDateTime::
insert_QDateTime_0200Timezone_TimestampColumn_OnReturnQDateTime() const
{
    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"timestamp",
                           QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                 Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "timestamp"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value("timestamp");
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        // TinyORM QueryBuilder fixes this and returns the QDateTime instead of QString
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

void tst_SQLite_QDateTime::insert_QString_TimestampColumn_OnReturnQDateTime() const
{
    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"timestamp", QString("2022-08-28 13:14:15")}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "timestamp"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value("timestamp");
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        // TinyORM QueryBuilder fixes this and returns the QDateTime instead of QString
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

/* Server timezone None and disabled return_qdatetime */

void tst_SQLite_QDateTime::
insert_QDateTime_UtcTimezone_DatetimeColumn_OffReturnQDateTime() const
{
    disableReturnQDateTime();

    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"datetime", QDateTime::fromString("2022-08-28 13:14:15z",
                                                             Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "datetime"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value("datetime");
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QString);

        /* Returning the QDateTime (fixed behavior by the TinyORM QueryBuilder) can be
           disabled by the return_qdatetime = false. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 13:14:15",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(lastId);
    enableReturnQDateTime();
}

void tst_SQLite_QDateTime::
insert_QDateTime_0200Timezone_DatetimeColumn_OffReturnQDateTime() const
{
    disableReturnQDateTime();

    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"datetime", QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                             Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "datetime"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value("datetime");
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QString);

        /* Returning the QDateTime (fixed behavior by the TinyORM QueryBuilder) can be
           disabled by the return_qdatetime = false. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 11:14:15",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(lastId);
    enableReturnQDateTime();
}

void tst_SQLite_QDateTime::insert_QString_DatetimeColumn_OffReturnQDateTime() const
{
    disableReturnQDateTime();

    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"datetime", QString("2022-08-28 13:14:15")}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "datetime"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value("datetime");
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QString);

        /* Returning the QDateTime (fixed behavior by the TinyORM QueryBuilder) can be
           disabled by the return_qdatetime = false. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 13:14:15",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(lastId);
    enableReturnQDateTime();
}

void tst_SQLite_QDateTime::
insert_QDateTime_UtcTimezone_TimestampColumn_OffReturnQDateTime() const
{
    disableReturnQDateTime();

    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"timestamp", QDateTime::fromString("2022-08-28 13:14:15z",
                                                              Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "timestamp"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value("timestamp");
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QString);

        /* Returning the QDateTime (fixed behavior by the TinyORM QueryBuilder) can be
           disabled by the return_qdatetime = false. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-28 13:14:15",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(lastId);
    enableReturnQDateTime();
}

void tst_SQLite_QDateTime::
insert_QDateTime_0200Timezone_TimestampColumn_OffReturnQDateTime() const
{
    disableReturnQDateTime();

    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"timestamp",
                           QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                 Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "timestamp"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value("timestamp");
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QString);

        /* Returning the QDateTime (fixed behavior by the TinyORM QueryBuilder) can be
           disabled by the return_qdatetime = false. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-28 11:14:15",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(lastId);
    enableReturnQDateTime();
}

void tst_SQLite_QDateTime::insert_QString_TimestampColumn_OffReturnQDateTime() const
{
    disableReturnQDateTime();

    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"timestamp", QString("2022-08-28 13:14:15")}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "timestamp"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value("timestamp");
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QString);

        /* Returning the QDateTime (fixed behavior by the TinyORM QueryBuilder) can be
           disabled by the return_qdatetime = false. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-28 13:14:15",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(lastId);
    enableReturnQDateTime();
}

/* QDate */

/* Raw QSqlQuery */

/* Server timezone None */

void tst_SQLite_QDateTime::insert_Qt_QDate_UtcTimezone_DateColumn() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("date") values (?))"));

        qtQuery.addBindValue(QDate(2022, 8, 28));

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

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = qtQuery.value("date");
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QString);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

void tst_SQLite_QDateTime::insert_Qt_QString_DateColumn() const
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

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = qtQuery.value("date");
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QString);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

/* Orm::QueryBuilder */

/* Server timezone None and enabled return_qdatetime */

void
tst_SQLite_QDateTime::insert_QDate_UtcTimezone_DateColumn_OnReturnQDateTime() const
{
    // Insert
    quint64 lastId = createQuery()->from("datetimes")
                     .insertGetId({{"date", QDate(2022, 8, 28)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "date"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value("date");
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

void tst_SQLite_QDateTime::insert_QString_DateColumn_OnReturnQDateTime() const
{
    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"date", QString("2022-08-28")}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "date"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value("date");
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

/* Server timezone None and disabled return_qdatetime */

void
tst_SQLite_QDateTime::insert_QDate_UtcTimezone_DateColumn_OffReturnQDateTime() const
{
    disableReturnQDateTime();

    // Insert
    quint64 lastId = createQuery()->from("datetimes")
                     .insertGetId({{"date", QDate(2022, 8, 28)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "date"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value("date");
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QString);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
    enableReturnQDateTime();
}

void tst_SQLite_QDateTime::insert_QString_DateColumn_OffReturnQDateTime() const
{
    disableReturnQDateTime();

    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"date", QString("2022-08-28")}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "date"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value("date");
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QString);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
    enableReturnQDateTime();
}

/* Null values QDateTime / QDate */

/* Raw QSqlQuery */

/* Server timezone None */

void tst_SQLite_QDateTime::
insert_Qt_QDateTime_Null_DatetimeColumn() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("datetime") values (?))"));

        qtQuery.addBindValue(NullVariant::QDateTime());

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

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value("datetime");
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(datetimeDbVariant.isNull());

        // QSQLITE driver simply returns QVariant(QMetaType::QString)
        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QString);

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

void tst_SQLite_QDateTime::insert_Qt_QDate_Null_DateColumn() const
{
    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = DB::connection(m_connection).getQtQuery();

        QVERIFY(qtQuery.prepare(R"(insert into "datetimes" ("date") values (?))"));

        qtQuery.addBindValue(NullVariant::QDate());

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

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = qtQuery.value("date");
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(dateDbVariant.isNull());

        // QSQLITE driver simply returns QVariant(QMetaType::QString)
        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QString);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();

        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

/* Orm::QueryBuilder */

/* Server timezone None */

void tst_SQLite_QDateTime::insert_QDateTime_Null_DatetimeColumn() const
{
    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"datetime", NullVariant::QDateTime()}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "datetime"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value("datetime");
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(datetimeDbVariant.isNull());

        /* Following is not true for QSQLITE driver:
           TinyORM QueryBuilder fixes this and returns the QDateTime instead of QString.
           Because there is no way we can detect a null datetime column. So instead
           of returning the QVariant(QMetaType::QDateTime) we get
           the QVariant(QMetaType::QString). */
        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QString);

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

void tst_SQLite_QDateTime::insert_QDate_Null_DateColumn() const
{
    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"date", NullVariant::QDate()}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "date"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = query.value("date");
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(dateDbVariant.isNull());

        // QSQLITE driver simply returns QVariant(QMetaType::QString)
        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QString);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();

        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

/* QtTimeZoneType::DontConvert */

/* Orm::QueryBuilder */

/* Server timezone None */

void tst_SQLite_QDateTime::
insert_QDateTime_0300Timezone_DatetimeColumn_UtcOnServer_DontConvert() const
{
    DB::connection(m_connection)
            .setQtTimeZone(QtTimeZoneConfig {QtTimeZoneType::DontConvert});
    QCOMPARE(DB::qtTimeZone(m_connection),
             QtTimeZoneConfig {QtTimeZoneType::DontConvert});

    // Insert
    quint64 lastId = createQuery()->from("datetimes").insertGetId(
                         {{"datetime", QDateTime::fromString("2022-08-28 13:14:15+03:00",
                                                             Qt::ISODate)}});

    // Verify
    {
        auto query = createQuery()->from("datetimes").find(lastId, {ID, "datetime"});

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value("datetime");
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QString);

        /* Practically it should behave as is the default QSQLITE driver behavior,
           so it also retuns QString because return_qdatetime only works when converting
           TZ is enabled. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 13:14:15",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(lastId);

    DB::connection(m_connection)
            .setQtTimeZone(QtTimeZoneConfig {QtTimeZoneType::QtTimeSpec,
                                             QVariant::fromValue(Qt::UTC)});
    QCOMPARE(DB::qtTimeZone(m_connection),
             (QtTimeZoneConfig {QtTimeZoneType::QtTimeSpec,
                                QVariant::fromValue(Qt::UTC)}));
}

/* private */

/* Common */

std::shared_ptr<QueryBuilder>
tst_SQLite_QDateTime::createQuery() const
{
    return DB::connection(m_connection).query();
}

/* QDateTime with/without timezone */

void tst_SQLite_QDateTime::restore(const quint64 lastId) const
{
    const auto [affected, query] = createQuery()->from("datetimes").remove(lastId);

    QVERIFY(!query.lastError().isValid());
    QVERIFY(!query.isValid() && query.isActive() && !query.isSelect());
    QCOMPARE(affected, 1);
}

void tst_SQLite_QDateTime::disableReturnQDateTime() const
{
    dynamic_cast<SQLiteConnection &>(DB::connection(m_connection))
            .setReturnQDateTime(false);
}

void tst_SQLite_QDateTime::enableReturnQDateTime() const
{
    dynamic_cast<SQLiteConnection &>(DB::connection(m_connection))
            .setReturnQDateTime(true);
}

QTEST_MAIN(tst_SQLite_QDateTime)

#include "tst_sqlite_qdatetime.moc"
