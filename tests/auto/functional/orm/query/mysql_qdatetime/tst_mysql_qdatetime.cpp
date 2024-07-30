#include <QCoreApplication>
#include <QTest>

#ifdef TINYORM_USING_TINYDRIVERS
#  include "orm/drivers/dummysqlerror.hpp"

#  ifdef TINYDRIVERS_MYSQL_DRIVER
#    include "orm/drivers/mysql/version.hpp"
#  endif
#endif

#include "orm/db.hpp"
#include "orm/utils/nullvariant.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"
#include "macros.hpp"

using Orm::Constants::ID;
using Orm::Constants::QMYSQL;
using Orm::Constants::TZ00;
using Orm::Constants::UTC;

using Orm::DB;
using Orm::QtTimeZoneConfig;
using Orm::QtTimeZoneType;
using Orm::TTimeZone;
using Orm::Utils::NullVariant;

using QueryBuilder = Orm::Query::Builder;
using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

/* Qt >=v6.8 fixes time zone handling, it calls toUTC() on QDateTime instance while
   sending QDateTime()-s to the database, calls SET time_zone = '+00:00' while opening
   a database connection, and returns QDateTime() instances with the UTC time zone during
   retrieving column values for both normal and prepared queries.
   This is the reason why we must use #if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
   everywhere for QtSql QMYSQL and QPSQL and tinymysql_lib_utc_qdatetime >= 20240618
   for TinyDrivers TinyMySql. */

class tst_MySql_QDateTime : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase() const;

    /* QDateTime with/without timezone */
    /* Raw QSqlQuery */
    /* Server timezone UTC */
    void insert_Qt_QDateTime_UtcTimezone_DatetimeColumn_UtcOnServer() const;
    void insert_Qt_QDateTime_0200Timezone_DatetimeColumn_UtcOnServer() const;
    void insert_Qt_QString_DatetimeColumn_UtcOnServer() const;
    void insert_Qt_QDateTime_UtcTimezone_TimestampColumn_UtcOnServer() const;
    void insert_Qt_QDateTime_0200Timezone_TimestampColumn_UtcOnServer() const;
    void insert_Qt_QString_TimestampColumn_UtcOnServer() const;

    /* Server timezone +02:00 */
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

    /* Server timezone +02:00 */
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

    /* Server timezone +02:00 */
    void insert_Qt_QDate_UtcTimezone_DateColumn_0200OnServer() const;
    void insert_Qt_QString_DateColumn_0200OnServer() const;

    /* Orm::QueryBuilder */
    /* Server timezone UTC */
    void insert_QDate_UtcTimezone_DateColumn_UtcOnServer() const;
    void insert_QString_DateColumn_UtcOnServer() const;

    /* Server timezone +02:00 */
    void insert_QDate_UtcTimezone_DateColumn_0200OnServer() const;
    void insert_QString_DateColumn_0200OnServer() const;

    /* QTime */
    /* Raw QSqlQuery */
    /* Server timezone UTC */
    void insert_Qt_QTime_UtcTimezone_TimeColumn_UtcOnServer() const;
    void insert_Qt_QString_TimeColumn_UtcOnServer() const;

    /* Server timezone +02:00 */
    void insert_Qt_QTime_UtcTimezone_TimeColumn_0200OnServer() const;
    void insert_Qt_QString_TimeColumn_0200OnServer() const;

    /* Orm::QueryBuilder */
    /* Server timezone UTC */
    void insert_QTime_UtcTimezone_TimeColumn_UtcOnServer() const;
    void insert_QString_TimeColumn_UtcOnServer() const;

    /* Server timezone +02:00 */
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
    [[nodiscard]] inline static std::shared_ptr<QueryBuilder>
    createQuery(const QString &connection);
    /*! Create a raw QSqlQuery instance for the given connection. */
    [[nodiscard]] inline static TSqlQuery
    createSqlQuery(const QString &connection);

    /* QDateTime with/without timezone */
    /*! Set the MySQL timezone session variable to the UTC value. */
    inline static void setUtcTimezone(const QString &connection);
    /*! Set the MySQL timezone session variable to the +02:00 value. */
    inline static void set0200Timezone(const QString &connection);
    /*! Set the qt_timezone to the QtTimeZoneType::DontConvert value. */
    inline static void setDontConvertTimezone(const QString &connection);
    /*! Set the MySQL timezone session variable to the given value. */
    static void setTimezone(const QString &connection, const QString &timeZone,
                            QtTimeZoneConfig &&qtTimeZone);

    /*! Get the UTC time zone string. */
    inline static const QString &utcTimezoneString(const QString &connection);
    /*! Are MySQL time zone table populated? */
    static bool mysqlTimezoneTablesNotPopulated(const QString &connection);

    /*! Restore the database after a QDateTime-related test. */
    static void restore(const QString &connection, quint64 lastId,
                        bool restoreTimezone = false);
};

