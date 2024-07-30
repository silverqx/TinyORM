#include <QCoreApplication>
#include <QTest>

#include "databases.hpp"
#include "macros.hpp"

#include "models/torrent.hpp"
#include "models/torrentpreviewablefile.hpp"

using Orm::Constants::Attached;
using Orm::Constants::Detached;
using Orm::Constants::HASH_;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;
using Orm::Constants::Updated_;

using Orm::One;

using TQueryError;

using TypeUtils = Orm::Utils::Type;

using Orm::Tiny::AttributeItem;
using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::Types::ModelsCollection;

using TestUtils::Databases;

using Models::Tag;
using Models::Tagged;
using Models::Torrent;
using Models::TorrentPreviewableFile;

class tst_Relations_Inserting_Updating : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
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

    void detach_BasicPivot_WithId() const;
    void detach_BasicPivot_WithIds() const;
    void detach_BasicPivot_WithModels() const;
    void detach_BasicPivot_All() const;
    void detach_CustomPivot_WithId() const;
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

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Relations_Inserting_Updating::initTestCase_data() const
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

void tst_Relations_Inserting_Updating::save_OnHasOneOrMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    TorrentPreviewableFile file({
        {"file_index", 3},
        {"filepath",   "test5_file4-save.mkv"},
        {SIZE_,        322322},
        {Progress,     777},
        {NOTE,         "relation's save"},
    });
    QVERIFY(!file.exists);

    auto [savedResult, fileRef] = torrent->torrentFiles()->save(file);
    QVERIFY(savedResult);
    QVERIFY(file.exists);
    QVERIFY(file[ID]->isValid());
    QVERIFY(file[ID]->value<quint64>() > 8);

    // save method have to return the same model as a reference
    QVERIFY(std::addressof(file) == std::addressof(fileRef));

    // Obtain file and verify saved values
    auto fileVerify = TorrentPreviewableFile::find(file[ID]);
    QCOMPARE((*fileVerify)[ID],           QVariant(file[ID]));
    QCOMPARE((*fileVerify)["torrent_id"], QVariant(5));
    QCOMPARE((*fileVerify)["file_index"], QVariant(3));
    QCOMPARE((*fileVerify)["filepath"],   QVariant("test5_file4-save.mkv"));
    QCOMPARE((*fileVerify)[SIZE_],        QVariant(322322));
    QCOMPARE((*fileVerify)[Progress],     QVariant(777));
    QCOMPARE((*fileVerify)[NOTE],         QVariant("relation's save"));

    // Remove file, restore db
    const auto result = file.remove();
    QVERIFY(result);
    QVERIFY(!file.exists);
}

void tst_Relations_Inserting_Updating::save_OnHasOneOrMany_WithRValue() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto [savedResult, file] = torrent->torrentFiles()->save({
        {"file_index", 3},
        {"filepath",   "test5_file4-save.mkv"},
        {SIZE_,        322322},
        {Progress,     777},
        {NOTE,         "relation's save"},
    });
    QVERIFY(savedResult);
    QVERIFY(file.exists);
    QVERIFY(file[ID]->isValid());
    QVERIFY(file[ID]->value<quint64>() > 8);

    // Obtain file and verify saved values
    auto fileVerify = TorrentPreviewableFile::find(file[ID]);
    QCOMPARE((*fileVerify)[ID],           QVariant(file[ID]));
    QCOMPARE((*fileVerify)["torrent_id"], QVariant(5));
    QCOMPARE((*fileVerify)["file_index"], QVariant(3));
    QCOMPARE((*fileVerify)["filepath"],   QVariant("test5_file4-save.mkv"));
    QCOMPARE((*fileVerify)[SIZE_],        QVariant(322322));
    QCOMPARE((*fileVerify)[Progress],     QVariant(777));
    QCOMPARE((*fileVerify)[NOTE],         QVariant("relation's save"));

    // Remove file, restore db
    const auto result = file.remove();
    QVERIFY(result);
    QVERIFY(!file.exists);
}

void tst_Relations_Inserting_Updating::save_OnHasOneOrMany_Failed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    TorrentPreviewableFile file({
        {"file_index", 1},
        {"filepath",   "test1_file1.mkv"},
        {SIZE_,        377477},
        {Progress,     222},
        {NOTE,         "relation's save fail"},
    });
    QVERIFY(!file.exists);

    TVERIFY_THROWS_EXCEPTION(QueryError, torrent->torrentFiles()->save(file));
    QVERIFY(!file.exists);
    QVERIFY(!file[ID]->isValid());
    QCOMPARE(file["torrent_id"], QVariant(1));
}

