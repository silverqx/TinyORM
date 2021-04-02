#include <QCoreApplication>
#include <QtTest>

#include "models/torrent.hpp"
#include "models/torrenteager.hpp"
#include "models/torrenteager_failed.hpp"
#include "models/torrentpeer.hpp"
#include "models/torrentpeereager.hpp"
#include "models/torrentpeereager_norelations.hpp"

#include "database.hpp"

using Orm::One;
using Orm::RuntimeError;
using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::RelationNotFoundError;
using Orm::Tiny::RelationNotLoadedError;

class tst_BaseModel_Relations : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase_data() const;

    void getRelation_EagerLoad_ManyAndOne() const;
    void getRelation_EagerLoad_BelongsTo() const;
    void getRelation_EagerLoad_Failed() const;
    void EagerLoad_Failed() const;

    void getRelationValue_LazyLoad_ManyAndOne() const;
    void getRelationValue_LazyLoad_BelongsTo() const;
    void getRelationValue_LazyLoad_Failed() const;

    void u_with_Empty() const;
    void with_HasOne() const;
    void with_HasMany() const;
    void with_BelongsTo() const;
    void with_NestedRelations() const;
    void with_Vector_MoreRelations() const;
    void with_NonExistentRelation_Failed() const;

    void without() const;
    void without_NestedRelations() const;
    void without_Vector_MoreRelations() const;

    void load() const;
    void load_Failed() const;

    void refresh_EagerLoad_OnlyRelations() const;
    void refresh_LazyLoad_OnlyRelations() const;

    void push_EagerLoad() const;
    void push_LazyLoad() const;
    // TEST test all return paths for push() silverqx

    void where_WithCallback() const;
    void orWhere_WithCallback() const;
};

void tst_BaseModel_Relations::initTestCase_data() const
{
    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : TestUtils::Database::createConnections())
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

void tst_BaseModel_Relations::getRelation_EagerLoad_ManyAndOne() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = TorrentEager::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // TorrentPreviewableFileEager has many relation
    auto files = torrent->getRelation<TorrentPreviewableFileEager>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (QVector<TorrentPreviewableFileEager *>), typeid (files));

    // Expected file IDs
    QVector<QVariant> fileIds {2, 3};
    // Expected file property IDs
    QVector<QVariant> filePropertyIds {1, 2};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getAttribute("id"));
        QVERIFY(fileIds.contains(file->getAttribute("id")));
        QCOMPARE(typeid (TorrentPreviewableFileEager *), typeid (file));

        /* TorrentPreviewableFilePropertyEager has one relation, loaded by
           dot notation in the u_with data member. */
        auto *fileProperty =
                file->getRelation<TorrentPreviewableFilePropertyEager, One>(
                    "fileProperty");
        QVERIFY(fileProperty);
        QVERIFY(fileProperty->exists);
        QCOMPARE(typeid (TorrentPreviewableFilePropertyEager *), typeid (fileProperty));
        QVERIFY(filePropertyIds.contains(fileProperty->getAttribute("id")));
        QCOMPARE(fileProperty->getAttribute("previewable_file_id"),
                 file->getAttribute("id"));
    }
}

void tst_BaseModel_Relations::getRelation_EagerLoad_BelongsTo() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrentPeer = TorrentPeerEager::find(2);
    QVERIFY(torrentPeer);
    QVERIFY(torrentPeer->exists);
    QCOMPARE(torrentPeer->getAttribute("torrent_id"), QVariant(2));

    // TorrentEager belongs to relation
    auto *torrent = torrentPeer->getRelation<TorrentEager, One>("torrent");
    QVERIFY(torrent);
    QCOMPARE(torrent->getAttribute("id"), torrentPeer->getAttribute("torrent_id"));
    QCOMPARE(typeid (TorrentEager *), typeid (torrent));
}

void tst_BaseModel_Relations::getRelation_EagerLoad_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent;

    // Many relation
    QVERIFY_EXCEPTION_THROWN(
                (torrent.getRelation<TorrentPreviewableFile>("torrentFiles")),
                RelationNotLoadedError);
    // One relation, obtained as QVector, also possible
    QVERIFY_EXCEPTION_THROWN(
                (torrent.getRelation<TorrentPeer>("torrentFiles")),
                RelationNotLoadedError);
    // Many relation
    QVERIFY_EXCEPTION_THROWN(
                (torrent.getRelation<TorrentPeer, One>("torrentFiles")),
                RelationNotLoadedError);
    // Belongs to relation
    QVERIFY_EXCEPTION_THROWN(
                (TorrentPeer().getRelation<Torrent, One>("torrent")),
                RelationNotLoadedError);
}