/*! Time zone +02:00. */
Q_GLOBAL_STATIC_WITH_ARGS(const QTimeZone, TimeZone0200, (QByteArray("UTC+02:00"))) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)

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
void tst_MySql_QDateTime::initTestCase() const
{
    /* I want to test these QDateTime and timezones related logic on both, the MySQL and
       also the MariaDB databases, to be sure that everything is ok. 🤓 */
    const auto connections = Databases::createConnections({Databases::MYSQL,
                                                           Databases::MARIADB});

    if (connections.isEmpty())
        QSKIP(TestUtils::AutoTestSkippedAny.arg(TypeUtils::classPureBasename(*this))
                                           .toUtf8().constData(), );

    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : connections)
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

/* QDateTime with/without timezone */

/* Raw QSqlQuery */

/* Server timezone UTC */

void tst_MySql_QDateTime::
insert_Qt_QDateTime_UtcTimezone_DatetimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`datetime`) values (?)"));

        sqlQuery.addBindValue(QDateTime({2022, 8, 28}, {13, 14, 15}, TTimeZone::UTC));

        // QMYSQL driver doesn't care about QDateTime timezone (fixed in Qt v6.8)
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `datetime` from `datetimes` where `id` = ?"));

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

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) || \
    (defined(tinymysql_lib_utc_qdatetime) && tinymysql_lib_utc_qdatetime >= 20240618)
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
    restore(connection, lastId);
}

void tst_MySql_QDateTime::
insert_Qt_QDateTime_0200Timezone_DatetimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`datetime`) values (?)"));

        sqlQuery.addBindValue(QDateTime({2022, 8, 28}, {13, 14, 15}, *TimeZone0200));

        // QMYSQL driver doesn't care about QDateTime timezone (fixed in Qt v6.8)
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `datetime` from `datetimes` where `id` = ?"));

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

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) || \
    (defined(tinymysql_lib_utc_qdatetime) && tinymysql_lib_utc_qdatetime >= 20240618)
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {11, 14, 15},
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
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_Qt_QString_DatetimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`datetime`) values (?)"));

        sqlQuery.addBindValue(QString("2022-08-28 13:14:15"));

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `datetime` from `datetimes` where `id` = ?"));

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

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) || \
    (defined(tinymysql_lib_utc_qdatetime) && tinymysql_lib_utc_qdatetime >= 20240618)
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
    restore(connection, lastId);
}

void tst_MySql_QDateTime::
insert_Qt_QDateTime_UtcTimezone_TimestampColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`timestamp`) values (?)"));

        sqlQuery.addBindValue(QDateTime({2022, 8, 29}, {13, 14, 15}, TTimeZone::UTC));

        // QMYSQL driver doesn't care about QDateTime timezone (fixed in Qt v6.8)
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `timestamp` from `datetimes` where `id` = ?"));

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

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) || \
    (defined(tinymysql_lib_utc_qdatetime) && tinymysql_lib_utc_qdatetime >= 20240618)
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
    restore(connection, lastId);
}