void tst_Relations_Inserting_Updating::saveMany_OnHasOneOrMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    TorrentPreviewableFile file1({
        {"file_index", 3},
        {"filepath",   "test5_file4-saveMany.mkv"},
        {SIZE_,        322322},
        {Progress,     777},
        {NOTE,         "relation's saveMany file1"},
    });
    QVERIFY(!file1.exists);
    TorrentPreviewableFile file2({
        {"file_index", 4},
        {"filepath",   "test5_file5-saveMany.mkv"},
        {SIZE_,        333322},
        {Progress,     888},
        {NOTE,         "relation's saveMany file2"},
    });
    QVERIFY(!file2.exists);

    ModelsCollection<TorrentPreviewableFile> filesToSave {
        std::move(file1),
        std::move(file2),
    };

    auto &savedFiles = torrent->torrentFiles()->saveMany(filesToSave);
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
    QVERIFY(std::addressof(*filesToSave.data()) == std::addressof(savedFile1));
    QVERIFY(std::addressof(filesToSave[1]) == std::addressof(savedFile2));

    // Obtain files and verify saved values
    auto file1Verify = TorrentPreviewableFile::find(savedFile1[ID]);
    QCOMPARE((*file1Verify)[ID],           QVariant(savedFile1[ID]));
    QCOMPARE((*file1Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file1Verify)["file_index"], QVariant(3));
    QCOMPARE((*file1Verify)["filepath"],   QVariant("test5_file4-saveMany.mkv"));
    QCOMPARE((*file1Verify)[SIZE_],        QVariant(322322));
    QCOMPARE((*file1Verify)[Progress],     QVariant(777));
    QCOMPARE((*file1Verify)[NOTE],         QVariant("relation's saveMany file1"));
    auto file2Verify = TorrentPreviewableFile::find(savedFile2[ID]);
    QCOMPARE((*file2Verify)[ID],           QVariant(savedFile2[ID]));
    QCOMPARE((*file2Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file2Verify)["file_index"], QVariant(4));
    QCOMPARE((*file2Verify)["filepath"],   QVariant("test5_file5-saveMany.mkv"));
    QCOMPARE((*file2Verify)[SIZE_],        QVariant(333322));
    QCOMPARE((*file2Verify)[Progress],     QVariant(888));
    QCOMPARE((*file2Verify)[NOTE],         QVariant("relation's saveMany file2"));

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto savedFiles = torrent->torrentFiles()->saveMany({{
        {"file_index", 3},
        {"filepath",   "test5_file4-saveMany.mkv"},
        {SIZE_,        322322},
        {Progress,     777},
        {NOTE,         "relation's saveMany file1"},
    }, {
        {"file_index", 4},
        {"filepath",   "test5_file5-saveMany.mkv"},
        {SIZE_,        333322},
        {Progress,     888},
        {NOTE,         "relation's saveMany file2"},
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
    QCOMPARE((*file1Verify)[ID],           QVariant(savedFile1[ID]));
    QCOMPARE((*file1Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file1Verify)["file_index"], QVariant(3));
    QCOMPARE((*file1Verify)["filepath"],   QVariant("test5_file4-saveMany.mkv"));
    QCOMPARE((*file1Verify)[SIZE_],        QVariant(322322));
    QCOMPARE((*file1Verify)[Progress],     QVariant(777));
    QCOMPARE((*file1Verify)[NOTE],         QVariant("relation's saveMany file1"));
    auto file2Verify = TorrentPreviewableFile::find(savedFile2[ID]);
    QCOMPARE((*file2Verify)[ID],           QVariant(savedFile2[ID]));
    QCOMPARE((*file2Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file2Verify)["file_index"], QVariant(4));
    QCOMPARE((*file2Verify)["filepath"],   QVariant("test5_file5-saveMany.mkv"));
    QCOMPARE((*file2Verify)[SIZE_],        QVariant(333322));
    QCOMPARE((*file2Verify)[Progress],     QVariant(888));
    QCOMPARE((*file2Verify)[NOTE],         QVariant("relation's saveMany file2"));

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    TorrentPreviewableFile file1({
        {"file_index", 1},
        {"filepath",   "test1_file1.mkv"},
        {SIZE_,        377477},
        {Progress,     222},
        {NOTE,         "relation's save fail"},
    });
    QVERIFY(!file1.exists);
    // Make a copy is enough
    auto file2 = file1;
    QVERIFY(!file2.exists);

    ModelsCollection<TorrentPreviewableFile> filesToSave {
        std::move(file1),
        std::move(file2),
    };
    ModelsCollection<TorrentPreviewableFile> savedFiles;
    TVERIFY_THROWS_EXCEPTION(QueryError,
                             savedFiles = torrent->torrentFiles()->saveMany(filesToSave));
    QVERIFY(savedFiles.isEmpty());
}

void tst_Relations_Inserting_Updating::create_OnHasOneOrMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QList<AttributeItem> fileAttribtues {
        {"file_index", 3},
        {"filepath",   "test5_file4-create.mkv"},
        {SIZE_,        322322},
        {Progress,     777},
        {NOTE,         "relation's save"},
    };

    auto file = torrent->torrentFiles()->create(fileAttribtues);
    QVERIFY(file.exists);
    QVERIFY(file[ID]->isValid());
    QVERIFY(file[ID]->value<quint64>() > 8);

    // Obtain file and verify saved values
    auto fileVerify = TorrentPreviewableFile::find(file[ID]);
    QCOMPARE((*fileVerify)[ID],           QVariant(file[ID]));
    QCOMPARE((*fileVerify)["torrent_id"], QVariant(5));
    QCOMPARE((*fileVerify)["file_index"], QVariant(3));
    QCOMPARE((*fileVerify)["filepath"],   QVariant("test5_file4-create.mkv"));
    QCOMPARE((*fileVerify)[SIZE_],        QVariant(322322));
    QCOMPARE((*fileVerify)[Progress],     QVariant(777));
    QCOMPARE((*fileVerify)[NOTE],         QVariant("relation's save"));

    // Remove file, restore db
    const auto result = file.remove();
    QVERIFY(result);
    QVERIFY(!file.exists);
}

void tst_Relations_Inserting_Updating::create_OnHasOneOrMany_WithRValue() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto file = torrent->torrentFiles()->create({
        {"file_index", 3},
        {"filepath",   "test5_file4-create.mkv"},
        {SIZE_,        322322},
        {Progress,     777},
        {NOTE,         "relation's save"},
    });
    QVERIFY(file.exists);
    QVERIFY(file[ID]->isValid());
    QVERIFY(file[ID]->value<quint64>() > 8);

    // Obtain file and verify saved values
    auto fileVerify = TorrentPreviewableFile::find(file[ID]);
    QCOMPARE((*fileVerify)[ID],           QVariant(file[ID]));
    QCOMPARE((*fileVerify)["torrent_id"], QVariant(5));
    QCOMPARE((*fileVerify)["file_index"], QVariant(3));
    QCOMPARE((*fileVerify)["filepath"],   QVariant("test5_file4-create.mkv"));
    QCOMPARE((*fileVerify)[SIZE_],        QVariant(322322));
    QCOMPARE((*fileVerify)[Progress],     QVariant(777));
    QCOMPARE((*fileVerify)[NOTE],         QVariant("relation's save"));

    // Remove file, restore db
    const auto result = file.remove();
    QVERIFY(result);
    QVERIFY(!file.exists);
}

void tst_Relations_Inserting_Updating::create_OnHasOneOrMany_Failed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QList<AttributeItem> fileAttribtues {
        {"file_index", 1},
        {"filepath",   "test1_file1.mkv"},
        {SIZE_,        377477},
        {Progress,     222},
        {NOTE,         "relation's save fail"},
    };
    TorrentPreviewableFile file;

    TVERIFY_THROWS_EXCEPTION(QueryError,
                             file = torrent->torrentFiles()->create(fileAttribtues));
    QVERIFY(!file.exists);
    QVERIFY(file.getAttributes().isEmpty());
}

