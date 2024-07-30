#include <QCoreApplication>
#include <QTest>

#include "databases.hpp"

#include "models/torrent.hpp"

using Orm::Constants::AND;
using Orm::Constants::LIKE;
using Orm::Constants::Progress;

using Orm::QueryBuilder;

using TypeUtils = Orm::Utils::Type;

using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::Relations::Relation;
using Orm::Tiny::TinyBuilder;

using TestUtils::Databases;

using Models::FilePropertyProperty;
using Models::Torrent;
using Models::TorrentPreviewableFile;

class tst_QueriesRelationships : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase_data() const;

    /* Querying Relationship Existence/Absence */
    void has_Basic_QString_OnHasMany() const;
    void has_Basic_UniquePtr_OnHasMany() const;
    void has_Basic_MethodPointer_OnHasMany() const;

    void has_Count_QString_OnHasMany() const;
    void has_Count_UniquePtr_OnHasMany() const;
    void has_Count_MethodPointer_OnHasMany() const;

    void whereHas_Basic_QString_QueryBuilder_OnHasMany() const;
    void whereHas_Basic_QString_TinyBuilder_OnHasMany() const;

    void whereHas_Count_QString_QueryBuilder_OnHasMany() const;
    void whereHas_Count_QString_TinyBuilder_OnHasMany() const;
    void whereHas_Count_MethodPointer_TinyBuilder_OnHasMany() const;

    void hasNested_Basic_OnHasMany() const;
    void hasNested_Count_OnHasMany() const;
    void hasNested_Count_TinyBuilder_OnHasMany() const;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_QueriesRelationships::initTestCase_data() const
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

/* Querying Relationship Existence/Absence */

void tst_QueriesRelationships::has_Basic_QString_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::has("torrentFiles")->get();

    const QList<QVariant> expectedIds {1, 2, 3, 4, 5, 7};

    for (const auto &torrent : std::as_const(torrents))
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_QueriesRelationships::has_Basic_UniquePtr_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Has to live long enough to avoid dangling reference
    Torrent dummyModel;

    // Ownership of a unique_ptr()
    auto relation =
            Relation<Torrent, TorrentPreviewableFile>::noConstraints(
                [&dummyModel]()
    {
        return std::invoke(&Torrent::torrentFiles, dummyModel);
    });

    auto torrents = Torrent::has<TorrentPreviewableFile, void>(std::move(relation))
                    ->get();

    const QList<QVariant> expectedIds {1, 2, 3, 4, 5, 7};

    for (const auto &torrent : std::as_const(torrents))
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_QueriesRelationships::has_Basic_MethodPointer_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::has<TorrentPreviewableFile>(&Torrent::torrentFiles)
                    ->get();

    const QList<QVariant> expectedIds {1, 2, 3, 4, 5, 7};

    for (const auto &torrent : std::as_const(torrents))
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_QueriesRelationships::has_Count_QString_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::has("torrentFiles", ">", 1)
                    ->get();

    const QList<QVariant> expectedIds {2, 5, 7};

    for (const auto &torrent : std::as_const(torrents))
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_QueriesRelationships::has_Count_UniquePtr_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Has to live long enough to avoid dangling reference
    Torrent dummyModel;

    // Ownership of a unique_ptr()
    auto relation =
            Relation<Torrent, TorrentPreviewableFile>::noConstraints(
                [&dummyModel]()
    {
        return std::invoke(&Torrent::torrentFiles, dummyModel);
    });

    auto torrents = Torrent::has<TorrentPreviewableFile, void>(std::move(relation),
                                                               ">=", 2)
                    ->get();

    const QList<QVariant> expectedIds {2, 5, 7};

    for (const auto &torrent : std::as_const(torrents))
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_QueriesRelationships::has_Count_MethodPointer_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::has<TorrentPreviewableFile>(&Torrent::torrentFiles, ">=", 2)
                    ->get();

    const QList<QVariant> expectedIds {2, 5, 7};

    for (const auto &torrent : std::as_const(torrents))
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_QueriesRelationships::whereHas_Basic_QString_QueryBuilder_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::whereHas("torrentFiles", [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), QueryBuilder &>));

        query.where("filepath", LIKE, "%_file2.mkv");
    })
            ->get();

    const QList<QVariant> expectedIds {2, 5, 7};

    for (const auto &torrent : std::as_const(torrents))
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_QueriesRelationships::whereHas_Basic_QString_TinyBuilder_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::whereHas<TorrentPreviewableFile>("torrentFiles",
                                                              [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query),
                                TinyBuilder<TorrentPreviewableFile> &>));

        query.where("filepath", LIKE, "%_file2.mkv");
    })
            ->get();

    const QList<QVariant> expectedIds {2, 5, 7};

    for (const auto &torrent : std::as_const(torrents))
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_QueriesRelationships::whereHas_Count_QString_QueryBuilder_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::whereHas("torrentFiles", [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), QueryBuilder &>));

        query.where(Progress, ">=", 870);
    }, ">=", 2)
            ->get();

    const QList<QVariant> expectedIds {2, 5};

    for (const auto &torrent : std::as_const(torrents))
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_QueriesRelationships::whereHas_Count_QString_TinyBuilder_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::whereHas<TorrentPreviewableFile>("torrentFiles",
                                                              [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query),
                                TinyBuilder<TorrentPreviewableFile> &>));

        query.where(Progress, ">=", 870);
    }, ">=", 2)
            ->get();

    const QList<QVariant> expectedIds {2, 5};

    for (const auto &torrent : std::as_const(torrents))
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_QueriesRelationships::whereHas_Count_MethodPointer_TinyBuilder_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::whereHas<TorrentPreviewableFile>(&Torrent::torrentFiles,
                                                              [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query),
                                TinyBuilder<TorrentPreviewableFile> &>));

        query.where(Progress, ">=", 870);
    }, ">=", 2)
            ->get();

    const QList<QVariant> expectedIds {2, 5};

    for (const auto &torrent : std::as_const(torrents))
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_QueriesRelationships::hasNested_Basic_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::has<FilePropertyProperty>(
                        "torrentFiles.fileProperty.filePropertyProperty")
                    ->get();

    const QList<QVariant> expectedIds {2, 3, 4, 5};

    for (const auto &torrent : std::as_const(torrents))
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_QueriesRelationships::hasNested_Count_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::has<FilePropertyProperty>(
                        "torrentFiles.fileProperty.filePropertyProperty", ">=", 2)
                    ->get();

    const QList<QVariant> expectedIds {3, 5};

    for (const auto &torrent : std::as_const(torrents))
        QVERIFY(expectedIds.contains(torrent.getKey()));
}

void tst_QueriesRelationships::hasNested_Count_TinyBuilder_OnHasMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = Torrent::has<FilePropertyProperty>(
                        "torrentFiles.fileProperty.filePropertyProperty", ">=", 2, AND,
                        [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query),
                                TinyBuilder<FilePropertyProperty> &>));

        query.where("value", ">=", 6);
    })
            ->get();

    const QList<QVariant> expectedIds {5};

    for (const auto &torrent : std::as_const(torrents))
        QVERIFY(expectedIds.contains(torrent.getKey()));
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_QueriesRelationships)

#include "tst_queriesrelationships.moc"