void tst_MySql_QDateTime::
insert_Qt_QDateTime_0200Timezone_TimestampColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`timestamp`) values (?)"));

        sqlQuery.addBindValue(QDateTime({2022, 8, 29}, {13, 14, 15}, *TimeZone0200));

        // QMYSQL driver doesn't care about QDateTime timezone (fixed in Qt v6.8)
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `timestamp` from `datetimes` where `id` = ?"));

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

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) || \
    (defined(tinymysql_lib_utc_qdatetime) && tinymysql_lib_utc_qdatetime >= 20240618)
        const auto timestampExpected = QDateTime({2022, 8, 29}, {11, 14, 15},
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
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_Qt_QString_TimestampColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`timestamp`) values (?)"));

        sqlQuery.addBindValue(QString("2022-08-29 13:14:15"));

        // QMYSQL driver doesn't care about QDateTime timezone (fixed in Qt v6.8)
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `timestamp` from `datetimes` where `id` = ?"));

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

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) || \
    (defined(tinymysql_lib_utc_qdatetime) && tinymysql_lib_utc_qdatetime >= 20240618)
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
    restore(connection, lastId);
}

/* Server timezone +02:00 */

void tst_MySql_QDateTime::
insert_Qt_QDateTime_UtcTimezone_DatetimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`datetime`) values (?)"));

        sqlQuery.addBindValue(QDateTime({2022, 8, 28}, {13, 14, 15}, TTimeZone::UTC));

        // QMYSQL driver doesn't care about QDateTime timezone (fixed in Qt v6.8)
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `datetime` from `datetimes` where `id` = ?"));

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

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) || \
    (defined(tinymysql_lib_utc_qdatetime) && tinymysql_lib_utc_qdatetime >= 20240618)
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
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::
insert_Qt_QDateTime_0200Timezone_DatetimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`datetime`) values (?)"));

        sqlQuery.addBindValue(QDateTime({2022, 8, 28}, {13, 14, 15}, *TimeZone0200));

        // QMYSQL driver doesn't care about QDateTime timezone (fixed in Qt v6.8)
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `datetime` from `datetimes` where `id` = ?"));

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

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) || \
    (defined(tinymysql_lib_utc_qdatetime) && tinymysql_lib_utc_qdatetime >= 20240618)
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {11, 14, 15},
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
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_Qt_QString_DatetimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`datetime`) values (?)"));

        sqlQuery.addBindValue(QString("2022-08-28 13:14:15"));

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `datetime` from `datetimes` where `id` = ?"));

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

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) || \
    (defined(tinymysql_lib_utc_qdatetime) && tinymysql_lib_utc_qdatetime >= 20240618)
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
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::
insert_Qt_QDateTime_UtcTimezone_TimestampColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`timestamp`) values (?)"));

        sqlQuery.addBindValue(QDateTime({2022, 8, 29}, {13, 14, 15}, TTimeZone::UTC));

        // QMYSQL driver doesn't care about QDateTime timezone (fixed in Qt v6.8)
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `timestamp` from `datetimes` where `id` = ?"));

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

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) || \
    (defined(tinymysql_lib_utc_qdatetime) && tinymysql_lib_utc_qdatetime >= 20240618)
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
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::
insert_Qt_QDateTime_0200Timezone_TimestampColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`timestamp`) values (?)"));

        sqlQuery.addBindValue(QDateTime({2022, 8, 29}, {13, 14, 15}, *TimeZone0200));

        // QMYSQL driver doesn't care about QDateTime timezone (fixed in Qt v6.8)
        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `timestamp` from `datetimes` where `id` = ?"));

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

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) || \
    (defined(tinymysql_lib_utc_qdatetime) && tinymysql_lib_utc_qdatetime >= 20240618)
        const auto timestampExpected = QDateTime({2022, 8, 29}, {11, 14, 15},
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
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_Qt_QString_TimestampColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);
    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`timestamp`) values (?)"));

        sqlQuery.addBindValue(QString("2022-08-29 13:14:15"));

        QVERIFY(sqlQuery.exec());

        QVERIFY(!sqlQuery.lastError().isValid());
        QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());
        QCOMPARE(sqlQuery.numRowsAffected(), 1);

        lastId = sqlQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `timestamp` from `datetimes` where `id` = ?"));

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

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) || \
    (defined(tinymysql_lib_utc_qdatetime) && tinymysql_lib_utc_qdatetime >= 20240618)
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
    restore(connection, lastId, true);
}

