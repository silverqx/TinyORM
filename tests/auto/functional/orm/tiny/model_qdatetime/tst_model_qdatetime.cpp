#include <QCoreApplication>
#include <QTest>

#ifdef TINYDRIVERS_MYSQL_DRIVER
#  include "orm/drivers/mysql/version.hpp"
#endif

#include "orm/db.hpp"
#include "orm/utils/nullvariant.hpp"

#include "databases.hpp"
#include "macros.hpp"

#include "models/datetime.hpp"

using Orm::Constants::ID;
using Orm::Constants::QMYSQL;
using Orm::Constants::QPSQL;
using Orm::Constants::QSQLITE;
using Orm::Constants::TZ00;
using Orm::Constants::UTC;

using Orm::DB;
using Orm::QtTimeZoneConfig;
using Orm::QtTimeZoneType;
using Orm::TTimeZone;
using Orm::Utils::NullVariant;

using TypeUtils = Orm::Utils::Type;

using Orm::Tiny::ConnectionOverride;

using TestUtils::Databases;

using Models::Datetime;

class tst_Model_QDateTime : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase_data() const;

    /* QDateTime with/without timezone */
    /* Server timezone UTC */
    void create_QDateTime_UtcTimezone_DatetimeAttribute_UtcOnServer() const;
    void create_QDateTime_0200Timezone_DatetimeAttribute_UtcOnServer() const;
    void create_QString_DatetimeAttribute_UtcOnServer() const;
    void create_QDateTime_UtcTimezone_TimestampAttribute_UtcOnServer() const;
    void create_QDateTime_0200Timezone_TimestampAttribute_UtcOnServer() const;
    void create_QString_TimestampAttribute_UtcOnServer() const;

    /* Server timezone +02:00 */
    void create_QDateTime_UtcTimezone_DatetimeAttribute_0200OnServer() const;
    void create_QDateTime_0200Timezone_DatetimeAttribute_0200OnServer() const;
    void create_QString_DatetimeAttribute_0200OnServer() const;
    void create_QDateTime_UtcTimezone_TimestampAttribute_0200OnServer() const;
    void create_QDateTime_0200Timezone_TimestampAttribute_0200OnServer() const;
    void create_QString_TimestampAttribute_0200OnServer() const;

    /* QDate */
    /* Server timezone UTC */
    void create_QDate_UtcTimezone_DateAttribute_UtcOnServer() const;
    void create_QString_DateAttribute_UtcOnServer() const;

    /* Server timezone +02:00 */
    void create_QDate_UtcTimezone_DateAttribute_0200OnServer() const;
    void create_QString_DateAttribute_0200OnServer() const;

    /* Null values QDateTime / QDate */
    /* Server timezone UTC */
    void create_QDateTime_Null_DatetimeAttribute_UtcOnServer() const;
    void create_QDate_Null_DateAttribute_UtcOnServer() const;

    /* Server timezone +02:00 */
    void create_QDateTime_Null_DatetimeAttribute_0200OnServer() const;
    void create_QDate_Null_DateAttribute_0200OnServer() const;

    /* QtTimeZoneType::DontConvert */
    /* Server timezone UTC */
    void create_QDateTime_0300Timezone_DatetimeAttribute_UtcOnServer_DontConvert() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Set the MySQL/PostgreSQL timezone session variable to the UTC value. */
    inline static void setUtcTimezone(const QString &connection = {});
    /*! Get the UTC time zone string. */
    static const QString &utcTimezoneString(const QString &connection);
    /*! Are MySQL time zone table populated? */
    static bool mysqlTimezoneTablesNotPopulated(const QString &connection);

    /*! Set the database timezone session variable to +02:00 value by connection. */
    inline static void set0200Timezone(const QString &connection = {});
    /*! Set the MySQL timezone session variable to the +02:00 value. */
    inline static void set0200TimezoneForMySQL(const QString &connection = {});
    /*! Set the PostgreSQL timezone session variable to the +02:00 value. */
    inline static void setEUBratislavaTimezoneForPSQL(const QString &connection = {});

    /*! Set the MySQL/PostgreSQL timezone session variable to the given value. */
    static void setTimezone(const QString &timeZone, QtTimeZoneConfig &&qtTimeZone,
                            const QString &connection);
    /*! Get a SQL query string to set a database time zone session variable. */
    static QString getSetTimezoneQueryString(const QString &connection);

    /*! Get the QTimeZone +02:00 instance for MySQL/PostgreSQL. */
    inline static const QTimeZone &timezone0200(const QString &connection = {});

    /*! Restore the database after a QDateTime-related test. */
    static void restore(quint64 lastId, bool restoreTimezone = false,
                        const QString &connection = {});
};

