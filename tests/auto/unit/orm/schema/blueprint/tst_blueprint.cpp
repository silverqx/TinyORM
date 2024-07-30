#include <QCoreApplication>
#include <QTest>

#include "orm/db.hpp"
#include "orm/schema/grammars/mysqlschemagrammar.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"

using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::SIZE_;

using Orm::DB;

using Orm::SchemaNs::Blueprint;
using Orm::SchemaNs::IndexCommand;
using Orm::SchemaNs::Grammars::MySqlSchemaGrammar;

using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

class tst_Blueprint : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void index_DefaultNames() const;
    void index_DefaultNames_WithPrefix() const;

    void dropIndex_ByColumns_DefaultNames() const;
    void dropIndex_ByColumns_DefaultNames_WithPrefix() const;

    void dropIndex_ByIndexName() const;

    void unsignedDecimal_WithTotalAndPlaces() const;

    void removeColumn() const;

    void softDeletes() const;
    void softDeletes_Custom_ColumnName() const;

    void dropSoftDeletes() const;
    void dropSoftDeletes_Custom_ColumnName() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Connection name used in this test case. */
    QString m_connection;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Blueprint::initTestCase()
{
    m_connection = Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );
}

void tst_Blueprint::index_DefaultNames() const
{
    // Unique
    {
        Blueprint blueprint("torrents");
        blueprint.unique({NAME, "foo"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "torrents_name_foo_unique");
    }
    // Index
    {
        Blueprint blueprint("torrents");
        blueprint.index(SIZE_);
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "torrents_size_index");
    }
    // SpatialIndex
    {
        Blueprint blueprint("torrents");
        blueprint.spatialIndex({"coordinates"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "torrents_coordinates_spatialindex");
    }
    // FullText
    {
        Blueprint blueprint("torrents");
        blueprint.fullText(NOTE);
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "torrents_note_fulltext");
    }
}

void tst_Blueprint::index_DefaultNames_WithPrefix() const
{
    // Unique
    {
        Blueprint blueprint("torrents", nullptr, "prefix_");
        blueprint.unique({NAME, "foo"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "prefix_torrents_name_foo_unique");
    }
    // Index
    {
        Blueprint blueprint("torrents", nullptr, "prefix_");
        blueprint.index(SIZE_);
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "prefix_torrents_size_index");
    }
    // SpatialIndex
    {
        Blueprint blueprint("torrents", nullptr, "prefix_");
        blueprint.spatialIndex({"coordinates"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "prefix_torrents_coordinates_spatialindex");
    }
    // FullText
    {
        Blueprint blueprint("torrents", nullptr, "prefix_");
        blueprint.fullText(NOTE);
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "prefix_torrents_note_fulltext");
    }
}

void tst_Blueprint::dropIndex_ByColumns_DefaultNames() const
{
    {
        Blueprint blueprint("torrents");
        blueprint.dropUnique({NAME, "foo"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "torrents_name_foo_unique");
    }
    // Index
    {
        Blueprint blueprint("torrents");
        blueprint.dropIndex(QList<QString> {SIZE_});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "torrents_size_index");
    }
    // SpatialIndex
    {
        Blueprint blueprint("torrents");
        blueprint.dropSpatialIndex({"coordinates"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "torrents_coordinates_spatialindex");
    }
    // FullText
    {
        Blueprint blueprint("torrents");
        blueprint.dropFullText(QList<QString> {NOTE});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "torrents_note_fulltext");
    }
}

void tst_Blueprint::dropIndex_ByColumns_DefaultNames_WithPrefix() const
{
    {
        Blueprint blueprint("torrents", nullptr, "prefix_");
        blueprint.dropUnique({NAME, "foo"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "prefix_torrents_name_foo_unique");
    }
    // Index
    {
        Blueprint blueprint("torrents", nullptr, "prefix_");
        blueprint.dropIndex(QList<QString> {SIZE_});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "prefix_torrents_size_index");
    }
    // SpatialIndex
    {
        Blueprint blueprint("torrents", nullptr, "prefix_");
        blueprint.dropSpatialIndex({"coordinates"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "prefix_torrents_coordinates_spatialindex");
    }
    // FullText
    {
        Blueprint blueprint("torrents", nullptr, "prefix_");
        blueprint.dropFullText(QList<QString> {NOTE});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "prefix_torrents_note_fulltext");
    }
}

void tst_Blueprint::dropIndex_ByIndexName() const
{
    {
        Blueprint blueprint("torrents");
        blueprint.dropUnique(NAME);
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 NAME);
    }
    // Index
    {
        Blueprint blueprint("torrents");
        blueprint.dropIndex(SIZE_);
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 SIZE_);
    }
    // SpatialIndex
    {
        Blueprint blueprint("torrents");
        blueprint.dropSpatialIndex("coordinates");
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 "coordinates");
    }
    // FullText
    {
        Blueprint blueprint("torrents");
        blueprint.dropFullText(NOTE);
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index, // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 NOTE);
    }
}

void tst_Blueprint::unsignedDecimal_WithTotalAndPlaces() const
{
    Blueprint blueprint("cars", [](Blueprint &table)
    {
        table.unsignedDecimal("money", 10, 2);
    });

    QCOMPARE(blueprint.toSql(DB::connection(m_connection), MySqlSchemaGrammar()),
             QList<QString> {"alter table `cars` "
                             "add column `money` decimal(10, 2) unsigned not null"});
}

void tst_Blueprint::removeColumn() const
{
    Blueprint blueprint("torrents", [](Blueprint &table)
    {
        table.string(NAME);
        table.integer(SIZE_);
        table.removeColumn(SIZE_);
    });

    QCOMPARE(blueprint.toSql(DB::connection(m_connection), MySqlSchemaGrammar()),
             QList<QString> {"alter table `torrents` "
                             "add column `name` varchar(255) not null"});
}

void tst_Blueprint::softDeletes() const
{
    Blueprint blueprint("torrents", [](Blueprint &table)
    {
        table.softDeletes();
    });

    QCOMPARE(blueprint.toSql(DB::connection(m_connection), MySqlSchemaGrammar()),
             QList<QString> {"alter table `torrents` "
                             "add column `deleted_at` timestamp null"});
}

void tst_Blueprint::softDeletes_Custom_ColumnName() const
{
    Blueprint blueprint("torrents", [](Blueprint &table)
    {
        table.softDeletes("deleted_on");
    });

    QCOMPARE(blueprint.toSql(DB::connection(m_connection), MySqlSchemaGrammar()),
             QList<QString> {"alter table `torrents` "
                             "add column `deleted_on` timestamp null"});
}

void tst_Blueprint::dropSoftDeletes() const
{
    Blueprint blueprint("torrents", [](Blueprint &table)
    {
        table.dropSoftDeletes();
    });

    QCOMPARE(blueprint.toSql(DB::connection(m_connection), MySqlSchemaGrammar()),
             QList<QString> {"alter table `torrents` drop `deleted_at`"});
}

void tst_Blueprint::dropSoftDeletes_Custom_ColumnName() const
{
    Blueprint blueprint("torrents", [](Blueprint &table)
    {
        table.dropSoftDeletes("deleted_on");
    });

    QCOMPARE(blueprint.toSql(DB::connection(m_connection), MySqlSchemaGrammar()),
             QList<QString> {"alter table `torrents` drop `deleted_on`"});
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_Blueprint)

#include "tst_blueprint.moc"
