#include <QCoreApplication>
#include <QTest>

#include "orm/db.hpp"
#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/schema.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"
#include "macros.hpp"

#ifndef TINYORM_DISABLE_ORM
#  include "models/user.hpp"
#endif

using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::PUBLIC;
using Orm::Constants::QPSQL;
using Orm::Constants::SIZE_;
using Orm::Constants::UTF8;
using Orm::Constants::UcsBasic;
using Orm::Constants::charset_;
using Orm::Constants::driver_;
using Orm::Constants::dummy_NONEXISTENT;
using Orm::Constants::search_path;
using Orm::Constants::username_;

using Orm::DB;
using Orm::Exceptions::InvalidArgumentError;
using Orm::Exceptions::LogicError;
using Orm::Schema;
using Orm::SchemaNs::Blueprint;
using Orm::SchemaNs::Constants::Cascade;
using Orm::SchemaNs::Constants::Restrict;

using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

class tst_PostgreSQL_SchemaBuilder : public QObject // clazy:exclude=ctor-missing-parent-argument
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
    void hasTable_DatabaseDiffers_ThrowException() const;
    void hasTable_SchemaDiffers() const;
    void hasTable_CustomSearchPath_QString_InConfiguration() const;
    void hasTable_CustomSearchPath_QStringList_InConfiguration() const;
    void hasTable_CustomSearchPath_QSet_InConfiguration_ThrowException() const;
    void hasTable_CustomSearchPath_WithUserVariable_InConfiguration() const;
    void hasTable_NoSearchPath_InConfiguration() const;

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

    /* Generated columns */
    void generatedAs_PrimaryKey() const;
    void generatedAs_PrimaryKey_Always() const;
    void generatedAs_PrimaryKey_SequenceOptions() const;
    void generatedAs_NotPrimaryKey() const;

    void virtualAs() const;
    void storedAs() const;

    void virtualAs_StoredAs_CreateTable() const;
    void virtualAs_StoredAs_Nullable_CreateTable() const;
    void virtualAs_StoredAs_ModifyTable() const;
    void virtualAs_StoredAs_Nullable_ModifyTable() const;

    void change_VirtualAs_ThrowException() const;
    void change_StoredAs_ThrowException() const;
    void drop_StoredAs() const;

    /* Indexes */
    void indexes_Fluent() const;
    void indexes_Blueprint() const;

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
    inline static const auto *ClassName = "tst_PostgreSQL_SchemaBuilder";

    /*! Connection name used in this test case. */
    QString m_connection;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_PostgreSQL_SchemaBuilder::initTestCase()
{
    m_connection = Databases::createConnection(Databases::POSTGRESQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );
}

void tst_PostgreSQL_SchemaBuilder::createDatabase() const
{
    auto &connection = DB::connection(m_connection);

    auto log = connection.pretend([](auto &connection_)
    {
        Schema::on(connection_.getName()).createDatabase(Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, sl(R"(create database "firewalls" encoding "%1")")
             .arg(connection.getConfig(charset_).value<QString>()));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::createDatabase_Charset_Collation() const
{
    // Add a new database connection with different charset
    const auto connectionName =
            Databases::createConnectionTempFrom(
                Databases::POSTGRESQL, {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {driver_,    QPSQL},
        {charset_,   "WIN1250"},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    auto log = DB::connection(*connectionName).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).createDatabase(Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             R"(create database "firewalls" encoding "WIN1250")");
    QVERIFY(firstLog.boundValues.isEmpty());

    // Restore
    Databases::removeConnection(*connectionName);
}

void tst_PostgreSQL_SchemaBuilder::dropDatabaseIfExists() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).dropDatabaseIfExists(Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             R"(drop database if exists "firewalls")");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::createTable() const
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

            // PostgreSQL doesn't have unsigned integers, so they should be same as above
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
             "create table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"char\" char(255) not null, "
             "\"char_10\" char(10) not null, "
             "\"string\" varchar(255) not null, "
             "\"string_22\" varchar(22) not null, "
             "\"tiny_text\" varchar(255) not null, "
             "\"text\" text not null, "
             "\"medium_text\" text not null, "
             "\"long_text\" text not null, "
             "\"integer\" integer not null, "
             "\"tinyInteger\" smallint not null, "
             "\"smallInteger\" smallint not null, "
             "\"mediumInteger\" integer not null, "
             "\"bigInteger\" bigint not null, "
             "\"unsignedInteger\" integer not null, "
             "\"unsignedTinyInteger\" smallint not null, "
             "\"unsignedSmallInteger\" smallint not null, "
             "\"unsignedMediumInteger\" integer not null, "
             "\"unsignedBigInteger\" bigint not null, "
             "\"uuid\" uuid not null, "
             "\"ip_address\" inet not null, "
             "\"mac_address\" macaddr not null, "
             "\"tiny_binary\" bytea not null, "
             "\"binary\" bytea not null, "
             "\"medium_binary\" bytea not null, "
             "\"long_binary\" bytea not null)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::createTable_Temporary() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
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
             "create temporary table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"name\" varchar(255) not null)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::createTable_Charset_Collation_Engine() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            // charset ignored with the PosrgreSQL grammar
            table.charset = "WIN1250";

            table.id();
            table.string(NAME);
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "create table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"name\" varchar(255) not null)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::createTable_WithComment() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.comment("Example 'table' comment");

            table.id();
            table.string(NAME);
        });
    });

    QCOMPARE(log.size(), 2);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"name\" varchar(255) not null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "comment on table \"firewalls\" is 'Example ''table'' comment'");
    QVERIFY(log1.boundValues.isEmpty());
}