void tst_Relations_Inserting_Updating::create_OnHasOneOrMany_WithRValue_Failed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    TorrentPreviewableFile file;

    TVERIFY_THROWS_EXCEPTION(QueryError,
            file = torrent->torrentFiles()->create({
                {"file_index", 1},
                {"filepath",   "test1_file1.mkv"},
                {SIZE_,        377477},
                {Progress,     222},
                {NOTE,         "relation's save fail"},
            }));

    QVERIFY(!file.exists);
    QVERIFY(file.getAttributes().isEmpty());
}

void tst_Relations_Inserting_Updating::createMany_OnHasOneOrMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QList<AttributeItem> file1Attributes({
        {"file_index", 3},
        {"filepath",   "test5_file4-createMany.mkv"},
        {SIZE_,        322322},
        {Progress,     777},
        {NOTE,         "relation's createMany file1"},
    });
    QList<AttributeItem> file2Attributes({
        {"file_index", 4},
        {"filepath",   "test5_file5-createMany.mkv"},
        {SIZE_,        333322},
        {Progress,     888},
        {NOTE,         "relation's createMany file2"},
    });

    QList<QList<AttributeItem>> fileAttributesToSave {
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
    QCOMPARE((*file1Verify)[ID],           QVariant(savedFile1[ID]));
    QCOMPARE((*file1Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file1Verify)["file_index"], QVariant(3));
    QCOMPARE((*file1Verify)["filepath"],   QVariant("test5_file4-createMany.mkv"));
    QCOMPARE((*file1Verify)[SIZE_],        QVariant(322322));
    QCOMPARE((*file1Verify)[Progress],     QVariant(777));
    QCOMPARE((*file1Verify)[NOTE],         QVariant("relation's createMany file1"));
    auto file2Verify = TorrentPreviewableFile::find(savedFile2[ID]);
    QCOMPARE((*file2Verify)[ID],           QVariant(savedFile2[ID]));
    QCOMPARE((*file2Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file2Verify)["file_index"], QVariant(4));
    QCOMPARE((*file2Verify)["filepath"],   QVariant("test5_file5-createMany.mkv"));
    QCOMPARE((*file2Verify)[SIZE_],        QVariant(333322));
    QCOMPARE((*file2Verify)[Progress],     QVariant(888));
    QCOMPARE((*file2Verify)[NOTE],         QVariant("relation's createMany file2"));

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto savedFiles = torrent->torrentFiles()->createMany({{
        {"file_index", 3},
        {"filepath",   "test5_file4-createMany.mkv"},
        {SIZE_,        322322},
        {Progress,     777},
        {NOTE,         "relation's createMany file1"},
    }, {
        {"file_index", 4},
        {"filepath",   "test5_file5-createMany.mkv"},
        {SIZE_,        333322},
        {Progress,     888},
        {NOTE,         "relation's createMany file2"},
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
    QCOMPARE((*file1Verify)[ID],           QVariant(savedFile1[ID]));
    QCOMPARE((*file1Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file1Verify)["file_index"], QVariant(3));
    QCOMPARE((*file1Verify)["filepath"],   QVariant("test5_file4-createMany.mkv"));
    QCOMPARE((*file1Verify)[SIZE_],        QVariant(322322));
    QCOMPARE((*file1Verify)[Progress],     QVariant(777));
    QCOMPARE((*file1Verify)[NOTE],         QVariant("relation's createMany file1"));
    auto file2Verify = TorrentPreviewableFile::find(savedFile2[ID]);
    QCOMPARE((*file2Verify)[ID],           QVariant(savedFile2[ID]));
    QCOMPARE((*file2Verify)["torrent_id"], QVariant(5));
    QCOMPARE((*file2Verify)["file_index"], QVariant(4));
    QCOMPARE((*file2Verify)["filepath"],   QVariant("test5_file5-createMany.mkv"));
    QCOMPARE((*file2Verify)[SIZE_],        QVariant(333322));
    QCOMPARE((*file2Verify)[Progress],     QVariant(888));
    QCOMPARE((*file2Verify)[NOTE],         QVariant("relation's createMany file2"));

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QList<AttributeItem> file1Attributes({
        {"file_index", 1},
        {"filepath",   "test1_file1.mkv"},
        {SIZE_,        377477},
        {Progress,     222},
        {NOTE,         "relation's save fail"},
    });
    // Make a copy is enough
    auto file2Attributes = file1Attributes;

    QList<QList<AttributeItem>> fileAttributesToSave {
        std::move(file1Attributes),
        std::move(file2Attributes),
    };

    ModelsCollection<TorrentPreviewableFile> savedFiles;
    TVERIFY_THROWS_EXCEPTION(QueryError,
                             savedFiles = torrent->torrentFiles()
                                          ->createMany(fileAttributesToSave));
    QVERIFY(savedFiles.isEmpty());
}

void
tst_Relations_Inserting_Updating::createMany_OnHasOneOrMany_WithRValue_Failed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    ModelsCollection<TorrentPreviewableFile> savedFiles;
    TVERIFY_THROWS_EXCEPTION(QueryError,
                savedFiles = torrent->torrentFiles()->createMany({{
                    {"file_index", 1},
                    {"filepath",   "test1_file1.mkv"},
                    {SIZE_,        377477},
                    {Progress,     222},
                    {NOTE,         "relation's save fail"},
                },
                /* It doesn't matter what is in this second vector, because after
                   the first attempt to save the exception will be thrown. */
                {
                    {"file_index", 1},
                    {"filepath",   "test1_file1.mkv"},
                    {SIZE_,        377477},
                    {Progress,     222},
                    {NOTE,         "relation's save fail"},
                }}));
    QVERIFY(savedFiles.isEmpty());
}

void tst_Relations_Inserting_Updating::save_OnBelongsToMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QVERIFY(std::addressof(tag) == std::addressof(tagRef));

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(1);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);

    Tag tag({{NAME, "tag1"}});
    QVERIFY(!tag.exists);

    TVERIFY_THROWS_EXCEPTION(QueryError, torrent->tags()->save(tag));
    QVERIFY(!tag.exists);
    QVERIFY(!tag[ID]->isValid());

    size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::saveMany_OnBelongsToMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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

    ModelsCollection<Tag> tagsToSave {std::move(tag1), std::move(tag2)};

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
    QVERIFY(std::addressof(tagsToSave) == std::addressof(savedTags));
    QVERIFY(std::addressof(*tagsToSave.data()) == std::addressof(savedTag1));
    QVERIFY(std::addressof(tagsToSave[1]) == std::addressof(savedTag2));

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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

    ModelsCollection<Tag> tagsToSave {std::move(tag1), std::move(tag2)};
    ModelsCollection<Tag> savedTags;
    TVERIFY_THROWS_EXCEPTION(QueryError,
                             savedTags = torrent->tags()->saveMany(tagsToSave));
    QVERIFY(savedTags.isEmpty());

    size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::create_OnBelongsToMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);

    QList<AttributeItem> tagAttributes {{NAME, "tag create"}};

    auto tag = torrent->tags()->create(tagAttributes, {{"active", false}});
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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);

    QList<AttributeItem> tagAttributes {{NAME, "tag1"}};
    Tag tag;

    TVERIFY_THROWS_EXCEPTION(QueryError,
                             tag = torrent->tags()->create(tagAttributes));
    QVERIFY(!tag.exists);
    QVERIFY(tag.getAttributes().isEmpty());

    size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::create_OnBelongsToMany_WithRValue_Failed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);

    Tag tag;

    TVERIFY_THROWS_EXCEPTION(QueryError,
                             tag = torrent->tags()->create({{NAME, "tag1"}}));
    QVERIFY(!tag.exists);
    QVERIFY(tag.getAttributes().isEmpty());

    size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::createMany_OnBelongsToMany() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);

    QList<QList<AttributeItem>> tagAttributes {{{NAME, "tag create 1"}},
                                               {{NAME, "tag create 2"}}};

    auto tags = torrent->tags()->createMany(tagAttributes, {{}, {{"active", false}}});
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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);

    QList<QList<AttributeItem>> tagAttributes {{{NAME, "tag1"}},
                                               {{NAME, "tag1"}}};

    ModelsCollection<Tag> tags;

    TVERIFY_THROWS_EXCEPTION(QueryError,
                             tags = torrent->tags()->createMany(tagAttributes,
                                                                {{{"active", false}}}));
    QVERIFY(tags.isEmpty());

    size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void
