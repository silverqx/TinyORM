#include <QCoreApplication>
#include <QTest>

#include "orm/db.hpp"
#include "orm/exceptions/logicerror.hpp"
#include "orm/schema.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"
#include "macros.hpp"

#ifndef TINYORM_DISABLE_ORM
#  include "models/user.hpp"
#endif

using Orm::Constants::HASH_;
using Orm::Constants::ID;
using Orm::Constants::MyISAM;
using Orm::Constants::NAME;
using Orm::Constants::QMYSQL;
using Orm::Constants::SIZE_;
using Orm::Constants::UTF8;
using Orm::Constants::UTF8Generalci;
using Orm::Constants::UTF8Unicodeci;
using Orm::Constants::Version;
using Orm::Constants::charset_;
using Orm::Constants::collation_;
using Orm::Constants::driver_;
using Orm::Constants::engine_;

using Orm::DB;
using Orm::Exceptions::LogicError;
using Orm::Schema;
using Orm::SchemaNs::Blueprint;
using Orm::SchemaNs::Constants::Cascade;
using Orm::SchemaNs::Constants::Restrict;

using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

// TEST future, divide tst_MySql_SchemaBuilder to two unit test cases, the new test case will be tst_MySql_SchemaGrammar, also divide them for SQLite and PostgreSQL databases silverqx
class tst_MySql_SchemaBuilder : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void createDatabase() const;
    void createDatabase_Charset_Collation() const;
    void dropDatabaseIfExists() const;

    void createTable() const;
    void createTable_Temporary() const;
    void createTable_Charset_Collation_Engine() const;
    void createTable_WithComment() const;

    void timestamps_rememberToken_softDeletes_CreateAndDrop() const;
    void datetimes_softDeletesDatetime_CreateAndDrop() const;

    void modifyTable() const;
    void modifyTable_WithComment() const;

    void dropTable() const;
    void dropTableIfExists() const;

    void rename() const;

    void dropColumns() const;
    void renameColumn() const;

    void dropAllTypes() const;

    void getAllTables() const;
    void getAllViews() const;

    void enableForeignKeyConstraints() const;
    void disableForeignKeyConstraints() const;

    void getColumnListing() const;

    void hasTable() const;

    void defaultStringLength_Set() const;

    void modifiers() const;
    void modifier_defaultValue_WithExpression() const;
    void modifier_defaultValue_WithBoolean() const;
    void modifier_defaultValue_Escaping() const;

    void change_modifiers() const;

    void useCurrent() const;
    void useCurrentOnUpdate() const;

    void multipleAutoIncrementStartingValue_CreateTable() const;
    void multipleAutoIncrementStartingValue_ModifyTable() const;

    /* Timestamps */
    void add_timestamp() const;
    void add_timestamp_WithPrecision() const;
    void add_timestamp_WithPrecision_StdNullopt() const;
    void add_timestamp_WithPrecision_LowerThanZero() const;
    void add_timestamp_WithDefaultValue() const;
    void add_timestamp_WithPrecision_UseCurrent() const;
    void add_timestamp_WithPrecision_UseCurrentOnUpdate() const;
    void add_timestamp_WithPrecision_UseCurrent_UseCurrentOnUpdate() const;

    void add_timestampTz() const;
    void add_timestampTz_WithPrecision() const;
    void add_timestampTz_WithPrecision_StdNullopt() const;
    void add_timestampTz_WithPrecision_LowerThanZero() const;

    /* Datetime */
    void add_datetime() const;
    void add_datetime_WithPrecision() const;
    void add_datetime_WithPrecision_StdNullopt() const;
    void add_datetime_WithPrecision_LowerThanZero() const;
    void add_datetime_WithDefaultValue() const;
    void add_datetime_WithPrecision_UseCurrent() const;
    void add_datetime_WithPrecision_UseCurrentOnUpdate() const;
    void add_datetime_WithPrecision_UseCurrent_UseCurrentOnUpdate() const;

    void add_datetimeTz() const;
    void add_datetimeTz_WithPrecision() const;
    void add_datetimeTz_WithPrecision_StdNullopt() const;
    void add_datetimeTz_WithPrecision_LowerThanZero() const;

    /* Time */
    void add_time() const;
    void add_time_WithPrecision() const;
    void add_time_WithPrecision_StdNullopt() const;
    void add_time_WithPrecision_LowerThanZero() const;

    void add_timeTz() const;
    void add_timeTz_WithPrecision() const;
    void add_timeTz_WithPrecision_StdNullopt() const;
    void add_timeTz_WithPrecision_LowerThanZero() const;

    /* MySQL generated columns */
    void virtualAs_StoredAs_CreateTable() const;
    void virtualAs_StoredAs_Nullable_CreateTable() const;
    void virtualAs_StoredAs_ModifyTable() const;
    void virtualAs_StoredAs_Nullable_ModifyTable() const;

    void virtualAs_StoredAs_WithCharset() const;

    /* MariaDB generated columns */
    void virtualAs_StoredAs_CreateTable_Maria() const;
    void virtualAs_StoredAs_Nullable_CreateTable_Maria() const;
    void virtualAs_StoredAs_ModifyTable_Maria() const;
    void virtualAs_StoredAs_Nullable_ModifyTable_Maria() const;

    /* Indexes */
    void indexes_Fluent() const;
    void indexes_Blueprint() const;

    void add_PrimaryKey() const;
    void add_PrimaryKey_WithAlgorithm() const;

    void renameIndex() const;

    void dropIndex_ByIndexName() const;
    void dropIndex_ByColumn() const;
    void dropIndex_ByMultipleColumns() const;

    void foreignKey() const;
    void foreignKey_TerserSyntax() const;
#ifndef TINYORM_DISABLE_ORM
    void foreignKey_WithModel() const;
#endif

    void dropForeign() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Table or database name used in tests. */
    inline static const auto Firewalls = sl("firewalls");
    /*! Test case class name. */
    inline static const auto *ClassName = "tst_MySql_SchemaBuilder";

    /*! Get the MySQL connection name. */
    static QString getMySqlConnectionName(QStringList &connections);
    /*! Get the MariaDB connection name. */
    static QString getMariaConnectionName(QStringList &connections);

    /*! Initialize m_charset/Maria and m_collation/Maria for both, MySQL and MariaDB. */
    void initializeCharsetAndCollation();

    /* Default MySQL connection */
    /*! Connection name used in this test case (default MYSQL connection). */
    QString m_connection;

    /*! The charset set for the current MySQL connection (based on env. variable). */
    QString m_charset;
    /*! The collation set for the current MySQL connection (based on env. variable). */
    QString m_collation;

    /* Second MariaDB connection */
    /*! Second connection name used in this test case (MARIADB connection). */
    QString m_connectionMaria;

    /*! The charset set for the second MariaDB connection (based on env. variable). */
    QString m_charsetMaria;
    /*! The collation set for the second MariaDB connection (based on env. variable). */
    QString m_collationMaria;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_MySql_SchemaBuilder::initTestCase()
{
    /* No need to test with the MariaDB (MARIADB connection) because it will produce
       the same output in 99% cases. I will create a new MARIADB connection as needed,
       eg. see nullable modifier tests for generated columns.
       Also, I can't use the setConfigVersion("11.3.2-MariaDB") technique here as
       in the upsert tests because of the MySqlSchemaGrammar::m_isMaria,
       the MySqlSchemaGrammar is instatiated only once for the DatabaseConnection. */
    auto connections = Databases::createConnections({Databases::MYSQL,
                                                     Databases::MARIADB});

    // Initialize connection names
    m_connection = getMySqlConnectionName(connections);
    m_connectionMaria = getMariaConnectionName(connections);

    // Also MariaDB related tests will be skiped if the MYSQL connection was not defined
    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );


    // The charset and collation in all queries is set on the base of env. variables
    initializeCharsetAndCollation();
}

