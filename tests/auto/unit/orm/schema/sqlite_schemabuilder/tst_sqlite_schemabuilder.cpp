#include <QCoreApplication>
#include <QTest>

#include <filesystem>

#include "orm/db.hpp"
#include "orm/exceptions/logicerror.hpp"
#include "orm/schema.hpp"
#include "orm/schema/grammars/sqliteschemagrammar.hpp"
#include "orm/utils/type.hpp"

#ifdef TINYORM_USING_TINYDRIVERS
#  include "orm/exceptions/runtimeerror.hpp"
#endif

#include "databases.hpp"
#include "macros.hpp"

#ifndef TINYORM_DISABLE_ORM
#  include "models/user.hpp"
#endif

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::SIZE_;
using Orm::Constants::UTF8;
using Orm::Constants::UcsBasic;

using Orm::DB;
using Orm::Exceptions::LogicError;
using Orm::Exceptions::RuntimeError;
using Orm::Schema;
using Orm::SchemaNs::Blueprint;
using Orm::SchemaNs::Constants::Cascade;
using Orm::SchemaNs::Constants::Restrict;
using Orm::SchemaNs::Grammars::SQLiteSchemaGrammar;

using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

class tst_SQLite_SchemaBuilder : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void createDatabase() const;
    void dropDatabaseIfExists() const;

    void createTable() const;
    void createTable_Temporary() const;
    void createTable_Charset_Collation_Engine() const;

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

    void enableWriteableSchema() const;
    void disableWriteableSchema() const;

    void rebuildDatabase() const;

    void getColumnListing() const;

    void hasTable() const;

    void defaultStringLength_Set() const;

    void modifiers() const;
    void modifier_defaultValue_WithExpression() const;
    void modifier_defaultValue_WithBoolean() const;
    void modifier_defaultValue_Escaping() const;
    void modifier_ColumnComment() const;

    void useCurrent() const;
    void useCurrentOnUpdate() const;

    /* Generated columns */
    void virtualAs_StoredAs_CreateTable() const;
    void virtualAs_StoredAs_Nullable_CreateTable() const;
    void virtualAs_StoredAs_ModifyTable() const;
    void virtualAs_StoredAs_Nullable_ModifyTable() const;

    /* Indexes */
    void indexes_Fluent() const;
    void indexes_Blueprint() const;

    void indexes_Fluent_Fulltext_SpatialIndex_Exceptions() const;
    void indexes_Blueprint_Fulltext_SpatialIndex_Exceptions() const;

    void renameIndex() const;

    void dropIndex_ByIndexName() const;
    void dropIndex_ByColumn() const;
    void dropIndex_ByMultipleColumns() const;

    void dropPrimary_Exception() const;
    void dropFullText_Exception() const;
    void dropSpatialIndex_Exception() const;

    void foreignKey() const;
    void foreignKey_TerserSyntax() const;
#ifndef TINYORM_DISABLE_ORM
    void foreignKey_WithModel() const;
#endif

    void dropForeign() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Get SQLite database filepath for unit tests. */
    static QString getDatabaseFilepath();

    /*! Table or database name used in tests. */
    inline static const auto Firewalls = sl("firewalls");

    /*! Connection name used in this test case. */
    QString m_connection;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_SQLite_SchemaBuilder::initTestCase()
{
    m_connection = Databases::createConnection(Databases::SQLITE);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::SQLITE)
              .toUtf8().constData(), );
}

void tst_SQLite_SchemaBuilder::createDatabase() const
{
    const auto database = getDatabaseFilepath();
    fspath databasePath(database.toUtf8().constData());

    // Clean up before test
    if (fs::exists(databasePath))
        fs::remove(databasePath);

    // Create tmp/ folder
    if (const auto databaseDirPath = databasePath.parent_path();
        !fs::exists(databaseDirPath)
    )
        fs::create_directory(databaseDirPath);

    QVERIFY(!fs::exists(databasePath));

    Schema::on(m_connection).createDatabase(database);

    QVERIFY(fs::exists(databasePath));
}

void tst_SQLite_SchemaBuilder::dropDatabaseIfExists() const
{
    const auto database = getDatabaseFilepath();
    const auto databaseStdString = database.toStdString();

    QVERIFY(fs::exists(databaseStdString));

    Schema::on(m_connection).dropDatabaseIfExists(database);

    QVERIFY(!fs::exists(databaseStdString));

    // Cleanup
    if (fs::exists(databaseStdString))
        fs::remove(databaseStdString);
}