tst_Relations_Inserting_Updating::createMany_OnBelongsToMany_WithRValue_Failed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);

    ModelsCollection<Tag> tags;

    TVERIFY_THROWS_EXCEPTION(QueryError,
                             tags = torrent->tags()->createMany({{{NAME, "tag1"}},
                                                                 {{NAME, "tag1"}}},

                                                                {{{"active", false}}}));
    QVERIFY(tags.isEmpty());

    size = torrent->tags()->get({ID}).size();
    QCOMPARE(size, 0);
    size = Tagged::whereEq("torrent_id", 5)->get({"torrent_id"}).size();
    QCOMPARE(size, 0);
}

void tst_Relations_Inserting_Updating::associate_WithModel() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Create new torrent file for this test
    TorrentPreviewableFile file {
        {"file_index", 3},
        {"filepath",   "test5_file4.mkv"},
        {SIZE_,        3255},
        {Progress,     115},
        {NOTE,         "associate"},
    };

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    const auto &fileRelations = file.getRelations();
    QVERIFY(fileRelations.empty());

    auto &fileRef = file.torrent()->associate(*torrent);

    // associate method have to return the same model as a reference
    QVERIFY(std::addressof(file) == std::addressof(fileRef));

    const auto &torrentForeignKeyName = torrent->getForeignKey();
    QVERIFY(file.getAttributesHash().contains(torrentForeignKeyName));
    QCOMPARE(file[torrentForeignKeyName], torrent->getKey());

    // Copy of the associated model have to be set on the file
    QCOMPARE(fileRelations.size(), static_cast<std::size_t>(1));
    QVERIFY(fileRelations.contains(file.torrent()->getRelationName()));
    QVERIFY(file.relationLoaded("torrent"));

    auto *verifyTorrent5 = file.getRelation<Torrent, One>("torrent");
    QVERIFY(verifyTorrent5);
    QVERIFY(verifyTorrent5->exists);
    QCOMPARE(typeid (verifyTorrent5), typeid (Torrent *));
    QVERIFY(verifyTorrent5->is(torrent));
    QVERIFY(torrent == *verifyTorrent5);

    file.save();

    // Little useless, but will be absolutely sure that nothing changed
    QVERIFY(file.relationLoaded("torrent"));
    verifyTorrent5 = file.getRelation<Torrent, One>("torrent");
    QVERIFY(verifyTorrent5);
    QVERIFY(verifyTorrent5->exists);
    QCOMPARE(typeid (verifyTorrent5), typeid (Torrent *));
    QVERIFY(verifyTorrent5->is(torrent));
    QVERIFY(torrent == *verifyTorrent5);

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Create new torrent file for this test
    TorrentPreviewableFile file {
        {"file_index", 3},
        {"filepath",   "test5_file4.mkv"},
        {SIZE_,        3255},
        {Progress,     115},
        {NOTE,         "associate"},
    };

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    const auto &fileRelations = file.getRelations();
    QVERIFY(fileRelations.empty());

    auto &fileRef = file.torrent()->associate((*torrent)[ID]);

    // associate method have to return the same model as a reference
    QVERIFY(std::addressof(file) == std::addressof(fileRef));

    const auto &torrentForeignKeyName = torrent->getForeignKey();
    QVERIFY(file.getAttributesHash().contains(torrentForeignKeyName));
    QCOMPARE(file[torrentForeignKeyName], torrent->getKey());

    // This is only difference, associate with Id should unset relation model
    QVERIFY(fileRelations.empty());
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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Create new torrent file for this test
    TorrentPreviewableFile file {
        {"file_index", 3},
        {"filepath",   "test5_file4.mkv"},
        {SIZE_,        3255},
        {Progress,     115},
        {NOTE,         "associate"},
    };

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    const auto &fileRelations = file.getRelations();
    QVERIFY(fileRelations.empty());

    auto &fileRef = file.torrent()->associate(*torrent);

    // associate method have to return the same model as a reference
    QVERIFY(std::addressof(file) == std::addressof(fileRef));

    const auto &torrentForeignKeyName = torrent->getForeignKey();
    QVERIFY(file.getAttributesHash().contains(torrentForeignKeyName));
    QCOMPARE(file[torrentForeignKeyName], torrent->getKey());

    // Copy of the associated model have to be set on the file
    QCOMPARE(fileRelations.size(), static_cast<std::size_t>(1));
    QVERIFY(fileRelations.contains(file.torrent()->getRelationName()));
    QVERIFY(file.relationLoaded("torrent"));

    auto *verifyTorrent5 = file.getRelation<Torrent, One>("torrent");
    QVERIFY(verifyTorrent5);
    QVERIFY(verifyTorrent5->exists);
    QCOMPARE(typeid (verifyTorrent5), typeid (Torrent *));
    QVERIFY(verifyTorrent5->is(torrent));
    QVERIFY(torrent == *verifyTorrent5);

    /* Have to unset current relationship, this is clearly visible in the Eqloquent's
       associate implementation. */
    fileRef = file.torrent()->associate(2);

    QVERIFY(file.getAttributesHash().contains(torrentForeignKeyName));
    QCOMPARE(file[torrentForeignKeyName], QVariant(2));

    // This is only difference, associate with Id should unset relation model
    QVERIFY(fileRelations.empty());
    QVERIFY(!fileRelations.contains(file.torrent()->getRelationName()));
}