void tst_MySql_SchemaBuilder::createDatabase() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).createDatabase(Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             sl("create database `firewalls` "
                "default character set `%1` default collate `%2`")
             .arg(m_charset, m_collation));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::createDatabase_Charset_Collation() const
{
    // Add a new database connection with different charset and collation
    const auto connectionName =
            Databases::createConnectionTempFrom(
                Databases::MYSQL, {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {driver_,    QMYSQL},
        {charset_,   UTF8},
        {collation_, UTF8Generalci},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );

    auto log = DB::connection(*connectionName).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).createDatabase(Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             sl("create database `firewalls` "
                "default character set `%1` default collate `%2`")
             .arg(UTF8, UTF8Generalci));
    QVERIFY(firstLog.boundValues.isEmpty());

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_MySql_SchemaBuilder::dropDatabaseIfExists() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).dropDatabaseIfExists(Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, sl("drop database if exists `%1`").arg(Firewalls));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::createTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id();
            table.Char("char");
            table.Char("char_10", 10);
            table.string("string");
            table.string("string_22", 22);
            table.tinyText("tiny_text");
            table.text("text");
            table.mediumText("medium_text");
            table.longText("long_text");

            table.integer("integer");
            table.tinyInteger("tinyInteger");
            table.smallInteger("smallInteger");
            table.mediumInteger("mediumInteger");
            table.bigInteger("bigInteger");

            table.unsignedInteger("unsignedInteger");
            table.unsignedTinyInteger("unsignedTinyInteger");
            table.unsignedSmallInteger("unsignedSmallInteger");
            table.unsignedMediumInteger("unsignedMediumInteger");
            table.unsignedBigInteger("unsignedBigInteger");

            table.uuid();
            table.ipAddress();
            table.macAddress();

            table.tinyBinary("tiny_binary");
            table.binary("binary");
            table.mediumBinary("medium_binary");
            table.longBinary("long_binary");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             sl("create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`char` char(255) not null, "
                "`char_10` char(10) not null, "
                "`string` varchar(255) not null, "
                "`string_22` varchar(22) not null, "
                "`tiny_text` tinytext not null, "
                "`text` text not null, "
                "`medium_text` mediumtext not null, "
                "`long_text` longtext not null, "
                "`integer` int not null, "
                "`tinyInteger` tinyint not null, "
                "`smallInteger` smallint not null, "
                "`mediumInteger` mediumint not null, "
                "`bigInteger` bigint not null, "
                "`unsignedInteger` int unsigned not null, "
                "`unsignedTinyInteger` tinyint unsigned not null, "
                "`unsignedSmallInteger` smallint unsigned not null, "
                "`unsignedMediumInteger` mediumint unsigned not null, "
                "`unsignedBigInteger` bigint unsigned not null, "
                "`uuid` char(36) not null, "
                "`ip_address` varchar(45) not null, "
                "`mac_address` varchar(17) not null, "
                "`tiny_binary` tinyblob not null, "
                "`binary` blob not null, "
                "`medium_binary` mediumblob not null, "
                "`long_binary` longblob not null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::createTable_Temporary() const
{
    auto &connection = DB::connection(m_connection);

    auto log = connection.pretend([](auto &connection_)
    {
        Schema::on(connection_.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.temporary();

            table.id();
            table.string(NAME);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             sl("create temporary table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`name` varchar(255) not null) "
                "default character set %1 collate '%2' "
                "engine = %3")
             .arg(m_charset, m_collation,
                  connection.getConfig(engine_).value<QString>()));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::createTable_Charset_Collation_Engine() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.charset = UTF8;
            table.collation = UTF8Generalci;
            table.engine = MyISAM;

            table.id();
            table.string(NAME);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             sl("create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`name` varchar(255) not null) "
                "default character set %1 collate '%2' "
                "engine = %3")
             .arg(UTF8, UTF8Generalci, MyISAM));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::createTable_WithComment() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.comment(sl("Example 'table' comment"));

            table.id();
            table.string(NAME);
        });
    });

    QCOMPARE(log.size(), 2);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             sl("create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`name` varchar(255) not null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` comment = 'Example ''table'' comment'");
    QVERIFY(log1.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::timestamps_rememberToken_softDeletes_CreateAndDrop() const
{
    auto &connection = DB::connection(m_connection);

    auto log = connection.pretend([](auto &connection_)
    {
        Schema::on(connection_.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id();

            table.timestamps();
            table.rememberToken();
            table.softDeletes();
        });

        Schema::on(connection_.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.dropTimestamps();
            table.dropRememberToken();
            table.dropSoftDeletes();
        });

        Schema::on(connection_.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id();

            table.timestamps(3);
        });
    });

    QCOMPARE(log.size(), 5);

    const auto engineConfig = connection.getConfig(engine_).value<QString>();

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             sl("create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`created_at` timestamp null, "
                "`updated_at` timestamp null, "
                "`remember_token` varchar(100) null, "
                "`deleted_at` timestamp null) "
                "default character set %1 collate '%2' "
                "engine = %3")
             .arg(m_charset, m_collation, engineConfig));
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` drop `created_at`, drop `updated_at`");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` drop `remember_token`");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table `firewalls` drop `deleted_at`");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             sl("create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`created_at` timestamp(3) null, "
                "`updated_at` timestamp(3) null) "
                "default character set %1 collate '%2' "
                "engine = %3")
             .arg(m_charset, m_collation, engineConfig));
    QVERIFY(log4.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::datetimes_softDeletesDatetime_CreateAndDrop() const
{
    auto &connection = DB::connection(m_connection);

    auto log = connection.pretend([](auto &connection_)
    {
        Schema::on(connection_.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id();

            table.datetimes();
            table.softDeletesDatetime();
        });

        Schema::on(connection_.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.dropDatetimes();
            table.dropSoftDeletesDatetime();
        });

        Schema::on(connection_.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id();

            table.datetimes(3);
        });
    });

    QCOMPARE(log.size(), 4);

    const auto engineConfig = connection.getConfig(engine_).value<QString>();

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             sl("create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`created_at` datetime null, "
                "`updated_at` datetime null, "
                "`deleted_at` datetime null) "
                "default character set %1 collate '%2' "
                "engine = %3")
             .arg(m_charset, m_collation, engineConfig));
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` drop `created_at`, drop `updated_at`");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` drop `deleted_at`");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             sl("create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`created_at` datetime(3) null, "
                "`updated_at` datetime(3) null) "
                "default character set %1 collate '%2' "
                "engine = %3")
             .arg(m_charset, m_collation, engineConfig));
    QVERIFY(log3.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::modifyTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.Char("char");
            table.Char("char_10", 10);
            table.string("string");
            table.string("string_22", 22);
            table.tinyText("tiny_text");
            table.text("text");
            table.mediumText("medium_text");
            table.longText("long_text");

            table.integer("integer").nullable();
            table.tinyInteger("tinyInteger");
            table.smallInteger("smallInteger");
            table.mediumInteger("mediumInteger");

            table.tinyBinary("tiny_binary");
            table.binary("binary");
            table.mediumBinary("medium_binary");
            table.longBinary("long_binary");

            table.dropColumn("long_text");
            table.dropColumns({"medium_text", "text"});
            table.dropColumns("smallInteger", "mediumInteger");

            table.renameColumn("integer", "integer_renamed");
            table.renameColumn("string_22", "string_22_renamed");
        });
    });

    QCOMPARE(log.size(), 6);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "alter table `firewalls` "
             "add column `char` char(255) not null, "
             "add column `char_10` char(10) not null, "
             "add column `string` varchar(255) not null, "
             "add column `string_22` varchar(22) not null, "
             "add column `tiny_text` tinytext not null, "
             "add column `text` text not null, "
             "add column `medium_text` mediumtext not null, "
             "add column `long_text` longtext not null, "
             "add column `integer` int null, "
             "add column `tinyInteger` tinyint not null, "
             "add column `smallInteger` smallint not null, "
             "add column `mediumInteger` mediumint not null, "
             "add column `tiny_binary` tinyblob not null, "
             "add column `binary` blob not null, "
             "add column `medium_binary` mediumblob not null, "
             "add column `long_binary` longblob not null");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` drop `long_text`");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` drop `medium_text`, drop `text`");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table `firewalls` drop `smallInteger`, drop `mediumInteger`");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "alter table `firewalls` rename column `integer` to `integer_renamed`");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             "alter table `firewalls` rename column `string_22` to `string_22_renamed`");
    QVERIFY(log5.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::modifyTable_WithComment() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.comment(sl("Example 'table' comment"));
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` comment = 'Example ''table'' comment'");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::dropTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).drop(Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, sl("drop table `%1`").arg(Firewalls));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::dropTableIfExists() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).dropIfExists(Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             sl("drop table if exists `%1`").arg(Firewalls));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::rename() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).rename("secured", Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, "rename table `secured` to `firewalls`");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::dropColumns() const
{
    {
        auto log = DB::connection(m_connection).pretend([](auto &connection)
        {
            Schema::on(connection.getName()).dropColumn(Firewalls, NAME);
        });

        QVERIFY(!log.isEmpty());
        const auto &firstLog = log.first();

        QCOMPARE(log.size(), 1);
        QCOMPARE(firstLog.query, "alter table `firewalls` drop `name`");
        QVERIFY(firstLog.boundValues.isEmpty());
    }
    {
        auto log = DB::connection(m_connection).pretend([](auto &connection)
        {
            Schema::on(connection.getName()).dropColumns(Firewalls, {NAME, SIZE_});
        });

        QVERIFY(!log.isEmpty());
        const auto &firstLog = log.first();

        QCOMPARE(log.size(), 1);
        QCOMPARE(firstLog.query, "alter table `firewalls` drop `name`, drop `size`");
        QVERIFY(firstLog.boundValues.isEmpty());
    }
    {
        auto log = DB::connection(m_connection).pretend([](auto &connection)
        {
            Schema::on(connection.getName()).dropColumns(Firewalls, NAME, SIZE_);
        });

        QVERIFY(!log.isEmpty());
        const auto &firstLog = log.first();

        QCOMPARE(log.size(), 1);
        QCOMPARE(firstLog.query, "alter table `firewalls` drop `name`, drop `size`");
        QVERIFY(firstLog.boundValues.isEmpty());
    }
}

void tst_MySql_SchemaBuilder::renameColumn() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).renameColumn(Firewalls, NAME, "first_name");
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` rename column `name` to `first_name`");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::dropAllTypes() const
{
    TVERIFY_THROWS_EXCEPTION(LogicError, Schema::on(m_connection).dropAllTypes());
}

void tst_MySql_SchemaBuilder::getAllTables() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).getAllTables();
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, "show full tables where table_type = 'BASE TABLE'");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::getAllViews() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).getAllViews();
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, "show full tables where table_type = 'VIEW'");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::enableForeignKeyConstraints() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).enableForeignKeyConstraints();
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, "set foreign_key_checks = on");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::disableForeignKeyConstraints() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).disableForeignKeyConstraints();
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, "set foreign_key_checks = off");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::getColumnListing() const
{
    auto &connection = DB::connection(m_connection);

    auto log = connection.pretend([](auto &connection_)
    {
        Schema::on(connection_.getName()).getColumnListing(Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select `column_name` as `column_name` "
             "from `information_schema`.`columns` "
             "where `table_schema` = ? and `table_name` = ?");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(connection.getDatabaseName()),
                              QVariant(Firewalls)}));
}