void tst_BaseModel_Relations::EagerLoad_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    QVERIFY_EXCEPTION_THROWN(TorrentEager_Failed::find(1),
                             RelationNotFoundError);
}

void tst_BaseModel_Relations::getRelationValue_LazyLoad_ManyAndOne() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // TorrentPreviewableFile has many relation
    auto files = torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (QVector<TorrentPreviewableFile *>), typeid (files));

    // Expected file IDs
    QVector<QVariant> fileIds {2, 3};
    // Expected file property IDs
    QVector<QVariant> filePropertyIds {1, 2};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getAttribute("id"));
        QVERIFY(fileIds.contains(file->getAttribute("id")));
        QCOMPARE(typeid (TorrentPreviewableFile *), typeid (file));

        /* TorrentPreviewableFileProperty has one relation, loaded by
           dot notation in the u_with data member. */
        auto *fileProperty =
                file->getRelationValue<TorrentPreviewableFileProperty, One>(
                    "fileProperty");
        QVERIFY(fileProperty);
        QVERIFY(fileProperty->exists);
        QCOMPARE(typeid (TorrentPreviewableFileProperty *), typeid (fileProperty));
        QVERIFY(filePropertyIds.contains(fileProperty->getAttribute("id")));
        QCOMPARE(fileProperty->getAttribute("previewable_file_id"),
                 file->getAttribute("id"));
    }
}

void tst_BaseModel_Relations::getRelationValue_LazyLoad_BelongsTo() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrentPeer = TorrentPeer::find(2);
    QVERIFY(torrentPeer);
    QVERIFY(torrentPeer->exists);
    QCOMPARE(torrentPeer->getAttribute("torrent_id"), QVariant(2));

    // Torrent belongs to relation
    auto *torrent = torrentPeer->getRelationValue<Torrent, One>("torrent");
    QVERIFY(torrent);
    QCOMPARE(torrent->getAttribute("id"), QVariant(2));
    QCOMPARE(typeid (Torrent *), typeid (torrent));
}

void tst_BaseModel_Relations::getRelationValue_LazyLoad_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    // Many relation
    QCOMPARE((Torrent().getRelationValue<TorrentPreviewableFile>("notExists")),
             QVector<TorrentPreviewableFile *>());
    // One relation
    QCOMPARE((Torrent().getRelationValue<TorrentPeer, One>("notExists")),
             nullptr);
    // One relation, obtained as QVector, also possible
    QCOMPARE((Torrent().getRelationValue<TorrentPeer>("notExists")),
             QVector<TorrentPeer *>());
}

void tst_BaseModel_Relations::u_with_Empty() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    Torrent torrent;

    QCOMPARE(torrent.getRelations().size(), 0);
}

void tst_BaseModel_Relations::with_HasOne() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with("torrentPeer")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto *peer = torrent->getRelation<TorrentPeer, One>("torrentPeer");
    QVERIFY(peer);
    QVERIFY(peer->exists);
    QCOMPARE(peer->getAttribute("id"), QVariant(2));
    QCOMPARE(peer->getAttribute("torrent_id"), torrent->getAttribute("id"));
    QCOMPARE(typeid (TorrentPeer *), typeid (peer));
}

void tst_BaseModel_Relations::with_HasMany() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with("torrentFiles")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (QVector<TorrentPreviewableFile *>), typeid (files));

    // Expected file IDs
    QVector<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getAttribute("id"));
        QVERIFY(fileIds.contains(file->getAttribute("id")));
        QCOMPARE(typeid (TorrentPreviewableFile *), typeid (file));
    }
}

void tst_BaseModel_Relations::with_BelongsTo() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto fileProperty = TorrentPreviewableFileProperty::with("torrentFile")->find(2);
    QVERIFY(fileProperty);
    QVERIFY(fileProperty->exists);
    QCOMPARE(fileProperty->getAttribute("previewable_file_id"), QVariant(3));

    auto *file = fileProperty->getRelation<TorrentPreviewableFile, One>("torrentFile");
    QVERIFY(file);
    QVERIFY(file->exists);
    QCOMPARE(file->getAttribute("id"), QVariant(3));
    QCOMPARE(typeid (TorrentPreviewableFile *), typeid (file));
}