void
tst_PostgreSQL_SchemaBuilder::timestamps_rememberToken_softDeletes_CreateAndDrop() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id();

            table.timestamps();
            table.rememberToken();
            table.softDeletes();
        });

        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.dropTimestamps();
            table.dropRememberToken();
            table.dropSoftDeletes();
        });

        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id();

            table.timestamps(3);
        });
    });

    QCOMPARE(log.size(), 5);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"created_at\" timestamp(0) without time zone null, "
             "\"updated_at\" timestamp(0) without time zone null, "
             "\"remember_token\" varchar(100) null, "
             "\"deleted_at\" timestamp(0) without time zone null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table \"firewalls\" "
             "drop column \"created_at\", drop column \"updated_at\"");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             R"(alter table "firewalls" drop column "remember_token")");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             R"(alter table "firewalls" drop column "deleted_at")");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "create table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"created_at\" timestamp(3) without time zone null, "
             "\"updated_at\" timestamp(3) without time zone null)");
    QVERIFY(log4.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::datetimes_softDeletesDatetime_CreateAndDrop() const
{
    /* On PostgreSQL there is not any DATETIME column type so the result should be
       the same like the previous unit test. */
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id();

            table.datetimes();
            table.softDeletesDatetime();
        });

        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.dropDatetimes();
            table.dropSoftDeletesDatetime();
        });

        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id();

            table.datetimes(3);
        });
    });

    QCOMPARE(log.size(), 4);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"created_at\" timestamp(0) without time zone null, "
             "\"updated_at\" timestamp(0) without time zone null, "
             "\"deleted_at\" timestamp(0) without time zone null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table \"firewalls\" "
             "drop column \"created_at\", drop column \"updated_at\"");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             R"(alter table "firewalls" drop column "deleted_at")");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "create table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"created_at\" timestamp(3) without time zone null, "
             "\"updated_at\" timestamp(3) without time zone null)");
    QVERIFY(log3.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::modifyTable() const
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
             "alter table \"firewalls\" "
             "add column \"char\" char(255) not null, "
             "add column \"char_10\" char(10) not null, "
             "add column \"string\" varchar(255) not null, "
             "add column \"string_22\" varchar(22) not null, "
             "add column \"tiny_text\" varchar(255) not null, "
             "add column \"text\" text not null, "
             "add column \"medium_text\" text not null, "
             "add column \"long_text\" text not null, "
             "add column \"integer\" integer null, "
             "add column \"tinyInteger\" smallint not null, "
             "add column \"smallInteger\" smallint not null, "
             "add column \"mediumInteger\" integer not null, "
             "add column \"tiny_binary\" bytea not null, "
             "add column \"binary\" bytea not null, "
             "add column \"medium_binary\" bytea not null, "
             "add column \"long_binary\" bytea not null");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             R"(alter table "firewalls" drop column "long_text")");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table \"firewalls\" "
             "drop column \"medium_text\", drop column \"text\"");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table \"firewalls\" "
             "drop column \"smallInteger\", drop column \"mediumInteger\"");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "alter table \"firewalls\" "
             "rename column \"integer\" to \"integer_renamed\"");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             "alter table \"firewalls\" "
             "rename column \"string_22\" to \"string_22_renamed\"");
    QVERIFY(log5.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::modifyTable_WithComment() const
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
             "comment on table \"firewalls\" is 'Example ''table'' comment'");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::dropTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).drop(Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, R"(drop table "firewalls")");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::dropTableIfExists() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).dropIfExists(Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, R"(drop table if exists "firewalls")");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::rename() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).rename("secured", Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, R"(alter table "secured" rename to "firewalls")");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::dropColumns() const
{
    {
        auto log = DB::connection(m_connection).pretend([](auto &connection)
        {
            Schema::on(connection.getName()).dropColumn(Firewalls, NAME);
        });

        QVERIFY(!log.isEmpty());
        const auto &firstLog = log.first();

        QCOMPARE(log.size(), 1);
        QCOMPARE(firstLog.query,
                 R"(alter table "firewalls" drop column "name")");
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
        QCOMPARE(firstLog.query,
                 R"(alter table "firewalls" drop column "name", drop column "size")");
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
        QCOMPARE(firstLog.query,
                 R"(alter table "firewalls" drop column "name", drop column "size")");
        QVERIFY(firstLog.boundValues.isEmpty());
    }
}

void tst_PostgreSQL_SchemaBuilder::renameColumn() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).renameColumn(Firewalls, NAME, "first_name");
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             R"(alter table "firewalls" rename column "name" to "first_name")");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::dropAllTypes() const
{
    TVERIFY_THROWS_EXCEPTION(LogicError, Schema::on(m_connection).dropAllTypes());
}

void tst_PostgreSQL_SchemaBuilder::getAllTables() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection_)
    {
        Schema::on(connection_.getName()).getAllTables();
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             sl("select tablename, "
                  "concat('\"', schemaname, '\".\"', tablename, '\"') as qualifiedname "
                "from pg_catalog.pg_tables "
                "where schemaname in ('%1')")
             .arg(DB::originalConfigValue(search_path, m_connection).value<QString>()));

    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::getAllViews() const
{
    auto &connection = DB::connection(m_connection);

    auto log = connection.pretend([](auto &connection_)
    {
        Schema::on(connection_.getName()).getAllViews();
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             sl("select viewname, "
                  "concat('\"', schemaname, '\".\"', viewname, '\"') as qualifiedname "
                "from pg_catalog.pg_views "
                "where schemaname in ('%1')")
             .arg(connection.getConfig(search_path).value<QString>()));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::enableForeignKeyConstraints() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).enableForeignKeyConstraints();
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, "set constraints all immediate");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::disableForeignKeyConstraints() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).disableForeignKeyConstraints();
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, "set constraints all deferred");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::getColumnListing() const
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
             "select column_name "
             "from information_schema.columns "
             "where table_catalog = ? and table_schema = ? and table_name = ?");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({connection.getDatabaseName(),
                              connection.getConfig(search_path),
                              QVariant(Firewalls)}));
}

void tst_PostgreSQL_SchemaBuilder::hasTable() const
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
             "from information_schema.tables "
             "where table_catalog = ? and table_schema = ? and table_name = ? and "
               "table_type = 'BASE TABLE'");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({connection.getDatabaseName(),
                              connection.getConfig(search_path),
                              QVariant(Firewalls)}));
}

void tst_PostgreSQL_SchemaBuilder::hasTable_DatabaseDiffers_ThrowException() const
{
    // Verify
    DB::connection(m_connection).pretend([](auto &connection)
    {
        TVERIFY_THROWS_EXCEPTION(
                    InvalidArgumentError,
                    Schema::on(connection.getName())
                    .hasTable(sl("%1-database.public.users").arg(dummy_NONEXISTENT)));
    });
}

void tst_PostgreSQL_SchemaBuilder::hasTable_SchemaDiffers() const
{
    // Prepare test variables
    auto &connection = DB::connection(m_connection);
    const auto &databaseName = connection.getDatabaseName();
    const auto schemaName = sl("schema_example");
    const auto tableName = sl("users");

    // Verify
    auto log = connection.pretend([&databaseName, &schemaName, &tableName]
                                  (auto &connection_)
    {
        const auto hasTable = Schema::on(connection_.getName())
                              .hasTable(sl("%1.%2.%3")
                                        .arg(databaseName, schemaName, tableName));

        QVERIFY(!hasTable);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select * "
             "from information_schema.tables "
             "where table_catalog = ? and table_schema = ? and "
               "table_name = ? and table_type = 'BASE TABLE'");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(databaseName),
                              QVariant(schemaName),
                              QVariant(tableName)}));
}

