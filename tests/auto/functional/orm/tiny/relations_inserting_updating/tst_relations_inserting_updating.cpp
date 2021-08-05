#include <QCoreApplication>
#include <QtTest>

#include <typeinfo>

#include "models/torrent.hpp"
#include "models/torrentpreviewablefile.hpp"

#include "databases.hpp"

using Orm::AttributeItem;
using Orm::Exceptions::QueryError;
using Orm::One;
using Orm::Tiny::ConnectionOverride;

using TestUtils::Databases;

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

    void save_OnBelongsToMany() const;
    void save_OnBelongsToMany_WithRValue() const;
    void save_OnBelongsToMany_Failed() const;

    void saveMany_OnBelongsToMany() const;
    void saveMany_OnBelongsToMany_WithRValue() const;
    void saveMany_OnBelongsToMany_Failed() const;

    void create_OnBelongsToMany() const;
    void create_OnBelongsToMany_WithRValue() const;
    void create_OnBelongsToMany_Failed() const;
    void create_OnBelongsToMany_WithRValue_Failed() const;

    void createMany_OnBelongsToMany() const;
    void createMany_OnBelongsToMany_WithRValue() const;
    void createMany_OnBelongsToMany_Failed() const;
    void createMany_OnBelongsToMany_WithRValue_Failed() const;

    void associate_WithModel() const;
    void associate_WithId() const;
    void associate_WithId_ShouldUnsetRelation() const;

    void dissociate() const;

    void attach_BasicPivot_WithIds() const;
    void attach_BasicPivot_WithModels() const;
    void attach_CustomPivot_WithIds() const;
    void attach_CustomPivot_WithModels() const;
    void attach_BasicPivot_IdsWithAttributes() const;
    void attach_CustomPivot_IdsWithAttributes() const;

    void detach_BasicPivot_WithIds() const;
    void detach_BasicPivot_WithModels() const;
    void detach_BasicPivot_All() const;
    void detach_CustomPivot_WithIds() const;
    void detach_CustomPivot_WithModels() const;
    void detach_CustomPivot_All() const;

    void updateExistingPivot_BasicPivot_WithId() const;
    void updateExistingPivot_BasicPivot_WithModel() const;
    void updateExistingPivot_CustomPivot_WithId() const;
    void updateExistingPivot_CustomPivot_WithModel() const;

    void sync_BasicPivot_WithIds() const;
    void sync_BasicPivot_IdsWithAttributes() const;
    void sync_CustomPivot_WithIds() const;
    void sync_CustomPivot_IdsWithAttributes() const;

    void syncWithoutDetaching_BasicPivot_WithIds() const;
    void syncWithoutDetaching_BasicPivot_IdsWithAttributes() const;
    void syncWithoutDetaching_CustomPivot_WithIds() const;
    void syncWithoutDetaching_CustomPivot_IdsWithAttributes() const;
};

void tst_Relations_Inserting_Updating::initTestCase_data() const
{
    const auto &connections = Databases::createConnections();

    if (connections.isEmpty())
        QSKIP(QStringLiteral("%1 autotest skipped, environment variables "
                             "for ANY connection have not been defined.")
              .arg("tst_Relations_Inserting_Updating").toUtf8().constData(), );

    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : connections)
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
    QVERIFY(file[ID]->isValid());
    QVERIFY(file[ID]->value<quint64>() > 8);

    // save method have to return the same model as a reference
    QVERIFY(reinterpret_cast<uintptr_t>(&file)
            == reinterpret_cast<uintptr_t>(&fileRef));

    // Obtain file and verify saved values
    auto fileVerify = TorrentPreviewableFile::find(file[ID]);
    QCOMPARE((*fileVerify)[ID],         QVariant(file[ID]));
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

    auto [savedResult, file] = torrent->torrentFiles()->save({
        {"file_index", 3},
        {"filepath", "test5_file4-save.mkv"},
        {"size", 322322},
        {"progress", 777},
        {"note", "relation's save"},
    });
    QVERIFY(savedResult);
    QVERIFY(file.exists);
    QVERIFY(file[ID]->isValid());
    QVERIFY(file[ID]->value<quint64>() > 8);

    // Obtain file and verify saved values
    auto fileVerify = TorrentPreviewableFile::find(file[ID]);
    QCOMPARE((*fileVerify)[ID],         QVariant(file[ID]));
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
    QVERIFY(!file[ID]->isValid());
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
    QVERIFY(savedFile1[ID]->isValid());
    QVERIFY(savedFile2[ID]->isValid());
    QVERIFY(savedFile1[ID]->value<quint64>() > 8);
    QVERIFY(savedFile2[ID]->value<quint64>() > 9);

    // saveMany() have to return reference to the same 'models' vector
    QVERIFY(reinterpret_cast<uintptr_t>(&filesToSave[0])
            != reinterpret_cast<uintptr_t>(&savedFile1));
    QVERIFY(reinterpret_cast<uintptr_t>(&filesToSave[1])
            != reinterpret_cast<uintptr_t>(&savedFile2));

    // Obtain files and verify saved values
    auto file1Verify = TorrentPreviewableFile::find(savedFile1[ID]);
    QCOMPARE((*file1Verify)[ID],         QVariant(savedFile1[ID]));
    QCOMPARE((*file1Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file1Verify)["file_index"], QVariant(3));
    QCOMPARE((*file1Verify)["filepath"],   QVariant("test5_file4-saveMany.mkv"));
    QCOMPARE((*file1Verify)["size"],       QVariant(322322));
    QCOMPARE((*file1Verify)["progress"],   QVariant(777));
    QCOMPARE((*file1Verify)["note"],       QVariant("relation's saveMany file1"));
    auto file2Verify = TorrentPreviewableFile::find(savedFile2[ID]);
    QCOMPARE((*file2Verify)[ID],         QVariant(savedFile2[ID]));
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
    QVERIFY(savedFile1[ID]->isValid());
    QVERIFY(savedFile2[ID]->isValid());
    QVERIFY(savedFile1[ID]->value<quint64>() > 8);
    QVERIFY(savedFile2[ID]->value<quint64>() > 9);

    // Obtain files and verify saved values
    auto file1Verify = TorrentPreviewableFile::find(savedFile1[ID]);
    QCOMPARE((*file1Verify)[ID],         QVariant(savedFile1[ID]));
    QCOMPARE((*file1Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file1Verify)["file_index"], QVariant(3));
    QCOMPARE((*file1Verify)["filepath"],   QVariant("test5_file4-saveMany.mkv"));
    QCOMPARE((*file1Verify)["size"],       QVariant(322322));
    QCOMPARE((*file1Verify)["progress"],   QVariant(777));
    QCOMPARE((*file1Verify)["note"],       QVariant("relation's saveMany file1"));
    auto file2Verify = TorrentPreviewableFile::find(savedFile2[ID]);
    QCOMPARE((*file2Verify)[ID],         QVariant(savedFile2[ID]));
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
    QVERIFY(file[ID]->isValid());
    QVERIFY(file[ID]->value<quint64>() > 8);

    // Obtain file and verify saved values
    auto fileVerify = TorrentPreviewableFile::find(file[ID]);
    QCOMPARE((*fileVerify)[ID],         QVariant(file[ID]));
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
    QVERIFY(file[ID]->isValid());
    QVERIFY(file[ID]->value<quint64>() > 8);

    // Obtain file and verify saved values
    auto fileVerify = TorrentPreviewableFile::find(file[ID]);
    QCOMPARE((*fileVerify)[ID],         QVariant(file[ID]));
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
    QVERIFY(savedFile1[ID]->isValid());
    QVERIFY(savedFile2[ID]->isValid());
    QVERIFY(savedFile1[ID]->value<quint64>() > 8);
    QVERIFY(savedFile2[ID]->value<quint64>() > 9);

    // Obtain files and verify saved values
    auto file1Verify = TorrentPreviewableFile::find(savedFile1[ID]);
    QCOMPARE((*file1Verify)[ID],         QVariant(savedFile1[ID]));
    QCOMPARE((*file1Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file1Verify)["file_index"], QVariant(3));
    QCOMPARE((*file1Verify)["filepath"],   QVariant("test5_file4-createMany.mkv"));
    QCOMPARE((*file1Verify)["size"],       QVariant(322322));
    QCOMPARE((*file1Verify)["progress"],   QVariant(777));
    QCOMPARE((*file1Verify)["note"],       QVariant("relation's createMany file1"));
    auto file2Verify = TorrentPreviewableFile::find(savedFile2[ID]);
    QCOMPARE((*file2Verify)[ID],         QVariant(savedFile2[ID]));
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
    QVERIFY(savedFile1[ID]->isValid());
    QVERIFY(savedFile2[ID]->isValid());
    QVERIFY(savedFile1[ID]->value<quint64>() > 8);
    QVERIFY(savedFile2[ID]->value<quint64>() > 9);

    // Obtain files and verify saved values
    auto file1Verify = TorrentPreviewableFile::find(savedFile1[ID]);
    QCOMPARE((*file1Verify)[ID],         QVariant(savedFile1[ID]));
    QCOMPARE((*file1Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file1Verify)["file_index"], QVariant(3));
    QCOMPARE((*file1Verify)["filepath"],   QVariant("test5_file4-createMany.mkv"));
    QCOMPARE((*file1Verify)["size"],       QVariant(322322));
    QCOMPARE((*file1Verify)["progress"],   QVariant(777));
    QCOMPARE((*file1Verify)["note"],       QVariant("relation's createMany file1"));
    auto file2Verify = TorrentPreviewableFile::find(savedFile2[ID]);
    QCOMPARE((*file2Verify)[ID],         QVariant(savedFile2[ID]));
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