void tst_Relations_Inserting_Updating::dissociate() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    // Create new torrent file for this test
    TorrentPreviewableFile file {
        {"file_index", 3},
        {"filepath",   "test5_file4.mkv"},
        {SIZE_,        3255},
        {Progress,     115},
        {NOTE,         "associate"},
    };

    auto torrent = Torrent::find(5);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    const auto &fileRelations = file.getRelations();
    QVERIFY(fileRelations.empty());

    auto &fileRef = file.torrent()->associate(*torrent);

    // associate method have to return the same model as a reference
    QVERIFY(std::addressof(file) == std::addressof(fileRef));

    const auto &torrentForeignKeyName = torrent->getForeignKey();
    QVERIFY(file.getAttributesHash().contains(torrentForeignKeyName));
    QCOMPARE(file[torrentForeignKeyName], torrent->getKey());

    // Copy of the associated model have to be set on the file
    QCOMPARE(fileRelations.size(), static_cast<std::size_t>(1));
    QVERIFY(fileRelations.contains(file.torrent()->getRelationName()));
    QVERIFY(file.relationLoaded("torrent"));

    auto *verifyTorrent5 = file.getRelation<Torrent, One>("torrent");
    QVERIFY(verifyTorrent5);
    QVERIFY(verifyTorrent5->exists);
    QCOMPARE(typeid (verifyTorrent5), typeid (Torrent *));
    QVERIFY(verifyTorrent5->is(torrent));
    QVERIFY(torrent == *verifyTorrent5);

    auto &fileRefDissociate = file.torrent()->dissociate();

    // dissociate method have to return the same model as a reference
    QVERIFY(std::addressof(file) == std::addressof(fileRefDissociate));

    QVERIFY(file.getAttributesHash().contains(torrentForeignKeyName));
    QVERIFY(!file[torrentForeignKeyName].value().isValid());

    // Relation have to be set to std::nullopt internally
    QCOMPARE(fileRelations.size(), static_cast<std::size_t>(1));
    QVERIFY(fileRelations.contains(file.torrent()->getRelationName()));
    QVERIFY(file.relationLoaded("torrent"));

    verifyTorrent5 = file.getRelation<Torrent, One>("torrent");
    QVERIFY(verifyTorrent5 == nullptr);
}

