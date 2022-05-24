#include <QCoreApplication>
#include <QtTest>

#include "orm/db.hpp"
#include "orm/schema/grammars/mysqlschemagrammar.hpp"

#include "databases.hpp"

using Orm::Constants::NAME;
using Orm::Constants::SIZE;

using Orm::DB;

using Orm::SchemaNs::Blueprint;
using Orm::SchemaNs::IndexCommand;
using Orm::SchemaNs::Grammars::MySqlSchemaGrammar;

using TestUtils::Databases;

class tst_Blueprint : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private slots:
    void initTestCase();

    void index_DefaultNames() const;
    void index_DefaultNames_WithPrefix() const;

    void dropIndex_ByColumns_DefaultNames() const;
    void dropIndex_ByColumns_DefaultNames_WithPrefix() const;

    void dropIndex_ByIndexName() const;

    void unsignedDecimal_WithTotalAndPlaces() const;

    void removeColumn() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Connection name used in this test case. */
    QString m_connection {};
};

void tst_Blueprint::initTestCase()
{
    m_connection = Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(QStringLiteral("%1 autotest skipped, environment variables "
                             "for '%2' connection have not been defined.")
              .arg("tst_Blueprint", Databases::MYSQL).toUtf8().constData(), );
}

void tst_Blueprint::index_DefaultNames() const
{
    // Unique
    {
        Blueprint blueprint("torrents");
        blueprint.unique({NAME, "foo"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "torrents_name_foo_unique");
    }
    // Index
    {
        Blueprint blueprint("torrents");
        blueprint.index(SIZE);
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "torrents_size_index");
    }
    // SpatialIndex
    {
        Blueprint blueprint("torrents");
        blueprint.spatialIndex({"coordinates"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "torrents_coordinates_spatialindex");
    }
    // FullText
    {
        Blueprint blueprint("torrents");
        blueprint.fullText("note");
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
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

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "prefix_torrents_name_foo_unique");
    }
    // Index
    {
        Blueprint blueprint("torrents", nullptr, "prefix_");
        blueprint.index(SIZE);
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "prefix_torrents_size_index");
    }
    // SpatialIndex
    {
        Blueprint blueprint("torrents", nullptr, "prefix_");
        blueprint.spatialIndex({"coordinates"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "prefix_torrents_coordinates_spatialindex");
    }
    // FullText
    {
        Blueprint blueprint("torrents", nullptr, "prefix_");
        blueprint.fullText("note");
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "prefix_torrents_note_fulltext");
    }
}

void tst_Blueprint::dropIndex_ByColumns_DefaultNames() const
{
    {
        Blueprint blueprint("torrents");
        blueprint.dropUnique({NAME, "foo"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "torrents_name_foo_unique");
    }
    // Index
    {
        Blueprint blueprint("torrents");
        blueprint.dropIndex(QVector<QString> {SIZE});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "torrents_size_index");
    }
    // SpatialIndex
    {
        Blueprint blueprint("torrents");
        blueprint.dropSpatialIndex({"coordinates"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "torrents_coordinates_spatialindex");
    }
    // FullText
    {
        Blueprint blueprint("torrents");
        blueprint.dropFullText({"note"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "torrents_note_fulltext");
    }
}

void tst_Blueprint::dropIndex_ByColumns_DefaultNames_WithPrefix() const
{
    {
        Blueprint blueprint("torrents", nullptr, "prefix_");
        blueprint.dropUnique({NAME, "foo"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "prefix_torrents_name_foo_unique");
    }
    // Index
    {
        Blueprint blueprint("torrents", nullptr, "prefix_");
        blueprint.dropIndex(QVector<QString> {SIZE});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "prefix_torrents_size_index");
    }
    // SpatialIndex
    {
        Blueprint blueprint("torrents", nullptr, "prefix_");
        blueprint.dropSpatialIndex({"coordinates"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "prefix_torrents_coordinates_spatialindex");
    }
    // FullText
    {
        Blueprint blueprint("torrents", nullptr, "prefix_");
        blueprint.dropFullText({"note"});
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "prefix_torrents_note_fulltext");
    }
}

void tst_Blueprint::dropIndex_ByIndexName() const
{
    {
        Blueprint blueprint("torrents");
        blueprint.dropUnique(NAME);
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 NAME);
    }
    // Index
    {
        Blueprint blueprint("torrents");
        blueprint.dropIndex(SIZE);
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 SIZE);
    }
    // SpatialIndex
    {
        Blueprint blueprint("torrents");
        blueprint.dropSpatialIndex("coordinates");
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "coordinates");
    }
    // FullText
    {
        Blueprint blueprint("torrents");
        blueprint.dropFullText("note");
        const auto &commands = blueprint.getCommands();

        QCOMPARE(reinterpret_cast<const IndexCommand &>(*commands.front()).index,
                 "note");
    }
}

void tst_Blueprint::unsignedDecimal_WithTotalAndPlaces() const
{
    Blueprint blueprint("cars", [](Blueprint &table)
    {
        table.unsignedDecimal("money", 10, 2);
    });

    QCOMPARE(blueprint.toSql(DB::connection(m_connection), MySqlSchemaGrammar()),
             QVector<QString> {"alter table `cars` "
                               "add `money` decimal(10, 2) unsigned not null"});
}

void tst_Blueprint::removeColumn() const
{
    Blueprint blueprint("torrents", [](Blueprint &table)
    {
        table.string(NAME);
        table.integer(SIZE);
        table.removeColumn(SIZE);
    });

    QCOMPARE(blueprint.toSql(DB::connection(m_connection), MySqlSchemaGrammar()),
             QVector<QString> {"alter table `torrents` "
                               "add `name` varchar(255) not null"});
}

QTEST_MAIN(tst_Blueprint)

#include "tst_blueprint.moc"
