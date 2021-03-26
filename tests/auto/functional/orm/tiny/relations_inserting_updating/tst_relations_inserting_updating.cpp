#include <QCoreApplication>
#include <QtTest>

#include "models/torrent.hpp"
#include "models/torrentpreviewablefile.hpp"

#include "database.hpp"

using Orm::AttributeItem;
using Orm::QueryError;
using Orm::Tiny::ConnectionOverride;

class tst_Relations_Inserting_Updating : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase_data() const;

    void save_OnHasOneOrMany() const;
    void save_OnHasOneOrMany_WithRValue() const;
    void save_OnHasOneOrMany_Failed() const;

    void saveMany_OnHasOneOrMany() const;
    void saveMany_OnHasOneOrMany_WithRValue() const;
    void saveMany_OnHasOneOrMany_Failed() const;

    void create_OnHasOneOrMany() const;
    void create_OnHasOneOrMany_WithRValue() const;
    void create_OnHasOneOrMany_Failed() const;
    void create_OnHasOneOrMany_WithRValue_Failed() const;

    void createMany_OnHasOneOrMany() const;
    void createMany_OnHasOneOrMany_WithRValue() const;
    void createMany_OnHasOneOrMany_Failed() const;
    void createMany_OnHasOneOrMany_WithRValue_Failed() const;
};

void tst_Relations_Inserting_Updating::initTestCase_data() const
{
    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : TestUtils::Database::createConnections())
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

void tst_Relations_Inserting_Updating::save_OnHasOneOrMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    TorrentPreviewableFile file({
        {"file_index", 3},
        {"filepath", "test5_file4-save.mkv"},
        {"size", 322322},
        {"progress", 777},
        {"note", "relation's save"},
    });
    QVERIFY(!file.exists);

    auto [savedResult, fileRef] = torrent->torrentFiles()->save(file);
    QVERIFY(savedResult);
    QVERIFY(file.exists);
    QVERIFY(file["id"].isValid());
    QVERIFY(file["id"].toULongLong() > 8);

    // save method have to return the same model as a reference
    QVERIFY(reinterpret_cast<uintptr_t>(&file)
            == reinterpret_cast<uintptr_t>(&fileRef));

    // Obtain file and verify saved values
    auto fileVerify = TorrentPreviewableFile::find(file["id"]);
    QCOMPARE((*fileVerify)["torrent_id"], QVariant(5));
    QCOMPARE((*fileVerify)["file_index"], QVariant(3));
    QCOMPARE((*fileVerify)["filepath"],   QVariant("test5_file4-save.mkv"));
    QCOMPARE((*fileVerify)["size"],       QVariant(322322));
    QCOMPARE((*fileVerify)["progress"],   QVariant(777));
    QCOMPARE((*fileVerify)["note"],       QVariant("relation's save"));

    // Remove file, restore db
    const auto result = file.remove();
    QVERIFY(result);
    QVERIFY(!file.exists);
}

void tst_Relations_Inserting_Updating::save_OnHasOneOrMany_WithRValue() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto [savedResult ,file] = torrent->torrentFiles()->save({
        {"file_index", 3},
        {"filepath", "test5_file4-save.mkv"},
        {"size", 322322},
        {"progress", 777},
        {"note", "relation's save"},
    });
    QVERIFY(savedResult);
    QVERIFY(file.exists);
    QVERIFY(file["id"].isValid());
    QVERIFY(file["id"].toULongLong() > 8);

    // Obtain file and verify saved values
    auto fileVerify = TorrentPreviewableFile::find(file["id"]);
    QCOMPARE((*fileVerify)["torrent_id"], QVariant(5));
    QCOMPARE((*fileVerify)["file_index"], QVariant(3));
    QCOMPARE((*fileVerify)["filepath"],   QVariant("test5_file4-save.mkv"));
    QCOMPARE((*fileVerify)["size"],       QVariant(322322));
    QCOMPARE((*fileVerify)["progress"],   QVariant(777));
    QCOMPARE((*fileVerify)["note"],       QVariant("relation's save"));

    // Remove file, restore db
    const auto result = file.remove();
    QVERIFY(result);
    QVERIFY(!file.exists);
}