/* Orm::QueryBuilder */

/* Server timezone UTC */

void tst_MySql_QDateTime::
insert_QDateTime_UtcTimezone_DatetimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*datetime,
                           QDateTime({2022, 8, 28}, {13, 14, 15}, TTimeZone::UTC)}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                TTimeZone::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(connection, lastId);
}

void tst_MySql_QDateTime::
insert_QDateTime_0200Timezone_DatetimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*datetime,
                           QDateTime({2022, 8, 28}, {13, 14, 15}, *TimeZone0200)}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {11, 14, 15},
                                                TTimeZone::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_QString_DatetimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*datetime, QString("2022-08-28 13:14:15")}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                TTimeZone::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(connection, lastId);
}

void tst_MySql_QDateTime::
insert_QDateTime_UtcTimezone_TimestampColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*timestamp,
                           QDateTime({2022, 8, 28}, {13, 14, 15}, TTimeZone::UTC)}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *timestamp});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                 TTimeZone::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(connection, lastId);
}

void tst_MySql_QDateTime::
insert_QDateTime_0200Timezone_TimestampColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*timestamp,
                           QDateTime({2022, 8, 28}, {13, 14, 15}, *TimeZone0200)}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *timestamp});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {11, 14, 15},
                                                 TTimeZone::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_QString_TimestampColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*timestamp, QString("2022-08-28 13:14:15")}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *timestamp});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                 TTimeZone::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(connection, lastId);
}

/* Server timezone +02:00 */

void tst_MySql_QDateTime::
insert_QDateTime_UtcTimezone_DatetimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*datetime,
                           QDateTime({2022, 8, 28}, {13, 14, 15}, TTimeZone::UTC)}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {15, 14, 15},
                                                *TimeZone0200);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), *TimeZone0200);
    }

    // Restore
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::
insert_QDateTime_0200Timezone_DatetimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*datetime,
                           QDateTime({2022, 8, 28}, {13, 14, 15}, *TimeZone0200)}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                *TimeZone0200);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), *TimeZone0200);
    }

    // Restore
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_QString_DatetimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*datetime, QString("2022-08-28 13:14:15")}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                *TimeZone0200);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), *TimeZone0200);
    }

    // Restore
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::
insert_QDateTime_UtcTimezone_TimestampColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*timestamp,
                           QDateTime({2022, 8, 28}, {13, 14, 15}, TTimeZone::UTC)}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *timestamp});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {15, 14, 15},
                                                 *TimeZone0200);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), *TimeZone0200);
    }

    // Restore
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::
insert_QDateTime_0200Timezone_TimestampColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*timestamp,
                           QDateTime({2022, 8, 28}, {13, 14, 15}, *TimeZone0200)}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *timestamp});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                 *TimeZone0200);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), *TimeZone0200);
    }

    // Restore
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_QString_TimestampColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*timestamp, QString("2022-08-28 13:14:15")}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *timestamp});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                 *TimeZone0200);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), *TimeZone0200);
    }

    // Restore
    restore(connection, lastId, true);
}

/* QDate */

/* Raw QSqlQuery */

/* Server timezone UTC */

void tst_MySql_QDateTime::insert_Qt_QDate_UtcTimezone_DateColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`date`) values (?)"));

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
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `date` from `datetimes` where `id` = ?"));

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
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_Qt_QString_DateColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`date`) values (?)"));

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
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `date` from `datetimes` where `id` = ?"));

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
    restore(connection, lastId);
}

/* Server timezone +02:00 */