void tst_PostgreSQL_SchemaBuilder::
     hasTable_CustomSearchPath_QString_InConfiguration() const
{
    // Add a new database connection
    const auto connectionName =
            Databases::createConnectionTempFrom(
                Databases::POSTGRESQL, {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                {{search_path, sl("schema_example, another_example")}});

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Prepare test variables
    auto &connection = DB::connection(*connectionName);
    const auto tableName = sl("users");

    // Verify
    auto log = connection.pretend([&tableName](auto &connection_)
    {
        const auto hasTable = Schema::on(connection_.getName())
                              .hasTable(tableName);

        QVERIFY(!hasTable);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select * "
             "from information_schema.tables "
             "where table_catalog = ? and table_schema = ? and "
               "table_name = ? and table_type = 'BASE TABLE'");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(connection.getDatabaseName()),
                              QVariant(sl("schema_example")),
                              QVariant(tableName)}));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_SchemaBuilder::
     hasTable_CustomSearchPath_QStringList_InConfiguration() const
{
    // Add a new database connection
    const auto connectionName =
            Databases::createConnectionTempFrom(
                Databases::POSTGRESQL, {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                {{search_path, QStringList {"schema_example", "another_example"}}});

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Prepare test variables
    auto &connection = DB::connection(*connectionName);
    const auto tableName = sl("users");

    // Verify
    auto log = connection.pretend([&tableName](auto &connection_)
    {
        const auto hasTable = Schema::on(connection_.getName())
                              .hasTable(tableName);

        QVERIFY(!hasTable);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select * "
             "from information_schema.tables "
             "where table_catalog = ? and table_schema = ? and "
               "table_name = ? and table_type = 'BASE TABLE'");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(connection.getDatabaseName()),
                              QVariant(sl("schema_example")),
                              QVariant(tableName)}));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_SchemaBuilder::
     hasTable_CustomSearchPath_QSet_InConfiguration_ThrowException() const
{
    // Add a new database connection
    const auto connectionName =
            Databases::createConnectionTempFrom(
                Databases::POSTGRESQL, {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                {{search_path, QVariant::fromValue(
                                   QSet<QString> {sl("schema_example"),
                                                  sl("another_example")})}});

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Create database connection
    TVERIFY_THROWS_EXCEPTION(InvalidArgumentError,
                             DB::connection(*connectionName));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_SchemaBuilder::
     hasTable_CustomSearchPath_WithUserVariable_InConfiguration() const
{
    // Add a new database connection
    const auto connectionName =
            Databases::createConnectionTempFrom(
                Databases::POSTGRESQL, {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                {{search_path, sl("\"$user\", public")}});

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Prepare test variables
    auto &connection = DB::connection(*connectionName);
    const auto tableName = sl("users");

    // Verify
    auto log = connection.pretend([&tableName](auto &connection_)
    {
        const auto hasTable = Schema::on(connection_.getName())
                              .hasTable(tableName);

        QVERIFY(!hasTable);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select * "
             "from information_schema.tables "
             "where table_catalog = ? and table_schema = ? and "
               "table_name = ? and table_type = 'BASE TABLE'");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(connection.getDatabaseName()),
                              connection.getConfig(username_),
                              QVariant(tableName)}));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_SchemaBuilder::hasTable_NoSearchPath_InConfiguration() const
{
    // Add a new database connection
    const auto connectionName =
            Databases::createConnectionTempFrom(
                Databases::POSTGRESQL, {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                {}, {search_path});

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Prepare test variables
    auto &connection = DB::connection(*connectionName);
    const auto tableName = sl("users");

    // Verify
    auto log = connection.pretend([&tableName](auto &connection_)
    {
        const auto hasTable = Schema::on(connection_.getName())
                              .hasTable(tableName);

        QVERIFY(!hasTable);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select * "
             "from information_schema.tables "
             "where table_catalog = ? and table_schema = ? and "
               "table_name = ? and table_type = 'BASE TABLE'");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(connection.getDatabaseName()),
                              // Should use hardcoded PUBLIC default in pretending
                              QVariant(PUBLIC),
                              QVariant(tableName)}));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_SchemaBuilder::defaultStringLength_Set() const
{
    QVERIFY(Blueprint::DefaultStringLength == Orm::SchemaNs::DefaultStringLength);

    Schema::defaultStringLength(191);
    QCOMPARE(Blueprint::DefaultStringLength, 191);

    // Restore
    Schema::defaultStringLength(Orm::SchemaNs::DefaultStringLength);
    QVERIFY(Blueprint::DefaultStringLength == Orm::SchemaNs::DefaultStringLength);
}

void tst_PostgreSQL_SchemaBuilder::modifiers() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.bigInteger(ID).autoIncrement().startingValue(5);
            // PostgreSQL doesn't support signed modifier or numbers
            table.bigInteger("big_int").isUnsigned();
            table.bigInteger("big_int1");
            table.string(NAME).defaultValue("guest");
            table.string("name1").nullable();
            table.string("name2").comment("name2 note");
            table.string("name3", 191);
            // PostgreSQL doesn't support invisible columns
//            table.string("name4").invisible().change();
            // PostgreSQL doesn't support charset on the column
            table.string("name5").charset(UTF8);
            table.string("name6").collation(UcsBasic);
            // PostgreSQL doesn't support charset on the column
            table.string("name7").charset(UTF8).collation(UcsBasic);
            // PostgreSQL doesn't support renaming columns during the change() call
//            table.string("name8_old", 64).renameTo("name8").change();
            table.Double("amount", 6, 2);
            table.multiPolygon("positions").srid(1234).storedAs("expression");
            table.point("positions1").isGeometry().srid(1234);
            table.timestamp("added_on").nullable(false).useCurrent();
            // PostgreSQL doesn't support useCurrentOnUpdate()
//            table.timestamp("updated_at", 4).useCurrent().useCurrentOnUpdate();
        });
        /* Tests from and also integerIncrements, this would of course fail on real DB
           as you can not have two primary keys. */
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.integerIncrements(ID).from(15);
        });
    });

    QCOMPARE(log.size(), 5);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"big_int\" bigint not null, "
             "\"big_int1\" bigint not null, "
             "\"name\" varchar(255) not null default 'guest', "
             "\"name1\" varchar(255) null, "
             "\"name2\" varchar(255) not null, "
             "\"name3\" varchar(191) not null, "
             "\"name5\" varchar(255) not null, "
             "\"name6\" varchar(255) collate \"ucs_basic\" not null, "
             "\"name7\" varchar(255) collate \"ucs_basic\" not null, "
             "\"amount\" double precision not null, "
             "\"positions\" geography(multipolygon, 1234) not null "
               "generated always as (expression) stored, "
             "\"positions1\" geometry(point, 1234) not null, "
             "\"added_on\" timestamp(0) without time zone not null "
               "default current_timestamp)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             R"(alter sequence "firewalls_id_seq" restart with 5)");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             R"(comment on column "firewalls"."name2" is 'name2 note')");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             R"(alter table "firewalls" add column "id" serial primary key not null)");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             R"(alter sequence "firewalls_id_seq" restart with 15)");
    QVERIFY(log4.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::modifier_defaultValue_WithExpression() const
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
             "create table \"firewalls\" ("
             "\"name\" varchar(255) not null default 'guest', "
             "\"name_raw\" varchar(255) not null default 'guest_raw')");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::modifier_defaultValue_WithBoolean() const
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
             "create table \"firewalls\" ("
             "\"boolean\" boolean not null, "
             "\"boolean_false\" boolean not null default '0', "
             "\"boolean_true\" boolean not null default '1', "
             "\"boolean_0\" boolean not null default '0', "
             "\"boolean_1\" boolean not null default '1')");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::modifier_defaultValue_Escaping() const
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
             // String contains \t after the tab word
             "create table \"firewalls\" ("
             "\"string\" varchar(255) not null "
             "default 'Text '' and \" or \\ newline\n"
             "and tab	end')");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::change_modifiers() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.bigInteger(ID).autoIncrement().startingValue(5).change();
            // PostgreSQL doesn't support signed modifier or signed numbers
            table.bigInteger("big_int").isUnsigned().change();
            table.bigInteger("big_int1").change();
            table.string(NAME).defaultValue("guest").change();
            table.string("name1").nullable().change();
            table.string("name2").comment("name2 note").change();
            table.string("name3", 191).change();
            // PostgreSQL doesn't support invisible columns