void tst_SQLite_SchemaBuilder::createTable() const
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

            // SQLite doesn't have unsigned integers, so they should be same as above
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
             "\"id\" integer not null primary key autoincrement, "
             "\"char\" varchar not null, "
             "\"char_10\" varchar not null, "
             "\"string\" varchar not null, "
             "\"string_22\" varchar not null, "
             "\"tiny_text\" text not null, "
             "\"text\" text not null, "
             "\"medium_text\" text not null, "
             "\"long_text\" text not null, "
             "\"integer\" integer not null, "
             "\"tinyInteger\" integer not null, "
             "\"smallInteger\" integer not null, "
             "\"mediumInteger\" integer not null, "
             "\"bigInteger\" integer not null, "
             "\"unsignedInteger\" integer not null, "
             "\"unsignedTinyInteger\" integer not null, "
             "\"unsignedSmallInteger\" integer not null, "
             "\"unsignedMediumInteger\" integer not null, "
             "\"unsignedBigInteger\" integer not null, "
             "\"uuid\" varchar not null, "
             "\"ip_address\" varchar not null, "
             "\"mac_address\" varchar not null, "
             "\"tiny_binary\" blob not null, "
             "\"binary\" blob not null, "
             "\"medium_binary\" blob not null, "
             "\"long_binary\" blob not null)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::createTable_Temporary() const
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
             "\"id\" integer not null primary key autoincrement, "
             "\"name\" varchar not null)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::createTable_Charset_Collation_Engine() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            // charset ignored with the SQLite grammar
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
             "\"id\" integer not null primary key autoincrement, "
             "\"name\" varchar not null)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::timestamps_rememberToken_softDeletes_CreateAndDrop() const
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

            // precision ignored with the SQLite grammar
            table.timestamps(3);
        });
    });

    QCOMPARE(log.size(), 6);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" integer not null primary key autoincrement, "
             "\"created_at\" datetime, "
             "\"updated_at\" datetime, "
             "\"remember_token\" varchar, "
             "\"deleted_at\" datetime)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             R"(alter table "firewalls" drop column "created_at")");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             R"(alter table "firewalls" drop column "updated_at")");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             R"(alter table "firewalls" drop column "remember_token")");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             R"(alter table "firewalls" drop column "deleted_at")");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             "create table \"firewalls\" ("
             "\"id\" integer not null primary key autoincrement, "
             "\"created_at\" datetime, "
             "\"updated_at\" datetime)");
    QVERIFY(log5.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::datetimes_softDeletesDatetime_CreateAndDrop() const
{
    /* On SQLite timestamp() and datetime() creates the DATETIME column type so
       the result should be the same like the previous unit test. */
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

            // precision ignored with the SQLite grammar
            table.datetimes(3);
        });
    });

    QCOMPARE(log.size(), 5);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" integer not null primary key autoincrement, "
             "\"created_at\" datetime, "
             "\"updated_at\" datetime, "
             "\"deleted_at\" datetime)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             R"(alter table "firewalls" drop column "created_at")");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             R"(alter table "firewalls" drop column "updated_at")");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             R"(alter table "firewalls" drop column "deleted_at")");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "create table \"firewalls\" ("
             "\"id\" integer not null primary key autoincrement, "
             "\"created_at\" datetime, "
             "\"updated_at\" datetime)");
    QVERIFY(log4.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::modifyTable() const
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

    QCOMPARE(log.size(), 23);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             R"(alter table "firewalls" add column "char" varchar not null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             R"(alter table "firewalls" add column "char_10" varchar not null)");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             R"(alter table "firewalls" add column "string" varchar not null)");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             R"(alter table "firewalls" add column "string_22" varchar not null)");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             R"(alter table "firewalls" add column "tiny_text" text not null)");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             R"(alter table "firewalls" add column "text" text not null)");
    QVERIFY(log5.boundValues.isEmpty());

    const auto &log6 = log.at(6);
    QCOMPARE(log6.query,
             R"(alter table "firewalls" add column "medium_text" text not null)");
    QVERIFY(log6.boundValues.isEmpty());

    const auto &log7 = log.at(7);
    QCOMPARE(log7.query,
             R"(alter table "firewalls" add column "long_text" text not null)");
    QVERIFY(log7.boundValues.isEmpty());

    const auto &log8 = log.at(8);
    QCOMPARE(log8.query,
             R"(alter table "firewalls" add column "integer" integer)");
    QVERIFY(log8.boundValues.isEmpty());

    const auto &log9 = log.at(9);
    QCOMPARE(log9.query,
             R"(alter table "firewalls" add column "tinyInteger" integer not null)");
    QVERIFY(log9.boundValues.isEmpty());

    const auto &log10 = log.at(10);
    QCOMPARE(log10.query,
             R"(alter table "firewalls" add column "smallInteger" integer not null)");
    QVERIFY(log10.boundValues.isEmpty());

    const auto &log11 = log.at(11);
    QCOMPARE(log11.query,
             R"(alter table "firewalls" add column "mediumInteger" integer not null)");
    QVERIFY(log11.boundValues.isEmpty());

    const auto &log12 = log.at(12);
    QCOMPARE(log12.query,
             R"(alter table "firewalls" add column "tiny_binary" blob not null)");
    QVERIFY(log12.boundValues.isEmpty());

    const auto &log13 = log.at(13);
    QCOMPARE(log13.query,
             R"(alter table "firewalls" add column "binary" blob not null)");
    QVERIFY(log13.boundValues.isEmpty());

    const auto &log14 = log.at(14);
    QCOMPARE(log14.query,
             R"(alter table "firewalls" add column "medium_binary" blob not null)");
    QVERIFY(log14.boundValues.isEmpty());

    const auto &log15 = log.at(15);
    QCOMPARE(log15.query,
             R"(alter table "firewalls" add column "long_binary" blob not null)");
    QVERIFY(log15.boundValues.isEmpty());

    const auto &log16 = log.at(16);
    QCOMPARE(log16.query,
             R"(alter table "firewalls" drop column "long_text")");
    QVERIFY(log16.boundValues.isEmpty());

    const auto &log17 = log.at(17);
    QCOMPARE(log17.query,
             R"(alter table "firewalls" drop column "medium_text")");
    QVERIFY(log17.boundValues.isEmpty());

    const auto &log18 = log.at(18);
    QCOMPARE(log18.query,
             R"(alter table "firewalls" drop column "text")");
    QVERIFY(log18.boundValues.isEmpty());

    const auto &log19 = log.at(19);
    QCOMPARE(log19.query,
             R"(alter table "firewalls" drop column "smallInteger")");
    QVERIFY(log19.boundValues.isEmpty());

    const auto &log20 = log.at(20);
    QCOMPARE(log20.query,
             R"(alter table "firewalls" drop column "mediumInteger")");
    QVERIFY(log20.boundValues.isEmpty());

    const auto &log21 = log.at(21);
    QCOMPARE(log21.query,
             "alter table \"firewalls\" "
             "rename column \"integer\" to \"integer_renamed\"");
    QVERIFY(log21.boundValues.isEmpty());

    const auto &log22 = log.at(22);
    QCOMPARE(log22.query,
             "alter table \"firewalls\" "
             "rename column \"string_22\" to \"string_22_renamed\"");
    QVERIFY(log22.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::modifyTable_WithComment() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            // Can't throw an exception (SQLite doesn't support table comments)
            table.comment(sl("Example 'table' comment"));
        });
    });

    // No database queries can't be generated
    QVERIFY(log.isEmpty());
}