void tst_BaseModel_Relations::with_NestedRelations() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with("torrentFiles.fileProperty")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // TorrentPreviewableFile has many relation
    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (QVector<TorrentPreviewableFile *>), typeid (files));

    // Expected file IDs
    QVector<QVariant> fileIds {2, 3};
    // Expected file property IDs
    QVector<QVariant> filePropertyIds {1, 2};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getAttribute("id"));
        QVERIFY(fileIds.contains(file->getAttribute("id")));
        QCOMPARE(typeid (TorrentPreviewableFile *), typeid (file));

        /* TorrentPreviewableFileProperty has one relation, loaded by
           dot notation in the u_with data member. */
        auto *fileProperty =
                file->getRelation<TorrentPreviewableFileProperty, One>(
                    "fileProperty");
        QVERIFY(fileProperty);
        QVERIFY(fileProperty->exists);
        QCOMPARE(typeid (TorrentPreviewableFileProperty *), typeid (fileProperty));
        QVERIFY(filePropertyIds.contains(fileProperty->getAttribute("id")));
        QCOMPARE(fileProperty->getAttribute("previewable_file_id"),
                 file->getAttribute("id"));
    }
}

void tst_BaseModel_Relations::with_Vector_MoreRelations() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::with({{"torrentFiles"}, {"torrentPeer"}})->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // TorrentPeer has one relation
    auto *peer = torrent->getRelation<TorrentPeer, One>("torrentPeer");
    QVERIFY(peer);
    QVERIFY(peer->exists);
    QCOMPARE(peer->getAttribute("id"), QVariant(2));
    QCOMPARE(peer->getAttribute("torrent_id"), torrent->getAttribute("id"));

    // TorrentPreviewableFile has many relation
    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (QVector<TorrentPreviewableFile *>), typeid (files));

    // Expected file IDs
    QVector<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getAttribute("id"));
        QVERIFY(fileIds.contains(file->getAttribute("id")));
        QCOMPARE(typeid (TorrentPreviewableFile *), typeid (file));

        // No TorrentPreviewableFileProperty loaded
        QVERIFY_EXCEPTION_THROWN(
                    (file->getRelation<TorrentPreviewableFileProperty, One>(
                         "fileProperty")),
                    RuntimeError);
    }
}

void tst_BaseModel_Relations::with_NonExistentRelation_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    QVERIFY_EXCEPTION_THROWN(Torrent::with("torrentFiles-NON_EXISTENT")->find(1),
                             RelationNotFoundError);
}

void tst_BaseModel_Relations::without() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = TorrentEager::without("torrentPeer")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto &relations = torrent->getRelations();
    QVERIFY(!relations.contains("torrentPeer"));
    QVERIFY(relations.contains("torrentFiles"));
    QCOMPARE(relations.size(), 1);
}

void tst_BaseModel_Relations::without_NestedRelations() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = TorrentEager::without("torrentFiles")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto &relations = torrent->getRelations();
    QVERIFY(!relations.contains("torrentFiles"));
    QVERIFY(relations.contains("torrentPeer"));
    QCOMPARE(relations.size(), 1);
}

void tst_BaseModel_Relations::without_Vector_MoreRelations() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = TorrentEager::without({"torrentPeer", "torrentFiles"})->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVERIFY(torrent->getRelations().empty());
}

void tst_BaseModel_Relations::load() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);

    QVERIFY(torrent->getRelations().empty());

    torrent->load({{"torrentFiles"}, {"torrentPeer"}});

    const auto &relations = torrent->getRelations();
    QVERIFY(relations.size() == 2);
    QVERIFY(relations.contains("torrentFiles"));
    QVERIFY(relations.contains("torrentPeer"));

    // TorrentPeer has one relation
    auto peer = torrent->getRelation<TorrentPeer, One>("torrentPeer");
    QVERIFY(peer);
    QVERIFY(peer->exists);
    QCOMPARE(peer->getAttribute("torrent_id"), torrent->getAttribute("id"));
    QCOMPARE(peer->getAttribute("id"), QVariant(2));
    QCOMPARE(typeid (TorrentPeer *), typeid (peer));

    // TorrentPreviewableFile has many relation
    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (QVector<TorrentPreviewableFile *>), typeid (files));

    // Expected file IDs
    QVector<QVariant> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getAttribute("id"));
        QVERIFY(fileIds.contains(file->getAttribute("id")));
        QCOMPARE(typeid (TorrentPreviewableFile *), typeid (file));

        // No TorrentPreviewableFileProperty loaded
        QVERIFY_EXCEPTION_THROWN(
                    (file->getRelation<TorrentPreviewableFileProperty, One>(
                         "fileProperty")),
                    RuntimeError);
    }
}