void tst_MySql_QDateTime::insert_Qt_QDate_UtcTimezone_DateColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`date`) values (?)"));

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
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `date` from `datetimes` where `id` = ?"));

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
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_Qt_QString_DateColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`date`) values (?)"));

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
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `date` from `datetimes` where `id` = ?"));

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
    restore(connection, lastId, true);
}

/* Orm::QueryBuilder */

/* Server timezone UTC */

void tst_MySql_QDateTime::insert_QDate_UtcTimezone_DateColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*date, QDate(2022, 8, 28)}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *date});

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
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_QString_DateColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*date, QString("2022-08-28")}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes).find(lastId, {ID, *date});

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
    restore(connection, lastId);
}

/* Server timezone +02:00 */

void tst_MySql_QDateTime::insert_QDate_UtcTimezone_DateColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*date, QDate(2022, 8, 28)}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes).find(lastId, {ID, *date});

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
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_QString_DateColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*date, QString("2022-08-28")}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes).find(lastId, {ID, *date});

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
    restore(connection, lastId, true);
}

/* QTime */

/* Raw QSqlQuery */

/* Server timezone UTC */

void tst_MySql_QDateTime::insert_Qt_QTime_UtcTimezone_TimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`time`) values (?)"));

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
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `time` from `datetimes` where `id` = ?"));

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

        /* MySQL TIME column type is returned as QString because it can be within
           the range '-838:59:59' to '838:59:59'. */
        QCOMPARE(timeDbVariant.typeId(), QMetaType::QString);

        const auto timeActual = timeDbVariant.value<QString>();
        const auto timeExpected = sl("17:02:59");
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_Qt_QString_TimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`time`) values (?)"));

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
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `time` from `datetimes` where `id` = ?"));

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

        /* MySQL TIME column type is returned as QString because it can be within
           the range '-838:59:59' to '838:59:59'. */
        QCOMPARE(timeDbVariant.typeId(), QMetaType::QString);

        const auto timeActual = timeDbVariant.value<QString>();
        const auto timeExpected = QString("17:02:59");
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(connection, lastId);
}

/* Server timezone +02:00 */

void tst_MySql_QDateTime::insert_Qt_QTime_UtcTimezone_TimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`time`) values (?)"));

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
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `time` from `datetimes` where `id` = ?"));

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

        /* MySQL TIME column type is returned as QString because it can be within
           the range '-838:59:59' to '838:59:59'. */
        QCOMPARE(timeDbVariant.typeId(), QMetaType::QString);

        const auto timeActual = timeDbVariant.value<QString>();
        const auto timeExpected = QString("17:02:59");
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_Qt_QString_TimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`time`) values (?)"));

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
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `time` from `datetimes` where `id` = ?"));

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

        /* MySQL TIME column type is returned as QString because it can be within
           the range '-838:59:59' to '838:59:59'. */
        QCOMPARE(timeDbVariant.typeId(), QMetaType::QString);

        const auto timeActual = timeDbVariant.value<QString>();
        const auto timeExpected = QString("17:02:59");
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(connection, lastId, true);
}

/* Orm::QueryBuilder */

/* Server timezone UTC */

void tst_MySql_QDateTime::insert_QTime_UtcTimezone_TimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*time_, QTime(17, 2, 59)}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *time_});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = query.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(!timeDbVariant.isNull());

        /* MySQL TIME column type is returned as QString because it can be within
           the range '-838:59:59' to '838:59:59'. */
        QCOMPARE(timeDbVariant.typeId(), QMetaType::QString);

        const auto timeActual = timeDbVariant.value<QString>();
        const auto timeExpected = QString("17:02:59");
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_QString_TimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*time_, QString("17:02:59")}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes).find(lastId, {ID, *time_});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = query.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(!timeDbVariant.isNull());

        /* MySQL TIME column type is returned as QString because it can be within
           the range '-838:59:59' to '838:59:59'. */
        QCOMPARE(timeDbVariant.typeId(), QMetaType::QString);

        const auto timeActual = timeDbVariant.value<QString>();
        const auto timeExpected = QString("17:02:59");
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(connection, lastId);
}

