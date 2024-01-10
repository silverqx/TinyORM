#include <QCoreApplication>
#include <QtTest>

#include "orm/db.hpp"
#include "orm/utils/nullvariant.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"

using Orm::Constants::ID;
using Orm::Constants::QMYSQL;
using Orm::Constants::TZ00;
using Orm::Constants::UTC;

using Orm::DB;
using Orm::QtTimeZoneConfig;
using Orm::QtTimeZoneType;
using Orm::Utils::Helpers;
using Orm::Utils::NullVariant;

using QueryBuilder = Orm::Query::Builder;
using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

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
    [[nodiscard]] inline static QSqlQuery
    createQtQuery(const QString &connection);

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
Q_GLOBAL_STATIC_WITH_ARGS(const QTimeZone, TimeZone0200, (QByteArray("UTC+02:00"))) // NOLINT(misc-use-anonymous-namespace)

/*! QString constant "datetimes" (perf. reason, one time initialization). */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, datetimes, ("datetimes")) // NOLINT(misc-use-anonymous-namespace)
/*! QString constant "datetime" (perf. reason, one time initialization). */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, datetime, ("datetime")) // NOLINT(misc-use-anonymous-namespace)
/*! QString constant "timestamp" (perf. reason, one time initialization). */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, timestamp, ("timestamp")) // NOLINT(misc-use-anonymous-namespace)
/*! QString constant "date" (perf. reason, one time initialization). */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, date, ("date")) // NOLINT(misc-use-anonymous-namespace)

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
    QFETCH_GLOBAL(QString, connection);

    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`datetime`) values (?)"));

        qtQuery.addBindValue(QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC));

        // QMYSQL driver doesn't care about QDateTime timezone
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `datetime` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15});

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(connection, lastId);
}

void tst_MySql_QDateTime::
insert_Qt_QDateTime_0200Timezone_DatetimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`datetime`) values (?)"));

        qtQuery.addBindValue(QDateTime({2022, 8, 28}, {13, 14, 15}, *TimeZone0200));

        // QMYSQL driver doesn't care about QDateTime timezone
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `datetime` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15});

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_Qt_QString_DatetimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`datetime`) values (?)"));

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
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `datetime` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15});

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(connection, lastId);
}

void tst_MySql_QDateTime::
insert_Qt_QDateTime_UtcTimezone_TimestampColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`timestamp`) values (?)"));

        qtQuery.addBindValue(QDateTime({2022, 8, 29}, {13, 14, 15}, Qt::UTC));

        // QMYSQL driver doesn't care about QDateTime timezone
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `timestamp` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = qtQuery.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 29}, {13, 14, 15});

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(connection, lastId);
}

void tst_MySql_QDateTime::
insert_Qt_QDateTime_0200Timezone_TimestampColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`timestamp`) values (?)"));

        qtQuery.addBindValue(QDateTime({2022, 8, 29}, {13, 14, 15}, *TimeZone0200));

        // QMYSQL driver doesn't care about QDateTime timezone
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `timestamp` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = qtQuery.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 29}, {13, 14, 15});

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_Qt_QString_TimestampColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`timestamp`) values (?)"));

        qtQuery.addBindValue(QString("2022-08-29 13:14:15"));

        // QMYSQL driver doesn't care about QDateTime timezone
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `timestamp` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = qtQuery.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 29}, {13, 14, 15});

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(connection, lastId);
}

/* Server timezone +02:00 */