void tst_BaseModel_Relations::load_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);

    QVERIFY(torrent->getRelations().empty());

    QVERIFY_EXCEPTION_THROWN(torrent->load("torrentFiles-NON_EXISTENT"),
                             RelationNotFoundError);
    QVERIFY(torrent->getRelations().empty());
}

void tst_BaseModel_Relations::refresh_EagerLoad_OnlyRelations() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = TorrentEager::find(3);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto &relations = torrent->getRelations();
    QVERIFY(relations.size() == 2);

    // Validate original attribute values in relations
    auto filesOriginal =
            torrent->getRelation<TorrentPreviewableFileEager>("torrentFiles");
    auto filepathOriginal =
            filesOriginal.first()->getAttribute("filepath");
    auto peerOriginal =
            torrent->getRelation<TorrentPeerEager_NoRelations, One>("torrentPeer");
    auto seedsOriginal =
            peerOriginal->getAttribute("seeds");
    QVERIFY(filepathOriginal == QVariant("test3_file1.mkv"));
    QVERIFY(seedsOriginal == QVariant(3));

    // Change attributes in relations
    filesOriginal.first()->setAttribute("filepath", "test3_file1-refresh.mkv");
    peerOriginal->setAttribute("seeds", 33);

    // Validate changed attributes in relations
    auto filepathOriginalChanged =
            torrent->getRelationValue<TorrentPreviewableFileEager>("torrentFiles")
            .first()->getAttribute("filepath");
    auto seedsOriginalChanged =
            torrent->getRelationValue<TorrentPeerEager_NoRelations, One>("torrentPeer")
            ->getAttribute("seeds");
    QVERIFY(filepathOriginalChanged == QVariant("test3_file1-refresh.mkv"));
    QVERIFY(seedsOriginalChanged == QVariant(33));

    // Memory address of the key and value for the relation
    uintptr_t relationFilesKeyOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->first);
    uintptr_t relationFilesValueOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->second);
    uintptr_t relationPeerKeyOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentPeer")->first);
    uintptr_t relationPeerValueOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentPeer")->second);

    torrent->refresh();

    QVERIFY(relations.size() == 2);
    /* Values in the std::unordered_map container has to be the same, because
       only loaded relations will be replaced with std::move directly
       to the relation std::variant reference in the Model::load() method. */
    QVERIFY(relationFilesKeyOriginal
            == reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->first));
    QVERIFY(relationFilesValueOriginal
            == reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->second));
    QVERIFY(relationPeerKeyOriginal
            == reinterpret_cast<uintptr_t>(&relations.find("torrentPeer")->first));
    QVERIFY(relationPeerValueOriginal
            == reinterpret_cast<uintptr_t>(&relations.find("torrentPeer")->second));

    // Validate refreshed attributes in relations
    auto filesRefreshed =
            torrent->getRelation<TorrentPreviewableFileEager>("torrentFiles");
    auto filepathRefreshed = filesRefreshed.first()->getAttribute("filepath");
    auto peerRefreshed =
            torrent->getRelation<TorrentPeerEager_NoRelations, One>("torrentPeer");
    auto seedsRefreshed = peerRefreshed->getAttribute("seeds");
    QVERIFY(filepathOriginal == filepathRefreshed);
    QVERIFY(seedsOriginal == seedsRefreshed);
}