void tst_SQLite_SchemaBuilder::dropTable() const
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

void tst_SQLite_SchemaBuilder::dropTableIfExists() const
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

void tst_SQLite_SchemaBuilder::rename() const
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

void tst_SQLite_SchemaBuilder::dropColumns() const
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

        QCOMPARE(log.size(), 2);

        const auto &log0 = log.at(0);
        QCOMPARE(log0.query,
                 R"(alter table "firewalls" drop column "name")");
        QVERIFY(log0.boundValues.isEmpty());

        const auto &log1 = log.at(1);
        QCOMPARE(log1.query,
                 R"(alter table "firewalls" drop column "size")");
        QVERIFY(log1.boundValues.isEmpty());
    }
    {
        auto log = DB::connection(m_connection).pretend([](auto &connection)
        {
            Schema::on(connection.getName()).dropColumns(Firewalls, NAME, SIZE_);
        });

        QCOMPARE(log.size(), 2);

        QCOMPARE(log.size(), 2);

        const auto &log0 = log.at(0);
        QCOMPARE(log0.query,
                 R"(alter table "firewalls" drop column "name")");
        QVERIFY(log0.boundValues.isEmpty());

        const auto &log1 = log.at(1);
        QCOMPARE(log1.query,
                 R"(alter table "firewalls" drop column "size")");
        QVERIFY(log1.boundValues.isEmpty());
    }
}