void tst_Relations_Inserting_Updating::save_OnBelongsToMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);

    Tag tag({{NAME, "tag save"}});
    QVERIFY(!tag.exists);

    auto [savedResult, tagRef] = torrent->tags()->save(tag, {{"active", false}});
    QVERIFY(savedResult);
    QVERIFY(tag.exists);
    QVERIFY(tag[ID]->isValid());
    QVERIFY(tag[ID]->value<quint64>() > 4);

    // Check records count in the pivot (tagged) table
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 1);

    // save method have to return the same model as a reference
    QVERIFY(reinterpret_cast<uintptr_t>(&tag)
            == reinterpret_cast<uintptr_t>(&tagRef));

    // Obtain tag and verify saved values
    auto tagVerify = Tag::find(tag[ID]);
    QCOMPARE((*tagVerify)[ID],   QVariant(tag[ID]));
    QCOMPARE((*tagVerify)[NAME], QVariant("tag save"));

    // Verify pivot attributes
    auto taggedVerify = Tagged::whereEq("torrent_id", 5)->first();
    QCOMPARE((*taggedVerify)["tag_id"], QVariant(tag[ID]));
    QCOMPARE((*taggedVerify)["active"], QVariant(0));

    /* Remove tag, restore db, pivot record will be deleted by ON DELETE CASCADE
       constraint. */
    const auto result = tag.remove();
    QVERIFY(result);
    QVERIFY(!tag.exists);

    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::save_OnBelongsToMany_WithRValue() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);

    auto [savedResult, tag] = torrent->tags()->save({{NAME, "tag save"}},
                                                     {{"active", false}});
    QVERIFY(savedResult);
    QVERIFY(tag.exists);
    QVERIFY(tag[ID]->isValid());
    QVERIFY(tag[ID]->value<quint64>() > 4);

    // Check records count in the pivot (tagged) table
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 1);

    // Obtain tag and verify saved values
    auto tagVerify = Tag::find(tag[ID]);
    QCOMPARE((*tagVerify)[ID],   QVariant(tag[ID]));
    QCOMPARE((*tagVerify)[NAME], QVariant("tag save"));

    // Verify pivot attributes
    auto taggedVerify = Tagged::whereEq("torrent_id", 5)->first();
    QCOMPARE((*taggedVerify)["tag_id"], QVariant(tag[ID]));
    QCOMPARE((*taggedVerify)["active"], QVariant(0));

    /* Remove tag, restore db, pivot record will be deleted by ON DELETE CASCADE
       constraint. */
    const auto result = tag.remove();
    QVERIFY(result);
    QVERIFY(!tag.exists);

    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::save_OnBelongsToMany_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);

    Tag tag({{NAME, "tag1"}});
    QVERIFY(!tag.exists);

    QVERIFY_EXCEPTION_THROWN(torrent->tags()->save(tag),
                             QueryError);
    QVERIFY(!tag.exists);
    QVERIFY(!tag[ID]->isValid());

    size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::saveMany_OnBelongsToMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);

    Tag tag1({{NAME, "tag1 save"}});
    Tag tag2({{NAME, "tag2 save"}});
    QVERIFY(!tag1.exists);
    QVERIFY(!tag2.exists);

    QVector<Tag> tagsToSave {std::move(tag1), std::move(tag2)};

    auto &savedTags = torrent->tags()->saveMany(tagsToSave, {{}, {{"active", false}}});
    QCOMPARE(savedTags.size(), 2);

    auto &savedTag1 = savedTags[0];
    auto &savedTag2 = savedTags[1];
    QVERIFY(savedTag1.exists);
    QVERIFY(savedTag1[ID]->isValid());
    QVERIFY(savedTag1[ID]->value<quint64>() > 4);
    QVERIFY(savedTag2.exists);
    QVERIFY(savedTag2[ID]->isValid());
    QVERIFY(savedTag2[ID]->value<quint64>() > 4);

    size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 2);
    // Check records size in the pivot table
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 2);

    // saveMany method have to return the same models as a reference_wrapper
    QVERIFY(reinterpret_cast<uintptr_t>(&tagsToSave[0])
            == reinterpret_cast<uintptr_t>(&savedTag1));
    QVERIFY(reinterpret_cast<uintptr_t>(&tagsToSave[1])
            == reinterpret_cast<uintptr_t>(&savedTag2));

    // Obtain tag and verify saved values
    auto tag1Verify = Tag::find(savedTag1[ID]);
    QCOMPARE((*tag1Verify)[ID],   QVariant(savedTag1[ID]));
    QCOMPARE((*tag1Verify)[NAME], QVariant("tag1 save"));
    auto tag2Verify = Tag::find(savedTag2[ID]);
    QCOMPARE((*tag2Verify)[ID],   QVariant(savedTag2[ID]));
    QCOMPARE((*tag2Verify)[NAME], QVariant("tag2 save"));

    // Verify pivot attributes
    auto taggedVerify = Tagged::whereEq("torrent_id", 5)->get();
    QCOMPARE(taggedVerify[0]["tag_id"], QVariant(savedTag1[ID]));
    QCOMPARE(taggedVerify[0]["active"], QVariant(1));
    QCOMPARE(taggedVerify[1]["tag_id"], QVariant(savedTag2[ID]));
    QCOMPARE(taggedVerify[1]["active"], QVariant(0));

    /* Remove tag, restore db, pivot record will be deleted by ON DELETE CASCADE
       constraint. */
    auto result = savedTag1.remove();
    QVERIFY(result);
    QVERIFY(!savedTag1.exists);
    result = savedTag2.remove();
    QVERIFY(result);
    QVERIFY(!savedTag2.exists);

    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::saveMany_OnBelongsToMany_WithRValue() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);

    auto savedTags = torrent->tags()->saveMany({{{NAME, "tag1 save"}},
                                                {{NAME, "tag2 save"}}},
                                               {{{"active", false}}});
    QCOMPARE(savedTags.size(), 2);

    auto &savedTag1 = savedTags[0];
    auto &savedTag2 = savedTags[1];
    QVERIFY(savedTag1.exists);
    QVERIFY(savedTag1[ID]->isValid());
    QVERIFY(savedTag1[ID]->value<quint64>() > 4);
    QVERIFY(savedTag2.exists);
    QVERIFY(savedTag2[ID]->isValid());
    QVERIFY(savedTag2[ID]->value<quint64>() > 4);

    size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 2);
    // Check records size in the pivot table
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 2);

    // Obtain tag and verify saved values
    auto tag1Verify = Tag::find(savedTag1[ID]);
    QCOMPARE((*tag1Verify)[ID],   QVariant(savedTag1[ID]));
    QCOMPARE((*tag1Verify)[NAME], QVariant("tag1 save"));
    auto tag2Verify = Tag::find(savedTag2[ID]);
    QCOMPARE((*tag2Verify)[ID],   QVariant(savedTag2[ID]));
    QCOMPARE((*tag2Verify)[NAME], QVariant("tag2 save"));

    // Verify pivot attributes
    auto taggedVerify = Tagged::whereEq("torrent_id", 5)->get();
    QCOMPARE(taggedVerify[0]["tag_id"], QVariant(savedTag1[ID]));
    QCOMPARE(taggedVerify[0]["active"], QVariant(0));
    QCOMPARE(taggedVerify[1]["tag_id"], QVariant(savedTag2[ID]));
    QCOMPARE(taggedVerify[1]["active"], QVariant(1));

    /* Remove tag, restore db, pivot record will be deleted by ON DELETE CASCADE
       constraint. */
    auto result = savedTag1.remove();
    QVERIFY(result);
    QVERIFY(!savedTag1.exists);
    result = savedTag2.remove();
    QVERIFY(result);
    QVERIFY(!savedTag2.exists);

    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::saveMany_OnBelongsToMany_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);

    Tag tag1({{NAME, "tag1"}});
    QVERIFY(!tag1.exists);
    // Make a copy is enough
    auto tag2 = tag1;
    QVERIFY(!tag2.exists);

    QVector<Tag> tagsToSave {std::move(tag1), std::move(tag2)};
    QVector<Tag> savedTags;
    QVERIFY_EXCEPTION_THROWN(savedTags = torrent->tags()->saveMany(tagsToSave),
                             QueryError);
    QVERIFY(savedTags.isEmpty());

    size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::create_OnBelongsToMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);

    QVector<AttributeItem> tagAttribtues {{NAME, "tag create"}};

    auto tag = torrent->tags()->create(tagAttribtues, {{"active", false}});
    QVERIFY(tag.exists);
    QVERIFY(tag[ID]->isValid());
    QVERIFY(tag[ID]->value<quint64>() > 4);

    // Check records count in the pivot (tagged) table
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 1);

    // Obtain tag and verify saved values
    auto tagVerify = Tag::find(tag[ID]);
    QCOMPARE((*tagVerify)[ID],   QVariant(tag[ID]));
    QCOMPARE((*tagVerify)[NAME], QVariant("tag create"));

    // Verify pivot attributes
    auto taggedVerify = Tagged::whereEq("torrent_id", 5)->first();
    QCOMPARE((*taggedVerify)["tag_id"], QVariant(tag[ID]));
    QCOMPARE((*taggedVerify)["active"], QVariant(0));

    /* Remove tag, restore db, pivot record will be deleted by ON DELETE CASCADE
       constraint. */
    const auto result = tag.remove();
    QVERIFY(result);
    QVERIFY(!tag.exists);

    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::create_OnBelongsToMany_WithRValue() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);

    auto tag = torrent->tags()->create({{NAME, "tag create rvalue"}},
                                       {{"active", false}});
    QVERIFY(tag.exists);
    QVERIFY(tag[ID]->isValid());
    QVERIFY(tag[ID]->value<quint64>() > 4);

    // Check records count in the pivot (tagged) table
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 1);

    // Obtain tag and verify saved values
    auto tagVerify = Tag::find(tag[ID]);
    QCOMPARE((*tagVerify)[ID],   QVariant(tag[ID]));
    QCOMPARE((*tagVerify)[NAME], QVariant("tag create rvalue"));

    // Verify pivot attributes
    auto taggedVerify = Tagged::whereEq("torrent_id", 5)->first();
    QCOMPARE((*taggedVerify)["tag_id"], QVariant(tag[ID]));
    QCOMPARE((*taggedVerify)["active"], QVariant(0));

    /* Remove tag, restore db, pivot record will be deleted by ON DELETE CASCADE
       constraint. */
    const auto result = tag.remove();
    QVERIFY(result);
    QVERIFY(!tag.exists);

    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::create_OnBelongsToMany_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);

    QVector<AttributeItem> tagAttributes {{NAME, "tag1"}};
    Tag tag;

    QVERIFY_EXCEPTION_THROWN(tag = torrent->tags()->create(tagAttributes),
                             QueryError);
    QVERIFY(!tag.exists);
    QVERIFY(tag.getAttributes().isEmpty());

    size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::create_OnBelongsToMany_WithRValue_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);

    Tag tag;

    QVERIFY_EXCEPTION_THROWN(tag = torrent->tags()->create({{NAME, "tag1"}}),
                             QueryError);
    QVERIFY(!tag.exists);
    QVERIFY(tag.getAttributes().isEmpty());

    size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::createMany_OnBelongsToMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);

    QVector<QVector<AttributeItem>> tagAttribtues {{{NAME, "tag create 1"}},
                                                   {{NAME, "tag create 2"}}};

    auto tags = torrent->tags()->createMany(tagAttribtues, {{}, {{"active", false}}});
    QCOMPARE(tags.size(), 2);

    auto &tag1 = tags[0];
    auto &tag2 = tags[1];
    QVERIFY(tag1.exists);
    QVERIFY(tag2.exists);
    QVERIFY(tag1[ID]->isValid());
    QVERIFY(tag2[ID]->isValid());
    QVERIFY(tag1[ID]->value<quint64>() > 4);
    QVERIFY(tag2[ID]->value<quint64>() > 4);

    // Check records count in the pivot (tagged) table
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 2);

    // Obtain tag and verify saved values
    auto tagVerify1 = Tag::find(tag1[ID]);
    QCOMPARE((*tagVerify1)[ID],   QVariant(tag1[ID]));
    QCOMPARE((*tagVerify1)[NAME], QVariant("tag create 1"));
    auto tagVerify2 = Tag::find(tag2[ID]);
    QCOMPARE((*tagVerify2)[ID],   QVariant(tag2[ID]));
    QCOMPARE((*tagVerify2)[NAME], QVariant("tag create 2"));

    // Verify pivot attributes
    auto taggedVerify = Tagged::whereEq("torrent_id", 5)->get();
    QCOMPARE(taggedVerify[0]["tag_id"], QVariant(tag1[ID]));
    QCOMPARE(taggedVerify[0]["active"], QVariant(1));
    QCOMPARE(taggedVerify[1]["tag_id"], QVariant(tag2[ID]));
    QCOMPARE(taggedVerify[1]["active"], QVariant(0));

    /* Remove tag, restore db, pivot record will be deleted by ON DELETE CASCADE
       constraint. */
    const auto result1 = tag1.remove();
    QVERIFY(result1);
    QVERIFY(!tag1.exists);
    const auto result2 = tag2.remove();
    QVERIFY(result2);
    QVERIFY(!tag2.exists);

    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::createMany_OnBelongsToMany_WithRValue() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);

    auto tags = torrent->tags()->createMany({{{NAME, "tag create 1 rvalue"}},
                                             {{NAME, "tag create 2 rvalue"}}},

                                            {{{"active", false}}});
    QCOMPARE(tags.size(), 2);

    auto &tag1 = tags[0];
    auto &tag2 = tags[1];
    QVERIFY(tag1.exists);
    QVERIFY(tag2.exists);
    QVERIFY(tag1[ID]->isValid());
    QVERIFY(tag2[ID]->isValid());
    QVERIFY(tag1[ID]->value<quint64>() > 4);
    QVERIFY(tag2[ID]->value<quint64>() > 4);

    // Check records count in the pivot (tagged) table
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 2);

    // Obtain tag and verify saved values
    auto tagVerify1 = Tag::find(tag1[ID]);
    QCOMPARE((*tagVerify1)[ID],   QVariant(tag1[ID]));
    QCOMPARE((*tagVerify1)[NAME], QVariant("tag create 1 rvalue"));
    auto tagVerify2 = Tag::find(tag2[ID]);
    QCOMPARE((*tagVerify2)[ID],   QVariant(tag2[ID]));
    QCOMPARE((*tagVerify2)[NAME], QVariant("tag create 2 rvalue"));

    // Verify pivot attributes
    auto taggedVerify = Tagged::whereEq("torrent_id", 5)->get();
    QCOMPARE(taggedVerify[0]["tag_id"], QVariant(tag1[ID]));
    QCOMPARE(taggedVerify[0]["active"], QVariant(0));
    QCOMPARE(taggedVerify[1]["tag_id"], QVariant(tag2[ID]));
    QCOMPARE(taggedVerify[1]["active"], QVariant(1));

    /* Remove tag, restore db, pivot record will be deleted by ON DELETE CASCADE
       constraint. */
    const auto result1 = tag1.remove();
    QVERIFY(result1);
    QVERIFY(!tag1.exists);
    const auto result2 = tag2.remove();
    QVERIFY(result2);
    QVERIFY(!tag2.exists);

    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::createMany_OnBelongsToMany_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);

    QVector<QVector<AttributeItem>> tagAttribtues {{{NAME, "tag1"}},
                                                   {{NAME, "tag1"}}};

    QVector<Tag> tags;

    QVERIFY_EXCEPTION_THROWN(tags = torrent->tags()->createMany(tagAttribtues,
                                                                {{{"active", false}}}),
                             QueryError);
    QVERIFY(tags.isEmpty());

    size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void