void tst_BaseModel_Relations::refresh_LazyLoad_OnlyRelations() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(3);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto &relations = torrent->getRelations();
    QVERIFY(relations.empty());

    // Validate original attribute values in relations
    auto filesOriginal =
            torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    auto filepathOriginal =
            filesOriginal.first()->getAttribute("filepath");
    auto peerOriginal =
            torrent->getRelationValue<TorrentPeer, One>("torrentPeer");
    auto seedsOriginal =
            peerOriginal->getAttribute("seeds");
    QVERIFY(relations.size() == 2);
    QVERIFY(filepathOriginal == QVariant("test3_file1.mkv"));
    QVERIFY(seedsOriginal == QVariant(3));

    // Change attributes in relations
    filesOriginal.first()->setAttribute("filepath", "test3_file1-refresh.mkv");
    peerOriginal->setAttribute("seeds", 33);

    // Validate changed attributes in relations
    auto filepathOriginalChanged =
            torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles")
            .first()->getAttribute("filepath");
    auto seedsOriginalChanged =
            torrent->getRelationValue<TorrentPeer, One>("torrentPeer")
            ->getAttribute("seeds");
    QVERIFY(filepathOriginalChanged == QVariant("test3_file1-refresh.mkv"));
    QVERIFY(seedsOriginalChanged == QVariant(33));

    // Memory address of the key and value for the relation
    uintptr_t relationFilesKeyOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->first);
    uintptr_t relationFilesValueOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->second);
    uintptr_t relationPeerKeyOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentPeer")->first);
    uintptr_t relationPeerValueOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentPeer")->second);

    torrent->refresh();

    QVERIFY(relations.size() == 2);
    /* Values in the std::unordered_map container has to be the same, because
       only loaded relations will be replaced with std::move directly
       to the relation std::variant reference in the Model::load() method. */
    QVERIFY(relationFilesKeyOriginal
            == reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->first));
    QVERIFY(relationFilesValueOriginal
            == reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->second));
    QVERIFY(relationPeerKeyOriginal
            == reinterpret_cast<uintptr_t>(&relations.find("torrentPeer")->first));
    QVERIFY(relationPeerValueOriginal
            == reinterpret_cast<uintptr_t>(&relations.find("torrentPeer")->second));

    // Validate refreshed attributes in relations
    auto filesRefreshed =
            torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    auto filepathRefreshed = filesRefreshed.first()->getAttribute("filepath");
    auto peerRefreshed =
            torrent->getRelationValue<TorrentPeer, One>("torrentPeer");
    auto seedsRefreshed = peerRefreshed->getAttribute("seeds");
    QVERIFY(filepathOriginal == filepathRefreshed);
    QVERIFY(seedsOriginal == seedsRefreshed);
}

void tst_BaseModel_Relations::push_EagerLoad() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = TorrentEager::find(2);

    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    /* Also contains torrentPeer relation, which is not needed for this test, but
       I will not create the new eager model class. */
    QCOMPARE(torrent->getRelations().size(), 2);

    auto files = torrent->getRelation<TorrentPreviewableFileEager>("torrentFiles");
    /* Make a copy of a pointer, because first() returns reference,
       reference to pointer is not needed. */
    auto *file = files.first();
    auto *fileProperty =
            file->getRelation<TorrentPreviewableFilePropertyEager, One>("fileProperty");

    QCOMPARE(files.size(), 2);
    QVERIFY(file);
    QVERIFY(file->exists);
    QVERIFY(fileProperty);
    QVERIFY(fileProperty->exists);

    auto torrentNameOriginal = torrent->getAttribute("name");
    auto fileFilepathOriginal = file->getAttribute("filepath");
    auto propertyNameOriginal = fileProperty->getAttribute("name");

    QCOMPARE(torrentNameOriginal, QVariant("test2"));
    QCOMPARE(fileFilepathOriginal, QVariant("test2_file1.mkv"));
    QCOMPARE(propertyNameOriginal, QVariant("test2_file1"));

    // Modify values in relations
    torrent->setAttribute("name", "test2 push");
    file->setAttribute("filepath", "test2_file1-push.mkv");
    fileProperty->setAttribute("name", "test2_file1 push");

    QVERIFY(torrent->push());

    // Verify saved values
    auto torrentVerify = TorrentEager::find(2);

    QVERIFY(torrentVerify);
    QVERIFY(torrentVerify->exists);

    QCOMPARE(torrent->getRelations().size(), 2);

    auto filesVerify =
            torrentVerify->getRelation<TorrentPreviewableFileEager>("torrentFiles");
    auto *fileVerify = filesVerify.first();
    auto *filePropertyVerify =
            fileVerify->getRelation<TorrentPreviewableFilePropertyEager, One>(
                "fileProperty");

    QCOMPARE(filesVerify.size(), 2);
    QVERIFY(fileVerify);
    QVERIFY(fileVerify->exists);
    QVERIFY(filePropertyVerify);
    QVERIFY(filePropertyVerify->exists);

    QCOMPARE(torrentVerify->getAttribute("name"), QVariant("test2 push"));
    QCOMPARE(fileVerify->getAttribute("filepath"), QVariant("test2_file1-push.mkv"));
    QCOMPARE(filePropertyVerify->getAttribute("name"), QVariant("test2_file1 push"));

    // Revert values back
    torrentVerify->setAttribute("name", "test2");
    fileVerify->setAttribute("filepath", "test2_file1.mkv");
    filePropertyVerify->setAttribute("name", "test2_file1");

    torrentVerify->push();
}