void tst_SQLite_SchemaBuilder::renameColumn() const
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

void tst_SQLite_SchemaBuilder::dropAllTypes() const
{
    TVERIFY_THROWS_EXCEPTION(LogicError, Schema::on(m_connection).dropAllTypes());
}

void tst_SQLite_SchemaBuilder::getAllTables() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).getAllTables();
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select name, type "
             "from sqlite_master "
             "where type = 'table' and name not like 'sqlite_%'");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::getAllViews() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).getAllViews();
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select name, type from sqlite_master where type = 'view'");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::enableForeignKeyConstraints() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).enableForeignKeyConstraints();
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, "pragma foreign_keys = on");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::disableForeignKeyConstraints() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).disableForeignKeyConstraints();
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, "pragma foreign_keys = off");
    QVERIFY(firstLog.boundValues.isEmpty());
}

// This shouldn't be strictly here but if I have written it then I leave it here
void tst_SQLite_SchemaBuilder::enableWriteableSchema() const
{
    const auto &sqliteGrammar =
            dynamic_cast<const SQLiteSchemaGrammar &>(
                DB::connection(m_connection).getSchemaGrammar());

    QCOMPARE(sqliteGrammar.compileEnableWriteableSchema(),
             "pragma writable_schema = on");
}

void tst_SQLite_SchemaBuilder::disableWriteableSchema() const
{
    const auto &sqliteGrammar =
            dynamic_cast<const SQLiteSchemaGrammar &>(
                DB::connection(m_connection).getSchemaGrammar());

    QCOMPARE(sqliteGrammar.compileDisableWriteableSchema(),
             "pragma writable_schema = off");
}

void tst_SQLite_SchemaBuilder::rebuildDatabase() const
{
    const auto &sqliteGrammar =
            dynamic_cast<const SQLiteSchemaGrammar &>(
                DB::connection(m_connection).getSchemaGrammar());

    QCOMPARE(sqliteGrammar.compileRebuild(), "vacuum");
}