tst_Relations_Inserting_Updating::createMany_OnBelongsToMany_WithRValue_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);

    QVector<Tag> tags;

    QVERIFY_EXCEPTION_THROWN(tags = torrent->tags()->createMany({{{NAME, "tag1"}},
                                                                 {{NAME, "tag1"}}},

                                                                {{{"active", false}}}),
                             QueryError);
    QVERIFY(tags.isEmpty());

    size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::associate_WithModel() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    // Create new torrent file for this test
    TorrentPreviewableFile file {
        {"file_index", 3},
        {"filepath", "test5_file4.mkv"},
        {"size", 3255},
        {"progress", 115},
        {"note", "associate"},
    };

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    const auto &fileRelations = file.getRelations();
    QCOMPARE(fileRelations.size(), static_cast<std::size_t>(0));

    auto &fileRef = file.torrent()->associate(*torrent);

    // associate method have to return the same model as a reference
    QVERIFY(reinterpret_cast<uintptr_t>(&file)
            == reinterpret_cast<uintptr_t>(&fileRef));

    const auto &torrentForeignKeyName = torrent->getForeignKey();
    QVERIFY(file.getAttributesHash().contains(torrentForeignKeyName));
    QCOMPARE(file[torrentForeignKeyName], torrent->getAttribute(ID));

    // Copy of the associated model have to be set on the file
    QCOMPARE(fileRelations.size(), static_cast<std::size_t>(1));
    QVERIFY(fileRelations.contains(file.torrent()->getRelationName()));

    auto *verifyTorrent5 = file.getRelation<Torrent, One>("torrent");
    QVERIFY(verifyTorrent5);
    QVERIFY(verifyTorrent5->exists);
    QCOMPARE(typeid (Torrent *), typeid (verifyTorrent5));
    QVERIFY(verifyTorrent5->is(torrent));
    // TEST ideal place for comparing models, verifyTorrent5 == torrent, have to have the same attribtues silverqx

    file.save();

    // Little useless, but will be absolutely sure that nothing changed
    verifyTorrent5 = file.getRelation<Torrent, One>("torrent");
    QVERIFY(verifyTorrent5);
    QVERIFY(verifyTorrent5->exists);
    QCOMPARE(typeid (Torrent *), typeid (verifyTorrent5));
    QVERIFY(verifyTorrent5->is(torrent));
    // TEST ideal place for comparing models, verifyTorrent5 == torrent, have to have the same attribtues silverqx

    // Obtain file from the database and verify saved values
    auto verifyFile = TorrentPreviewableFile::find(file[ID]);
    QCOMPARE((*verifyFile)[ID], QVariant(file[ID]));
    QCOMPARE((*verifyFile)["filepath"], file["filepath"].value());
    QCOMPARE((*verifyFile)[torrentForeignKeyName], file[torrentForeignKeyName].value());

    // Restore db
    fileRef.remove();
}

void tst_Relations_Inserting_Updating::associate_WithId() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    // Create new torrent file for this test
    TorrentPreviewableFile file {
        {"file_index", 3},
        {"filepath", "test5_file4.mkv"},
        {"size", 3255},
        {"progress", 115},
        {"note", "associate"},
    };

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    const auto &fileRelations = file.getRelations();
    QCOMPARE(fileRelations.size(), static_cast<std::size_t>(0));

    auto &fileRef = file.torrent()->associate((*torrent)[ID]);

    // associate method have to return the same model as a reference
    QVERIFY(reinterpret_cast<uintptr_t>(&file)
            == reinterpret_cast<uintptr_t>(&fileRef));

    const auto &torrentForeignKeyName = torrent->getForeignKey();
    QVERIFY(file.getAttributesHash().contains(torrentForeignKeyName));
    QCOMPARE(file[torrentForeignKeyName], torrent->getAttribute(ID));

    // This is only difference, associate with Id should unset relation model
    QCOMPARE(fileRelations.size(), static_cast<std::size_t>(0));
    QVERIFY(!fileRelations.contains(file.torrent()->getRelationName()));

    file.save();

    // Obtain file from the database and verify saved values
    auto verifyFile = TorrentPreviewableFile::find(file[ID]);
    QCOMPARE((*verifyFile)[ID], QVariant(file[ID]));
    QCOMPARE((*verifyFile)["filepath"], file["filepath"].value());
    QCOMPARE((*verifyFile)[torrentForeignKeyName], file[torrentForeignKeyName].value());

    // Restore db
    fileRef.remove();
}