void tst_BaseModel_Relations::push_LazyLoad() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrent = Torrent::find(2);

    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVERIFY(torrent->getRelations().empty());

    auto files = torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    /* Make a copy of a pointer, because first() returns reference,
       reference to pointer is not needed. */
    auto *file = files.first();
    auto *fileProperty =
            file->getRelationValue<TorrentPreviewableFileProperty, One>("fileProperty");

    QCOMPARE(files.size(), 2);
    QVERIFY(file);
    QVERIFY(file->exists);
    QVERIFY(fileProperty);
    QVERIFY(fileProperty->exists);

    auto torrentNameOriginal = torrent->getAttribute("name");
    auto fileFilepathOriginal = file->getAttribute("filepath");
    auto propertyNameOriginal = fileProperty->getAttribute("name");

    QCOMPARE(torrentNameOriginal, QVariant("test2"));
    QCOMPARE(fileFilepathOriginal, QVariant("test2_file1.mkv"));
    QCOMPARE(propertyNameOriginal, QVariant("test2_file1"));

    // Modify values in relations
    torrent->setAttribute("name", "test2 push");
    file->setAttribute("filepath", "test2_file1-push.mkv");
    fileProperty->setAttribute("name", "test2_file1 push");

    QVERIFY(torrent->push());

    // Verify saved values
    auto torrentVerify = Torrent::find(2);

    QVERIFY(torrentVerify);
    QVERIFY(torrentVerify->exists);

    QVERIFY(torrentVerify->getRelations().empty());

    auto filesVerify =
            torrentVerify->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    auto *fileVerify = filesVerify.first();
    auto *filePropertyVerify =
            fileVerify->getRelationValue<TorrentPreviewableFileProperty, One>(
                "fileProperty");

    QCOMPARE(filesVerify.size(), 2);
    QVERIFY(fileVerify);
    QVERIFY(fileVerify->exists);
    QVERIFY(filePropertyVerify);
    QVERIFY(filePropertyVerify->exists);

    QCOMPARE(torrentVerify->getAttribute("name"), QVariant("test2 push"));
    QCOMPARE(fileVerify->getAttribute("filepath"), QVariant("test2_file1-push.mkv"));
    QCOMPARE(filePropertyVerify->getAttribute("name"), QVariant("test2_file1 push"));

    // Revert values back
    torrentVerify->setAttribute("name", "test2");
    fileVerify->setAttribute("filepath", "test2_file1.mkv");
    filePropertyVerify->setAttribute("name", "test2_file1");

    torrentVerify->push();
}

void tst_BaseModel_Relations::where_WithCallback() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto files = Torrent::find(5)->torrentFiles()
                 ->where([](auto &query)
    {
        return query.whereEq("id", 6)
                .orWhereEq("file_index", 2);
    })
                 .get();

    QCOMPARE(files.size(), 2);

    // Expected file IDs
    QVector<QVariant> fileIds {6, 8};
    for (auto &file : files) {
        QVERIFY(file.exists);
        QVERIFY(fileIds.contains(file["id"]));
        QCOMPARE(typeid (TorrentPreviewableFile), typeid (file));
    }
}

void tst_BaseModel_Relations::orWhere_WithCallback() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto files = Torrent::find(5)->torrentFiles()
                 ->where("progress", ">", 990)
                 .orWhere([](auto &query)
    {
        return query.whereEq("id", 8)
                .whereEq("file_index", 2);
    })
                 .get();

    QCOMPARE(files.size(), 2);

    // Expected file IDs
    QVector<QVariant> fileIds {6, 8};
    for (auto &file : files) {
        QVERIFY(file.exists);
        QVERIFY(fileIds.contains(file["id"]));
        QCOMPARE(typeid (TorrentPreviewableFile), typeid (file));
    }
}

QTEST_MAIN(tst_BaseModel_Relations)

#include "tst_basemodel_relations.moc"