void tst_Relations_Inserting_Updating::attach_BasicPivot_WithIds() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {SIZE_, 100}, {Progress, 555},
        {HASH_, "xyzhash100"}, {NOTE, "attach with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {SIZE_, 101}, {Progress, 556},
        {HASH_, "xyzhash101"}, {NOTE, "attach with pivot"},
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
    QList<QVariant> torrentIds {torrent100[ID], torrent101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {SIZE_, 100}, {Progress, 555},
        {HASH_, "xyzhash100"}, {NOTE, "attach with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {SIZE_, 101}, {Progress, 556},
        {HASH_, "xyzhash101"}, {NOTE, "attach with pivot"},
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
    QList<QVariant> torrentIds {torrent100[ID], torrent101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QList<QVariant> tagIds {tag100[ID], tag101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QList<QVariant> tagIds {tag100[ID], tag101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {SIZE_, 100}, {Progress, 555},
        {HASH_, "xyzhash100"}, {NOTE, "attach with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {SIZE_, 101}, {Progress, 556},
        {HASH_, "xyzhash101"}, {NOTE, "attach with pivot"},
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
        QCOMPARE(typeid (tagged), typeid (Tagged));

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
        QCOMPARE(typeid (tagged), typeid (Tagged));

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

void tst_Relations_Inserting_Updating::detach_BasicPivot_WithId() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {SIZE_, 100}, {Progress, 555},
        {HASH_, "xyzhash100"}, {NOTE, "attach with pivot"},
    };
    torrent100.save();

    auto tag4 = Tag::find(4);
    QVERIFY(tag4);
    QVERIFY(tag4->exists);
                             // Passing the quint64 to test it
    tag4->torrents()->attach(torrent100.getAttribute<quint64>(ID),
                             {{"active", false}},
                             false);

    auto taggeds = Tagged::whereEq("tag_id", (*tag4)[ID])
                   ->whereIn("torrent_id", {torrent100[ID]})
                   .get();

    QCOMPARE(taggeds.size(), 1);

    // Expected torrent ID
    QList<QVariant> torrentIds {torrent100[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["tag_id"].value(), (*tag4)[ID]);
        QVERIFY(torrentIds.contains(tagged["torrent_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }
                                             // Passing the quint64 to test it
    auto affected = tag4->torrents()->detach(torrent100.getAttribute<quint64>(ID), false);

    QCOMPARE(affected, 1);

    auto taggedsSize = Tagged::whereEq("tag_id", (*tag4)[ID])
                       ->whereIn("torrent_id", {torrent100[ID]})
                       .count();

    QCOMPARE(taggedsSize, 0);

    // Restore db
    torrent100.remove();
}

void tst_Relations_Inserting_Updating::detach_BasicPivot_WithIds() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {SIZE_, 100}, {Progress, 555},
        {HASH_, "xyzhash100"}, {NOTE, "attach with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {SIZE_, 101}, {Progress, 556},
        {HASH_, "xyzhash101"}, {NOTE, "attach with pivot"},
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
    QList<QVariant> torrentIds {torrent100[ID], torrent101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["tag_id"].value(), (*tag4)[ID]);
        QVERIFY(torrentIds.contains(tagged["torrent_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    auto affected = tag4->torrents()->detach({torrent100[ID], torrent101[ID]},
                                             false);

    QCOMPARE(affected, 2);

    auto taggedsSize = Tagged::whereEq("tag_id", (*tag4)[ID])
                       ->whereIn("torrent_id", {torrent100[ID], torrent101[ID]})
                       .count();

    QCOMPARE(taggedsSize, 0);

    // Restore db
    torrent100.remove();
    torrent101.remove();
}

void tst_Relations_Inserting_Updating::detach_BasicPivot_WithModels() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {SIZE_, 100}, {Progress, 555},
        {HASH_, "xyzhash100"}, {NOTE, "attach with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {SIZE_, 101}, {Progress, 556},
        {HASH_, "xyzhash101"}, {NOTE, "attach with pivot"},
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
    QList<QVariant> torrentIds {torrent100[ID], torrent101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["tag_id"].value(), (*tag4)[ID]);
        QVERIFY(torrentIds.contains(tagged["torrent_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    auto affected = tag4->torrents()->detach({torrent100, torrent101}, false);

    QCOMPARE(affected, 2);

    auto taggedsSize = Tagged::whereEq("tag_id", (*tag4)[ID])
                       ->whereIn("torrent_id", {torrent100[ID], torrent101[ID]})
                       .count();

    QCOMPARE(taggedsSize, 0);

    // Restore db
    torrent100.remove();
    torrent101.remove();
}

void tst_Relations_Inserting_Updating::detach_BasicPivot_All() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {SIZE_, 100}, {Progress, 555},
        {HASH_, "xyzhash100"}, {NOTE, "attach with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {SIZE_, 101}, {Progress, 556},
        {HASH_, "xyzhash101"}, {NOTE, "attach with pivot"},
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
    QList<QVariant> torrentIds {torrent100[ID], torrent101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["tag_id"].value(), (*tag5)[ID]);
        QVERIFY(torrentIds.contains(tagged["torrent_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    auto affected = tag5->torrents()->detachAll(false);

    QCOMPARE(affected, 2);

    auto taggedsSize = Tagged::whereEq("tag_id", (*tag5)[ID])
                       ->whereIn("torrent_id", {torrent100[ID], torrent101[ID]})
                       .count();

    QCOMPARE(taggedsSize, 0);

    // Restore db
    torrent100.remove();
    torrent101.remove();
}

void tst_Relations_Inserting_Updating::detach_CustomPivot_WithId() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Tag tag100({{NAME, "tag100"}});
    tag100.save();

    auto torrent5 = Torrent::find(5);
    QVERIFY(torrent5);
    QVERIFY(torrent5->exists);

    const auto torrent5Id = (*torrent5)[ID];

    torrent5->tags()->attach(tag100[ID],
                             {{"active", false}},
                             false);

    auto taggeds = Tagged::whereEq("torrent_id", torrent5Id)
                   ->whereIn("tag_id", {tag100[ID]})
                   .get();

    QCOMPARE(taggeds.size(), 1);

    // Expected tag IDs
    QList<QVariant> tagIds {tag100[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(tagIds.contains(tagged["tag_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    auto affected = torrent5->tags()->detach(tag100[ID], false);

    QCOMPARE(affected, 1);

    auto taggedsSize = Tagged::whereEq("torrent_id", torrent5Id)
                       ->whereIn("tag_id", {tag100[ID]})
                       .count();

    QCOMPARE(taggedsSize, 0);

    // Restore db
    tag100.remove();
}

void tst_Relations_Inserting_Updating::detach_CustomPivot_WithIds() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QList<QVariant> tagIds {tag100[ID], tag101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(tagIds.contains(tagged["tag_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    auto affected = torrent5->tags()->detach({tag100[ID], tag101[ID]}, false);

    QCOMPARE(affected, 2);

    auto taggedsSize = Tagged::whereEq("torrent_id", torrent5Id)
                       ->whereIn("tag_id", {tag100[ID], tag101[ID]})
                       .count();

    QCOMPARE(taggedsSize, 0);

    // Restore db
    tag100.remove();
    tag101.remove();
}

void tst_Relations_Inserting_Updating::detach_CustomPivot_WithModels() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QList<QVariant> tagIds {tag100[ID], tag101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(tagIds.contains(tagged["tag_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    auto affected = torrent5->tags()->detach({tag100, tag101}, false);

    QCOMPARE(affected, 2);

    auto taggedsSize = Tagged::whereEq("torrent_id", torrent5Id)
                       ->whereIn("tag_id", {tag100[ID], tag101[ID]})
                       .count();

    QCOMPARE(taggedsSize, 0);

    // Restore db
    tag100.remove();
    tag101.remove();
}

void tst_Relations_Inserting_Updating::detach_CustomPivot_All() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QList<QVariant> tagIds {tag100[ID], tag101[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(tagIds.contains(tagged["tag_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(false));
    }

    auto affected = torrent5->tags()->detachAll(false);

    QCOMPARE(affected, 2);

    auto taggedsSize = Tagged::whereEq("torrent_id", torrent5Id)
                       ->whereIn("tag_id", {tag100[ID], tag101[ID]})
                       .count();

    QCOMPARE(taggedsSize, 0);

    // Restore db
    tag100.remove();
    tag101.remove();
}

void tst_Relations_Inserting_Updating::updateExistingPivot_BasicPivot_WithId() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {SIZE_, 100}, {Progress, 555},
        {HASH_, "xyzhash100"}, {NOTE, "sync with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {SIZE_, 101}, {Progress, 556},
        {HASH_, "xyzhash101"}, {NOTE, "sync with pivot"},
    };
    torrent101.save();
    Torrent torrent102 {
        {NAME, "test102"}, {SIZE_, 102}, {Progress, 557},
        {HASH_, "xyzhash102"}, {NOTE, "sync with pivot"},
    };
    torrent102.save();
    Torrent torrent103 {
        {NAME, "test103"}, {SIZE_, 103}, {Progress, 558},
        {HASH_, "xyzhash103"}, {NOTE, "sync with pivot"},
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
    QList<QVariant> torrentIds {torrent101[ID], torrent102[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["tag_id"].value(), tagId);
        QVERIFY(torrentIds.contains(tagged["torrent_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(true));
    }

    const auto changed =
            tag5->torrents()->sync({*torrent100[ID], *torrent101[ID],
                                    *torrent103[ID]});

    // Verify result
    QVERIFY(changed.isValidKey(Attached));
    QVERIFY(changed.isValidKey(Detached));
    QVERIFY(changed.isValidKey(Updated_));

    const auto &attachedVector = changed.at(Attached);
    QCOMPARE(attachedVector.size(), 2);
    const auto &detachedVector = changed.at(Detached);
    QCOMPARE(detachedVector.size(), 1);
    QVERIFY(changed.at(Updated_).isEmpty());

    const QList<QVariant> expectedAttached {torrent100[ID], torrent103[ID]};
    const QList<QVariant> expectedDetached {torrent102[ID]};

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
        QCOMPARE(typeid (tagged), typeid (Tagged));

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {SIZE_, 100}, {Progress, 555},
        {HASH_, "xyzhash100"}, {NOTE, "sync with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {SIZE_, 101}, {Progress, 556},
        {HASH_, "xyzhash101"}, {NOTE, "sync with pivot"},
    };
    torrent101.save();
    Torrent torrent102 {
        {NAME, "test102"}, {SIZE_, 102}, {Progress, 557},
        {HASH_, "xyzhash102"}, {NOTE, "sync with pivot"},
    };
    torrent102.save();
    Torrent torrent103 {
        {NAME, "test103"}, {SIZE_, 103}, {Progress, 558},
        {HASH_, "xyzhash103"}, {NOTE, "sync with pivot"},
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
    QList<QVariant> torrentIds {torrent101[ID], torrent102[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

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
    QVERIFY(changed.isValidKey(Attached));
    QVERIFY(changed.isValidKey(Detached));
    QVERIFY(changed.isValidKey(Updated_));

    const auto &attachedVector = changed.at(Attached);
    QCOMPARE(attachedVector.size(), 2);
    const auto &detachedVector = changed.at(Detached);
    QCOMPARE(detachedVector.size(), 1);
    const auto &updatedVector = changed.at(Updated_);
    QCOMPARE(updatedVector.size(), 1);

    const QList<QVariant> expectedAttached {torrent100[ID], torrent103[ID]};
    const QList<QVariant> expectedDetached {torrent102[ID]};
    const QList<QVariant> expectedUpdated {torrent101[ID]};

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
        QCOMPARE(typeid (tagged), typeid (Tagged));

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QList<QVariant> tagIds {tag101[ID], tag102[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(tagIds.contains(tagged["tag_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(true));
    }

    const auto changed =
            torrent5->tags()->sync({*tag100[ID], *tag101[ID], *tag103[ID]});

    // Verify result
    QVERIFY(changed.isValidKey(Attached));
    QVERIFY(changed.isValidKey(Detached));
    QVERIFY(changed.isValidKey(Updated_));

    const auto &attachedVector = changed.at(Attached);
    QCOMPARE(attachedVector.size(), 2);
    const auto &detachedVector = changed.at(Detached);
    QCOMPARE(detachedVector.size(), 1);
    QVERIFY(changed.at(Updated_).isEmpty());

    const QList<QVariant> expectedAttached {tag100[ID], tag103[ID]};
    const QList<QVariant> expectedDetached {tag102[ID]};

    for (const auto &attached : attachedVector)
        QVERIFY(expectedAttached.contains(attached));
    for (const auto &detached : detachedVector)
        QVERIFY(expectedDetached.contains(detached));

    // Verify tagged values in the database
    taggeds = Tagged::whereEq("torrent_id", torrent5Id)
              ->whereIn("tag_id", {tag100[ID], tag101[ID], tag102[ID], tag103[ID]})
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
        QCOMPARE(typeid (tagged), typeid (Tagged));

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QList<QVariant> tagIds {tag101[ID], tag102[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

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
    QVERIFY(changed.isValidKey(Attached));
    QVERIFY(changed.isValidKey(Detached));
    QVERIFY(changed.isValidKey(Updated_));

    const auto &attachedVector = changed.at(Attached);
    QCOMPARE(attachedVector.size(), 2);
    const auto &detachedVector = changed.at(Detached);
    QCOMPARE(detachedVector.size(), 1);
    const auto &updatedVector = changed.at(Updated_);
    QCOMPARE(updatedVector.size(), 1);

    const QList<QVariant> expectedAttached {tag100[ID], tag103[ID]};
    const QList<QVariant> expectedDetached {tag102[ID]};
    const QList<QVariant> expectedUpdated {tag101[ID]};

    for (const auto &attached : attachedVector)
        QVERIFY(expectedAttached.contains(attached));
    for (const auto &detached : detachedVector)
        QVERIFY(expectedDetached.contains(detached));
    for (const auto &updated : updatedVector)
        QVERIFY(expectedUpdated.contains(updated));

    // Verify tagged values in the database
    taggeds = Tagged::whereEq("torrent_id", torrent5Id)
              ->whereIn("tag_id", {tag100[ID], tag101[ID], tag102[ID], tag103[ID]})
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
        QCOMPARE(typeid (tagged), typeid (Tagged));

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {SIZE_, 100}, {Progress, 555},
        {HASH_, "xyzhash100"}, {NOTE, "sync with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {SIZE_, 101}, {Progress, 556},
        {HASH_, "xyzhash101"}, {NOTE, "sync with pivot"},
    };
    torrent101.save();
    Torrent torrent102 {
        {NAME, "test102"}, {SIZE_, 102}, {Progress, 557},
        {HASH_, "xyzhash102"}, {NOTE, "sync with pivot"},
    };
    torrent102.save();
    Torrent torrent103 {
        {NAME, "test103"}, {SIZE_, 103}, {Progress, 558},
        {HASH_, "xyzhash103"}, {NOTE, "sync with pivot"},
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
    QList<QVariant> torrentIds {torrent101[ID], torrent102[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["tag_id"].value(), tagId);
        QVERIFY(torrentIds.contains(tagged["torrent_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(true));
    }

    const auto changed =
            tag5->torrents()->syncWithoutDetaching({*torrent100[ID], *torrent101[ID],
                                                    *torrent103[ID]});

    // Verify result
    QVERIFY(changed.isValidKey(Attached));
    QVERIFY(changed.isValidKey(Detached));
    QVERIFY(changed.isValidKey(Updated_));

    const auto &attachedVector = changed.at(Attached);
    QCOMPARE(attachedVector.size(), 2);
    QVERIFY(changed.at(Detached).isEmpty());
    QVERIFY(changed.at(Updated_).isEmpty());

    const QList<QVariant> expectedAttached {torrent100[ID], torrent103[ID]};

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
        QCOMPARE(typeid (tagged), typeid (Tagged));

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

void tst_Relations_Inserting_Updating::
     syncWithoutDetaching_BasicPivot_IdsWithAttributes() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    Torrent torrent100 {
        {NAME, "test100"}, {SIZE_, 100}, {Progress, 555},
        {HASH_, "xyzhash100"}, {NOTE, "sync with pivot"},
    };
    torrent100.save();
    Torrent torrent101 {
        {NAME, "test101"}, {SIZE_, 101}, {Progress, 556},
        {HASH_, "xyzhash101"}, {NOTE, "sync with pivot"},
    };
    torrent101.save();
    Torrent torrent102 {
        {NAME, "test102"}, {SIZE_, 102}, {Progress, 557},
        {HASH_, "xyzhash102"}, {NOTE, "sync with pivot"},
    };
    torrent102.save();
    Torrent torrent103 {
        {NAME, "test103"}, {SIZE_, 103}, {Progress, 558},
        {HASH_, "xyzhash103"}, {NOTE, "sync with pivot"},
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
    QList<QVariant> torrentIds {torrent101[ID], torrent102[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

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
    QVERIFY(changed.isValidKey(Attached));
    QVERIFY(changed.isValidKey(Detached));
    QVERIFY(changed.isValidKey(Updated_));

    const auto &attachedVector = changed.at(Attached);
    QCOMPARE(attachedVector.size(), 2);
    QVERIFY(changed.at(Detached).isEmpty());
    const auto &updatedVector = changed.at(Updated_);
    QCOMPARE(updatedVector.size(), 1);

    const QList<QVariant> expectedAttached {torrent100[ID], torrent103[ID]};
    const QList<QVariant> expectedUpdated {torrent101[ID]};

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
        QCOMPARE(typeid (tagged), typeid (Tagged));

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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QList<QVariant> tagIds {tag101[ID], tag102[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

        QCOMPARE(tagged.getAttributes().size(), 5);

        QCOMPARE(tagged["torrent_id"].value(), torrent5Id);
        QVERIFY(tagIds.contains(tagged["tag_id"].value()));
        QCOMPARE(tagged["active"].value(), QVariant(true));
    }

    const auto changed =
            torrent5->tags()->syncWithoutDetaching({*tag100[ID], *tag101[ID],
                                                    *tag103[ID]});

    // Verify result
    QVERIFY(changed.isValidKey(Attached));
    QVERIFY(changed.isValidKey(Detached));
    QVERIFY(changed.isValidKey(Updated_));

    const auto &attachedVector = changed.at(Attached);
    QCOMPARE(attachedVector.size(), 2);
    QVERIFY(changed.at(Detached).isEmpty());
    QVERIFY(changed.at(Updated_).isEmpty());

    const QList<QVariant> expectedAttached {tag100[ID], tag103[ID]};

    for (const auto &attached : changed.at(Attached))
        QVERIFY(expectedAttached.contains(attached));

    // Verify tagged values in the database
    taggeds = Tagged::whereEq("torrent_id", torrent5Id)
              ->whereIn("tag_id", {tag100[ID], tag101[ID], tag102[ID], tag103[ID]})
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
        QCOMPARE(typeid (tagged), typeid (Tagged));

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

void tst_Relations_Inserting_Updating::
     syncWithoutDetaching_CustomPivot_IdsWithAttributes() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

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
    QList<QVariant> tagIds {tag101[ID], tag102[ID]};

    for (auto &tagged : taggeds) {
        QVERIFY(tagged.exists);
        QCOMPARE(typeid (tagged), typeid (Tagged));

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
    QVERIFY(changed.isValidKey(Attached));
    QVERIFY(changed.isValidKey(Detached));
    QVERIFY(changed.isValidKey(Updated_));

    const auto &attachedVector = changed.at(Attached);
    QCOMPARE(attachedVector.size(), 2);
    QVERIFY(changed.at(Detached).isEmpty());
    const auto &updatedVector = changed.at(Updated_);
    QCOMPARE(updatedVector.size(), 1);

    const QList<QVariant> expectedAttached {tag100[ID], tag103[ID]};
    const QList<QVariant> expectedUpdated {tag101[ID]};

    for (const auto &attached : attachedVector)
        QVERIFY(expectedAttached.contains(attached));
    for (const auto &updated : updatedVector)
        QVERIFY(expectedUpdated.contains(updated));

    // Verify tagged values in the database
    taggeds = Tagged::whereEq("torrent_id", torrent5Id)
              ->whereIn("tag_id", {tag100[ID], tag101[ID], tag102[ID], tag103[ID]})
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
        QCOMPARE(typeid (tagged), typeid (Tagged));

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
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_Relations_Inserting_Updating)

#include "tst_relations_inserting_updating.moc"