//            table.string("name4").invisible().change();
            // PostgreSQL doesn't support charset on the column
            table.string("name5").charset(UTF8).change();
            table.string("name6").collation(UcsBasic).change();
            // PostgreSQL doesn't support charset on the column
            table.string("name7").charset(UTF8).collation(UcsBasic).change();
            // PostgreSQL doesn't support renaming columns during the change() call
//            table.string("name8_old", 64).renameTo("name8").change();
            table.Double("amount", 6, 2).change();
            // PostgreSQL doesn't support changing generated columns
//            table.multiPolygon("positions").srid(1234).storedAs("expression").change();
            table.point("positions1").isGeometry().projection(1234).change();
            table.timestamp("added_on").nullable(false).useCurrent().change();
            // PostgreSQL doesn't support useCurrentOnUpdate()
//            table.timestamp("updated_at", 4).useCurrent().useCurrentOnUpdate().change();
        });
        /* Tests from and also integerIncrements, this would of course fail on real DB
           as you can not have two primary keys. */
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.integerIncrements(ID).from(15).change();
        });
    });

    // The following is really wild 🤯

    QCOMPARE(log.size(), 18);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "alter table \"firewalls\" "

             "alter column \"id\" type bigserial, "
             "alter column \"id\" set not null, "
             "alter column \"id\" drop default, "
             "alter column \"id\" drop identity if exists, "

             "alter column \"big_int\" type bigint, "
             "alter column \"big_int\" set not null, "
             "alter column \"big_int\" drop default, "
             "alter column \"big_int\" drop identity if exists, "

             "alter column \"big_int1\" type bigint, "
             "alter column \"big_int1\" set not null, "
             "alter column \"big_int1\" drop default, "
             "alter column \"big_int1\" drop identity if exists, "

             "alter column \"name\" type varchar(255), "
             "alter column \"name\" set not null, "
             "alter column \"name\" set default 'guest', "
             "alter column \"name\" drop identity if exists, "

             "alter column \"name1\" type varchar(255), "
             "alter column \"name1\" drop not null, "
             "alter column \"name1\" drop default, "
             "alter column \"name1\" drop identity if exists, "

             "alter column \"name2\" type varchar(255), "
             "alter column \"name2\" set not null, "
             "alter column \"name2\" drop default, "
             "alter column \"name2\" drop identity if exists, "

             "alter column \"name3\" type varchar(191), "
             "alter column \"name3\" set not null, "
             "alter column \"name3\" drop default, "
             "alter column \"name3\" drop identity if exists, "

             "alter column \"name5\" type varchar(255), "
             "alter column \"name5\" set not null, "
             "alter column \"name5\" drop default, "
             "alter column \"name5\" drop identity if exists, "

             "alter column \"name6\" type varchar(255) collate \"ucs_basic\", "
             "alter column \"name6\" set not null, "
             "alter column \"name6\" drop default, "
             "alter column \"name6\" drop identity if exists, "

             "alter column \"name7\" type varchar(255) collate \"ucs_basic\", "
             "alter column \"name7\" set not null, "
             "alter column \"name7\" drop default, "
             "alter column \"name7\" drop identity if exists, "

             "alter column \"amount\" type double precision, "
             "alter column \"amount\" set not null, "
             "alter column \"amount\" drop default, "
             "alter column \"amount\" drop identity if exists, "

             "alter column \"positions1\" type geometry(point, 1234), "
             "alter column \"positions1\" set not null, "
             "alter column \"positions1\" drop default, "
             "alter column \"positions1\" drop identity if exists, "

             "alter column \"added_on\" type timestamp(0) without time zone, "
             "alter column \"added_on\" set not null, "
             "alter column \"added_on\" set default current_timestamp, "
             "alter column \"added_on\" drop identity if exists");

    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             R"(alter sequence "firewalls_id_seq" restart with 5)");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             R"(comment on column "firewalls"."id" is null)");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             R"(comment on column "firewalls"."big_int" is null)");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             R"(comment on column "firewalls"."big_int1" is null)");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             R"(comment on column "firewalls"."name" is null)");
    QVERIFY(log5.boundValues.isEmpty());

    const auto &log6 = log.at(6);
    QCOMPARE(log6.query,
             R"(comment on column "firewalls"."name1" is null)");
    QVERIFY(log6.boundValues.isEmpty());

    const auto &log7 = log.at(7);
    QCOMPARE(log7.query,
             R"(comment on column "firewalls"."name2" is 'name2 note')");
    QVERIFY(log7.boundValues.isEmpty());

    const auto &log8 = log.at(8);
    QCOMPARE(log8.query,
             R"(comment on column "firewalls"."name3" is null)");
    QVERIFY(log8.boundValues.isEmpty());

    const auto &log9 = log.at(9);
    QCOMPARE(log9.query,
             R"(comment on column "firewalls"."name5" is null)");
    QVERIFY(log9.boundValues.isEmpty());

    const auto &log10 = log.at(10);
    QCOMPARE(log10.query,
             R"(comment on column "firewalls"."name6" is null)");
    QVERIFY(log10.boundValues.isEmpty());

    const auto &log11 = log.at(11);
    QCOMPARE(log11.query,
             R"(comment on column "firewalls"."name7" is null)");
    QVERIFY(log11.boundValues.isEmpty());

    const auto &log12 = log.at(12);
    QCOMPARE(log12.query,
             R"(comment on column "firewalls"."amount" is null)");
    QVERIFY(log12.boundValues.isEmpty());

    const auto &log13 = log.at(13);
    QCOMPARE(log13.query,
             R"(comment on column "firewalls"."positions1" is null)");
    QVERIFY(log13.boundValues.isEmpty());

    const auto &log14 = log.at(14);
    QCOMPARE(log14.query,
             R"(comment on column "firewalls"."added_on" is null)");
    QVERIFY(log14.boundValues.isEmpty());

    const auto &log15 = log.at(15);
    QCOMPARE(log15.query,
             "alter table \"firewalls\" "
             "alter column \"id\" type serial, "
             "alter column \"id\" set not null, "
             "alter column \"id\" drop default, "
             "alter column \"id\" drop identity if exists");
    QVERIFY(log15.boundValues.isEmpty());

    const auto &log16 = log.at(16);
    QCOMPARE(log16.query,
             R"(alter sequence "firewalls_id_seq" restart with 15)");
    QVERIFY(log16.boundValues.isEmpty());

    const auto &log17 = log.at(17);
    QCOMPARE(log17.query,
             R"(comment on column "firewalls"."id" is null)");
    QVERIFY(log17.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::useCurrent() const
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
             "create table \"firewalls\" ("
             "\"created\" timestamp(0) without time zone not null, "
             "\"created_current\" timestamp(0) without time zone "
               "not null default current_timestamp, "
             "\"created_t\" timestamp(0) without time zone not null, "
             "\"created_t_current\" timestamp(0) without time zone "
               "not null default current_timestamp)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::useCurrentOnUpdate() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.datetime("updated");
            // PostgreSQL doesn't support on update
            table.datetime("updated_current").useCurrentOnUpdate();

            table.timestamp("updated_t");
            table.timestamp("updated_t_current").useCurrentOnUpdate();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "create table \"firewalls\" ("
             "\"updated\" timestamp(0) without time zone not null, "
             "\"updated_current\" timestamp(0) without time zone not null, "
             "\"updated_t\" timestamp(0) without time zone not null, "
             "\"updated_t_current\" timestamp(0) without time zone not null)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::multipleAutoIncrementStartingValue_CreateTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        /* Tests from and also increments, this would of course fail on real DB as
           you can not have two primary keys. */
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.id().from(100);
            table.increments("increments").from(200);
            table.string(NAME).from(300);
        });
    });

    QCOMPARE(log.size(), 3);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"increments\" serial primary key not null, "
             "\"name\" varchar(255) not null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             R"(alter sequence "firewalls_id_seq" restart with 100)");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             R"(alter sequence "firewalls_increments_seq" restart with 200)");
    QVERIFY(log2.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::multipleAutoIncrementStartingValue_ModifyTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        /* Tests from and also increments, this would of course fail on real DB as
           you can not have two primary keys. */
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.id().from(100);
            table.increments("increments").from(200);
            table.string(NAME).from(300);
        });
    });

    QCOMPARE(log.size(), 3);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "alter table \"firewalls\" "
             "add column \"id\" bigserial primary key not null, "
             "add column \"increments\" serial primary key not null, "
             "add column \"name\" varchar(255) not null");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             R"(alter sequence "firewalls_id_seq" restart with 100)");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             R"(alter sequence "firewalls_increments_seq" restart with 200)");
    QVERIFY(log2.boundValues.isEmpty());
}