void tst_SQLite_SchemaBuilder::getColumnListing() const
{
    auto &connection = DB::connection(m_connection);

    auto log = connection.pretend([](auto &connection_)
    {
        Schema::on(connection_.getName()).getColumnListing(Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, R"(pragma table_info("firewalls"))");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::hasTable() const
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
             "select * from sqlite_master where type = 'table' and name = ?");
    QCOMPARE(firstLog.boundValues,
             QList<QVariant>({QVariant(Firewalls)}));
}

void tst_SQLite_SchemaBuilder::defaultStringLength_Set() const
{
    // This doesn't make sense with the SQLite grammar but it passes tests anyway
    QVERIFY(Blueprint::DefaultStringLength == Orm::SchemaNs::DefaultStringLength);

    Schema::defaultStringLength(191);
    QCOMPARE(Blueprint::DefaultStringLength, 191);

    // Restore
    Schema::defaultStringLength(Orm::SchemaNs::DefaultStringLength);
    QVERIFY(Blueprint::DefaultStringLength == Orm::SchemaNs::DefaultStringLength);
}

void tst_SQLite_SchemaBuilder::modifiers() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            // startingValue/from ignored with the SQLite grammar
            table.bigInteger(ID).autoIncrement().startingValue(5);
            table.bigInteger("big_int");
            table.string(NAME).defaultValue("guest");
            table.string("name1").nullable();
            // comment ignored with the SQLite grammar
            table.string("name2").comment("name2 note");
            table.string("name3");
            // SQLite doesn't support invisible columns
//            table.string("name4").invisible().change();
            // charset/collation ignored with the SQLite grammar
            table.string("name5").charset(UTF8);
            table.string("name6").collation(UcsBasic);
            table.string("name7").charset(UTF8).collation(UcsBasic);
            // SQLite doesn't support renaming columns during the change() call
//            table.string("name8_old", 64).renameTo("name8").change();
        });
        /* Tests from and also integerIncrements, this would of course fail on real DB
           as you can not have two primary keys. */
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            // startingValue/from ignored with the SQLite grammar
            table.integerIncrements(ID).from(15);
        });
    });

    QCOMPARE(log.size(), 2);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" integer not null primary key autoincrement, "
             "\"big_int\" integer not null, "
             "\"name\" varchar not null default 'guest', "
             "\"name1\" varchar, "
             "\"name2\" varchar not null, "
             "\"name3\" varchar not null, "
             "\"name5\" varchar not null, "
             "\"name6\" varchar not null, "
             "\"name7\" varchar not null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table \"firewalls\" "
             "add column \"id\" integer not null primary key autoincrement");
    QVERIFY(log1.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::modifier_defaultValue_WithExpression() const
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
             "\"name\" varchar not null default 'guest', "
             "\"name_raw\" varchar not null default 'guest_raw')");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::modifier_defaultValue_WithBoolean() const
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
             "\"boolean\" tinyint(1) not null, "
             "\"boolean_false\" tinyint(1) not null default '0', "
             "\"boolean_true\" tinyint(1) not null default '1', "
             "\"boolean_0\" tinyint(1) not null default '0', "
             "\"boolean_1\" tinyint(1) not null default '1')");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::modifier_defaultValue_Escaping() const
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
             "create table \"firewalls\" ("
             "\"string\" varchar not null "
             "default 'Text '' and \" or \\ newline\n"
             "and tab	end')");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::modifier_ColumnComment() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            // Can't throw an exception (SQLite doesn't support column comments)
            table.string("string").comment("string note");
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             R"(alter table "firewalls" add column "string" varchar not null)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::useCurrent() const
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
             "\"created\" datetime not null, "
             "\"created_current\" datetime not null default current_timestamp, "
             "\"created_t\" datetime not null, "
             "\"created_t_current\" datetime not null default current_timestamp)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::useCurrentOnUpdate() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.datetime("updated");
            // SQLite doesn't support on update
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
             "\"updated\" datetime not null, "
             "\"updated_current\" datetime not null, "
             "\"updated_t\" datetime not null, "
             "\"updated_t_current\" datetime not null)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

/* Generated columns */

void tst_SQLite_SchemaBuilder::virtualAs_StoredAs_CreateTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.integer("price");
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
             "\"discounted_virtual\" integer generated always as (price - 5) not null, "
             "\"discounted_stored\" integer "
               "generated always as (\"price\" - 5) stored not null)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::virtualAs_StoredAs_Nullable_CreateTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.integer("price");
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
             "\"discounted_virtual\" integer generated always as (price - 5), "
             "\"discounted_stored\" integer "
               "generated always as (\"price\" - 5) stored)");
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::virtualAs_StoredAs_ModifyTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.integer("price");
            table.integer("discounted_virtual").virtualAs("price - 5");
            // SQLite doesn't support a stored in the add column clause, will be removed
            table.integer("discounted_stored").storedAs(R"("price" - 5)");
        });
    });

    QCOMPARE(log.size(), 2);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "alter table \"firewalls\" "
             "add column \"price\" integer not null");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table \"firewalls\" "
             "add column \"discounted_virtual\" integer "
               "generated always as (price - 5) not null");
    QVERIFY(log1.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::virtualAs_StoredAs_Nullable_ModifyTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.integer("price");
            table.integer("discounted_virtual").virtualAs("price - 5").nullable();
            // SQLite doesn't support a stored in the add column clause, will be removed
            table.integer("discounted_stored").storedAs(R"("price" - 5)").nullable();
        });
    });

    QCOMPARE(log.size(), 2);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "alter table \"firewalls\" "
             "add column \"price\" integer not null");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table \"firewalls\" "
             "add column \"discounted_virtual\" integer "
               "generated always as (price - 5)");
    QVERIFY(log1.boundValues.isEmpty());
}

/* Indexes */