void tst_Relations_Inserting_Updating::save_OnHasOneOrMany_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    TorrentPreviewableFile file({
        {"file_index", 1},
        {"filepath", "test1_file1.mkv"},
        {"size", 377477},
        {"progress", 222},
        {"note", "relation's save fail"},
    });
    QVERIFY(!file.exists);

    QVERIFY_EXCEPTION_THROWN(torrent->torrentFiles()->save(file),
                             QueryError);
    QVERIFY(!file.exists);
    QVERIFY(!file["id"].isValid());
    QCOMPARE(file["torrent_id"], QVariant(1));
}

void tst_Relations_Inserting_Updating::saveMany_OnHasOneOrMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    TorrentPreviewableFile file1({
        {"file_index", 3},
        {"filepath", "test5_file4-saveMany.mkv"},
        {"size", 322322},
        {"progress", 777},
        {"note", "relation's saveMany file1"},
    });
    QVERIFY(!file1.exists);
    TorrentPreviewableFile file2({
        {"file_index", 4},
        {"filepath", "test5_file5-saveMany.mkv"},
        {"size", 333322},
        {"progress", 888},
        {"note", "relation's saveMany file2"},
    });
    QVERIFY(!file2.exists);

    QVector<TorrentPreviewableFile> filesToSave {std::move(file1), std::move(file2)};

    auto savedFiles = torrent->torrentFiles()->saveMany(filesToSave);
    QCOMPARE(savedFiles.size(), 2);

    auto &savedFile1 = savedFiles[0];
    auto &savedFile2 = savedFiles[1];
    QVERIFY(savedFile1.exists);
    QVERIFY(savedFile2.exists);
    QVERIFY(savedFile1["id"].isValid());
    QVERIFY(savedFile2["id"].isValid());
    QVERIFY(savedFile1["id"].toULongLong() > 8);
    QVERIFY(savedFile2["id"].toULongLong() > 9);

    // saveMany() have to return reference to the same 'models' vector
    QVERIFY(reinterpret_cast<uintptr_t>(&filesToSave[0])
            != reinterpret_cast<uintptr_t>(&savedFile1));
    QVERIFY(reinterpret_cast<uintptr_t>(&filesToSave[1])
            != reinterpret_cast<uintptr_t>(&savedFile2));

    // Obtain files and verify saved values
    auto file1Verify = TorrentPreviewableFile::find(savedFile1["id"]);
    QCOMPARE((*file1Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file1Verify)["file_index"], QVariant(3));
    QCOMPARE((*file1Verify)["filepath"],   QVariant("test5_file4-saveMany.mkv"));
    QCOMPARE((*file1Verify)["size"],       QVariant(322322));
    QCOMPARE((*file1Verify)["progress"],   QVariant(777));
    QCOMPARE((*file1Verify)["note"],       QVariant("relation's saveMany file1"));
    auto file2Verify = TorrentPreviewableFile::find(savedFile2["id"]);
    QCOMPARE((*file2Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file2Verify)["file_index"], QVariant(4));
    QCOMPARE((*file2Verify)["filepath"],   QVariant("test5_file5-saveMany.mkv"));
    QCOMPARE((*file2Verify)["size"],       QVariant(333322));
    QCOMPARE((*file2Verify)["progress"],   QVariant(888));
    QCOMPARE((*file2Verify)["note"],       QVariant("relation's saveMany file2"));

    // Remove files, restore db
    const auto result1 = savedFile1.remove();
    QVERIFY(result1);
    QVERIFY(!savedFile1.exists);
    const auto result2 = savedFile2.remove();
    QVERIFY(result2);
    QVERIFY(!savedFile2.exists);
}