/*! QString constant "datetime" (perf. reason, one time initialization). */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, datetime_, ("datetime")) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
/*! QString constant "timestamp" (perf. reason, one time initialization). */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, timestamp, ("timestamp")) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
/*! QString constant "date" (perf. reason, one time initialization). */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, date, ("date")) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Model_QDateTime::initTestCase_data() const
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

/* QDateTime with/without timezone */

/* Server timezone UTC */

void tst_Model_QDateTime::
create_QDateTime_UtcTimezone_DatetimeAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = QDateTime({2022, 8, 28}, {13, 14, 15}, TTimeZone::UTC);

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
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

void tst_Model_QDateTime::
create_QDateTime_0200Timezone_DatetimeAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = QDateTime({2022, 8, 28}, {13, 14, 15},
                                         timezone0200(connection));

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 11:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
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

void tst_Model_QDateTime::create_QString_DatetimeAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = QString("2022-08-28 13:14:15");

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
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

void tst_Model_QDateTime::
create_QDateTime_UtcTimezone_TimestampAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*timestamp] = QDateTime({2022, 8, 28}, {13, 14, 15}, TTimeZone::UTC);

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*timestamp);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
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

void tst_Model_QDateTime::
create_QDateTime_0200Timezone_TimestampAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*timestamp] = QDateTime({2022, 8, 28}, {13, 14, 15},
                                         timezone0200(connection));

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*timestamp);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 11:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
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

void tst_Model_QDateTime::create_QString_TimestampAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*timestamp] = QString("2022-08-28 13:14:15");

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*timestamp);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
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

/* Server timezone +02:00 */

void tst_Model_QDateTime::
create_QDateTime_UtcTimezone_DatetimeAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = QDateTime({2022, 8, 28}, {13, 14, 15}, TTimeZone::UTC);

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 15:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {15, 14, 15},
                                                timezone0200(connection));
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::
create_QDateTime_0200Timezone_DatetimeAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = QDateTime({2022, 8, 28}, {13, 14, 15},
                                         timezone0200(connection));

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                timezone0200(connection));
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::create_QString_DatetimeAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = QString("2022-08-28 13:14:15");

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                timezone0200(connection));
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::
create_QDateTime_UtcTimezone_TimestampAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*timestamp] = QDateTime({2022, 8, 28}, {13, 14, 15}, TTimeZone::UTC);

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*timestamp);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 15:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {15, 14, 15},
                                                 timezone0200(connection));
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::
create_QDateTime_0200Timezone_TimestampAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*timestamp] = QDateTime({2022, 8, 28}, {13, 14, 15},
                                         timezone0200(connection));

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*timestamp);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                 timezone0200(connection));
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::create_QString_TimestampAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*timestamp] = QString("2022-08-28 13:14:15");

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*timestamp);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute(*timestamp);
        QVERIFY(timestampDbVariant.isValid());
        QVERIFY(!timestampDbVariant.isNull());

        QCOMPARE(timestampDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                 timezone0200(connection));
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

/* QDate */

/* Server timezone UTC */

void tst_Model_QDateTime::create_QDate_UtcTimezone_DateAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*date] = QDate(2022, 8, 28);

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*date);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto dateDbVariant = datetime->getAttribute(*date);
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

void tst_Model_QDateTime::create_QString_DateAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        // Will use isStandardDateFormat()
        datetime[*date] = QString("2022-08-28");

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*date);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto dateDbVariant = datetime->getAttribute(*date);
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

/* Server timezone +02:00 */

void tst_Model_QDateTime::create_QDate_UtcTimezone_DateAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*date] = QDate(2022, 8, 28);

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*date);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto dateDbVariant = datetime->getAttribute(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(dateDbVariant.typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::create_QString_DateAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        // Will use isStandardDateFormat()
        datetime[*date] = QString("2022-08-28");

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*date);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto dateDbVariant = datetime->getAttribute(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(!dateDbVariant.isNull());

        QCOMPARE(dateDbVariant.typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate(2022, 8, 28);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true, connection);
}

/* Null values QDateTime / QDate */

/* Server timezone UTC */

void tst_Model_QDateTime::create_QDateTime_Null_DatetimeAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = NullVariant::QDateTime();

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QVERIFY(attribute.isValid() && attribute.isNull());
        QCOMPARE(attribute.typeId(), QMetaType::QDateTime);

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(datetimeDbVariant.isNull());

        /* Following is not true for QSQLITE driver:
           TinyORM Model fixes this and returns the QDateTime instead of QString.
           Would be possible to detect whether it's QSQLITE driver and a null attribute
           and on the base of that return the QVariant(QMetaType::QDateTime), but
           I have decided to leave it as it's now, I will not override this behavior.
           QSQLITE driver simply returns QVariant(QMetaType::QString) for all null
           values. */
        if (DB::driverName(connection) == QSQLITE)
            QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QString);
        else
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