void tst_SQLite_SchemaBuilder::indexes_Fluent() const
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
        });
    });

    QCOMPARE(log.size(), 4);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" integer not null primary key autoincrement, "
             "\"name_u\" varchar not null, "
             "\"name_i\" varchar not null, "
             "\"name_i_cn\" varchar not null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "create unique index \"firewalls_name_u_unique\" "
             "on \"firewalls\" (\"name_u\")");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             R"(create index "firewalls_name_i_index" on "firewalls" ("name_i"))");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             R"(create index "name_i_cn_index" on "firewalls" ("name_i_cn"))");
    QVERIFY(log3.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::indexes_Blueprint() const
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
        });
    });

    QCOMPARE(log.size(), 5);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" integer not null primary key autoincrement, "
             "\"name_u\" varchar not null, "
             "\"name_i\" varchar not null, "
             "\"name_i_cn\" varchar not null, "
             "\"name_r\" varchar not null, "
             "\"name_r1\" varchar not null)");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             R"(create unique index "name_u_unique" on "firewalls" ("name_u"))");
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
}

void tst_SQLite_SchemaBuilder::indexes_Fluent_Fulltext_SpatialIndex_Exceptions() const
{
    TVERIFY_THROWS_EXCEPTION(
                RuntimeError,
                Schema::on(m_connection).create(Firewalls, [](Blueprint &table)
                {
                    table.id();

                    table.string("name_f").fulltext();
                }));

    TVERIFY_THROWS_EXCEPTION(
                RuntimeError,
                Schema::on(m_connection).create(Firewalls, [](Blueprint &table)
                {
                    table.id();

                    table.geometry("coordinates_s").spatialIndex();
                }));
}

void tst_SQLite_SchemaBuilder::indexes_Blueprint_Fulltext_SpatialIndex_Exceptions() const
{
    TVERIFY_THROWS_EXCEPTION(
                RuntimeError,
                Schema::on(m_connection).create(Firewalls, [](Blueprint &table)
                {
                    table.id();

                    table.string("name_f");
                    table.fullText({"name_f"});
                }));

    TVERIFY_THROWS_EXCEPTION(
                RuntimeError,
                Schema::on(m_connection).create(Firewalls, [](Blueprint &table)
                {
                    table.id();

                    table.geometry("coordinates_s").isGeometry();
                    table.spatialIndex("coordinates_s");
                }));
}

void tst_SQLite_SchemaBuilder::renameIndex() const
{
    TVERIFY_THROWS_EXCEPTION(
                RuntimeError,
                Schema::on(m_connection).table(Firewalls, [](Blueprint &table)
                {
                    table.renameIndex("firewalls_name_unique",
                                      "firewalls_name_unique_renamed");
                }));
}