/* Server timezone +02:00 */

void tst_MySql_QDateTime::insert_QTime_UtcTimezone_TimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*time_, QTime(17, 2, 59)}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes).find(lastId, {ID, *time_});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = query.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(!timeDbVariant.isNull());

        /* MySQL TIME column type is returned as QString because it can be within
           the range '-838:59:59' to '838:59:59'. */
        QCOMPARE(timeDbVariant.typeId(), QMetaType::QString);

        const auto timeActual = timeDbVariant.value<QString>();
        const auto timeExpected = QString("17:02:59");
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_QString_TimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*time_, QString("17:02:59")}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes).find(lastId, {ID, *time_});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = query.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(!timeDbVariant.isNull());

        /* MySQL TIME column type is returned as QString because it can be within
           the range '-838:59:59' to '838:59:59'. */
        QCOMPARE(timeDbVariant.typeId(), QMetaType::QString);

        const auto timeActual = timeDbVariant.value<QString>();
        const auto timeExpected = QString("17:02:59");
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(connection, lastId, true);
}

/* Null values QDateTime / QDate */

/* Raw QSqlQuery */

/* Server timezone UTC */

void tst_MySql_QDateTime::insert_Qt_QDateTime_Null_DatetimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`datetime`) values (?)"));

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
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `datetime` from `datetimes` where `id` = ?"));

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
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_Qt_QDate_Null_DateColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`date`) values (?)"));

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
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `date` from `datetimes` where `id` = ?"));

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
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_Qt_QTime_Null_TimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`time`) values (?)"));

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
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `time` from `datetimes` where `id` = ?"));

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

        QCOMPARE(timeDbVariant.typeId(), QMetaType::QString);

        const auto timeActual = timeDbVariant.value<QString>();
        const auto timeExpected = QString();
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(connection, lastId);
}

/* Server timezone +02:00 */

void tst_MySql_QDateTime::insert_Qt_QDateTime_Null_DatetimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`datetime`) values (?)"));

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
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `datetime` from `datetimes` where `id` = ?"));

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
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_Qt_QDate_Null_DateColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`date`) values (?)"));

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
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `date` from `datetimes` where `id` = ?"));

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
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_Qt_QTime_Null_TimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare("insert into `datetimes` (`time`) values (?)"));

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
        auto sqlQuery = createSqlQuery(connection);

        QVERIFY(sqlQuery.prepare(
                    "select `id`, `time` from `datetimes` where `id` = ?"));

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

        QCOMPARE(timeDbVariant.typeId(), QMetaType::QString);

        const auto timeActual = timeDbVariant.value<QString>();
        const auto timeExpected = QString();
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(connection, lastId, true);
}

/* Orm::QueryBuilder */

/* Server timezone UTC */

void tst_MySql_QDateTime::insert_QDateTime_Null_DatetimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*datetime, NullVariant::QDateTime()}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *datetime});

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
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_QDate_Null_DateColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*date, NullVariant::QDate()}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes).find(lastId, {ID, *date});

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
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_QTime_Null_TimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*time_, NullVariant::QTime()}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes).find(lastId, {ID, *time_});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = query.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(timeDbVariant.isNull());

        QCOMPARE(timeDbVariant.typeId(), QMetaType::QString);

        const auto timeActual = timeDbVariant.value<QString>();
        const auto timeExpected = QString();
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(connection, lastId);
}

/* Server timezone +02:00 */

void tst_MySql_QDateTime::insert_QDateTime_Null_DatetimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*datetime, NullVariant::QDateTime()}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *datetime});

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
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_QDate_Null_DateColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*date, NullVariant::QDate()}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes).find(lastId, {ID, *date});

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
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_QTime_Null_TimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    set0200Timezone(connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*time_, NullVariant::QTime()}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes).find(lastId, {ID, *time_});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timeDbVariant = query.value(*time_);
        QVERIFY(timeDbVariant.isValid());
        QVERIFY(timeDbVariant.isNull());

        QCOMPARE(timeDbVariant.typeId(), QMetaType::QString);

        const auto timeActual = timeDbVariant.value<QString>();
        const auto timeExpected = QString();
        QCOMPARE(timeActual, timeExpected);
    }

    // Restore
    restore(connection, lastId, true);
}