void tst_Model_QDateTime::create_QDate_Null_DateAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*date] = NullVariant::QDate();

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*date);
        QVERIFY(attribute.isValid() && attribute.isNull());
        QCOMPARE(attribute.typeId(), QMetaType::QDate);

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto dateDbVariant = datetime->getAttribute(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(dateDbVariant.isNull());

        /* Following is not true for QSQLITE driver:
           TinyORM Model fixes this and returns the QDate instead of QString.
           Would be possible to detect whether it's QSQLITE driver and a null attribute
           and on the base of that return the QVariant(QMetaType::QDate), but
           I have decided to leave it as it's now, I will not override this behavior.
           QSQLITE driver simply returns QVariant(QMetaType::QString) for all null
           values. */
        if (DB::driverName(connection) == QSQLITE)
            QCOMPARE(dateDbVariant.typeId(), QMetaType::QString);
        else
            QCOMPARE(dateDbVariant.typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

/* Server timezone +02:00 */

void tst_Model_QDateTime::create_QDateTime_Null_DatetimeAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = NullVariant::QDateTime();

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QVERIFY(attribute.isValid() && attribute.isNull());
        QCOMPARE(attribute.typeId(), QMetaType::QDateTime);

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(datetimeDbVariant.isNull());

        /* Following is not true for QSQLITE driver:
           TinyORM Model fixes this and returns the QDateTime instead of QString.
           Would be possible to detect whether it's QSQLITE driver and a null attribute
           and on the base of that return the QVariant(QMetaType::QDateTime), but
           I have decided to leave it as it's now, I will not override this behavior.
           QSQLITE driver simply returns QVariant(QMetaType::QString) for all null
           values. */
        if (DB::driverName(connection) == QSQLITE)
            QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QString);
        else
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
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::create_QDate_Null_DateAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*date] = NullVariant::QDate();

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*date);
        QVERIFY(attribute.isValid() && attribute.isNull());
        QCOMPARE(attribute.typeId(), QMetaType::QDate);

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto dateDbVariant = datetime->getAttribute(*date);
        QVERIFY(dateDbVariant.isValid());
        QVERIFY(dateDbVariant.isNull());

        /* Following is not true for QSQLITE driver:
           TinyORM Model fixes this and returns the QDate instead of QString.
           Would be possible to detect whether it's QSQLITE driver and a null attribute
           and on the base of that return the QVariant(QMetaType::QDate), but
           I have decided to leave it as it's now, I will not override this behavior.
           QSQLITE driver simply returns QVariant(QMetaType::QString) for all null
           values. */
        if (DB::driverName(connection) == QSQLITE)
            QCOMPARE(dateDbVariant.typeId(), QMetaType::QString);
        else
            QCOMPARE(dateDbVariant.typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate();
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true, connection);
}

/* QtTimeZoneType::DontConvert */

/* Server timezone UTC */

void tst_Model_QDateTime::
create_QDateTime_0300Timezone_DatetimeAttribute_UtcOnServer_DontConvert() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    /* The QDateTime's time zone is ignored with the QtTimeZoneType::DontConvert
       connection option, only toString(m_queryGrammar->getDateFormat()) is applied. */

    DB::setQtTimeZone(QtTimeZoneConfig {QtTimeZoneType::DontConvert}, connection);
    QCOMPARE(DB::qtTimeZone(connection),
             QtTimeZoneConfig {QtTimeZoneType::DontConvert});

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime[*datetime_] = QDateTime({2022, 8, 28}, {13, 14, 15},
                                         QTimeZone("UTC+03:00"));

        // Check whether a stored value and type are correct
        const auto attribute = datetime.getAttributeFromArray(*datetime_);
        QCOMPARE(attribute.typeId(), QMetaType::QString);
        QCOMPARE(attribute.value<QString>(), QString("2022-08-28 13:14:15"));

        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute(*datetime_);
        QVERIFY(datetimeDbVariant.isValid());
        QVERIFY(!datetimeDbVariant.isNull());

        QCOMPARE(datetimeDbVariant.typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();

/* Qt >=v6.8 fixes time zone handling, it calls toUTC() on QDateTime instance while
   sending QDateTime()-s to the database, calls SET time_zone = '+00:00' while opening
   a database connection, and returns QDateTime() instances with the UTC time zone during
   retrieving column values for both normal and prepared queries.
   This is the reason why we must use #if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
   everywhere for QtSql QMYSQL and QPSQL and tinymysql_lib_utc_qdatetime >= 20240618
   for TinyDrivers TinyMySql. */
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) || \
    (defined(tinymysql_lib_utc_qdatetime) && tinymysql_lib_utc_qdatetime >= 20240618)
        if (const auto driverName = DB::driverName(connection);
            driverName == QMYSQL || driverName == QPSQL
        ) {
            const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15},
                                                    QTimeZone::UTC);

            QCOMPARE(datetimeActual, datetimeExpected);
            QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
        }
        else if (driverName == QSQLITE) {
            const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15});

            QCOMPARE(datetimeActual, datetimeExpected);
            QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
            QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
        }
        else
            Q_UNREACHABLE();