void tst_Relations_Inserting_Updating::saveMany_OnHasOneOrMany_WithRValue() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto savedFiles = torrent->torrentFiles()->saveMany({{
        {"file_index", 3},
        {"filepath", "test5_file4-saveMany.mkv"},
        {"size", 322322},
        {"progress", 777},
        {"note", "relation's saveMany file1"},
    }, {
        {"file_index", 4},
        {"filepath", "test5_file5-saveMany.mkv"},
        {"size", 333322},
        {"progress", 888},
        {"note", "relation's saveMany file2"},
    }});
    QCOMPARE(savedFiles.size(), 2);

    auto &savedFile1 = savedFiles[0];
    auto &savedFile2 = savedFiles[1];
    QVERIFY(savedFile1.exists);
    QVERIFY(savedFile2.exists);
    QVERIFY(savedFile1["id"].isValid());
    QVERIFY(savedFile2["id"].isValid());
    QVERIFY(savedFile1["id"].toULongLong() > 8);
    QVERIFY(savedFile2["id"].toULongLong() > 9);

    // Obtain files and verify saved values
    auto file1Verify = TorrentPreviewableFile::find(savedFile1["id"]);
    QCOMPARE((*file1Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file1Verify)["file_index"], QVariant(3));
    QCOMPARE((*file1Verify)["filepath"],   QVariant("test5_file4-saveMany.mkv"));
    QCOMPARE((*file1Verify)["size"],       QVariant(322322));
    QCOMPARE((*file1Verify)["progress"],   QVariant(777));
    QCOMPARE((*file1Verify)["note"],       QVariant("relation's saveMany file1"));
    auto file2Verify = TorrentPreviewableFile::find(savedFile2["id"]);
    QCOMPARE((*file2Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file2Verify)["file_index"], QVariant(4));
    QCOMPARE((*file2Verify)["filepath"],   QVariant("test5_file5-saveMany.mkv"));
    QCOMPARE((*file2Verify)["size"],       QVariant(333322));
    QCOMPARE((*file2Verify)["progress"],   QVariant(888));
    QCOMPARE((*file2Verify)["note"],       QVariant("relation's saveMany file2"));

    // Remove files, restore db
    const auto result1 = savedFile1.remove();
    QVERIFY(result1);
    QVERIFY(!savedFile1.exists);
    const auto result2 = savedFile2.remove();
    QVERIFY(result2);
    QVERIFY(!savedFile2.exists);
}

void tst_Relations_Inserting_Updating::saveMany_OnHasOneOrMany_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    TorrentPreviewableFile file1({
        {"file_index", 1},
        {"filepath", "test1_file1.mkv"},
        {"size", 377477},
        {"progress", 222},
        {"note", "relation's save fail"},
    });
    QVERIFY(!file1.exists);
    // Make a copy is enough
    auto file2 = file1;
    QVERIFY(!file2.exists);

    QVector<TorrentPreviewableFile> filesToSave {std::move(file1), std::move(file2)};
    QVector<TorrentPreviewableFile> savedFiles;
    QVERIFY_EXCEPTION_THROWN(savedFiles = torrent->torrentFiles()->saveMany(filesToSave),
                             QueryError);
    QVERIFY(savedFiles.isEmpty());
}

void tst_Relations_Inserting_Updating::create_OnHasOneOrMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVector<AttributeItem> fileAttribtues {
        {"file_index", 3},
        {"filepath", "test5_file4-create.mkv"},
        {"size", 322322},
        {"progress", 777},
        {"note", "relation's save"},
    };

    auto file = torrent->torrentFiles()->create(fileAttribtues);
    QVERIFY(file.exists);
    QVERIFY(file["id"].isValid());
    QVERIFY(file["id"].toULongLong() > 8);

    // Obtain file and verify saved values
    auto fileVerify = TorrentPreviewableFile::find(file["id"]);
    QCOMPARE((*fileVerify)["torrent_id"], QVariant(5));
    QCOMPARE((*fileVerify)["file_index"], QVariant(3));
    QCOMPARE((*fileVerify)["filepath"],   QVariant("test5_file4-create.mkv"));
    QCOMPARE((*fileVerify)["size"],       QVariant(322322));
    QCOMPARE((*fileVerify)["progress"],   QVariant(777));
    QCOMPARE((*fileVerify)["note"],       QVariant("relation's save"));

    // Remove file, restore db
    const auto result = file.remove();
    QVERIFY(result);
    QVERIFY(!file.exists);
}