/* Timestamps */

void tst_PostgreSQL_SchemaBuilder::add_timestamp() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp(0) without time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_timestamp_WithPrecision() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp(1) without time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_timestamp_WithPrecision_StdNullopt() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp without time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_timestamp_WithPrecision_LowerThanZero() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp without time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_timestamp_WithDefaultValue() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp(0) without time zone not null "
               "default '2023-02-27 10:10:11'");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_timestamp_WithPrecision_UseCurrent() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp(1) without time zone not null "
               "default current_timestamp");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_timestamp_WithPrecision_UseCurrentOnUpdate() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            // PostgreSQL doesn't support on update
            table.timestamp("created_at", 1).useCurrentOnUpdate();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp(1) without time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::
     add_timestamp_WithPrecision_UseCurrent_UseCurrentOnUpdate() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            // PostgreSQL doesn't support on update
            table.timestamp("created_at", 1).useCurrent().useCurrentOnUpdate();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp(1) without time zone not null "
               "default current_timestamp");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_timestampTz() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp(0) with time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_timestampTz_WithPrecision() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp(1) with time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_timestampTz_WithPrecision_StdNullopt() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp with time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_timestampTz_WithPrecision_LowerThanZero() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp with time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

/* Datetime */

void tst_PostgreSQL_SchemaBuilder::add_datetime() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp(0) without time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_datetime_WithPrecision() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp(1) without time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_datetime_WithPrecision_StdNullopt() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp without time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_datetime_WithPrecision_LowerThanZero() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp without time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_datetime_WithDefaultValue() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp(0) without time zone not null "
               "default '2023-02-27 10:10:11'");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_datetime_WithPrecision_UseCurrent() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp(1) without time zone not null "
               "default current_timestamp");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_datetime_WithPrecision_UseCurrentOnUpdate() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            // PostgreSQL doesn't support on update
            table.datetime("created_at", 1).useCurrentOnUpdate();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp(1) without time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::
     add_datetime_WithPrecision_UseCurrent_UseCurrentOnUpdate() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            // PostgreSQL doesn't support on update
            table.datetime("created_at", 1).useCurrent().useCurrentOnUpdate();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp(1) without time zone not null "
               "default current_timestamp");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_datetimeTz() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp(0) with time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_datetimeTz_WithPrecision() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp(1) with time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_datetimeTz_WithPrecision_StdNullopt() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp with time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_datetimeTz_WithPrecision_LowerThanZero() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" timestamp with time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

/* Time */