void tst_MySql_QDateTime::
insert_Qt_QDateTime_UtcTimezone_DatetimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    set0200Timezone(connection);
    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`datetime`) values (?)"));

        qtQuery.addBindValue(QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC));

        // QMYSQL driver doesn't care about QDateTime timezone
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `datetime` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15});

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::
insert_Qt_QDateTime_0200Timezone_DatetimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    set0200Timezone(connection);
    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`datetime`) values (?)"));

        qtQuery.addBindValue(QDateTime({2022, 8, 28}, {13, 14, 15}, *TimeZone0200));

        // QMYSQL driver doesn't care about QDateTime timezone
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `datetime` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15});

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_Qt_QString_DatetimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    set0200Timezone(connection);
    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`datetime`) values (?)"));

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
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `datetime` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15});

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::
insert_Qt_QDateTime_UtcTimezone_TimestampColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    set0200Timezone(connection);
    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`timestamp`) values (?)"));

        qtQuery.addBindValue(QDateTime({2022, 8, 29}, {13, 14, 15}, Qt::UTC));

        // QMYSQL driver doesn't care about QDateTime timezone
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `timestamp` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = qtQuery.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 29}, {13, 14, 15});

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::
insert_Qt_QDateTime_0200Timezone_TimestampColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    set0200Timezone(connection);
    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`timestamp`) values (?)"));

        qtQuery.addBindValue(QDateTime({2022, 8, 29}, {13, 14, 15}, *TimeZone0200));

        // QMYSQL driver doesn't care about QDateTime timezone
        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());
        QCOMPARE(qtQuery.numRowsAffected(), 1);

        lastId = qtQuery.lastInsertId().value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `timestamp` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = qtQuery.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 29}, {13, 14, 15});

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_Qt_QString_TimestampColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    set0200Timezone(connection);
    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`timestamp`) values (?)"));

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
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `timestamp` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = qtQuery.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* QMYSQL driver doesn't care about QDateTime timezone and returns the QDateTime
           in a local timezone. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 29}, {13, 14, 15});

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());
    }

    // Restore
    restore(connection, lastId, true);
}

/* Orm::QueryBuilder */

/* Server timezone UTC */

void tst_MySql_QDateTime::
insert_QDateTime_UtcTimezone_DatetimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*datetime,
                           QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC)}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC);

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
    QFETCH_GLOBAL(QString, connection);

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

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {11, 14, 15}, Qt::UTC);

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_QString_DatetimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

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

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC);

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
    QFETCH_GLOBAL(QString, connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*timestamp,
                           QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC)}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *timestamp});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC);

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
    QFETCH_GLOBAL(QString, connection);

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

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {11, 14, 15}, Qt::UTC);

        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_QString_TimestampColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

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

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM QueryBuilder fixes the buggy time zone behavior
           of the QMYSQL driver. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC);

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
    QFETCH_GLOBAL(QString, connection);

    set0200Timezone(connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*datetime,
                           QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC)}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *datetime});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = query.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

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
    QFETCH_GLOBAL(QString, connection);

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

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

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
    QFETCH_GLOBAL(QString, connection);

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

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

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
    QFETCH_GLOBAL(QString, connection);

    set0200Timezone(connection);

    // Insert
    quint64 lastId = createQuery(connection)->from(*datetimes).insertGetId(
                         {{*timestamp,
                           QDateTime({2022, 8, 28}, {13, 14, 15}, Qt::UTC)}});

    // Verify
    {
        auto query = createQuery(connection)->from(*datetimes)
                     .find(lastId, {ID, *timestamp});

        QCOMPARE(query.size(), 1);

        QCOMPARE(query.value(ID).value<quint64>(), lastId);

        const auto timestampDbVariant = query.value(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

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
    QFETCH_GLOBAL(QString, connection);

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

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

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
    QFETCH_GLOBAL(QString, connection);

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

        QCOMPARE(Helpers::qVariantTypeId(timestampDbVariant), QMetaType::QDateTime);

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
    QFETCH_GLOBAL(QString, connection);

    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`date`) values (?)"));

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
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `date` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = qtQuery.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_Qt_QString_DateColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`date`) values (?)"));

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
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `date` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = qtQuery.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

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
    QFETCH_GLOBAL(QString, connection);

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`date`) values (?)"));

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
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `date` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = qtQuery.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_Qt_QString_DateColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`date`) values (?)"));

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
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `date` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = qtQuery.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

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
    QFETCH_GLOBAL(QString, connection);

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

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_QString_DateColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

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

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

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
    QFETCH_GLOBAL(QString, connection);

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

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_QString_DateColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

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

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(connection, lastId, true);
}

/* Null values QDateTime / QDate */