void tst_Relations_Inserting_Updating::create_OnHasOneOrMany_WithRValue() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto file = torrent->torrentFiles()->create({
        {"file_index", 3},
        {"filepath", "test5_file4-create.mkv"},
        {"size", 322322},
        {"progress", 777},
        {"note", "relation's save"},
    });
    QVERIFY(file.exists);
    QVERIFY(file["id"].isValid());
    QVERIFY(file["id"].toULongLong() > 8);

    // Obtain file and verify saved values
    auto fileVerify = TorrentPreviewableFile::find(file["id"]);
    QCOMPARE((*fileVerify)["torrent_id"], QVariant(5));
    QCOMPARE((*fileVerify)["file_index"], QVariant(3));
    QCOMPARE((*fileVerify)["filepath"],   QVariant("test5_file4-create.mkv"));
    QCOMPARE((*fileVerify)["size"],       QVariant(322322));
    QCOMPARE((*fileVerify)["progress"],   QVariant(777));
    QCOMPARE((*fileVerify)["note"],       QVariant("relation's save"));

    // Remove file, restore db
    const auto result = file.remove();
    QVERIFY(result);
    QVERIFY(!file.exists);
}

void tst_Relations_Inserting_Updating::create_OnHasOneOrMany_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVector<AttributeItem> fileAttribtues {
        {"file_index", 1},
        {"filepath", "test1_file1.mkv"},
        {"size", 377477},
        {"progress", 222},
        {"note", "relation's save fail"},
    };
    TorrentPreviewableFile file;

    QVERIFY_EXCEPTION_THROWN(file = torrent->torrentFiles()->create(fileAttribtues),
                             QueryError);
    QVERIFY(!file.exists);
    QVERIFY(file.getAttributes().isEmpty());
}

void tst_Relations_Inserting_Updating::create_OnHasOneOrMany_WithRValue_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    TorrentPreviewableFile file;

    QVERIFY_EXCEPTION_THROWN(file = torrent->torrentFiles()->create({
        {"file_index", 1},
        {"filepath", "test1_file1.mkv"},
        {"size", 377477},
        {"progress", 222},
        {"note", "relation's save fail"},
    }), QueryError);

    QVERIFY(!file.exists);
    QVERIFY(file.getAttributes().isEmpty());
}

void tst_Relations_Inserting_Updating::createMany_OnHasOneOrMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVector<AttributeItem> file1Attributes({
        {"file_index", 3},
        {"filepath", "test5_file4-createMany.mkv"},
        {"size", 322322},
        {"progress", 777},
        {"note", "relation's createMany file1"},
    });
    QVector<AttributeItem> file2Attributes({
        {"file_index", 4},
        {"filepath", "test5_file5-createMany.mkv"},
        {"size", 333322},
        {"progress", 888},
        {"note", "relation's createMany file2"},
    });

    QVector<QVector<AttributeItem>> fileAttributesToSave {
        std::move(file1Attributes),
        std::move(file2Attributes),
    };

    auto savedFiles = torrent->torrentFiles()->createMany(fileAttributesToSave);
    QCOMPARE(savedFiles.size(), 2);

    auto &savedFile1 = savedFiles[0];
    auto &savedFile2 = savedFiles[1];
    QVERIFY(savedFile1.exists);
    QVERIFY(savedFile2.exists);
    QVERIFY(savedFile1["id"].isValid());
    QVERIFY(savedFile2["id"].isValid());
    QVERIFY(savedFile1["id"].toULongLong() > 8);
    QVERIFY(savedFile2["id"].toULongLong() > 9);

    // Obtain files and verify saved values
    auto file1Verify = TorrentPreviewableFile::find(savedFile1["id"]);
    QCOMPARE((*file1Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file1Verify)["file_index"], QVariant(3));
    QCOMPARE((*file1Verify)["filepath"],   QVariant("test5_file4-createMany.mkv"));
    QCOMPARE((*file1Verify)["size"],       QVariant(322322));
    QCOMPARE((*file1Verify)["progress"],   QVariant(777));
    QCOMPARE((*file1Verify)["note"],       QVariant("relation's createMany file1"));
    auto file2Verify = TorrentPreviewableFile::find(savedFile2["id"]);
    QCOMPARE((*file2Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file2Verify)["file_index"], QVariant(4));
    QCOMPARE((*file2Verify)["filepath"],   QVariant("test5_file5-createMany.mkv"));
    QCOMPARE((*file2Verify)["size"],       QVariant(333322));
    QCOMPARE((*file2Verify)["progress"],   QVariant(888));
    QCOMPARE((*file2Verify)["note"],       QVariant("relation's createMany file2"));

    // Remove files, restore db
    const auto result1 = savedFile1.remove();
    QVERIFY(result1);
    QVERIFY(!savedFile1.exists);
    const auto result2 = savedFile2.remove();
    QVERIFY(result2);
    QVERIFY(!savedFile2.exists);
}