void tst_PostgreSQL_SchemaBuilder::add_time() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" time(0) without time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_time_WithPrecision() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" time(1) without time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_time_WithPrecision_StdNullopt() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" time without time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_time_WithPrecision_LowerThanZero() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" time without time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_timeTz() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" time(0) with time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_timeTz_WithPrecision() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" time(1) with time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_timeTz_WithPrecision_StdNullopt() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" time with time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::add_timeTz_WithPrecision_LowerThanZero() const
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
             "alter table \"firewalls\" "
             "add column \"created_at\" time with time zone not null");
    QVERIFY(firstLog.boundValues.isEmpty());
}

/* Generated columns */

void tst_PostgreSQL_SchemaBuilder::generatedAs_PrimaryKey() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.id().generatedAs();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table \"firewalls\" "
             "add column \"id\" bigint primary key not null "
               "generated by default as identity");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::generatedAs_PrimaryKey_Always() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.id().generatedAs().always();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table \"firewalls\" "
             "add column \"id\" bigint primary key not null "
               "generated always as identity");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::generatedAs_PrimaryKey_SequenceOptions() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.increments(ID).generatedAs("increment by 10 start with 100");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table \"firewalls\" "
             "add column \"id\" integer primary key not null "
               "generated by default as identity (increment by 10 start with 100)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::generatedAs_NotPrimaryKey() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.integer(SIZE_).generatedAs();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table \"firewalls\" "
             "add column \"size\" integer not null generated by default as identity");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::virtualAs() const
{
    /* Currently, PostgreSQL 15 doesn't support virtual generated columns, only stored,
       but I test it anyway. */
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.integer("foo").nullable();
            table.boolean("bar").virtualAs("foo is not null");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table \"firewalls\" "
             "add column \"foo\" integer null, "
             "add column \"bar\" boolean not null generated always as (foo is not null)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::storedAs() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.integer("foo").nullable();
            table.boolean("bar").storedAs("foo is not null");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table \"firewalls\" "
             "add column \"foo\" integer null, "
             "add column \"bar\" boolean not null "
               "generated always as (foo is not null) stored");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::virtualAs_StoredAs_CreateTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.integer("price");
            /* Currently, PostgreSQL 15 doesn't support virtual generated columns,
               only stored, but I test it anyway. */
            table.integer("discounted_virtual").virtualAs("price - 5");
            table.integer("discounted_stored").storedAs(R"("price" - 5)");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "create table \"firewalls\" ("
             "\"price\" integer not null, "
             "\"discounted_virtual\" integer not null generated always as (price - 5), "
             "\"discounted_stored\" integer not null "
               "generated always as (\"price\" - 5) stored)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::virtualAs_StoredAs_Nullable_CreateTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.integer("price");
            /* Currently, PostgreSQL 15 doesn't support virtual generated columns,
               only stored, but I test it anyway. */
            table.integer("discounted_virtual").virtualAs("price - 5").nullable();
            table.integer("discounted_stored").storedAs(R"("price" - 5)").nullable();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "create table \"firewalls\" ("
             "\"price\" integer not null, "
             "\"discounted_virtual\" integer null generated always as (price - 5), "
             "\"discounted_stored\" integer null "
               "generated always as (\"price\" - 5) stored)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::virtualAs_StoredAs_ModifyTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.integer("price");
            /* Currently, PostgreSQL 15 doesn't support virtual generated columns,
               only stored, but I test it anyway. */
            table.integer("discounted_virtual").virtualAs("price - 5");
            table.integer("discounted_stored").storedAs(R"("price" - 5)");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table \"firewalls\" "
             "add column \"price\" integer not null, "
             "add column \"discounted_virtual\" integer not null "
               "generated always as (price - 5), "
             "add column \"discounted_stored\" integer not null "
               "generated always as (\"price\" - 5) stored");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::virtualAs_StoredAs_Nullable_ModifyTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.integer("price");
            /* Currently, PostgreSQL 15 doesn't support virtual generated columns,
               only stored, but I test it anyway. */
            table.integer("discounted_virtual").virtualAs("price - 5").nullable();
            table.integer("discounted_stored").storedAs(R"("price" - 5)").nullable();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "alter table \"firewalls\" "
             "add column \"price\" integer not null, "
             "add column \"discounted_virtual\" integer null "
               "generated always as (price - 5), "
             "add column \"discounted_stored\" integer null "
               "generated always as (\"price\" - 5) stored");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::change_VirtualAs_ThrowException() const
{
    TVERIFY_THROWS_EXCEPTION(
                LogicError,
                DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            /* Currently, PostgreSQL 15 doesn't support virtual generated columns,
               only stored, but I test it anyway. Changing generated column must throw
               exception, PostgreSQL doesn't support modifying generated columns. */
            table.integer("discounted_virtual").virtualAs("price - 5").change();
        });
    }));
}

void tst_PostgreSQL_SchemaBuilder::change_StoredAs_ThrowException() const
{
    TVERIFY_THROWS_EXCEPTION(
                LogicError,
                DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            /* Currently, PostgreSQL 15 doesn't support virtual generated columns,
               only stored, but I test it anyway. Changing generated column must throw
               exception, PostgreSQL doesn't support modifying generated columns. */
            table.integer("discounted_virtual").storedAs("price - 5").change();
        });
    }));
}

void tst_PostgreSQL_SchemaBuilder::drop_StoredAs() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            // Because of this the CommandDefinition::storedAs must be std::optional
            table.integer("foo").storedAs({}).nullable().change();
        });
    });

    QCOMPARE(log.size(), 2);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "alter table \"firewalls\" "
             "alter column \"foo\" type integer, "
             "alter column \"foo\" drop not null, "
             "alter column \"foo\" drop default, "
             "alter column \"foo\" drop expression if exists, "
             "alter column \"foo\" drop identity if exists");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             R"(comment on column "firewalls"."foo" is null)");
    QVERIFY(log1.boundValues.isEmpty());
}

/* Indexes */