/* Raw QSqlQuery */

/* Server timezone UTC */

void tst_MySql_QDateTime::insert_Qt_QDateTime_Null_DatetimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`datetime`) values (?)"));

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
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `datetime` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(datetimeDbVariant.isNull());

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
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_Qt_QDate_Null_DateColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`date`) values (?)"));

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
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `date` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = qtQuery.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(dateDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(connection, lastId);
}

/* Server timezone +02:00 */

void tst_MySql_QDateTime::insert_Qt_QDateTime_Null_DatetimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`datetime`) values (?)"));

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
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `datetime` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto datetimeDbVariant = qtQuery.value(*datetime);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(datetimeDbVariant.isNull());

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
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_Qt_QDate_Null_DateColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare("insert into `datetimes` (`date`) values (?)"));

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
        auto qtQuery = createQtQuery(connection);

        QVERIFY(qtQuery.prepare(
                    "select `id`, `date` from `datetimes` where `id` = ?"));

        qtQuery.addBindValue(lastId);

        QVERIFY(qtQuery.exec());

        QVERIFY(!qtQuery.lastError().isValid());
        QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && qtQuery.isSelect());
        QCOMPARE(qtQuery.size(), 1);

        QVERIFY(qtQuery.first());

        QCOMPARE(qtQuery.value(ID).value<quint64>(), lastId);

        const auto dateDbVariant = qtQuery.value(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(dateDbVariant.isNull());

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(connection, lastId, true);
}

/* Orm::QueryBuilder */

/* Server timezone UTC */

void tst_MySql_QDateTime::insert_QDateTime_Null_DatetimeColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

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
    restore(connection, lastId);
}

void tst_MySql_QDateTime::insert_QDate_Null_DateColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

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

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(connection, lastId);
}

/* Server timezone +02:00 */

void tst_MySql_QDateTime::insert_QDateTime_Null_DatetimeColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

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
    restore(connection, lastId, true);
}

void tst_MySql_QDateTime::insert_QDate_Null_DateColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

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

        QCOMPARE(Helpers::qVariantTypeId(dateDbVariant), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();
        QCOMPARE(dateActual, dateExpected);
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
    QFETCH_GLOBAL(QString, connection);

    setDontConvertTimezone(connection);

    QCOMPARE(DB::qtTimeZone(connection),
             QtTimeZoneConfig {QtTimeZoneType::DontConvert});

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

        QCOMPARE(Helpers::qVariantTypeId(datetimeDbVariant), QMetaType::QDateTime);

        // Practically it should behave as is the default QMYSQL driver behavior
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15});

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
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

QSqlQuery tst_MySql_QDateTime::createQtQuery(const QString &connection)
{
    return DB::qtQuery(connection);
}

/* QDateTime with/without timezone */

void tst_MySql_QDateTime::setUtcTimezone(const QString &connection)
{
    setTimezone(connection, utcTimezoneString(connection),
                {QtTimeZoneType::QtTimeSpec, QVariant::fromValue(Qt::UTC)});
}

void tst_MySql_QDateTime::set0200Timezone(const QString &connection)
{
    setTimezone(connection, QStringLiteral("+02:00"),
                {QtTimeZoneType::QTimeZone, QVariant::fromValue(*TimeZone0200)});
}

void tst_MySql_QDateTime::setDontConvertTimezone(const QString &connection)
{
    setTimezone(connection, utcTimezoneString(connection), {QtTimeZoneType::DontConvert});
}

void tst_MySql_QDateTime::setTimezone(const QString &connection, const QString &timeZone,
                                      Orm::QtTimeZoneConfig &&qtTimeZone)
{
    const auto qtQuery = DB::unprepared(
                             QStringLiteral("set time_zone=\"%1\";").arg(timeZone),
                             connection);

    QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());

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
    auto qtQuery = DB::select(
                       QStringLiteral("select count(*) from `mysql`.`time_zone_name`"),
                       {}, connection);

    if (!qtQuery.first())
        return true;

    const auto tzTableRows = qtQuery.value(0);

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