#else
        const auto datetimeExpected = QDateTime({2022, 8, 28}, {13, 14, 15});

        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());
#endif
    }

    // Restore
    restore(lastId);

    DB::setQtTimeZone(QtTimeZoneConfig::utc(), connection);
    QCOMPARE(DB::qtTimeZone(connection), QtTimeZoneConfig::utc());
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

void tst_Model_QDateTime::setUtcTimezone(const QString &connection)
{
    setTimezone(utcTimezoneString(connection), QtTimeZoneConfig::utc(), connection);
}

const QString &tst_Model_QDateTime::utcTimezoneString(const QString &connection)
{
    const auto driverName = DB::driverName(connection);

    if (driverName == QMYSQL && mysqlTimezoneTablesNotPopulated(connection))
        return TZ00;

    return UTC;
}

bool
tst_Model_QDateTime::mysqlTimezoneTablesNotPopulated(const QString &connection)
{
    auto sqlQuery = DB::select(sl("select count(*) from `mysql`.`time_zone_name`"), {},
                               connection);

    if (!sqlQuery.first())
        return true;

    const auto tzTableRows = sqlQuery.value(0);

    // If 0 then TZ tables are empty
    return tzTableRows.value<quint64>() == 0;
}

void tst_Model_QDateTime::set0200Timezone(const QString &connection)
{
    const auto driverName = DB::driverName(connection);

    if (driverName == QMYSQL)
        set0200TimezoneForMySQL(connection);

    else if (driverName == QPSQL)
        setEUBratislavaTimezoneForPSQL(connection);

    // Do nothing for QSQLITE
}

/*! Time zone +02:00 for MySQL. */
Q_GLOBAL_STATIC_WITH_ARGS(const QTimeZone, TimeZone0200, (QByteArray("UTC+02:00"))) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)

/*! Time zone Europe/Bratislava for PostgreSQL. */
Q_GLOBAL_STATIC_WITH_ARGS(const QTimeZone, TimeZoneEUBratislava, // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
                          (QByteArray("Europe/Bratislava")))

void tst_Model_QDateTime::set0200TimezoneForMySQL(const QString &connection)
{
    setTimezone(sl("+02:00"),
                {QtTimeZoneType::QTimeZone, QVariant::fromValue(*TimeZone0200)},
                connection);
}

void tst_Model_QDateTime::setEUBratislavaTimezoneForPSQL(const QString &connection)
{
    setTimezone(sl("Europe/Bratislava"),
                {QtTimeZoneType::QTimeZone, QVariant::fromValue(*TimeZoneEUBratislava)},
                connection);
}

void tst_Model_QDateTime::setTimezone(
        const QString &timeZone, Orm::QtTimeZoneConfig &&qtTimeZone,
        const QString &connection)
{
    const auto sqlQuery = DB::unprepared(
                              getSetTimezoneQueryString(connection).arg(timeZone),
                              connection);

    QVERIFY(!sqlQuery.isValid() && sqlQuery.isActive() && !sqlQuery.isSelect());

    DB::setQtTimeZone(std::move(qtTimeZone), connection);
}

QString tst_Model_QDateTime::getSetTimezoneQueryString(const QString &connection)
{
    const auto driverName = DB::driverName(connection);

    if (driverName == QMYSQL)
        return sl("set time_zone=\"%1\";");

    if (driverName == QPSQL)
        return sl("set time zone '%1';");

    Q_UNREACHABLE();
}

const QTimeZone &tst_Model_QDateTime::timezone0200(const QString &connection)
{
    const auto driverName = DB::driverName(connection);

    if (driverName == QMYSQL || driverName == QSQLITE)
        return *TimeZone0200;

    if (driverName == QPSQL)
        return *TimeZoneEUBratislava;

    Q_UNREACHABLE();
}

void tst_Model_QDateTime::restore(const quint64 lastId, const bool restoreTimezone,
                                  const QString &connection)
{
    const auto affected = Datetime::destroy(lastId);

    QCOMPARE(affected, 1);

    if (!restoreTimezone)
        return;

    /* Restore also the MySQL/PostgreSQL timezone session variable to auto tests
       default UTC value. */
    if (!connection.isEmpty() && DB::driverName(connection) != QSQLITE)
        setUtcTimezone(connection);
}

QTEST_MAIN(tst_Model_QDateTime)

#include "tst_model_qdatetime.moc"