void tst_PostgreSQL_SchemaBuilder::indexes_Fluent() const
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
             "create table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"name_u\" varchar(255) not null, "
             "\"name_i\" varchar(255) not null, "
             "\"name_i_cn\" varchar(255) not null, "
             "\"name_f\" varchar(255) not null, "
             "\"name_f_cn\" varchar(255) not null, "
             "\"coordinates_s\" geography(geometry, 4326) not null, "
             "\"coordinates_s_cn\" geography(geometry, 4326) not null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table \"firewalls\" "
             "add constraint \"firewalls_name_u_unique\" unique (\"name_u\")");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             R"(create index "firewalls_name_i_index" on "firewalls" ("name_i"))");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             R"(create index "name_i_cn_index" on "firewalls" ("name_i_cn"))");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "create index \"firewalls_name_f_fulltext\" on \"firewalls\" "
             "using gin ((to_tsvector('english', \"name_f\")))");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             "create index \"name_f_cn_fulltext\" on \"firewalls\" "
             "using gin ((to_tsvector('english', \"name_f_cn\")))");
    QVERIFY(log5.boundValues.isEmpty());

    const auto &log6 = log.at(6);
    QCOMPARE(log6.query,
             "create index \"firewalls_coordinates_s_spatialindex\" on \"firewalls\" "
             "using gist (\"coordinates_s\")");
    QVERIFY(log6.boundValues.isEmpty());

    const auto &log7 = log.at(7);
    QCOMPARE(log7.query,
             "create index \"coordinates_s_cn_spatial\" on \"firewalls\" "
             "using gist (\"coordinates_s_cn\")");
    QVERIFY(log7.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::indexes_Blueprint() const
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
            table.rawIndex(DB::raw(R"("name_r", name_r1)"), "name_r_raw");

            table.string("name_f");
            table.fullText({"name_f"});

            table.string("name_f_cn");
            table.fullText("name_f_cn", "name_f_cn_fulltext");

            table.geometry("coordinates_s").isGeometry();
            table.spatialIndex("coordinates_s");

            table.point("coordinates_s_cn", 3200).isGeometry();
            table.spatialIndex("coordinates_s_cn", "coordinates_s_cn_spatial");
        });
    });

    QCOMPARE(log.size(), 9);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"name_u\" varchar(255) not null, "
             "\"name_i\" varchar(255) not null, "
             "\"name_i_cn\" varchar(255) not null, "
             "\"name_r\" varchar(255) not null, "
             "\"name_r1\" varchar(255) not null, "
             "\"name_f\" varchar(255) not null, "
             "\"name_f_cn\" varchar(255) not null, "
             "\"coordinates_s\" geometry(geometry) not null, "
             "\"coordinates_s_cn\" geometry(point, 3200) not null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table \"firewalls\" "
             "add constraint \"name_u_unique\" unique (\"name_u\")");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             R"(create index "firewalls_name_i_index" on "firewalls" ("name_i"))");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             R"(create index "name_i_cn_index" on "firewalls" ("name_i_cn"))");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             R"(create index "name_r_raw" on "firewalls" ("name_r", name_r1))");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             "create index \"firewalls_name_f_fulltext\" on \"firewalls\" "
             "using gin ((to_tsvector('english', \"name_f\")))");
    QVERIFY(log5.boundValues.isEmpty());

    const auto &log6 = log.at(6);
    QCOMPARE(log6.query,
             "create index \"name_f_cn_fulltext\" on \"firewalls\" "
             "using gin ((to_tsvector('english', \"name_f_cn\")))");
    QVERIFY(log6.boundValues.isEmpty());

    const auto &log7 = log.at(7);
    QCOMPARE(log7.query,
             "create index \"firewalls_coordinates_s_spatialindex\" on \"firewalls\" "
             "using gist (\"coordinates_s\")");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log8 = log.at(8);
    QCOMPARE(log8.query,
             "create index \"coordinates_s_cn_spatial\" on \"firewalls\" "
             "using gist (\"coordinates_s_cn\")");
    QVERIFY(log8.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::renameIndex() const
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
             "create table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"name\" varchar(255) not null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table \"firewalls\" "
             "add constraint \"firewalls_name_unique\" unique (\"name\")");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter index \"firewalls_name_unique\" "
             "rename to \"firewalls_name_unique_renamed\"");
    QVERIFY(log2.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::dropIndex_ByIndexName() const
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
             "create table \"firewalls\" ("
             "\"id\" integer not null, "
             "\"name_u\" varchar(255) not null, "
             "\"name_i\" varchar(255) not null, "
             "\"name_f\" varchar(255) not null, "
             "\"coordinates_s\" geography(geometry, 4326) not null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             R"(alter table "firewalls" add primary key ("id"))");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table \"firewalls\" "
             "add constraint \"firewalls_name_u_unique\" unique (\"name_u\")");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             R"(create index "firewalls_name_i_index" on "firewalls" ("name_i"))");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "create index \"firewalls_name_f_fulltext\" on \"firewalls\" "
             "using gin ((to_tsvector('english', \"name_f\")))");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             "create index \"firewalls_coordinates_s_spatialindex\" on \"firewalls\" "
             "using gist (\"coordinates_s\")");
    QVERIFY(log5.boundValues.isEmpty());

    const auto &log6 = log.at(6);
    QCOMPARE(log6.query,
             R"(alter table "firewalls" drop constraint "firewalls_pkey")");
    QVERIFY(log6.boundValues.isEmpty());

    const auto &log7 = log.at(7);
    QCOMPARE(log7.query,
             R"(alter table "firewalls" drop constraint "firewalls_name_u_unique")");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log8 = log.at(8);
    QCOMPARE(log8.query,
             R"(drop index "firewalls_name_i_index")");
    QVERIFY(log8.boundValues.isEmpty());

    const auto &log9 = log.at(9);
    QCOMPARE(log9.query,
             R"(drop index "firewalls_name_f_fulltext")");
    QVERIFY(log9.boundValues.isEmpty());

    const auto &log10 = log.at(10);
    QCOMPARE(log10.query,
             R"(drop index "firewalls_coordinates_s_spatialindex")");
    QVERIFY(log10.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::dropIndex_ByColumn() const
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
             "create table \"firewalls\" ("
             "\"id\" integer not null, "
             "\"name_u\" varchar(255) not null, "
             "\"name_i\" varchar(255) not null, "
             "\"name_f\" varchar(255) not null, "
             "\"coordinates_s\" geography(geometry, 4326) not null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             R"(alter table "firewalls" add primary key ("id"))");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table \"firewalls\" "
             "add constraint \"firewalls_name_u_unique\" unique (\"name_u\")");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             R"(create index "firewalls_name_i_index" on "firewalls" ("name_i"))");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "create index \"firewalls_name_f_fulltext\" on \"firewalls\" "
             "using gin ((to_tsvector('english', \"name_f\")))");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             "create index \"firewalls_coordinates_s_spatialindex\" on \"firewalls\" "
             "using gist (\"coordinates_s\")");
    QVERIFY(log5.boundValues.isEmpty());

    const auto &log6 = log.at(6);
    QCOMPARE(log6.query,
             R"(alter table "firewalls" drop constraint "firewalls_pkey")");
    QVERIFY(log6.boundValues.isEmpty());

    const auto &log7 = log.at(7);
    QCOMPARE(log7.query,
             R"(alter table "firewalls" drop constraint "firewalls_name_u_unique")");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log8 = log.at(8);
    QCOMPARE(log8.query,
             R"(drop index "firewalls_name_i_index")");
    QVERIFY(log8.boundValues.isEmpty());

    const auto &log9 = log.at(9);
    QCOMPARE(log9.query,
             R"(drop index "firewalls_name_f_fulltext")");
    QVERIFY(log9.boundValues.isEmpty());

    const auto &log10 = log.at(10);
    QCOMPARE(log10.query,
             R"(drop index "firewalls_coordinates_s_spatialindex")");
    QVERIFY(log10.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::dropIndex_ByMultipleColumns() const
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
             "create table \"firewalls\" ("
             "\"id\" integer not null, "
             "\"id1\" integer not null, "
             "\"name_u\" varchar(255) not null, "
             "\"name_u1\" varchar(255) not null, "
             "\"name_i\" varchar(255) not null, "
             "\"name_i1\" varchar(255) not null, "
             "\"name_f\" varchar(255) not null, "
             "\"name_f1\" varchar(255) not null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             R"(alter table "firewalls" add primary key ("id", "id1"))");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table \"firewalls\" "
             "add constraint \"firewalls_name_u_name_u1_unique\" "
               "unique (\"name_u\", \"name_u1\")");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "create index \"firewalls_name_i_name_i1_index\" "
             "on \"firewalls\" (\"name_i\", \"name_i1\")");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "create index \"firewalls_name_f_name_f1_fulltext\" on \"firewalls\" "
             "using gin ((to_tsvector('english', \"name_f\") || "
               "to_tsvector('english', \"name_f1\")))");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             R"(alter table "firewalls" drop constraint "firewalls_pkey")");
    QVERIFY(log5.boundValues.isEmpty());

    const auto &log6 = log.at(6);
    QCOMPARE(log6.query,
             "alter table \"firewalls\" drop constraint "
             "\"firewalls_name_u_name_u1_unique\"");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log7 = log.at(7);
    QCOMPARE(log7.query,
             R"(drop index "firewalls_name_i_name_i1_index")");
    QVERIFY(log7.boundValues.isEmpty());

    const auto &log8 = log.at(8);
    QCOMPARE(log8.query,
             R"(drop index "firewalls_name_f_name_f1_fulltext")");
    QVERIFY(log8.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::foreignKey() const
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
             "create table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"user_id\" bigint not null, "
             "\"torrent_id\" bigint not null, "
             "\"role_id\" bigint null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table \"firewalls\" "
             "add constraint \"firewalls_user_id_foreign\" "
             "foreign key (\"user_id\") "
             "references \"users\" (\"id\") "
             "on delete cascade on update restrict");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table \"firewalls\" "
             "add constraint \"firewalls_torrent_id_foreign\" "
             "foreign key (\"torrent_id\") "
             "references \"torrents\" (\"id\") "
             "on delete restrict on update restrict");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table \"firewalls\" "
             "add constraint \"firewalls_role_id_foreign\" "
             "foreign key (\"role_id\") "
             "references \"roles\" (\"id\") "
             "on delete set null on update cascade");
    QVERIFY(log3.boundValues.isEmpty());
}