void tst_Relations_Inserting_Updating::createMany_OnHasOneOrMany_WithRValue() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto savedFiles = torrent->torrentFiles()->createMany({{
        {"file_index", 3},
        {"filepath", "test5_file4-createMany.mkv"},
        {"size", 322322},
        {"progress", 777},
        {"note", "relation's createMany file1"},
    }, {
        {"file_index", 4},
        {"filepath", "test5_file5-createMany.mkv"},
        {"size", 333322},
        {"progress", 888},
        {"note", "relation's createMany file2"},
    }});
    QCOMPARE(savedFiles.size(), 2);

    auto &savedFile1 = savedFiles[0];
    auto &savedFile2 = savedFiles[1];
    QVERIFY(savedFile1.exists);
    QVERIFY(savedFile2.exists);
    QVERIFY(savedFile1["id"].isValid());
    QVERIFY(savedFile2["id"].isValid());
    QVERIFY(savedFile1["id"].toULongLong() > 8);
    QVERIFY(savedFile2["id"].toULongLong() > 9);

    // Obtain files and verify saved values
    auto file1Verify = TorrentPreviewableFile::find(savedFile1["id"]);
    QCOMPARE((*file1Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file1Verify)["file_index"], QVariant(3));
    QCOMPARE((*file1Verify)["filepath"],   QVariant("test5_file4-createMany.mkv"));
    QCOMPARE((*file1Verify)["size"],       QVariant(322322));
    QCOMPARE((*file1Verify)["progress"],   QVariant(777));
    QCOMPARE((*file1Verify)["note"],       QVariant("relation's createMany file1"));
    auto file2Verify = TorrentPreviewableFile::find(savedFile2["id"]);
    QCOMPARE((*file2Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file2Verify)["file_index"], QVariant(4));
    QCOMPARE((*file2Verify)["filepath"],   QVariant("test5_file5-createMany.mkv"));
    QCOMPARE((*file2Verify)["size"],       QVariant(333322));
    QCOMPARE((*file2Verify)["progress"],   QVariant(888));
    QCOMPARE((*file2Verify)["note"],       QVariant("relation's createMany file2"));

    // Remove files, restore db
    const auto result1 = savedFile1.remove();
    QVERIFY(result1);
    QVERIFY(!savedFile1.exists);
    const auto result2 = savedFile2.remove();
    QVERIFY(result2);
    QVERIFY(!savedFile2.exists);
}

void tst_Relations_Inserting_Updating::createMany_OnHasOneOrMany_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVector<AttributeItem> file1Attributes({
        {"file_index", 1},
        {"filepath", "test1_file1.mkv"},
        {"size", 377477},
        {"progress", 222},
        {"note", "relation's save fail"},
    });
    // Make a copy is enough
    auto file2Attributes = file1Attributes;

    QVector<QVector<AttributeItem>> fileAttributesToSave {
        std::move(file1Attributes),
        std::move(file2Attributes),
    };

    QVector<TorrentPreviewableFile> savedFiles;
    QVERIFY_EXCEPTION_THROWN(savedFiles = torrent->torrentFiles()
                                          ->createMany(fileAttributesToSave),
                             QueryError);
    QVERIFY(savedFiles.isEmpty());
}

void
tst_Relations_Inserting_Updating::createMany_OnHasOneOrMany_WithRValue_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVector<TorrentPreviewableFile> savedFiles;
    QVERIFY_EXCEPTION_THROWN(
                savedFiles = torrent->torrentFiles()->createMany({{
                    {"file_index", 1},
                    {"filepath", "test1_file1.mkv"},
                    {"size", 377477},
                    {"progress", 222},
                    {"note", "relation's save fail"},
                },
                /* It doesn't matter what is in this second vector, because after
                   the first attempt to save the exception will be thrown. */
                {
                    {"file_index", 1},
                    {"filepath", "test1_file1.mkv"},
                    {"size", 377477},
                    {"progress", 222},
                    {"note", "relation's save fail"},
                }}),
                QueryError);
    QVERIFY(savedFiles.isEmpty());
}

QTEST_MAIN(tst_Relations_Inserting_Updating)

#include "tst_relations_inserting_updating.moc"