void tst_SQLite_SchemaBuilder::dropIndex_ByIndexName() const
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
        });

        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.dropUnique("firewalls_name_u_unique");
            table.dropIndex("firewalls_name_i_index");
        });
    });

    QCOMPARE(log.size(), 5);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" integer not null, "
             "\"name_u\" varchar not null, "
             "\"name_i\" varchar not null, "
             "primary key (\"id\"))");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "create unique index \"firewalls_name_u_unique\" "
             "on \"firewalls\" (\"name_u\")");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             R"(create index "firewalls_name_i_index" on "firewalls" ("name_i"))");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             R"(drop index "firewalls_name_u_unique")");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             R"(drop index "firewalls_name_i_index")");
    QVERIFY(log4.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::dropIndex_ByColumn() const
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
        });

        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.dropUnique({"name_u"});
            table.dropIndex({"name_i"});
        });
    });

    QCOMPARE(log.size(), 5);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" integer not null, "
             "\"name_u\" varchar not null, "
             "\"name_i\" varchar not null, "
             "primary key (\"id\"))");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "create unique index \"firewalls_name_u_unique\" "
             "on \"firewalls\" (\"name_u\")");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             R"(create index "firewalls_name_i_index" on "firewalls" ("name_i"))");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             R"(drop index "firewalls_name_u_unique")");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             R"(drop index "firewalls_name_i_index")");
    QVERIFY(log4.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::dropIndex_ByMultipleColumns() const
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
        });

        Schema::on(connection.getName())
                .table(Firewalls, [](Blueprint &table)
        {
            table.dropUnique({"name_u", "name_u1"});
            table.dropIndex({"name_i", "name_i1"});
        });
    });

    QCOMPARE(log.size(), 5);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" integer not null, "
             "\"id1\" integer not null, "
             "\"name_u\" varchar not null, "
             "\"name_u1\" varchar not null, "
             "\"name_i\" varchar not null, "
             "\"name_i1\" varchar not null, "
             "primary key (\"id\", \"id1\"))");
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "create unique index \"firewalls_name_u_name_u1_unique\" "
             "on \"firewalls\" (\"name_u\", \"name_u1\")");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "create index \"firewalls_name_i_name_i1_index\" "
             "on \"firewalls\" (\"name_i\", \"name_i1\")");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             R"(drop index "firewalls_name_u_name_u1_unique")");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             R"(drop index "firewalls_name_i_name_i1_index")");
    QVERIFY(log4.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::dropPrimary_Exception() const
{
    TVERIFY_THROWS_EXCEPTION(
                RuntimeError,
                Schema::on(m_connection).table(Firewalls, [](Blueprint &table)
                {
                    table.dropPrimary();
                }));
}

void tst_SQLite_SchemaBuilder::dropFullText_Exception() const
{
    TVERIFY_THROWS_EXCEPTION(
                RuntimeError,
                Schema::on(m_connection).table(Firewalls, [](Blueprint &table)
                {
                    // By column name
                    table.dropFullText({"name_f"});
                }));
}

void tst_SQLite_SchemaBuilder::dropSpatialIndex_Exception() const
{
    TVERIFY_THROWS_EXCEPTION(
                RuntimeError,
                Schema::on(m_connection).table(Firewalls, [](Blueprint &table)
                {
                    // By index name
                    table.dropSpatialIndex("firewalls_coordinates_s_spatialindex");
                }));
}

void tst_SQLite_SchemaBuilder::foreignKey() const
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

    QCOMPARE(log.size(), 1);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" integer not null primary key autoincrement, "
             "\"user_id\" integer not null, "
             "\"torrent_id\" integer not null, "
             "\"role_id\" integer, "
             "foreign key(\"user_id\") references \"users\"(\"id\") "
              "on delete cascade on update restrict, "
             "foreign key(\"torrent_id\") references \"torrents\"(\"id\") "
              "on delete restrict on update restrict, "
             "foreign key(\"role_id\") references \"roles\"(\"id\") "
              "on delete set null on update cascade)");
    QVERIFY(log0.boundValues.isEmpty());
}

void tst_SQLite_SchemaBuilder::foreignKey_TerserSyntax() const
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

    QCOMPARE(log.size(), 1);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" integer not null primary key autoincrement, "
             "\"user_id\" integer not null, "
             "\"torrent_id\" integer not null, "
             "\"role_id\" integer, "
             "foreign key(\"user_id\") references \"users\"(\"id\") "
              "on delete cascade on update restrict, "
             "foreign key(\"torrent_id\") references \"torrents\"(\"id\") "
              "on delete restrict on update restrict, "
             "foreign key(\"role_id\") references \"roles\"(\"id\") "
              "on delete set null on update cascade)");
    QVERIFY(log0.boundValues.isEmpty());
}

#ifndef TINYORM_DISABLE_ORM
void tst_SQLite_SchemaBuilder::foreignKey_WithModel() const
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

    QCOMPARE(log.size(), 1);

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             "create table \"firewalls\" ("
             "\"id\" integer not null primary key autoincrement, "
             "\"torrent_id\" integer not null, "
             "\"user_id\" integer, "
             "foreign key(\"torrent_id\") references \"torrents\"(\"id\") "
              "on delete cascade on update restrict, "
             "foreign key(\"user_id\") references \"users\"(\"id\") "
              "on delete set null on update cascade)");
    QVERIFY(log0.boundValues.isEmpty());
}
#endif

void tst_SQLite_SchemaBuilder::dropForeign() const
{
    TVERIFY_THROWS_EXCEPTION(
                RuntimeError,
                Schema::on(m_connection).table(Firewalls, [](Blueprint &table)
                {
                    // By column name
                    table.dropForeign({"user_id"});
                }));
}

/* private */

QString tst_SQLite_SchemaBuilder::getDatabaseFilepath()
{
    static const auto cached = []
    {
        QString database;
        database.reserve(Firewalls.size() + 32);

        return database
                .append(sl("tmp/tinyorm_tests_"))
                .append(Firewalls)
                .append(sl(".sqlite3"));
    }();

    return cached;
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_SQLite_SchemaBuilder)

#include "tst_sqlite_schemabuilder.moc"