void tst_MySql_SchemaBuilder::hasTable() const
{
    auto &connection = DB::connection(m_connection);

    auto log = connection.pretend([](auto &connection_)
    {
        Schema::on(connection_.getName()).hasTable(Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select * "
             "from `information_schema`.`tables` "
             "where `table_schema` = ? and `table_name` = ? and "
             "`table_type` = 'BASE TABLE'");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(connection.getDatabaseName()),
                              QVariant(Firewalls)}));
}

void tst_MySql_SchemaBuilder::defaultStringLength_Set() const
{
    QVERIFY(Blueprint::DefaultStringLength == Orm::SchemaNs::DefaultStringLength);

    Schema::defaultStringLength(191);
    QCOMPARE(Blueprint::DefaultStringLength, 191);

    // Restore
    Schema::defaultStringLength(Orm::SchemaNs::DefaultStringLength);
    QVERIFY(Blueprint::DefaultStringLength == Orm::SchemaNs::DefaultStringLength);
}

void tst_MySql_SchemaBuilder::modifiers() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.bigInteger(ID).autoIncrement().isUnsigned().startingValue(5);
            table.bigInteger("big_int").isUnsigned();
            table.bigInteger("big_int1");
            table.string(NAME).defaultValue("guest");
            table.string("name1").nullable();
            table.string("name2").comment("name2 note");
            table.string("name3", 191);
            table.string("name4").invisible();
            table.string("name5").charset(UTF8);
            table.string("name6").collation("utf8mb4_unicode_ci");
            table.string("name7").charset(UTF8).collation(UTF8Unicodeci);
            table.Double("amount", 6, 2);
        });
        /* Tests from and also integerIncrements, this would of course fail on real DB
           as you can not have two primary keys. */
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.string(NAME).after("big_int");
            table.integerIncrements(ID).from(15).first();
        });
    });

    QCOMPARE(log.size(), 4);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             sl("create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`big_int` bigint unsigned not null, "
                "`big_int1` bigint not null, "
                "`name` varchar(255) not null default 'guest', "
                "`name1` varchar(255) null, "
                "`name2` varchar(255) not null comment 'name2 note', "
                "`name3` varchar(191) not null, "
                "`name4` varchar(255) not null invisible, "
                "`name5` varchar(255) character set 'utf8' not null, "
                "`name6` varchar(255) collate 'utf8mb4_unicode_ci' not null, "
                "`name7` varchar(255) character set 'utf8' collate 'utf8_unicode_ci' "
                  "not null, "
                "`amount` double(6, 2) not null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` auto_increment = 5");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` "
             "add column `name` varchar(255) not null after `big_int`, "
             "add column `id` int unsigned not null auto_increment primary key first");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table `firewalls` auto_increment = 15");
    QVERIFY(log3.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::modifier_defaultValue_WithExpression() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.string(NAME).defaultValue("guest");
            table.string("name_raw").defaultValue(DB::raw("'guest_raw'"));
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             sl("create table `firewalls` ("
                "`name` varchar(255) not null default 'guest', "
                "`name_raw` varchar(255) not null default 'guest_raw') "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::modifier_defaultValue_WithBoolean() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.boolean("boolean");
            table.boolean("boolean_false").defaultValue(false);
            table.boolean("boolean_true").defaultValue(true);
            table.boolean("boolean_0").defaultValue(0);
            table.boolean("boolean_1").defaultValue(1);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             sl("create table `firewalls` ("
                "`boolean` tinyint(1) not null, "
                "`boolean_false` tinyint(1) not null default '0', "
                "`boolean_true` tinyint(1) not null default '1', "
                "`boolean_0` tinyint(1) not null default '0', "
                "`boolean_1` tinyint(1) not null default '1') "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::modifier_defaultValue_Escaping() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            // String contains \t after tab word
            table.string("string").defaultValue(R"(Text ' and " or \ newline
and tab	end)");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             // String contains \t after tab word
             sl("create table `firewalls` ("
                "`string` varchar(255) not null default 'Text '' and \" or \\\\ newline\n"
                "and tab	end') "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::change_modifiers() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.bigInteger(ID).autoIncrement().isUnsigned().startingValue(5).change();
            table.bigInteger("big_int").isUnsigned().change();
            table.bigInteger("big_int1").change();
            table.string(NAME).defaultValue("guest").change();
            table.string("name1").nullable().change();
            table.string("name2").comment("name2 note").change();
            table.string("name3", 191).change();
            table.string("name4").invisible().change();
            table.string("name5").charset(UTF8).change();
            table.string("name6").collation("utf8mb4_unicode_ci").change();
            table.string("name7").charset(UTF8).collation(UTF8Unicodeci).change();
            table.string("name8_old", 64).renameTo("name8").change();
            table.Double("amount", 6, 2).change();
            table.multiPolygon("positions").srid(1234).storedAs("expression").change();
            table.multiPoint("positions1").srid(1234).virtualAs("expression").nullable()
                 .change();
            table.timestamp("added_on").nullable(false).useCurrent().change();
            table.timestamp("updated_at", 4).useCurrent().useCurrentOnUpdate().change();
        });
        /* Tests from and also integerIncrements, this would of course fail on real DB
           as you can not have two primary keys. */
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.string(NAME).after("big_int").change();
            table.integerIncrements(ID).from(15).first().change();
        });
    });

    QCOMPARE(log.size(), 4);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "alter table `firewalls` "
             "modify column `id` bigint unsigned not null auto_increment primary key, "
             "modify column `big_int` bigint unsigned not null, "
             "modify column `big_int1` bigint not null, "
             "modify column `name` varchar(255) not null default 'guest', "
             "modify column `name1` varchar(255) null, "
             "modify column `name2` varchar(255) not null comment 'name2 note', "
             "modify column `name3` varchar(191) not null, "
             "modify column `name4` varchar(255) not null invisible, "
             "modify column `name5` varchar(255) character set 'utf8' not null, "
             "modify column `name6` varchar(255) collate 'utf8mb4_unicode_ci' not null, "
             "modify column `name7` varchar(255) character set 'utf8' "
               "collate 'utf8_unicode_ci' not null, "
             "change column `name8_old` `name8` varchar(64) not null, "
             "modify column `amount` double(6, 2) not null, "
             "modify column `positions` multipolygon "
               "generated always as (expression) stored not null srid 1234, "
             "modify column `positions1` multipoint generated always as (expression) "
               "null srid 1234, "
             "modify column `added_on` timestamp not null default current_timestamp, "
             "modify column `updated_at` timestamp(4) not null "
               "default current_timestamp(4) on update current_timestamp(4)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` auto_increment = 5");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` "
             "modify column `name` varchar(255) not null after `big_int`, "
             "modify column `id` int unsigned not null auto_increment primary key first");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table `firewalls` auto_increment = 15");
    QVERIFY(log3.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::useCurrent() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.datetime("created");
            table.datetime("created_current").useCurrent();

            table.timestamp("created_t");
            table.timestamp("created_t_current").useCurrent();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             sl("create table `firewalls` ("
                "`created` datetime not null, "
                "`created_current` datetime not null default current_timestamp, "
                "`created_t` timestamp not null, "
                "`created_t_current` timestamp not null default current_timestamp) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::useCurrentOnUpdate() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.datetime("updated");
            table.datetime("updated_current").useCurrentOnUpdate();

            table.timestamp("updated_t");
            table.timestamp("updated_t_current").useCurrentOnUpdate();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             sl("create table `firewalls` ("
                "`updated` datetime not null, "
                "`updated_current` datetime not null on update current_timestamp, "
                "`updated_t` timestamp not null, "
                "`updated_t_current` timestamp not null on update current_timestamp) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::multipleAutoIncrementStartingValue_CreateTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id().from(100);
            table.string(NAME).from(200);
        });
    });

    QCOMPARE(log.size(), 2);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             sl("create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`name` varchar(255) not null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` auto_increment = 100");
    QVERIFY(log1.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::multipleAutoIncrementStartingValue_ModifyTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.id().from(100);
            table.string(NAME).from(200);
        });
    });

    QCOMPARE(log.size(), 2);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "alter table `firewalls` "
             "add column `id` bigint unsigned not null auto_increment primary key, "
             "add column `name` varchar(255) not null");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` auto_increment = 100");
    QVERIFY(log1.boundValues.isEmpty());
}

/* Timestamps */

void tst_MySql_SchemaBuilder::add_timestamp() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.timestamp("created_at");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` timestamp not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_timestamp_WithPrecision() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.timestamp("created_at", 1);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` timestamp(1) not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_timestamp_WithPrecision_StdNullopt() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.timestamp("created_at", std::nullopt);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` timestamp not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_timestamp_WithPrecision_LowerThanZero() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.timestamp("created_at", -1);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` timestamp not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_timestamp_WithDefaultValue() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.timestamp("created_at").defaultValue("2023-02-27 10:10:11");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` timestamp not null default '2023-02-27 10:10:11'");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_timestamp_WithPrecision_UseCurrent() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.timestamp("created_at", 1).useCurrent();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` timestamp(1) not null "
               "default current_timestamp(1)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_timestamp_WithPrecision_UseCurrentOnUpdate() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.timestamp("created_at", 1).useCurrentOnUpdate();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` timestamp(1) not null "
               "on update current_timestamp(1)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::
     add_timestamp_WithPrecision_UseCurrent_UseCurrentOnUpdate() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.timestamp("created_at", 1).useCurrent().useCurrentOnUpdate();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` timestamp(1) not null default current_timestamp(1) "
               "on update current_timestamp(1)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_timestampTz() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.timestampTz("created_at");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` timestamp not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_timestampTz_WithPrecision() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.timestampTz("created_at", 1);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` timestamp(1) not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_timestampTz_WithPrecision_StdNullopt() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.timestampTz("created_at", std::nullopt);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` timestamp not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_timestampTz_WithPrecision_LowerThanZero() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.timestampTz("created_at", -1);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` timestamp not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

/* Datetime */

void tst_MySql_SchemaBuilder::add_datetime() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.datetime("created_at");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` datetime not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_datetime_WithPrecision() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.datetime("created_at", 1);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` datetime(1) not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_datetime_WithPrecision_StdNullopt() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.datetime("created_at", std::nullopt);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` datetime not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_datetime_WithPrecision_LowerThanZero() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.datetime("created_at", -1);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` datetime not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_datetime_WithDefaultValue() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.datetime("created_at").defaultValue("2023-02-27 10:10:11");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` datetime not null default '2023-02-27 10:10:11'");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_datetime_WithPrecision_UseCurrent() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.datetime("created_at", 1).useCurrent();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` datetime(1) not null default current_timestamp(1)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_datetime_WithPrecision_UseCurrentOnUpdate() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.datetime("created_at", 1).useCurrentOnUpdate();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` datetime(1) not null "
               "on update current_timestamp(1)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::
     add_datetime_WithPrecision_UseCurrent_UseCurrentOnUpdate() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.datetime("created_at", 1).useCurrent().useCurrentOnUpdate();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` datetime(1) not null default current_timestamp(1) "
               "on update current_timestamp(1)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_datetimeTz() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.datetimeTz("created_at");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` datetime not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_datetimeTz_WithPrecision() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.datetimeTz("created_at", 1);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` datetime(1) not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_datetimeTz_WithPrecision_StdNullopt() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.datetimeTz("created_at", std::nullopt);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` datetime not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_datetimeTz_WithPrecision_LowerThanZero() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.datetimeTz("created_at", -1);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` datetime not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

/* Time */

void tst_MySql_SchemaBuilder::add_time() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.time("created_at");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` time not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_time_WithPrecision() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.time("created_at", 1);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` time(1) not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_time_WithPrecision_StdNullopt() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.time("created_at", std::nullopt);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` time not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_time_WithPrecision_LowerThanZero() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.time("created_at", -1);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` time not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_timeTz() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.timeTz("created_at");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` time not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_timeTz_WithPrecision() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.timeTz("created_at", 1);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` time(1) not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_timeTz_WithPrecision_StdNullopt() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.timeTz("created_at", std::nullopt);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` time not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_timeTz_WithPrecision_LowerThanZero() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.timeTz("created_at", -1);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `created_at` time not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