void tst_Relations_Inserting_Updating::associate_WithId_ShouldUnsetRelation() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    // Create new torrent file for this test
    TorrentPreviewableFile file {
        {"file_index", 3},
        {"filepath", "test5_file4.mkv"},
        {"size", 3255},
        {"progress", 115},
        {"note", "associate"},
    };

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    const auto &fileRelations = file.getRelations();
    QCOMPARE(fileRelations.size(), static_cast<std::size_t>(0));

    auto &fileRef = file.torrent()->associate(*torrent);

    // associate method have to return the same model as a reference
    QVERIFY(reinterpret_cast<uintptr_t>(&file)
            == reinterpret_cast<uintptr_t>(&fileRef));

    const auto &torrentForeignKeyName = torrent->getForeignKey();
    QVERIFY(file.getAttributesHash().contains(torrentForeignKeyName));
    QCOMPARE(file[torrentForeignKeyName], torrent->getAttribute(ID));

    // Copy of the associated model have to be set on the file
    QCOMPARE(fileRelations.size(), static_cast<std::size_t>(1));
    QVERIFY(fileRelations.contains(file.torrent()->getRelationName()));

    auto *verifyTorrent5 = file.getRelation<Torrent, One>("torrent");
    QVERIFY(verifyTorrent5);
    QVERIFY(verifyTorrent5->exists);
    QCOMPARE(typeid (Torrent *), typeid (verifyTorrent5));
    QVERIFY(verifyTorrent5->is(torrent));
    // TEST ideal place for comparing models, verifyTorrent5 == torrent, have to have the same attribtues silverqx

    /* Have to unset current relationship, this is clearly visible in the Eqloquent's
       associate implementation. */
    fileRef = file.torrent()->associate(2);

    QVERIFY(file.getAttributesHash().contains(torrentForeignKeyName));
    QCOMPARE(file[torrentForeignKeyName], QVariant(2));

    // This is only difference, associate with Id should unset relation model
    QCOMPARE(fileRelations.size(), static_cast<std::size_t>(0));
    QVERIFY(!fileRelations.contains(file.torrent()->getRelationName()));
}

void tst_Relations_Inserting_Updating::dissociate() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    // Create new torrent file for this test
    TorrentPreviewableFile file {
        {"file_index", 3},
        {"filepath", "test5_file4.mkv"},
        {"size", 3255},
        {"progress", 115},
        {"note", "associate"},
    };

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    const auto &fileRelations = file.getRelations();
    QCOMPARE(fileRelations.size(), static_cast<std::size_t>(0));

    auto &fileRef = file.torrent()->associate(*torrent);

    // associate method have to return the same model as a reference
    QVERIFY(reinterpret_cast<uintptr_t>(&file)
            == reinterpret_cast<uintptr_t>(&fileRef));

    const auto &torrentForeignKeyName = torrent->getForeignKey();
    QVERIFY(file.getAttributesHash().contains(torrentForeignKeyName));
    QCOMPARE(file[torrentForeignKeyName], torrent->getAttribute(ID));

    // Copy of the associated model have to be set on the file
    QCOMPARE(fileRelations.size(), static_cast<std::size_t>(1));
    QVERIFY(fileRelations.contains(file.torrent()->getRelationName()));

    auto *verifyTorrent5 = file.getRelation<Torrent, One>("torrent");
    QVERIFY(verifyTorrent5);
    QVERIFY(verifyTorrent5->exists);
    QCOMPARE(typeid (Torrent *), typeid (verifyTorrent5));
    QVERIFY(verifyTorrent5->is(torrent));

    auto &fileRefDissociate = file.torrent()->dissociate();

    // dissociate method have to return the same model as a reference
    QVERIFY(reinterpret_cast<uintptr_t>(&file)
            == reinterpret_cast<uintptr_t>(&fileRefDissociate));

    QVERIFY(file.getAttributesHash().contains(torrentForeignKeyName));
    QVERIFY(!file[torrentForeignKeyName].value().isValid());

    // Relation have to be set to std::nullopt internally
    QCOMPARE(fileRelations.size(), static_cast<std::size_t>(1));
    QVERIFY(fileRelations.contains(file.torrent()->getRelationName()));

    verifyTorrent5 = file.getRelation<Torrent, One>("torrent");
    QVERIFY(verifyTorrent5 == nullptr);
}

