#include <QCoreApplication>
#include <QtTest>

#include "orm/db.hpp"
#include "orm/exceptions/logicerror.hpp"
#include "orm/schema.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"

#ifndef TINYORM_DISABLE_ORM
#  include "models/user.hpp"
#endif

using Orm::Constants::ID;
using Orm::Constants::MyISAM;
using Orm::Constants::NAME;
using Orm::Constants::QMYSQL;
using Orm::Constants::SIZE;
using Orm::Constants::UTF8;
using Orm::Constants::UTF8Generalci;
using Orm::Constants::UTF8Unicodeci;
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

    void timestamps_rememberToken_softDeletes_CreateAndDrop() const;

    void modifyTable() const;

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

    void useCurrent() const;
    void useCurrentOnUpdate() const;

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
    inline static const auto Firewalls = QStringLiteral("firewalls");

    /*! The charset set for the current MySQL connection (based on env. variable). */
    QString m_charset {};
    /*! The collation set for the current MySQL connection (based on env. variable). */
    QString m_collation {};

    /*! Connection name used in this test case. */
    QString m_connection {};
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_MySql_SchemaBuilder::initTestCase()
{
    m_connection = Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );

    // The charset and collation in all queries is set on the base of env. variables
    const auto &connection = DB::connection(m_connection);
    m_charset = connection.getConfig(charset_).value<QString>();
    m_collation = connection.getConfig(collation_).value<QString>();
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
             QStringLiteral(
                 "create database `firewalls` "
                 "default character set `%1` default collate `%2`")
             .arg(m_charset, m_collation));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::createDatabase_Charset_Collation() const
{
    static const auto mysqlCreateDb = QStringLiteral("tinyorm_mysql_tests_create_db");

    // Create a new connection with different charset and collation
    DB::addConnection({
        {driver_,    QMYSQL},
        {charset_,   UTF8},
        {collation_, UTF8Generalci},
    }, mysqlCreateDb);

    auto log = DB::connection(mysqlCreateDb).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).createDatabase(Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             QStringLiteral(
                 "create database `firewalls` "
                 "default character set `%1` default collate `%2`")
             .arg(UTF8, UTF8Generalci));
    QVERIFY(firstLog.boundValues.isEmpty());

    // Restore
    DB::removeConnection(mysqlCreateDb);
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
    QCOMPARE(firstLog.query,
             QStringLiteral("drop database if exists `%1`").arg(Firewalls));
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
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             QStringLiteral(
                 "create table `firewalls` ("
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
                 "`mac_address` varchar(17) not null) "
                 "default character set %1 collate '%2' "
                 "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::createTable_Temporary() const
{
    auto &connection = DB::connection(m_connection);

    auto log = connection.pretend([](auto &connection)
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
             QStringLiteral(
                 "create temporary table `firewalls` ("
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
             QStringLiteral(
                 "create table `firewalls` ("
                 "`id` bigint unsigned not null auto_increment primary key, "
                 "`name` varchar(255) not null) "
                 "default character set %1 collate '%2' "
                 "engine = %3")
             .arg(UTF8, UTF8Generalci, MyISAM));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::timestamps_rememberToken_softDeletes_CreateAndDrop() const
{
    auto &connection = DB::connection(m_connection);

    auto log = connection.pretend([](auto &connection)
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

    const auto engineConfig = connection.getConfig(engine_).value<QString>();

    const auto &log0 = log.at(0);
    QCOMPARE(log0.query,
             QStringLiteral(
                 "create table `firewalls` ("
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
             QStringLiteral(
                "create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`created_at` timestamp(3) null, "
                "`updated_at` timestamp(3) null) "
                "default character set %1 collate '%2' "
                "engine = %3")
             .arg(m_charset, m_collation, engineConfig));
    QVERIFY(log4.boundValues.isEmpty());
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
             "add `char` char(255) not null, "
             "add `char_10` char(10) not null, "
             "add `string` varchar(255) not null, "
             "add `string_22` varchar(22) not null, "
             "add `tiny_text` tinytext not null, "
             "add `text` text not null, "
             "add `medium_text` mediumtext not null, "
             "add `long_text` longtext not null, "
             "add `integer` int null, "
             "add `tinyInteger` tinyint not null, "
             "add `smallInteger` smallint not null, "
             "add `mediumInteger` mediumint not null");
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

void tst_MySql_SchemaBuilder::dropTable() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName()).drop(Firewalls);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query, QStringLiteral("drop table `%1`").arg(Firewalls));
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
             QStringLiteral("drop table if exists `%1`").arg(Firewalls));
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
            Schema::on(connection.getName()).dropColumns(Firewalls, {NAME, SIZE});
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
            Schema::on(connection.getName()).dropColumns(Firewalls, NAME, SIZE);
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
    QVERIFY_EXCEPTION_THROWN(Schema::on(m_connection).dropAllTypes(), LogicError);
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
    QCOMPARE(firstLog.query, "SHOW FULL TABLES WHERE table_type = 'BASE TABLE'");
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
    QCOMPARE(firstLog.query, "SHOW FULL TABLES WHERE table_type = 'VIEW'");
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
    QCOMPARE(firstLog.query, "SET FOREIGN_KEY_CHECKS=1");
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
    QCOMPARE(firstLog.query, "SET FOREIGN_KEY_CHECKS=0");
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
             QVector<QVariant>({QVariant(connection.getDatabaseName()),
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
             QVector<QVariant>({QVariant(connection.getDatabaseName()),
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
            table.string(NAME).defaultValue("guest");
            table.string("name1").nullable();
            table.string("name2").comment("name2 note");
            table.string("name3");
            table.string("name4").invisible();
            table.string("name5").charset(UTF8);
            table.string("name6").collation("utf8mb4_unicode_ci");
            table.string("name7").charset(UTF8).collation(UTF8Unicodeci);
            table.bigInteger("big_int").isUnsigned();
            table.bigInteger("big_int1");
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
             QStringLiteral(
                "create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`name` varchar(255) not null default 'guest', "
                "`name1` varchar(255) null, "
                "`name2` varchar(255) not null comment 'name2 note', "
                "`name3` varchar(255) not null, "
                "`name4` varchar(255) not null invisible, "
                "`name5` varchar(255) character set 'utf8' not null, "
                "`name6` varchar(255) collate 'utf8mb4_unicode_ci' not null, "
                "`name7` varchar(255) character set 'utf8' collate 'utf8_unicode_ci' "
                  "not null, "
                "`big_int` bigint unsigned not null, "
                "`big_int1` bigint not null) "
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
             "add `name` varchar(255) not null after `big_int`, "
             "add `id` int unsigned not null auto_increment primary key first");
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
             QStringLiteral(
                "create table `firewalls` ("
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
             QStringLiteral(
                "create table `firewalls` ("
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
             QStringLiteral(
                "create table `firewalls` ("
                "`string` varchar(255) not null default 'Text '' and \" or \\\\ newline\n"
                "and tab	end') "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(firstLog.boundValues.isEmpty());
}

void tst_MySql_SchemaBuilder::useCurrent() const
{
    auto log = DB::connection(m_connection).pretend([](auto &connection)
    {
        Schema::on(connection.getName())
                .create(Firewalls, [](Blueprint &table)
        {
            table.dateTime("created");
            table.dateTime("created_current").useCurrent();

            table.timestamp("created_t");
            table.timestamp("created_t_current").useCurrent();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             QStringLiteral(
                "create table `firewalls` ("
                "`created` datetime not null, "
                "`created_current` datetime default CURRENT_TIMESTAMP not null, "
                "`created_t` timestamp not null, "
                "`created_t_current` timestamp default CURRENT_TIMESTAMP not null) "
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
            table.dateTime("updated");
            table.dateTime("updated_current").useCurrentOnUpdate();

            table.timestamp("updated_t");
            table.timestamp("updated_t_current").useCurrentOnUpdate();
        });
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             QStringLiteral(
                "create table `firewalls` ("
                "`updated` datetime not null, "
                "`updated_current` datetime on update CURRENT_TIMESTAMP not null, "
                "`updated_t` timestamp not null, "
                "`updated_t_current` timestamp on update CURRENT_TIMESTAMP not null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(firstLog.boundValues.isEmpty());
}

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
             QStringLiteral(
                "create table `firewalls` ("
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
             "alter table `firewalls` add unique `firewalls_name_u_unique`(`name_u`)");
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
             "add fulltext `firewalls_name_f_fulltext`(`name_f`)");
    QVERIFY(log4.boundValues.isEmpty());

    const auto &log5 = log.at(5);
    QCOMPARE(log5.query,
             "alter table `firewalls` add fulltext `name_f_cn_fulltext`(`name_f_cn`)");
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
             QStringLiteral(
                "create table `firewalls` ("
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
             "alter table `firewalls` add unique `name_u_unique`(`name_u`)");
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
             "add fulltext `firewalls_name_f_fulltext`(`name_f`)");
    QVERIFY(log5.boundValues.isEmpty());

    const auto &log6 = log.at(6);
    QCOMPARE(log6.query,
             "alter table `firewalls` add fulltext `name_f_cn_fulltext`(`name_f_cn`)");
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
             QStringLiteral(
                "create table `firewalls` ("
                "`id` bigint unsigned not null auto_increment primary key, "
                "`name` varchar(255) not null) "
                "default character set %1 collate '%2' "
                "engine = InnoDB")
             .arg(m_charset, m_collation));
    QVERIFY(log0.boundValues.isEmpty());

    const auto &log1 = log.at(1);
    QCOMPARE(log1.query,
             "alter table `firewalls` add unique `firewalls_name_unique`(`name`)");
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
             QStringLiteral(
                "create table `firewalls` ("
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
             "alter table `firewalls` add primary key `firewalls_id_primary`(`id`)");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` add unique `firewalls_name_u_unique`(`name_u`)");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table `firewalls` add index `firewalls_name_i_index`(`name_i`)");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "alter table `firewalls` "
             "add fulltext `firewalls_name_f_fulltext`(`name_f`)");
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
             QStringLiteral(
                "create table `firewalls` ("
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
             "alter table `firewalls` add primary key `firewalls_id_primary`(`id`)");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` add unique `firewalls_name_u_unique`(`name_u`)");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table `firewalls` add index `firewalls_name_i_index`(`name_i`)");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "alter table `firewalls` "
             "add fulltext `firewalls_name_f_fulltext`(`name_f`)");
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
             QStringLiteral(
                "create table `firewalls` ("
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
             "alter table `firewalls` "
             "add primary key `firewalls_id_id1_primary`(`id`, `id1`)");
    QVERIFY(log1.boundValues.isEmpty());

    const auto &log2 = log.at(2);
    QCOMPARE(log2.query,
             "alter table `firewalls` "
             "add unique `firewalls_name_u_name_u1_unique`(`name_u`, `name_u1`)");
    QVERIFY(log2.boundValues.isEmpty());

    const auto &log3 = log.at(3);
    QCOMPARE(log3.query,
             "alter table `firewalls` "
             "add index `firewalls_name_i_name_i1_index`(`name_i`, `name_i1`)");
    QVERIFY(log3.boundValues.isEmpty());

    const auto &log4 = log.at(4);
    QCOMPARE(log4.query,
             "alter table `firewalls` "
             "add fulltext `firewalls_name_f_name_f1_fulltext`(`name_f`, `name_f1`)");
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
             QStringLiteral(
                "create table `firewalls` ("
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
             QStringLiteral(
                "create table `firewalls` ("
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
             QStringLiteral(
                "create table `firewalls` ("
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
             QStringLiteral(
                "create table `firewalls` ("
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

QTEST_MAIN(tst_MySql_SchemaBuilder)

#include "tst_mysql_schemabuilder.moc"