/* QtTimeZoneType::DontConvert */

/* Orm::QueryBuilder */

/* Server timezone UTC */

void tst_MySql_QDateTime::
insert_QDateTime_0300Timezone_DatetimeColumn_UtcOnServer_DontConvert() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    setDontConvertTimezone(connection);

    QCOMPARE(DB::qtTimeZone(connection),
             QtTimeZoneConfig {QtTimeZoneType::DontConvert});

    /* The QDateTime's time zone is ignored with the QtTimeZoneType::DontConvert
       connection option, only toString(m_queryGrammar->getDateFormat()) is applied. */

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*datetime,
                           QDateTime({2022, 8, 28}, {13, 14, 15},
                                     QTimeZone("UTC+03:00"))}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        // Practically it should behave as is the default QMYSQL driver behavior
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) || \
    (defined(tinymysql_lib_utc_qdatetime) && tinymysql_lib_utc_qdatetime >= 20240618)
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
    restore(connection, lastId, true);
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

/* Common */

std::shared_ptr<QueryBuilder>
tst_MySql_QDateTime::createQuery(const QString &connection)
{
    return DB::query(connection);
}

TSqlQuery tst_MySql_QDateTime::createSqlQuery(const QString &connection)
{
    return DB::sqlQuery(connection);
}

/* QDateTime with/without timezone */

void tst_MySql_QDateTime::setUtcTimezone(const QString &connection)
{
    setTimezone(connection, utcTimezoneString(connection), QtTimeZoneConfig::utc());
}

void tst_MySql_QDateTime::set0200Timezone(const QString &connection)
{
    setTimezone(connection, sl("+02:00"),
                {QtTimeZoneType::QTimeZone, QVariant::fromValue(*TimeZone0200)});
}

void tst_MySql_QDateTime::setDontConvertTimezone(const QString &connection)
{
    setTimezone(connection, utcTimezoneString(connection), {QtTimeZoneType::DontConvert});
}

void tst_MySql_QDateTime::setTimezone(const QString &connection, const QString &timeZone,
                                      Orm::QtTimeZoneConfig &&qtTimeZone)
{
    const auto sqlQuery = DB::unprepared(sl("set time_zone=\"%1\";").arg(timeZone),
                                        connection);

    QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());

    DB::connection(connection).setQtTimeZone(std::move(qtTimeZone));
}

const QString &tst_MySql_QDateTime::utcTimezoneString(const QString &connection)
{
    Q_ASSERT(DB::driverName(connection) == QMYSQL);

    return mysqlTimezoneTablesNotPopulated(connection) ? TZ00 : UTC;
}

bool
tst_MySql_QDateTime::mysqlTimezoneTablesNotPopulated(const QString &connection)
{
    auto sqlQuery = DB::select(sl("select count(*) from `mysql`.`time_zone_name`"), {},
                              connection);

    if (!sqlQuery.first())
        return true;

    const auto tzTableRows = sqlQuery.value(0);

    // If 0 then TZ tables are empty
    return tzTableRows.value<quint64>() == 0;
}

void tst_MySql_QDateTime::restore(const QString &connection, const quint64 lastId,
                                  const bool restoreTimezone)
{
    const auto [affected, query] = createQuery(connection)->from(*datetimes)
                                   .remove(lastId);

    QVERIFY(!query.lastError().isValid());
    QVERIFY(!query.isValid() && query.isActive() && !query.isSelect());
    QCOMPARE(affected, 1);

    if (!restoreTimezone)
        return;

    // Restore also the MySQL timezone session variable to auto tests default UTC value
    setUtcTimezone(connection);
}

QTEST_MAIN(tst_MySql_QDateTime)

#include "tst_mysql_qdatetime.moc"