void tst_Relations_Inserting_Updating::attach_BasicPivot_WithIds() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {"size", 100}, {"progress", 555},
        {"hash", "xyzhash100"}, {"note", "attach with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {"size", 101}, {"progress", 556},
        {"hash", "xyzhash101"}, {"note", "attach with pivot"},
    };
    torrent101.save();

    auto tag4 = Tag::find(4);
    QVERIFY(tag4);
    QVERIFY(tag4->exists);

    tag4->torrents()->attach({torrent100[ID], torrent101[ID]},
                             {{"active", false}},
                             false);

    auto taggeds = Tagged::whereEq("tag_id", (*tag4)[ID])
            ->whereIn("torrent_id", {torrent100[ID], torrent101[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected torrent IDs
    QVector<QVariant> torrentIds {torrent100[ID], torrent101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(*tagged["tag_id"], (*tag4)[ID]);
        QVERIFY(torrentIds.contains(*tagged["torrent_id"]));
        QCOMPARE(*tagged["active"], QVariant(0));
    }

    // Restore db
    torrent100.remove();
    torrent101.remove();
}

void tst_Relations_Inserting_Updating::attach_BasicPivot_WithModels() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {"size", 100}, {"progress", 555},
        {"hash", "xyzhash100"}, {"note", "attach with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {"size", 101}, {"progress", 556},
        {"hash", "xyzhash101"}, {"note", "attach with pivot"},
    };
    torrent101.save();

    auto tag4 = Tag::find(4);
    QVERIFY(tag4);
    QVERIFY(tag4->exists);

    tag4->torrents()->attach({{torrent100}, {torrent101}},
                             {{"active", false}},
                             false);

    auto taggeds = Tagged::whereEq("tag_id", (*tag4)[ID])
            ->whereIn("torrent_id", {torrent100[ID], torrent101[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected torrent IDs
    QVector<QVariant> torrentIds {torrent100[ID], torrent101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["tag_id"].value(), (*tag4)[ID]);
        QVERIFY(torrentIds.contains(tagged["torrent_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(0));
    }

    // Restore db
    torrent100.remove();
    torrent101.remove();
}

void tst_Relations_Inserting_Updating::attach_CustomPivot_WithIds() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Tag tag100({{NAME, "tag100"}});
    tag100.save();
    Tag tag101({{NAME, "tag101"}});
    tag101.save();

    auto torrent5 = Torrent::find(5);
    QVERIFY(torrent5);
    QVERIFY(torrent5->exists);

    const auto torrent5Id = (*torrent5)[ID];

    torrent5->tags()->attach({tag100[ID], tag101[ID]},
                             {{"active", false}},
                             false);

    auto taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag100[ID], tag101[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected tag IDs
    QVector<QVariant> tagIds {tag100[ID], tag101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(tagIds.contains(tagged["tag_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(0));
    }

    // Restore db
    tag100.remove();
    tag101.remove();
}

void tst_Relations_Inserting_Updating::attach_CustomPivot_WithModels() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Tag tag100({{NAME, "tag100"}});
    tag100.save();
    Tag tag101({{NAME, "tag101"}});
    tag101.save();

    auto torrent5 = Torrent::find(5);
    QVERIFY(torrent5);
    QVERIFY(torrent5->exists);

    const auto torrent5Id = (*torrent5)[ID];

    torrent5->tags()->attach({{tag100}, {tag101}},
                             {{"active", false}},
                             false);

    auto taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag100[ID], tag101[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected tag IDs
    QVector<QVariant> tagIds {tag100[ID], tag101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(tagIds.contains(tagged["tag_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(0));
    }

    // Restore db
    tag100.remove();
    tag101.remove();
}

void tst_Relations_Inserting_Updating::attach_BasicPivot_IdsWithAttributes() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {"size", 100}, {"progress", 555},
        {"hash", "xyzhash100"}, {"note", "attach with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {"size", 101}, {"progress", 556},
        {"hash", "xyzhash101"}, {"note", "attach with pivot"},
    };
    torrent101.save();

    auto tag4 = Tag::find(4);
    QVERIFY(tag4);
    QVERIFY(tag4->exists);

    tag4->torrents()->attach({
        {torrent100[ID]->value<quint64>(), {{"active", false}}},
        {torrent101[ID]->value<quint64>(), {{"active", true}}},
    }, false);

    auto taggeds = Tagged::whereEq("tag_id", (*tag4)[ID])
            ->whereIn("torrent_id", {torrent100[ID], torrent101[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected active attribute values by the torrent ID
    std::unordered_map<quint64, bool> taggedActive {
        {torrent100[ID]->value<quint64>(), false},
        {torrent101[ID]->value<quint64>(), true},
    };

    quint64 torrentId = 0;

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        torrentId = tagged["torrent_id"]->value<quint64>();

        QCOMPARE(tagged["tag_id"].value(), (*tag4)[ID]);
        QVERIFY(taggedActive.contains(torrentId));
        QCOMPARE(tagged["active"].value(), QVariant(taggedActive.at(torrentId)));
    }

    // Restore db
    torrent100.remove();
    torrent101.remove();
}

void tst_Relations_Inserting_Updating::attach_CustomPivot_IdsWithAttributes() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Tag tag100({{NAME, "tag100"}});
    tag100.save();
    Tag tag101({{NAME, "tag101"}});
    tag101.save();

    auto torrent5 = Torrent::find(5);
    QVERIFY(torrent5);
    QVERIFY(torrent5->exists);

    const auto torrent5Id = (*torrent5)[ID];

    torrent5->tags()->attach({
        {tag100[ID]->value<quint64>(), {{"active", false}}},
        {tag101[ID]->value<quint64>(), {{"active", true}}}
    }, false);

    auto taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag100[ID], tag101[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected active attribute values by the tag ID
    std::unordered_map<quint64, int> taggedActive {
        {tag100[ID]->value<quint64>(), false},
        {tag101[ID]->value<quint64>(), true},
    };

    quint64 tagId = 0;

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        tagId = tagged["tag_id"]->value<quint64>();

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(taggedActive.contains(tagId));
        QCOMPARE(tagged["active"].value(), QVariant(taggedActive.at(tagId)));
    }

    // Restore db
    tag100.remove();
    tag101.remove();
}

void tst_Relations_Inserting_Updating::detach_BasicPivot_WithIds() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {"size", 100}, {"progress", 555},
        {"hash", "xyzhash100"}, {"note", "attach with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {"size", 101}, {"progress", 556},
        {"hash", "xyzhash101"}, {"note", "attach with pivot"},
    };
    torrent101.save();

    auto tag4 = Tag::find(4);
    QVERIFY(tag4);
    QVERIFY(tag4->exists);

    tag4->torrents()->attach({torrent100[ID], torrent101[ID]},
                             {{"active", false}},
                             false);

    auto taggeds = Tagged::whereEq("tag_id", (*tag4)[ID])
            ->whereIn("torrent_id", {torrent100[ID], torrent101[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected torrent IDs
    QVector<QVariant> torrentIds {torrent100[ID], torrent101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["tag_id"].value(), (*tag4)[ID]);
        QVERIFY(torrentIds.contains(tagged["torrent_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    auto affected = tag4->torrents()->detach({torrent100[ID], torrent101[ID]},
                                             false);

    QCOMPARE(affected, 2);

    // FEATURE aggregates, use count silverqx
    taggeds = Tagged::whereEq("tag_id", (*tag4)[ID])
            ->whereIn("torrent_id", {torrent100[ID], torrent101[ID]})
            .get({"torrent_id"});

    QCOMPARE(taggeds.size(), 0);

    // Restore db
    torrent100.remove();
    torrent101.remove();
}

void tst_Relations_Inserting_Updating::detach_BasicPivot_WithModels() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {"size", 100}, {"progress", 555},
        {"hash", "xyzhash100"}, {"note", "attach with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {"size", 101}, {"progress", 556},
        {"hash", "xyzhash101"}, {"note", "attach with pivot"},
    };
    torrent101.save();

    auto tag4 = Tag::find(4);
    QVERIFY(tag4);
    QVERIFY(tag4->exists);

    tag4->torrents()->attach({torrent100[ID], torrent101[ID]},
                             {{"active", false}},
                             false);

    auto taggeds = Tagged::whereEq("tag_id", (*tag4)[ID])
            ->whereIn("torrent_id", {torrent100[ID], torrent101[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected torrent IDs
    QVector<QVariant> torrentIds {torrent100[ID], torrent101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["tag_id"].value(), (*tag4)[ID]);
        QVERIFY(torrentIds.contains(tagged["torrent_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    auto affected = tag4->torrents()->detach({torrent100, torrent101}, false);

    QCOMPARE(affected, 2);

    // FEATURE aggregates, use count silverqx
    taggeds = Tagged::whereEq("tag_id", (*tag4)[ID])
            ->whereIn("torrent_id", {torrent100[ID], torrent101[ID]})
            .get({"torrent_id"});

    QCOMPARE(taggeds.size(), 0);

    // Restore db
    torrent100.remove();
    torrent101.remove();
}

void tst_Relations_Inserting_Updating::detach_BasicPivot_All() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {"size", 100}, {"progress", 555},
        {"hash", "xyzhash100"}, {"note", "attach with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {"size", 101}, {"progress", 556},
        {"hash", "xyzhash101"}, {"note", "attach with pivot"},
    };
    torrent101.save();

    auto tag5 = Tag::find(5);
    QVERIFY(tag5);
    QVERIFY(tag5->exists);

    tag5->torrents()->attach({torrent100[ID], torrent101[ID]},
                             {{"active", false}},
                             false);

    auto taggeds = Tagged::whereEq("tag_id", (*tag5)[ID])
            ->whereIn("torrent_id", {torrent100[ID], torrent101[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected torrent IDs
    QVector<QVariant> torrentIds {torrent100[ID], torrent101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["tag_id"].value(), (*tag5)[ID]);
        QVERIFY(torrentIds.contains(tagged["torrent_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    auto affected = tag5->torrents()->detach(false);

    QCOMPARE(affected, 2);

    // FEATURE aggregates, use count silverqx
    taggeds = Tagged::whereEq("tag_id", (*tag5)[ID])
            ->whereIn("torrent_id", {torrent100[ID], torrent101[ID]})
            .get({"torrent_id"});

    QCOMPARE(taggeds.size(), 0);

    // Restore db
    torrent100.remove();
    torrent101.remove();
}

void tst_Relations_Inserting_Updating::detach_CustomPivot_WithIds() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Tag tag100({{NAME, "tag100"}});
    tag100.save();
    Tag tag101({{NAME, "tag101"}});
    tag101.save();

    auto torrent5 = Torrent::find(5);
    QVERIFY(torrent5);
    QVERIFY(torrent5->exists);

    const auto torrent5Id = (*torrent5)[ID];

    torrent5->tags()->attach({tag100[ID], tag101[ID]},
                             {{"active", false}},
                             false);

    auto taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag100[ID], tag101[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected tag IDs
    QVector<QVariant> tagIds {tag100[ID], tag101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(tagIds.contains(tagged["tag_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    auto affected = torrent5->tags()->detach({tag100[ID], tag101[ID]}, false);

    QCOMPARE(affected, 2);

    // FEATURE aggregates, use count silverqx
    taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag100[ID], tag101[ID]})
            .get({"tag_id"});

    QCOMPARE(taggeds.size(), 0);

    // Restore db
    tag100.remove();
    tag101.remove();
}

void tst_Relations_Inserting_Updating::detach_CustomPivot_WithModels() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Tag tag100({{NAME, "tag100"}});
    tag100.save();
    Tag tag101({{NAME, "tag101"}});
    tag101.save();

    auto torrent5 = Torrent::find(5);
    QVERIFY(torrent5);
    QVERIFY(torrent5->exists);

    const auto torrent5Id = (*torrent5)[ID];

    torrent5->tags()->attach({tag100[ID], tag101[ID]},
                             {{"active", false}},
                             false);

    auto taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag100[ID], tag101[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected tag IDs
    QVector<QVariant> tagIds {tag100[ID], tag101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(tagIds.contains(tagged["tag_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    auto affected = torrent5->tags()->detach({tag100, tag101}, false);

    QCOMPARE(affected, 2);

    // FEATURE aggregates, use count silverqx
    taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag100[ID], tag101[ID]})
            .get({"tag_id"});

    QCOMPARE(taggeds.size(), 0);

    // Restore db
    tag100.remove();
    tag101.remove();
}

void tst_Relations_Inserting_Updating::detach_CustomPivot_All() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Tag tag100({{NAME, "tag100"}});
    tag100.save();
    Tag tag101({{NAME, "tag101"}});
    tag101.save();

    auto torrent5 = Torrent::find(5);
    QVERIFY(torrent5);
    QVERIFY(torrent5->exists);

    const auto torrent5Id = (*torrent5)[ID];

    torrent5->tags()->attach({tag100[ID], tag101[ID]},
                             {{"active", false}},
                             false);

    auto taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag100[ID], tag101[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected tag IDs
    QVector<QVariant> tagIds {tag100[ID], tag101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(tagIds.contains(tagged["tag_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    auto affected = torrent5->tags()->detach(false);

    QCOMPARE(affected, 2);

    // FEATURE aggregates, use count silverqx
    taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag100[ID], tag101[ID]})
            .get({"tag_id"});

    QCOMPARE(taggeds.size(), 0);

    // Restore db
    tag100.remove();
    tag101.remove();
}

void tst_Relations_Inserting_Updating::updateExistingPivot_BasicPivot_WithId() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto tag = Tag::find(4);
    QVERIFY(tag);
    QVERIFY(tag->exists);

    const auto tagId = (*tag)[ID];

    // Check values before update
    auto taggeds = Tagged::whereEq("tag_id", tagId)
            ->whereEq("torrent_id", 3)
            .get();

    {
        QCOMPARE(taggeds.size(), 1);
        auto &tagged = taggeds.first();
        QCOMPARE(tagged["tag_id"].value(), tagId);
        QCOMPARE(tagged["torrent_id"].value(), QVariant(3));
        QCOMPARE(tagged["active"].value(), QVariant(true));
    }

    auto updated = tag->torrents()->updateExistingPivot(3, {{"active", false}}, false);
    QCOMPARE(updated, 1);

    // Check values after update
    taggeds = Tagged::whereEq("tag_id", tagId)
            ->whereEq("torrent_id", 3)
            .get();

    {
        QCOMPARE(taggeds.size(), 1);
        auto &tagged = taggeds.first();
        QCOMPARE(tagged["tag_id"].value(), tagId);
        QCOMPARE(tagged["torrent_id"].value(), QVariant(3));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    // Restore db
    tag->torrents()->updateExistingPivot(3, {{"active", true}}, false);
}

void tst_Relations_Inserting_Updating::updateExistingPivot_BasicPivot_WithModel() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto tag = Tag::find(4);
    QVERIFY(tag);
    QVERIFY(tag->exists);

    const auto tagId = (*tag)[ID];

    // Check values before update
    auto taggeds = Tagged::whereEq("tag_id", tagId)
            ->whereEq("torrent_id", 3)
            .get();

    {
        QCOMPARE(taggeds.size(), 1);
        auto &tagged = taggeds.first();
        QCOMPARE(tagged["tag_id"].value(), tagId);
        QCOMPARE(tagged["torrent_id"].value(), QVariant(3));
        QCOMPARE(tagged["active"].value(), QVariant(true));
    }

    const auto torrent = Torrent::find(3);
    auto updated = tag->torrents()->updateExistingPivot(*torrent, {{"active", false}},
                                                        false);
    QCOMPARE(updated, 1);

    // Check values after update
    taggeds = Tagged::whereEq("tag_id", tagId)
            ->whereEq("torrent_id", 3)
            .get();

    {
        QCOMPARE(taggeds.size(), 1);
        auto &tagged = taggeds.first();
        QCOMPARE(tagged["tag_id"].value(), tagId);
        QCOMPARE(tagged["torrent_id"].value(), QVariant(3));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    // Restore db
    tag->torrents()->updateExistingPivot(3, {{"active", true}}, false);
}

void tst_Relations_Inserting_Updating::updateExistingPivot_CustomPivot_WithId() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(3);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    const auto torrentId = (*torrent)[ID];

    // Check values before update
    auto taggeds = Tagged::whereEq("torrent_id", torrentId)
            ->whereEq("tag_id", 4)
            .get();

    {
        QCOMPARE(taggeds.size(), 1);
        auto &tagged = taggeds.first();
        QCOMPARE(tagged["torrent_id"].value(), torrentId);
        QCOMPARE(tagged["tag_id"].value(), QVariant(4));
        QCOMPARE(tagged["active"].value(), QVariant(true));
    }

    auto updated = torrent->tags()->updateExistingPivot(4, {{"active", false}}, false);
    QCOMPARE(updated, 1);

    // Check values after update
    taggeds = Tagged::whereEq("torrent_id", torrentId)
            ->whereEq("tag_id", 4)
            .get();

    {
        QCOMPARE(taggeds.size(), 1);
        auto &tagged = taggeds.first();
        QCOMPARE(tagged["torrent_id"].value(), torrentId);
        QCOMPARE(tagged["tag_id"].value(), QVariant(4));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    // Restore db
    torrent->tags()->updateExistingPivot(4, {{"active", true}}, false);
}

void tst_Relations_Inserting_Updating::updateExistingPivot_CustomPivot_WithModel() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(3);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    const auto torrentId = (*torrent)[ID];

    // Check values before update
    auto taggeds = Tagged::whereEq("torrent_id", torrentId)
            ->whereEq("tag_id", 4)
            .get();

    {
        QCOMPARE(taggeds.size(), 1);
        auto &tagged = taggeds.first();
        QCOMPARE(tagged["torrent_id"].value(), torrentId);
        QCOMPARE(tagged["tag_id"].value(), QVariant(4));
        QCOMPARE(tagged["active"].value(), QVariant(true));
    }

    const auto tag = Tag::find(4);
    auto updated = torrent->tags()->updateExistingPivot(*tag, {{"active", false}},
                                                        false);
    QCOMPARE(updated, 1);

    // Check values after update
    taggeds = Tagged::whereEq("torrent_id", torrentId)
            ->whereEq("tag_id", 4)
            .get();

    {
        QCOMPARE(taggeds.size(), 1);
        auto &tagged = taggeds.first();
        QCOMPARE(tagged["torrent_id"].value(), torrentId);
        QCOMPARE(tagged["tag_id"].value(), QVariant(4));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    // Restore db
    torrent->tags()->updateExistingPivot(4, {{"active", true}}, false);
}

void tst_Relations_Inserting_Updating::sync_BasicPivot_WithIds() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {"size", 100}, {"progress", 555},
        {"hash", "xyzhash100"}, {"note", "sync with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {"size", 101}, {"progress", 556},
        {"hash", "xyzhash101"}, {"note", "sync with pivot"},
    };
    torrent101.save();
    Torrent torrent102 {
        {NAME, "test102"}, {"size", 102}, {"progress", 557},
        {"hash", "xyzhash102"}, {"note", "sync with pivot"},
    };
    torrent102.save();
    Torrent torrent103 {
        {NAME, "test103"}, {"size", 103}, {"progress", 558},
        {"hash", "xyzhash103"}, {"note", "sync with pivot"},
    };
    torrent103.save();

    auto tag5 = Tag::find(5);
    QVERIFY(tag5);
    QVERIFY(tag5->exists);

    const auto tagId = (*tag5)[ID];

    // Check values before update
    auto taggeds = Tagged::whereEq("tag_id", tagId)->get();
    QCOMPARE(taggeds.size(), 0);

    tag5->torrents()->attach({{torrent101}, {torrent102}}, {{"active", true}}, false);

    // Verify attached tags
    taggeds = Tagged::whereEq("tag_id", tagId)
            ->whereIn("torrent_id", {torrent101[ID], torrent102[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected torrent IDs
    QVector<QVariant> torrentIds {torrent101[ID], torrent102[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["tag_id"].value(), tagId);
        QVERIFY(torrentIds.contains(tagged["torrent_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(true));
    }

    const auto changed =
            tag5->torrents()->sync({*torrent100[ID], *torrent101[ID],
                                    *torrent103[ID]});

    // Verify result
    QCOMPARE(changed.size(), static_cast<std::size_t>(3));
    QVERIFY(changed.contains("attached"));
    QVERIFY(changed.contains("detached"));
    QVERIFY(changed.contains("updated"));

    const auto &attachedVector = changed.at("attached");
    QCOMPARE(attachedVector.size(), 2);
    const auto &detachedVector = changed.at("detached");
    QCOMPARE(detachedVector.size(), 1);
    QVERIFY(changed.at("updated").isEmpty());

    const QVector<QVariant> expectedAttached {torrent100[ID], torrent103[ID]};
    const QVector<QVariant> expectedDetached {torrent102[ID]};

    for (const auto &attached : attachedVector)
        QVERIFY(expectedAttached.contains(attached));
    for (const auto &detached : detachedVector)
        QVERIFY(expectedDetached.contains(detached));

    // Verify tagged values in the database
    taggeds = Tagged::whereEq("tag_id", tagId)
            ->whereIn("torrent_id", {torrent100[ID], torrent101[ID],
                                     torrent102[ID], torrent103[ID]})
            .get();

    QCOMPARE(taggeds.size(), 3);

    // Expected active attribute values by the tag ID
    std::unordered_map<quint64, bool> taggedActive {
        {torrent100[ID]->value<quint64>(), true},
        {torrent101[ID]->value<quint64>(), true},
        {torrent103[ID]->value<quint64>(), true},
    };

    quint64 torrentId = 0;

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        torrentId = tagged["torrent_id"]->value<quint64>();

        QCOMPARE(tagged["tag_id"].value(), tagId);
        QVERIFY(taggedActive.contains(torrentId));
        QCOMPARE(tagged["active"]->value<bool>(), taggedActive.at(torrentId));
    }

    torrent100.remove();
    torrent101.remove();
    torrent102.remove();
    torrent103.remove();
}

void tst_Relations_Inserting_Updating::sync_BasicPivot_IdsWithAttributes() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {"size", 100}, {"progress", 555},
        {"hash", "xyzhash100"}, {"note", "sync with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {"size", 101}, {"progress", 556},
        {"hash", "xyzhash101"}, {"note", "sync with pivot"},
    };
    torrent101.save();
    Torrent torrent102 {
        {NAME, "test102"}, {"size", 102}, {"progress", 557},
        {"hash", "xyzhash102"}, {"note", "sync with pivot"},
    };
    torrent102.save();
    Torrent torrent103 {
        {NAME, "test103"}, {"size", 103}, {"progress", 558},
        {"hash", "xyzhash103"}, {"note", "sync with pivot"},
    };
    torrent103.save();

    auto tag5 = Tag::find(5);
    QVERIFY(tag5);
    QVERIFY(tag5->exists);

    const auto tagId = (*tag5)[ID];

    // Check values before update
    auto taggeds = Tagged::whereEq("tag_id", tagId)->get();
    QCOMPARE(taggeds.size(), 0);

    tag5->torrents()->attach({{torrent101}, {torrent102}}, {{"active", true}}, false);

    // Verify attached tags
    taggeds = Tagged::whereEq("tag_id", tagId)
            ->whereIn("torrent_id", {torrent101[ID], torrent102[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected torrent IDs
    QVector<QVariant> torrentIds {torrent101[ID], torrent102[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["tag_id"].value(), tagId);
        QVERIFY(torrentIds.contains(tagged["torrent_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(true));
    }

    const auto changed = tag5->torrents()->sync(
                           {{torrent100[ID]->value<quint64>(), {{"active", true}}},
                            {torrent101[ID]->value<quint64>(), {{"active", false}}},
                            {torrent103[ID]->value<quint64>(), {{"active", true}}}});

    // Verify result
    QCOMPARE(changed.size(), static_cast<std::size_t>(3));
    QVERIFY(changed.contains("attached"));
    QVERIFY(changed.contains("detached"));
    QVERIFY(changed.contains("updated"));

    const auto &attachedVector = changed.at("attached");
    QCOMPARE(attachedVector.size(), 2);
    const auto &detachedVector = changed.at("detached");
    QCOMPARE(detachedVector.size(), 1);
    const auto &updatedVector = changed.at("updated");
    QCOMPARE(updatedVector.size(), 1);

    const QVector<QVariant> expectedAttached {torrent100[ID], torrent103[ID]};
    const QVector<QVariant> expectedDetached {torrent102[ID]};
    const QVector<QVariant> expectedUpdated {torrent101[ID]};

    for (const auto &attached : attachedVector)
        QVERIFY(expectedAttached.contains(attached));
    for (const auto &detached : detachedVector)
        QVERIFY(expectedDetached.contains(detached));
    for (const auto &updated : updatedVector)
        QVERIFY(expectedUpdated.contains(updated));

    // Verify tagged values in the database
    taggeds = Tagged::whereEq("tag_id", tagId)
            ->whereIn("torrent_id", {torrent100[ID], torrent101[ID],
                                     torrent102[ID], torrent103[ID]})
            .get();

    QCOMPARE(taggeds.size(), 3);

    // Expected active attribute values by the tag ID
    std::unordered_map<quint64, bool> taggedActive {
        {torrent100[ID]->value<quint64>(), true},
        {torrent101[ID]->value<quint64>(), false},
        {torrent103[ID]->value<quint64>(), true},
    };

    quint64 torrentId = 0;

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        torrentId = tagged["torrent_id"]->value<quint64>();

        QCOMPARE(tagged["tag_id"].value(), tagId);
        QVERIFY(taggedActive.contains(torrentId));
        QCOMPARE(tagged["active"]->value<bool>(), taggedActive.at(torrentId));
    }

    torrent100.remove();
    torrent101.remove();
    torrent102.remove();
    torrent103.remove();
}

void tst_Relations_Inserting_Updating::sync_CustomPivot_WithIds() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Tag tag100({{NAME, "tag100"}});
    tag100.save();
    Tag tag101({{NAME, "tag101"}});
    tag101.save();
    Tag tag102({{NAME, "tag102"}});
    tag102.save();
    Tag tag103({{NAME, "tag103"}});
    tag103.save();

    auto torrent5 = Torrent::find(5);
    QVERIFY(torrent5);
    QVERIFY(torrent5->exists);

    const auto torrent5Id = (*torrent5)[ID];

    // Check values before update
    auto taggeds = Tagged::whereEq("torrent_id", torrent5Id)->get();
    QCOMPARE(taggeds.size(), 0);

    torrent5->tags()->attach({{tag101}, {tag102}}, {{"active", true}}, false);

    // Verify attached tags
    taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag101[ID], tag102[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected tag IDs
    QVector<QVariant> tagIds {tag101[ID], tag102[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(tagIds.contains(tagged["tag_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(true));
    }

    const auto changed =
            torrent5->tags()->sync({*tag100[ID], *tag101[ID], *tag103[ID]});

    // Verify result
    QCOMPARE(changed.size(), static_cast<std::size_t>(3));
    QVERIFY(changed.contains("attached"));
    QVERIFY(changed.contains("detached"));
    QVERIFY(changed.contains("updated"));

    const auto &attachedVector = changed.at("attached");
    QCOMPARE(attachedVector.size(), 2);
    const auto &detachedVector = changed.at("detached");
    QCOMPARE(detachedVector.size(), 1);
    QVERIFY(changed.at("updated").isEmpty());

    const QVector<QVariant> expectedAttached {tag100[ID], tag103[ID]};
    const QVector<QVariant> expectedDetached {tag102[ID]};

    for (const auto &attached : attachedVector)
        QVERIFY(expectedAttached.contains(attached));
    for (const auto &detached : detachedVector)
        QVERIFY(expectedDetached.contains(detached));

    // Verify tagged values in the database
    taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag100[ID], tag101[ID],
                                 tag102[ID], tag103[ID]})
            .get();

    QCOMPARE(taggeds.size(), 3);

    // Expected active attribute values by the tag ID
    std::unordered_map<quint64, bool> taggedActive {
        {tag100[ID]->value<quint64>(), true},
        {tag101[ID]->value<quint64>(), true},
        {tag103[ID]->value<quint64>(), true},
    };

    quint64 tagId = 0;

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        tagId = tagged["tag_id"]->value<quint64>();

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(taggedActive.contains(tagId));
        QCOMPARE(tagged["active"]->value<bool>(), taggedActive.at(tagId));
    }

    tag100.remove();
    tag101.remove();
    tag102.remove();
    tag103.remove();
}

void tst_Relations_Inserting_Updating::sync_CustomPivot_IdsWithAttributes() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Tag tag100({{NAME, "tag100"}});
    tag100.save();
    Tag tag101({{NAME, "tag101"}});
    tag101.save();
    Tag tag102({{NAME, "tag102"}});
    tag102.save();
    Tag tag103({{NAME, "tag103"}});
    tag103.save();

    auto torrent5 = Torrent::find(5);
    QVERIFY(torrent5);
    QVERIFY(torrent5->exists);

    const auto torrent5Id = (*torrent5)[ID];

    // Check values before update
    auto taggeds = Tagged::whereEq("torrent_id", torrent5Id)->get();
    QCOMPARE(taggeds.size(), 0);

    torrent5->tags()->attach({{tag101}, {tag102}}, {{"active", true}}, false);

    // Verify attached tags
    taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag101[ID], tag102[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected tag IDs
    QVector<QVariant> tagIds {tag101[ID], tag102[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(tagIds.contains(tagged["tag_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(true));
    }

    const auto changed = torrent5->tags()->sync(
                       {{tag100[ID]->value<quint64>(), {{"active", true}}},
                        {tag101[ID]->value<quint64>(), {{"active", false}}},
                        {tag103[ID]->value<quint64>(), {{"active", true}}}});

    // Verify result
    QCOMPARE(changed.size(), static_cast<std::size_t>(3));
    QVERIFY(changed.contains("attached"));
    QVERIFY(changed.contains("detached"));
    QVERIFY(changed.contains("updated"));

    const auto &attachedVector = changed.at("attached");
    QCOMPARE(attachedVector.size(), 2);
    const auto &detachedVector = changed.at("detached");
    QCOMPARE(detachedVector.size(), 1);
    const auto &updatedVector = changed.at("updated");
    QCOMPARE(updatedVector.size(), 1);

    const QVector<QVariant> expectedAttached {tag100[ID], tag103[ID]};
    const QVector<QVariant> expectedDetached {tag102[ID]};
    const QVector<QVariant> expectedUpdated {tag101[ID]};

    for (const auto &attached : attachedVector)
        QVERIFY(expectedAttached.contains(attached));
    for (const auto &detached : detachedVector)
        QVERIFY(expectedDetached.contains(detached));
    for (const auto &updated : updatedVector)
        QVERIFY(expectedUpdated.contains(updated));

    // Verify tagged values in the database
    taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag100[ID], tag101[ID],
                                 tag102[ID], tag103[ID]})
            .get();

    QCOMPARE(taggeds.size(), 3);

    // Expected active attribute values by the tag ID
    std::unordered_map<quint64, bool> taggedActive {
        {tag100[ID]->value<quint64>(), true},
        {tag101[ID]->value<quint64>(), false},
        {tag103[ID]->value<quint64>(), true},
    };

    quint64 tagId = 0;

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        tagId = tagged["tag_id"]->value<quint64>();

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(taggedActive.contains(tagId));
        QCOMPARE(tagged["active"]->value<bool>(), taggedActive.at(tagId));
    }

    tag100.remove();
    tag101.remove();
    tag102.remove();
    tag103.remove();
}

void tst_Relations_Inserting_Updating::syncWithoutDetaching_BasicPivot_WithIds() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {"size", 100}, {"progress", 555},
        {"hash", "xyzhash100"}, {"note", "sync with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {"size", 101}, {"progress", 556},
        {"hash", "xyzhash101"}, {"note", "sync with pivot"},
    };
    torrent101.save();
    Torrent torrent102 {
        {NAME, "test102"}, {"size", 102}, {"progress", 557},
        {"hash", "xyzhash102"}, {"note", "sync with pivot"},
    };
    torrent102.save();
    Torrent torrent103 {
        {NAME, "test103"}, {"size", 103}, {"progress", 558},
        {"hash", "xyzhash103"}, {"note", "sync with pivot"},
    };
    torrent103.save();

    auto tag5 = Tag::find(5);
    QVERIFY(tag5);
    QVERIFY(tag5->exists);

    const auto tagId = (*tag5)[ID];

    // Check values before update
    auto taggeds = Tagged::whereEq("tag_id", tagId)->get();
    QCOMPARE(taggeds.size(), 0);

    tag5->torrents()->attach({{torrent101}, {torrent102}}, {{"active", true}}, false);

    // Verify attached tags
    taggeds = Tagged::whereEq("tag_id", tagId)
            ->whereIn("torrent_id", {torrent101[ID], torrent102[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected torrent IDs
    QVector<QVariant> torrentIds {torrent101[ID], torrent102[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["tag_id"].value(), tagId);
        QVERIFY(torrentIds.contains(tagged["torrent_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(true));
    }

    const auto changed =
            tag5->torrents()->syncWithoutDetaching({*torrent100[ID], *torrent101[ID],
                                                    *torrent103[ID]});

    // Verify result
    QCOMPARE(changed.size(), static_cast<std::size_t>(3));
    QVERIFY(changed.contains("attached"));
    QVERIFY(changed.contains("detached"));
    QVERIFY(changed.contains("updated"));

    const auto &attachedVector = changed.at("attached");
    QCOMPARE(attachedVector.size(), 2);
    QVERIFY(changed.at("detached").isEmpty());
    QVERIFY(changed.at("updated").isEmpty());

    const QVector<QVariant> expectedAttached {torrent100[ID], torrent103[ID]};

    for (const auto &attached : attachedVector)
        QVERIFY(expectedAttached.contains(attached));

    // Verify tagged values in the database
    taggeds = Tagged::whereEq("tag_id", tagId)
            ->whereIn("torrent_id", {torrent100[ID], torrent101[ID],
                                     torrent102[ID], torrent103[ID]})
            .get();

    QCOMPARE(taggeds.size(), 4);

    // Expected active attribute values by the tag ID
    std::unordered_map<quint64, bool> taggedActive {
        {torrent100[ID]->value<quint64>(), true},
        {torrent101[ID]->value<quint64>(), true},
        {torrent102[ID]->value<quint64>(), true},
        {torrent103[ID]->value<quint64>(), true},
    };

    quint64 torrentId = 0;

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        torrentId = tagged["torrent_id"]->value<quint64>();

        QCOMPARE(tagged["tag_id"].value(), tagId);
        QVERIFY(taggedActive.contains(torrentId));
        QCOMPARE(tagged["active"]->value<bool>(), taggedActive.at(torrentId));
    }

    torrent100.remove();
    torrent101.remove();
    torrent102.remove();
    torrent103.remove();
}

void tst_Relations_Inserting_Updating
        ::syncWithoutDetaching_BasicPivot_IdsWithAttributes() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {"size", 100}, {"progress", 555},
        {"hash", "xyzhash100"}, {"note", "sync with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {"size", 101}, {"progress", 556},
        {"hash", "xyzhash101"}, {"note", "sync with pivot"},
    };
    torrent101.save();
    Torrent torrent102 {
        {NAME, "test102"}, {"size", 102}, {"progress", 557},
        {"hash", "xyzhash102"}, {"note", "sync with pivot"},
    };
    torrent102.save();
    Torrent torrent103 {
        {NAME, "test103"}, {"size", 103}, {"progress", 558},
        {"hash", "xyzhash103"}, {"note", "sync with pivot"},
    };
    torrent103.save();

    auto tag5 = Tag::find(5);
    QVERIFY(tag5);
    QVERIFY(tag5->exists);

    const auto tagId = (*tag5)[ID];

    // Check values before update
    auto taggeds = Tagged::whereEq("tag_id", tagId)->get();
    QCOMPARE(taggeds.size(), 0);

    tag5->torrents()->attach({{torrent101}, {torrent102}}, {{"active", true}}, false);

    // Verify attached tags
    taggeds = Tagged::whereEq("tag_id", tagId)
            ->whereIn("torrent_id", {torrent101[ID], torrent102[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected torrent IDs
    QVector<QVariant> torrentIds {torrent101[ID], torrent102[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["tag_id"].value(), tagId);
        QVERIFY(torrentIds.contains(tagged["torrent_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(true));
    }

    const auto changed = tag5->torrents()->syncWithoutDetaching(
                           {{torrent100[ID]->value<quint64>(), {{"active", true}}},
                            {torrent101[ID]->value<quint64>(), {{"active", false}}},
                            {torrent103[ID]->value<quint64>(), {{"active", true}}}});

    // Verify result
    QCOMPARE(changed.size(), static_cast<std::size_t>(3));
    QVERIFY(changed.contains("attached"));
    QVERIFY(changed.contains("detached"));
    QVERIFY(changed.contains("updated"));

    const auto &attachedVector = changed.at("attached");
    QCOMPARE(attachedVector.size(), 2);
    QVERIFY(changed.at("detached").isEmpty());
    const auto &updatedVector = changed.at("updated");
    QCOMPARE(updatedVector.size(), 1);

    const QVector<QVariant> expectedAttached {torrent100[ID], torrent103[ID]};
    const QVector<QVariant> expectedUpdated {torrent101[ID]};

    for (const auto &attached : attachedVector)
        QVERIFY(expectedAttached.contains(attached));
    for (const auto &updated : updatedVector)
        QVERIFY(expectedUpdated.contains(updated));

    // Verify tagged values in the database
    taggeds = Tagged::whereEq("tag_id", tagId)
            ->whereIn("torrent_id", {torrent100[ID], torrent101[ID],
                                     torrent102[ID], torrent103[ID]})
            .get();

    QCOMPARE(taggeds.size(), 4);

    // Expected active attribute values by the tag ID
    std::unordered_map<quint64, bool> taggedActive {
        {torrent100[ID]->value<quint64>(), true},
        {torrent101[ID]->value<quint64>(), false},
        {torrent102[ID]->value<quint64>(), true},
        {torrent103[ID]->value<quint64>(), true},
    };

    quint64 torrentId = 0;

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        torrentId = tagged["torrent_id"]->value<quint64>();

        QCOMPARE(tagged["tag_id"].value(), tagId);
        QVERIFY(taggedActive.contains(torrentId));
        QCOMPARE(tagged["active"]->value<bool>(), taggedActive.at(torrentId));
    }

    torrent100.remove();
    torrent101.remove();
    torrent102.remove();
    torrent103.remove();
}

void tst_Relations_Inserting_Updating::syncWithoutDetaching_CustomPivot_WithIds() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Tag tag100({{NAME, "tag100"}});
    tag100.save();
    Tag tag101({{NAME, "tag101"}});
    tag101.save();
    Tag tag102({{NAME, "tag102"}});
    tag102.save();
    Tag tag103({{NAME, "tag103"}});
    tag103.save();

    auto torrent5 = Torrent::find(5);
    QVERIFY(torrent5);
    QVERIFY(torrent5->exists);

    const auto torrent5Id = (*torrent5)[ID];

    // Check values before update
    auto taggeds = Tagged::whereEq("torrent_id", torrent5Id)->get();
    QCOMPARE(taggeds.size(), 0);

    torrent5->tags()->attach({{tag101}, {tag102}}, {{"active", true}}, false);

    // Verify attached tags
    taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag101[ID], tag102[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected tag IDs
    QVector<QVariant> tagIds {tag101[ID], tag102[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(tagIds.contains(tagged["tag_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(true));
    }

    const auto changed =
            torrent5->tags()->syncWithoutDetaching({*tag100[ID], *tag101[ID],
                                                    *tag103[ID]});

    // Verify result
    QCOMPARE(changed.size(), static_cast<std::size_t>(3));
    QVERIFY(changed.contains("attached"));
    QVERIFY(changed.contains("detached"));
    QVERIFY(changed.contains("updated"));

    const auto &attachedVector = changed.at("attached");
    QCOMPARE(attachedVector.size(), 2);
    QVERIFY(changed.at("detached").isEmpty());
    QVERIFY(changed.at("updated").isEmpty());

    const QVector<QVariant> expectedAttached {tag100[ID], tag103[ID]};

    for (const auto &attached : changed.at("attached"))
        QVERIFY(expectedAttached.contains(attached));

    // Verify tagged values in the database
    taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag100[ID], tag101[ID],
                                 tag102[ID], tag103[ID]})
            .get();

    QCOMPARE(taggeds.size(), 4);

    // Expected active attribute values by the tag ID
    std::unordered_map<quint64, bool> taggedActive {
        {tag100[ID]->value<quint64>(), true},
        {tag101[ID]->value<quint64>(), true},
        {tag102[ID]->value<quint64>(), true},
        {tag103[ID]->value<quint64>(), true},
    };

    quint64 tagId = 0;

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        tagId = tagged["tag_id"]->value<quint64>();

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(taggedActive.contains(tagId));
        QCOMPARE(tagged["active"]->value<bool>(), taggedActive.at(tagId));
    }

    tag100.remove();
    tag101.remove();
    tag102.remove();
    tag103.remove();
}

void tst_Relations_Inserting_Updating
        ::syncWithoutDetaching_CustomPivot_IdsWithAttributes() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Tag tag100({{NAME, "tag100"}});
    tag100.save();
    Tag tag101({{NAME, "tag101"}});
    tag101.save();
    Tag tag102({{NAME, "tag102"}});
    tag102.save();
    Tag tag103({{NAME, "tag103"}});
    tag103.save();

    auto torrent5 = Torrent::find(5);
    QVERIFY(torrent5);
    QVERIFY(torrent5->exists);

    const auto torrent5Id = (*torrent5)[ID];

    // Check values before update
    auto taggeds = Tagged::whereEq("torrent_id", torrent5Id)->get();
    QCOMPARE(taggeds.size(), 0);

    torrent5->tags()->attach({{tag101}, {tag102}}, {{"active", true}}, false);

    // Verify attached tags
    taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag101[ID], tag102[ID]})
            .get();

    QCOMPARE(taggeds.size(), 2);

    // Expected tag IDs
    QVector<QVariant> tagIds {tag101[ID], tag102[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(tagIds.contains(tagged["tag_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(true));
    }

    const auto changed = torrent5->tags()->syncWithoutDetaching(
                       {{tag100[ID]->value<quint64>(), {{"active", true}}},
                        {tag101[ID]->value<quint64>(), {{"active", false}}},
                        {tag103[ID]->value<quint64>(), {{"active", true}}}});

    // Verify result
    QCOMPARE(changed.size(), static_cast<std::size_t>(3));
    QVERIFY(changed.contains("attached"));
    QVERIFY(changed.contains("detached"));
    QVERIFY(changed.contains("updated"));

    const auto &attachedVector = changed.at("attached");
    QCOMPARE(attachedVector.size(), 2);
    QVERIFY(changed.at("detached").isEmpty());
    const auto &updatedVector = changed.at("updated");
    QCOMPARE(updatedVector.size(), 1);

    const QVector<QVariant> expectedAttached {tag100[ID], tag103[ID]};
    const QVector<QVariant> expectedUpdated {tag101[ID]};

    for (const auto &attached : attachedVector)
        QVERIFY(expectedAttached.contains(attached));
    for (const auto &updated : updatedVector)
        QVERIFY(expectedUpdated.contains(updated));

    // Verify tagged values in the database
    taggeds = Tagged::whereEq("torrent_id", torrent5Id)
            ->whereIn("tag_id", {tag100[ID], tag101[ID],
                                 tag102[ID], tag103[ID]})
            .get();

    QCOMPARE(taggeds.size(), 4);

    // Expected active attribute values by the tag ID
    std::unordered_map<quint64, bool> taggedActive {
        {tag100[ID]->value<quint64>(), true},
        {tag101[ID]->value<quint64>(), false},
        {tag102[ID]->value<quint64>(), true},
        {tag103[ID]->value<quint64>(), true},
    };

    quint64 tagId = 0;

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (Tagged), typeid (tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        tagId = tagged["tag_id"]->value<quint64>();

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(taggedActive.contains(tagId));
        QCOMPARE(tagged["active"]->value<bool>(), taggedActive.at(tagId));
    }

    tag100.remove();
    tag101.remove();
    tag102.remove();
    tag103.remove();
}

QTEST_MAIN(tst_Relations_Inserting_Updating)

#include "tst_relations_inserting_updating.moc"
