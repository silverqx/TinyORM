#include <QCoreApplication>
#include <QtTest>

#include "models/torrent.hpp"
#include "models/torrentpreviewablefile.hpp"

#include "database.hpp"

using Orm::ConnectionInterface;
using Orm::QueryError;

class tst_Relations_Inserting_Updating : public QObject
{
    Q_OBJECT

public:
    tst_Relations_Inserting_Updating();
    ~tst_Relations_Inserting_Updating() = default;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void save() const;
    void save_Failed() const;

    void saveMany() const;
    void saveMany_Failed() const;

private:
    /*! The database connection instance. */
    ConnectionInterface &m_connection;
};

tst_Relations_Inserting_Updating::tst_Relations_Inserting_Updating()
    : m_connection(TestUtils::Database::createConnection())
{}

void tst_Relations_Inserting_Updating::initTestCase()
{}

void tst_Relations_Inserting_Updating::cleanupTestCase()
{}

void tst_Relations_Inserting_Updating::save() const
{
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

    auto savedResult = torrent->torrentFiles()->save(file);
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

void tst_Relations_Inserting_Updating::save_Failed() const
{
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
}

void tst_Relations_Inserting_Updating::saveMany() const
{
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

void tst_Relations_Inserting_Updating::saveMany_Failed() const
{
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
    // Make copy is enough
    auto file2 = file1;
    QVERIFY(!file2.exists);

    QVector<TorrentPreviewableFile> filesToSave {std::move(file1), std::move(file2)};
    QVector<TorrentPreviewableFile> savedFiles;
    QVERIFY_EXCEPTION_THROWN(savedFiles = torrent->torrentFiles()->saveMany(filesToSave),
                             QueryError);
    QVERIFY(savedFiles.isEmpty());
}

QTEST_MAIN(tst_Relations_Inserting_Updating)

#include "tst_relations_inserting_updating.moc"