/* MySQL generated columns */

void tst_MySql_SchemaBuilder::virtualAs_StoredAs_CreateTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.string("first_name");
            table.string("last_name");

            table.string("full_name_virtual")
                 .virtualAs("concat(`first_name`, ' ', `last_name`)");

            table.string("full_name_stored")
                 .storedAs("concat(`first_name`, ' ', `last_name`)");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             sl("create table `firewalls` ("
                "`first_name` varchar(255) not null, "
                "`last_name` varchar(255) not null, "
                "`full_name_virtual` varchar(255) "
                  "generated always as (concat(`first_name`, ' ', `last_name`)) "
                  "not null, "
                "`full_name_stored` varchar(255) "
                  "generated always as (concat(`first_name`, ' ', `last_name`)) stored "
                  "not null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::virtualAs_StoredAs_Nullable_CreateTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.string("first_name");
            table.string("last_name");

            table.string("full_name_virtual")
                 .virtualAs("concat(`first_name`, ' ', `last_name`)").nullable();

            table.string("full_name_stored")
                 .storedAs("concat(`first_name`, ' ', `last_name`)").nullable();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             sl("create table `firewalls` ("
                "`first_name` varchar(255) not null, "
                "`last_name` varchar(255) not null, "
                "`full_name_virtual` varchar(255) "
                  "generated always as (concat(`first_name`, ' ', `last_name`)) null, "
                "`full_name_stored` varchar(255) "
                  "generated always as (concat(`first_name`, ' ', `last_name`)) stored "
                  "null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::virtualAs_StoredAs_ModifyTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.integer("price");
            table.integer("discounted_virtual").virtualAs("price - 5");
            table.integer("discounted_stored").storedAs("`price` - 5");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `price` int not null, "
             "add column `discounted_virtual` int "
               "generated always as (price - 5) not null, "
             "add column `discounted_stored` int "
               "generated always as (`price` - 5) stored not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::virtualAs_StoredAs_Nullable_ModifyTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.integer("price");
            table.integer("discounted_virtual").virtualAs("`price` - 5").nullable();
            table.integer("discounted_stored").storedAs("price - 5").nullable();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `price` int not null, "
             "add column `discounted_virtual` int generated always as (`price` - 5) "
               "null, "
             "add column `discounted_stored` int generated always as (price - 5) stored "
               "null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::virtualAs_StoredAs_WithCharset() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.string("url", 2083);
            table.string("url_hash_virtual", 64).virtualAs("sha2(url, 256)")
                 .charset("ascii");
            table.string("url_hash_stored", 64).storedAs("sha2(`url`, 256)")
                 .charset("ascii");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `url` varchar(2083) not null, "
             "add column `url_hash_virtual` varchar(64) character set 'ascii' "
               "generated always as (sha2(url, 256)) not null, "
             "add column `url_hash_stored` varchar(64) character set 'ascii' "
               "generated always as (sha2(`url`, 256)) stored not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

/* MariaDB generated columns */

void tst_MySql_SchemaBuilder::virtualAs_StoredAs_CreateTable_Maria() const
{
    // Add a new MARIADB database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::MARIADB,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        // Force MariaDB version because autodetection can't work with pretended queries
        {Version, "11.3.2-MariaDB"}
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MARIADB)
              .toUtf8().constData(), );

    // Verify
    auto log = DB::connection(*connectionName).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.string("first_name");
            table.string("last_name");

            table.string("full_name_virtual")
                 .virtualAs("concat(`first_name`, ' ', `last_name`)");

            table.string("full_name_stored")
                 .storedAs("concat(`first_name`, ' ', `last_name`)");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             sl("create table `firewalls` ("
                "`first_name` varchar(255) not null, "
                "`last_name` varchar(255) not null, "
                "`full_name_virtual` varchar(255) "
                  "generated always as (concat(`first_name`, ' ', `last_name`)), "
                "`full_name_stored` varchar(255) "
                  "generated always as (concat(`first_name`, ' ', `last_name`)) stored) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charsetMaria, m_collationMaria));
    QVERIFY(firstLog.boundValues.isEmpty());

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_MySql_SchemaBuilder::virtualAs_StoredAs_Nullable_CreateTable_Maria() const
{
    // Add a new MARIADB database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::MARIADB,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        // Force MariaDB version because autodetection can't work with pretended queries
        {Version, "11.3.2-MariaDB"}
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MARIADB)
              .toUtf8().constData(), );

    // Verify
    auto log = DB::connection(*connectionName).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.string("first_name");
            table.string("last_name");

            table.string("full_name_virtual")
                 .virtualAs("concat(`first_name`, ' ', `last_name`)").nullable();

            table.string("full_name_stored")
                 .storedAs("concat(`first_name`, ' ', `last_name`)").nullable();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             sl("create table `firewalls` ("
                "`first_name` varchar(255) not null, "
                "`last_name` varchar(255) not null, "
                "`full_name_virtual` varchar(255) "
                  "generated always as (concat(`first_name`, ' ', `last_name`)), "
                "`full_name_stored` varchar(255) "
                  "generated always as (concat(`first_name`, ' ', `last_name`)) stored) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charsetMaria, m_collationMaria));
    QVERIFY(firstLog.boundValues.isEmpty());

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_MySql_SchemaBuilder::virtualAs_StoredAs_ModifyTable_Maria() const
{
    // Add a new MARIADB database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::MARIADB,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        // Force MariaDB version because autodetection can't work with pretended queries
        {Version, "11.3.2-MariaDB"}
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MARIADB)
              .toUtf8().constData(), );

    // Verify
    auto log = DB::connection(*connectionName).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.integer("price");
            table.integer("discounted_virtual").virtualAs("price - 5");
            table.integer("discounted_stored").storedAs("`price` - 5");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `price` int not null, "
             "add column `discounted_virtual` int generated always as (price - 5), "
             "add column `discounted_stored` int "
               "generated always as (`price` - 5) stored");
    QVERIFY(firstLog.boundValues.isEmpty());

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_MySql_SchemaBuilder::virtualAs_StoredAs_Nullable_ModifyTable_Maria() const
{
    // Add a new MARIADB database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::MARIADB,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        // Force MariaDB version because autodetection can't work with pretended queries
        {Version, "11.3.2-MariaDB"}
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MARIADB)
              .toUtf8().constData(), );

    // Verify
    auto log = DB::connection(*connectionName).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.integer("price");
            table.integer("discounted_virtual").virtualAs("`price` - 5").nullable();
            table.integer("discounted_stored").storedAs("price - 5").nullable();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` "
             "add column `price` int not null, "
             "add column `discounted_virtual` int generated always as (`price` - 5), "
             "add column `discounted_stored` int generated always as (price - 5) stored");
    QVERIFY(firstLog.boundValues.isEmpty());

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

/* Indexes */

void tst_MySql_SchemaBuilder::indexes_Fluent() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        // Fluent indexes
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id();

            table.string("name_u").unique();

            table.string("name_i").index();
            table.string("name_i_cn").index("name_i_cn_index");

            table.string("name_f").fulltext();
            table.string("name_f_cn").fulltext("name_f_cn_fulltext");

            table.geometry("coordinates_s").spatialIndex();
            table.geometry("coordinates_s_cn").spatialIndex("coordinates_s_cn_spatial");
        });
    });

    QCOMPARE(log.size(), 8);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             sl("create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`name_u` varchar(255) not null, "
                "`name_i` varchar(255) not null, "
                "`name_i_cn` varchar(255) not null, "
                "`name_f` varchar(255) not null, "
                "`name_f_cn` varchar(255) not null, "
                "`coordinates_s` geometry not null, "
                "`coordinates_s_cn` geometry not null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` "
             "add unique index `firewalls_name_u_unique`(`name_u`)");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` add index `firewalls_name_i_index`(`name_i`)");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table `firewalls` add index `name_i_cn_index`(`name_i_cn`)");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "alter table `firewalls` "
             "add fulltext index `firewalls_name_f_fulltext`(`name_f`)");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             "alter table `firewalls` "
             "add fulltext index `name_f_cn_fulltext`(`name_f_cn`)");
    QVERIFY(log5.boundValues.isEmpty());

    const auto &log6 = log.at(6);
    QCOMPARE(log6.query,
             "alter table `firewalls` "
             "add spatial index `firewalls_coordinates_s_spatialindex`(`coordinates_s`)");
    QVERIFY(log6.boundValues.isEmpty());

    const auto &log7 = log.at(7);
    QCOMPARE(log7.query,
             "alter table `firewalls` "
             "add spatial index `coordinates_s_cn_spatial`(`coordinates_s_cn`)");
    QVERIFY(log7.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::indexes_Blueprint() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        // Blueprint indexes
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id();

            table.string("name_u");
            table.unique({"name_u"}, "name_u_unique");

            table.string("name_i");
            table.index({"name_i"});

            table.string("name_i_cn");
            table.index("name_i_cn", "name_i_cn_index");

            table.string("name_r");
            table.string("name_r1");
            table.rawIndex(DB::raw("`name_r`, name_r1"), "name_r_raw");

            table.string("name_f");
            table.fullText({"name_f"});

            table.string("name_f_cn");
            table.fullText("name_f_cn", "name_f_cn_fulltext");

            table.geometry("coordinates_s");
            table.spatialIndex("coordinates_s");

            table.geometry("coordinates_s_cn");
            table.spatialIndex("coordinates_s_cn", "coordinates_s_cn_spatial");
        });
    });

    QCOMPARE(log.size(), 9);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             sl("create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`name_u` varchar(255) not null, "
                "`name_i` varchar(255) not null, "
                "`name_i_cn` varchar(255) not null, "
                "`name_r` varchar(255) not null, "
                "`name_r1` varchar(255) not null, "
                "`name_f` varchar(255) not null, "
                "`name_f_cn` varchar(255) not null, "
                "`coordinates_s` geometry not null, "
                "`coordinates_s_cn` geometry not null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` add unique index `name_u_unique`(`name_u`)");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` add index `firewalls_name_i_index`(`name_i`)");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table `firewalls` add index `name_i_cn_index`(`name_i_cn`)");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "alter table `firewalls` add index `name_r_raw`(`name_r`, name_r1)");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             "alter table `firewalls` "
             "add fulltext index `firewalls_name_f_fulltext`(`name_f`)");
    QVERIFY(log5.boundValues.isEmpty());

    const auto &log6 = log.at(6);
    QCOMPARE(log6.query,
             "alter table `firewalls` "
             "add fulltext index `name_f_cn_fulltext`(`name_f_cn`)");
    QVERIFY(log6.boundValues.isEmpty());

    const auto &log7 = log.at(7);
    QCOMPARE(log7.query,
             "alter table `firewalls` "
             "add spatial index `firewalls_coordinates_s_spatialindex`(`coordinates_s`)");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log8 = log.at(8);
    QCOMPARE(log8.query,
             "alter table `firewalls` "
             "add spatial index `coordinates_s_cn_spatial`(`coordinates_s_cn`)");
    QVERIFY(log8.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_PrimaryKey() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.primary("id", "key_name");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` add primary key (`id`)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::add_PrimaryKey_WithAlgorithm() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.primary("id", "key_name", HASH_);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table `firewalls` add primary key using hash(`id`)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::renameIndex() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id();

            table.string(NAME).unique();
        });

        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.renameIndex("firewalls_name_unique", "firewalls_name_unique_renamed");
        });
    });

    QCOMPARE(log.size(), 3);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             sl("create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`name` varchar(255) not null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` add unique index `firewalls_name_unique`(`name`)");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` "
             "rename index `firewalls_name_unique` to `firewalls_name_unique_renamed`");
    QVERIFY(log2.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::dropIndex_ByIndexName() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.unsignedInteger(ID);
            table.primary(ID);

            table.string("name_u").unique();
            table.string("name_i").index();
            table.string("name_f").fulltext();
            table.geometry("coordinates_s").spatialIndex();
        });

        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.dropPrimary();
            table.dropUnique("firewalls_name_u_unique");
            table.dropIndex("firewalls_name_i_index");
            table.dropFullText("firewalls_name_f_fulltext");
            table.dropSpatialIndex("firewalls_coordinates_s_spatialindex");
        });
    });

    QCOMPARE(log.size(), 11);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             sl("create table `firewalls` ("
                "`id` int unsigned not null, "
                "`name_u` varchar(255) not null, "
                "`name_i` varchar(255) not null, "
                "`name_f` varchar(255) not null, "
                "`coordinates_s` geometry not null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` add primary key (`id`)");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` "
             "add unique index `firewalls_name_u_unique`(`name_u`)");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table `firewalls` add index `firewalls_name_i_index`(`name_i`)");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "alter table `firewalls` "
             "add fulltext index `firewalls_name_f_fulltext`(`name_f`)");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             "alter table `firewalls` "
             "add spatial index "
               "`firewalls_coordinates_s_spatialindex`(`coordinates_s`)");
    QVERIFY(log5.boundValues.isEmpty());

    const auto &log6 = log.at(6);
    QCOMPARE(log6.query,
             "alter table `firewalls` drop primary key");
    QVERIFY(log6.boundValues.isEmpty());

    const auto &log7 = log.at(7);
    QCOMPARE(log7.query,
             "alter table `firewalls` drop index `firewalls_name_u_unique`");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log8 = log.at(8);
    QCOMPARE(log8.query,
             "alter table `firewalls` drop index `firewalls_name_i_index`");
    QVERIFY(log8.boundValues.isEmpty());

    const auto &log9 = log.at(9);
    QCOMPARE(log9.query,
             "alter table `firewalls` drop index `firewalls_name_f_fulltext`");
    QVERIFY(log9.boundValues.isEmpty());

    const auto &log10 = log.at(10);
    QCOMPARE(log10.query,
             "alter table `firewalls` "
             "drop index `firewalls_coordinates_s_spatialindex`");
    QVERIFY(log10.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::dropIndex_ByColumn() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.unsignedInteger(ID);
            table.primary(ID);

            table.string("name_u").unique();
            table.string("name_i").index();
            table.string("name_f").fulltext();
            table.geometry("coordinates_s").spatialIndex();
        });

        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.dropPrimary();
            table.dropUnique({"name_u"});
            table.dropIndex({"name_i"});
            table.dropFullText({"name_f"});
            table.dropSpatialIndex({"coordinates_s"});
        });
    });

    QCOMPARE(log.size(), 11);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             sl("create table `firewalls` ("
                "`id` int unsigned not null, "
                "`name_u` varchar(255) not null, "
                "`name_i` varchar(255) not null, "
                "`name_f` varchar(255) not null, "
                "`coordinates_s` geometry not null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` add primary key (`id`)");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` "
             "add unique index `firewalls_name_u_unique`(`name_u`)");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table `firewalls` add index `firewalls_name_i_index`(`name_i`)");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "alter table `firewalls` "
             "add fulltext index `firewalls_name_f_fulltext`(`name_f`)");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             "alter table `firewalls` "
             "add spatial index "
               "`firewalls_coordinates_s_spatialindex`(`coordinates_s`)");
    QVERIFY(log5.boundValues.isEmpty());

    const auto &log6 = log.at(6);
    QCOMPARE(log6.query,
             "alter table `firewalls` drop primary key");
    QVERIFY(log6.boundValues.isEmpty());

    const auto &log7 = log.at(7);
    QCOMPARE(log7.query,
             "alter table `firewalls` drop index `firewalls_name_u_unique`");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log8 = log.at(8);
    QCOMPARE(log8.query,
             "alter table `firewalls` drop index `firewalls_name_i_index`");
    QVERIFY(log8.boundValues.isEmpty());

    const auto &log9 = log.at(9);
    QCOMPARE(log9.query,
             "alter table `firewalls` drop index `firewalls_name_f_fulltext`");
    QVERIFY(log9.boundValues.isEmpty());

    const auto &log10 = log.at(10);
    QCOMPARE(log10.query,
             "alter table `firewalls` "
             "drop index `firewalls_coordinates_s_spatialindex`");
    QVERIFY(log10.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::dropIndex_ByMultipleColumns() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.unsignedInteger(ID);
            table.unsignedInteger("id1");
            table.primary({ID, "id1"});

            table.string("name_u");
            table.string("name_u1");
            table.unique({"name_u", "name_u1"});

            table.string("name_i");
            table.string("name_i1");
            table.index({"name_i", "name_i1"});

            table.string("name_f");
            table.string("name_f1");
            table.fullText({"name_f", "name_f1"});
        });

        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.dropPrimary({ID, "id1"});
            table.dropUnique({"name_u", "name_u1"});
            table.dropIndex({"name_i", "name_i1"});
            table.dropFullText({"name_f", "name_f1"});
        });
    });

    QCOMPARE(log.size(), 9);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             sl("create table `firewalls` ("
                "`id` int unsigned not null, "
                "`id1` int unsigned not null, "
                "`name_u` varchar(255) not null, "
                "`name_u1` varchar(255) not null, "
                "`name_i` varchar(255) not null, "
                "`name_i1` varchar(255) not null, "
                "`name_f` varchar(255) not null, "
                "`name_f1` varchar(255) not null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` add primary key (`id`, `id1`)");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` "
             "add unique index `firewalls_name_u_name_u1_unique`(`name_u`, `name_u1`)");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table `firewalls` "
             "add index `firewalls_name_i_name_i1_index`(`name_i`, `name_i1`)");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "alter table `firewalls` "
             "add fulltext index "
               "`firewalls_name_f_name_f1_fulltext`(`name_f`, `name_f1`)");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             "alter table `firewalls` drop primary key");
    QVERIFY(log5.boundValues.isEmpty());

    const auto &log6 = log.at(6);
    QCOMPARE(log6.query,
             "alter table `firewalls` drop index `firewalls_name_u_name_u1_unique`");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log7 = log.at(7);
    QCOMPARE(log7.query,
             "alter table `firewalls` drop index `firewalls_name_i_name_i1_index`");
    QVERIFY(log7.boundValues.isEmpty());

    const auto &log8 = log.at(8);
    QCOMPARE(log8.query,
             "alter table `firewalls` drop index `firewalls_name_f_name_f1_fulltext`");
    QVERIFY(log8.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::foreignKey() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id();

            table.unsignedBigInteger("user_id");
            table.unsignedBigInteger("torrent_id");
            table.unsignedBigInteger("role_id").nullable();

            table.foreign("user_id").references(ID).on("users")
                    .onDelete(Cascade).onUpdate(Restrict);
            table.foreign("torrent_id").references(ID).on("torrents")
                    .restrictOnDelete().restrictOnUpdate();
            table.foreign("role_id").references(ID).on("roles")
                    .nullOnDelete().cascadeOnUpdate();
        });
    });

    QCOMPARE(log.size(), 4);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             sl("create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`user_id` bigint unsigned not null, "
                "`torrent_id` bigint unsigned not null, "
                "`role_id` bigint unsigned null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` "
             "add constraint `firewalls_user_id_foreign` "
             "foreign key (`user_id`) "
             "references `users` (`id`) "
             "on delete cascade on update restrict");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` "
             "add constraint `firewalls_torrent_id_foreign` "
             "foreign key (`torrent_id`) "
             "references `torrents` (`id`) "
             "on delete restrict on update restrict");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table `firewalls` "
             "add constraint `firewalls_role_id_foreign` "
             "foreign key (`role_id`) "
             "references `roles` (`id`) "
             "on delete set null on update cascade");
    QVERIFY(log3.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::foreignKey_TerserSyntax() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id();

            table.foreignId("user_id").constrained()
                    .onDelete(Cascade).onUpdate(Restrict);
            table.foreignId("torrent_id").constrained()
                    .restrictOnDelete().restrictOnUpdate();
            table.foreignId("role_id").nullable().constrained()
                    .nullOnDelete().cascadeOnUpdate();
        });
    });

    QCOMPARE(log.size(), 4);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             sl("create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`user_id` bigint unsigned not null, "
                "`torrent_id` bigint unsigned not null, "
                "`role_id` bigint unsigned null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` "
             "add constraint `firewalls_user_id_foreign` "
             "foreign key (`user_id`) "
             "references `users` (`id`) "
             "on delete cascade on update restrict");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` "
             "add constraint `firewalls_torrent_id_foreign` "
             "foreign key (`torrent_id`) "
             "references `torrents` (`id`) "
             "on delete restrict on update restrict");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table `firewalls` "
             "add constraint `firewalls_role_id_foreign` "
             "foreign key (`role_id`) "
             "references `roles` (`id`) "
             "on delete set null on update cascade");
    QVERIFY(log3.boundValues.isEmpty());
}

#ifndef TINYORM_DISABLE_ORM
void tst_MySql_SchemaBuilder::foreignKey_WithModel() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Models::Torrent torrent;
        Models::User user;

        Schema::on(connection.getName())
                .create(Firewalls, [&torrent, &user](Blueprint &table)
        {
            table.id();

            table.foreignIdFor(torrent).constrained()
                    .onDelete(Cascade).onUpdate(Restrict);
            table.foreignIdFor(user).nullable().constrained()
                    .nullOnDelete().cascadeOnUpdate();
        });
    });

    QCOMPARE(log.size(), 3);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             sl("create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`torrent_id` bigint unsigned not null, "
                "`user_id` bigint unsigned null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` "
             "add constraint `firewalls_torrent_id_foreign` "
             "foreign key (`torrent_id`) "
             "references `torrents` (`id`) "
             "on delete cascade on update restrict");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` "
             "add constraint `firewalls_user_id_foreign` "
             "foreign key (`user_id`) "
             "references `users` (`id`) "
             "on delete set null on update cascade");
    QVERIFY(log2.boundValues.isEmpty());
}
#endif

void tst_MySql_SchemaBuilder::dropForeign() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id();

            table.foreignId("user_id").constrained()
                    .onDelete(Cascade).onUpdate(Restrict);
            table.foreignId("torrent_id").constrained()
                    .restrictOnDelete().restrictOnUpdate();
            table.foreignId("role_id").nullable().constrained()
                    .nullOnDelete().cascadeOnUpdate();

            // By column name
            table.dropForeign({"user_id"});
            // By index name
            table.dropForeign("firewalls_torrent_id_foreign");
            // Drop index and also a column
            table.dropConstrainedForeignId("role_id");
        });
    });

    QCOMPARE(log.size(), 8);

    // I leave these comparisons here, even if they are doubled from the previous test
    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             sl("create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`user_id` bigint unsigned not null, "
                "`torrent_id` bigint unsigned not null, "
                "`role_id` bigint unsigned null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` "
             "add constraint `firewalls_user_id_foreign` "
             "foreign key (`user_id`) "
             "references `users` (`id`) "
             "on delete cascade on update restrict");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` "
             "add constraint `firewalls_torrent_id_foreign` "
             "foreign key (`torrent_id`) "
             "references `torrents` (`id`) "
             "on delete restrict on update restrict");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table `firewalls` "
             "add constraint `firewalls_role_id_foreign` "
             "foreign key (`role_id`) "
             "references `roles` (`id`) "
             "on delete set null on update cascade");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "alter table `firewalls` drop foreign key `firewalls_user_id_foreign`");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             "alter table `firewalls` drop foreign key `firewalls_torrent_id_foreign`");
    QVERIFY(log5.boundValues.isEmpty());

    const auto &log6 = log.at(6);
    QCOMPARE(log6.query,
             "alter table `firewalls` drop foreign key `firewalls_role_id_foreign`");
    QVERIFY(log6.boundValues.isEmpty());

    const auto &log7 = log.at(7);
    QCOMPARE(log7.query,
             "alter table `firewalls` drop `role_id`");
    QVERIFY(log7.boundValues.isEmpty());
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

QString tst_MySql_SchemaBuilder::getMySqlConnectionName(QStringList &connections)
{
    if (connections.isEmpty())
        return {};

    for (auto &&connection : connections)
        if (connection == Databases::MYSQL)
            return std::move(connection);

    return {};
}

QString tst_MySql_SchemaBuilder::getMariaConnectionName(QStringList &connections)
{
    if (connections.isEmpty())
        return {};

    for (auto &&connection : connections)
        if (connection == Databases::MARIADB)
            return std::move(connection);

    return {};
}

void tst_MySql_SchemaBuilder::initializeCharsetAndCollation()
{
    // The charset and collation in all queries is set on the base of env. variables

    // MySQL charset and collation
    {
        const auto &connection = DB::connection(m_connection);
        m_charset = connection.getConfig(charset_).value<QString>();
        m_collation = connection.getConfig(collation_).value<QString>();
    }

    // Nothing to do, env. variables for MariaDB were not defined
    if (m_connectionMaria.isEmpty())
        return;

    // MariaDB charset and collation
    {
        const auto &connection = DB::connection(m_connectionMaria);
        m_charsetMaria = connection.getConfig(charset_).value<QString>();
        m_collationMaria = connection.getConfig(collation_).value<QString>();
    }
}

QTEST_MAIN(tst_MySql_SchemaBuilder)

#include "tst_mysql_schemabuilder.moc"