void tst_PostgreSQL_SchemaBuilder::foreignKey_TerserSyntax() const
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
             "create table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"user_id\" bigint not null, "
             "\"torrent_id\" bigint not null, "
             "\"role_id\" bigint null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table \"firewalls\" "
             "add constraint \"firewalls_user_id_foreign\" "
             "foreign key (\"user_id\") "
             "references \"users\" (\"id\") "
             "on delete cascade on update restrict");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table \"firewalls\" "
             "add constraint \"firewalls_torrent_id_foreign\" "
             "foreign key (\"torrent_id\") "
             "references \"torrents\" (\"id\") "
             "on delete restrict on update restrict");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table \"firewalls\" "
             "add constraint \"firewalls_role_id_foreign\" "
             "foreign key (\"role_id\") "
             "references \"roles\" (\"id\") "
             "on delete set null on update cascade");
    QVERIFY(log3.boundValues.isEmpty());
}

#ifndef TINYORM_DISABLE_ORM
void tst_PostgreSQL_SchemaBuilder::foreignKey_WithModel() const
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
             "create table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"torrent_id\" bigint not null, "
             "\"user_id\" bigint null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table \"firewalls\" "
             "add constraint \"firewalls_torrent_id_foreign\" "
             "foreign key (\"torrent_id\") "
             "references \"torrents\" (\"id\") "
             "on delete cascade on update restrict");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table \"firewalls\" "
             "add constraint \"firewalls_user_id_foreign\" "
             "foreign key (\"user_id\") "
             "references \"users\" (\"id\") "
             "on delete set null on update cascade");
    QVERIFY(log2.boundValues.isEmpty());
}
#endif

void tst_PostgreSQL_SchemaBuilder::dropForeign() const
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
             "create table \"firewalls\" ("
             "\"id\" bigserial primary key not null, "
             "\"user_id\" bigint not null, "
             "\"torrent_id\" bigint not null, "
             "\"role_id\" bigint null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table \"firewalls\" "
             "add constraint \"firewalls_user_id_foreign\" "
             "foreign key (\"user_id\") "
             "references \"users\" (\"id\") "
             "on delete cascade on update restrict");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table \"firewalls\" "
             "add constraint \"firewalls_torrent_id_foreign\" "
             "foreign key (\"torrent_id\") "
             "references \"torrents\" (\"id\") "
             "on delete restrict on update restrict");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table \"firewalls\" "
             "add constraint \"firewalls_role_id_foreign\" "
             "foreign key (\"role_id\") "
             "references \"roles\" (\"id\") "
             "on delete set null on update cascade");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             R"(alter table "firewalls" drop constraint "firewalls_user_id_foreign")");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             R"(alter table "firewalls" drop constraint "firewalls_torrent_id_foreign")");
    QVERIFY(log5.boundValues.isEmpty());

    const auto &log6 = log.at(6);
    QCOMPARE(log6.query,
             R"(alter table "firewalls" drop constraint "firewalls_role_id_foreign")");
    QVERIFY(log6.boundValues.isEmpty());

    const auto &log7 = log.at(7);
    QCOMPARE(log7.query,
             R"(alter table "firewalls" drop column "role_id")");
    QVERIFY(log7.boundValues.isEmpty());
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_PostgreSQL_SchemaBuilder)

#include "tst_postgresql_schemabuilder.moc"
