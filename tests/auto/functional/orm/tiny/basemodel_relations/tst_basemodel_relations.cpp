#include <QCoreApplication>
#include <QtTest>

#include "models/torrent.hpp"
#include "models/torrenteager.hpp"
#include "models/torrenteager_failed.hpp"
#include "models/torrentpeer.hpp"
#include "models/torrentpeereager.hpp"
#include "models/torrentpeereager_norelations.hpp"

#include "database.hpp"

using namespace Orm;
// TODO tests, namespace silverqx
using namespace Orm::Tiny;

class tst_BaseModel_Relations : public QObject
{
    Q_OBJECT

public:
    tst_BaseModel_Relations();
    ~tst_BaseModel_Relations() = default;

private slots:
    void initTestCase();
    void cleanupTestCase();

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

    void load() const;
    void load_Failed() const;

    void refresh_EagerLoad_OnlyRelations() const;
    void refresh_LazyLoad_OnlyRelations() const;

    void push_EagerLoad() const;
    void push_LazyLoad() const;
    // TODO tests, test all return paths for push() silverqx

private:
    /*! The database connection instance. */
    ConnectionInterface &m_connection;
};

tst_BaseModel_Relations::tst_BaseModel_Relations()
    : m_connection(TestUtils::Database::createConnection())
{}

void tst_BaseModel_Relations::initTestCase()
{}

void tst_BaseModel_Relations::cleanupTestCase()
{}

void tst_BaseModel_Relations::getRelation_EagerLoad_ManyAndOne() const
{
    auto torrent = TorrentEager::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // TorrentPreviewableFileEager has many relation
    auto files = torrent->getRelation<TorrentPreviewableFileEager>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (QVector<TorrentPreviewableFileEager *>), typeid (files));

    // Expected file IDs
    QVector<quint64> fileIds {2, 3};
    // Expected file property IDs
    QVector<quint64> filePropertyIds {1, 2};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getAttribute("id"));
        QVERIFY(fileIds.contains(file->getAttribute("id").toULongLong()));
        QCOMPARE(typeid (TorrentPreviewableFileEager *), typeid (file));

        /* TorrentPreviewableFilePropertyEager has one relation, loaded by
           dot notation in the u_with data member. */
        auto *fileProperty =
                file->getRelation<TorrentPreviewableFilePropertyEager, One>(
                    "fileProperty");
        QVERIFY(fileProperty);
        QVERIFY(fileProperty->exists);
        QCOMPARE(typeid (TorrentPreviewableFilePropertyEager *), typeid (fileProperty));
        QVERIFY(filePropertyIds.contains(fileProperty->getAttribute("id")
                                         .toULongLong()));
        QCOMPARE(fileProperty->getAttribute("previewable_file_id"),
                 file->getAttribute("id"));
    }
}

void tst_BaseModel_Relations::getRelation_EagerLoad_BelongsTo() const
{
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
    QVERIFY_EXCEPTION_THROWN(TorrentEager_Failed::find(1),
                             RelationNotFoundError);
}

void tst_BaseModel_Relations::getRelationValue_LazyLoad_ManyAndOne() const
{
    auto torrent = Torrent::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // TorrentPreviewableFile has many relation
    auto files = torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (QVector<TorrentPreviewableFile *>), typeid (files));

    // Expected file IDs
    QVector<quint64> fileIds {2, 3};
    // Expected file property IDs
    QVector<quint64> filePropertyIds {1, 2};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getAttribute("id"));
        QVERIFY(fileIds.contains(file->getAttribute("id").toULongLong()));
        QCOMPARE(typeid (TorrentPreviewableFile *), typeid (file));

        /* TorrentPreviewableFileProperty has one relation, loaded by
           dot notation in the u_with data member. */
        auto *fileProperty =
                file->getRelationValue<TorrentPreviewableFileProperty, One>(
                    "fileProperty");
        QVERIFY(fileProperty);
        QVERIFY(fileProperty->exists);
        QCOMPARE(typeid (TorrentPreviewableFileProperty *), typeid (fileProperty));
        QVERIFY(filePropertyIds.contains(fileProperty->getAttribute("id")
                                         .toULongLong()));
        QCOMPARE(fileProperty->getAttribute("previewable_file_id"),
                 file->getAttribute("id"));
    }
}

void tst_BaseModel_Relations::getRelationValue_LazyLoad_BelongsTo() const
{
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
    Torrent torrent;

    QCOMPARE(torrent.getRelations().size(), 0);
}

void tst_BaseModel_Relations::with_HasOne() const
{
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
    auto torrent = Torrent::with("torrentFiles")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (QVector<TorrentPreviewableFile *>), typeid (files));

    // Expected file IDs
    QVector<quint64> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getAttribute("id"));
        QVERIFY(fileIds.contains(file->getAttribute("id").toULongLong()));
        QCOMPARE(typeid (TorrentPreviewableFile *), typeid (file));
    }
}

void tst_BaseModel_Relations::with_BelongsTo() const
{
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
    auto torrent = Torrent::with("torrentFiles.fileProperty")->find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // TorrentPreviewableFile has many relation
    auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (QVector<TorrentPreviewableFile *>), typeid (files));

    // Expected file IDs
    QVector<quint64> fileIds {2, 3};
    // Expected file property IDs
    QVector<quint64> filePropertyIds {1, 2};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getAttribute("id"));
        QVERIFY(fileIds.contains(file->getAttribute("id").toULongLong()));
        QCOMPARE(typeid (TorrentPreviewableFile *), typeid (file));

        /* TorrentPreviewableFileProperty has one relation, loaded by
           dot notation in the u_with data member. */
        auto *fileProperty =
                file->getRelation<TorrentPreviewableFileProperty, One>(
                    "fileProperty");
        QVERIFY(fileProperty);
        QVERIFY(fileProperty->exists);
        QCOMPARE(typeid (TorrentPreviewableFileProperty *), typeid (fileProperty));
        QVERIFY(filePropertyIds.contains(fileProperty->getAttribute("id")
                                         .toULongLong()));
        QCOMPARE(fileProperty->getAttribute("previewable_file_id"),
                 file->getAttribute("id"));
    }
}

void tst_BaseModel_Relations::with_Vector_MoreRelations() const
{
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
    QVector<quint64> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getAttribute("id"));
        QVERIFY(fileIds.contains(file->getAttribute("id").toULongLong()));
        QCOMPARE(typeid (TorrentPreviewableFile *), typeid (file));

        // No TorrentPreviewableFileProperty loaded
        QVERIFY_EXCEPTION_THROWN(
                    (file->getRelation<TorrentPreviewableFileProperty, One>(
                         "fileProperty")),
                    OrmRuntimeError);
    }
}

void tst_BaseModel_Relations::with_NonExistentRelation_Failed() const
{
    QVERIFY_EXCEPTION_THROWN(Torrent::with("torrentFiles-NON_EXISTENT")->find(1),
                             RelationNotFoundError);
}

void tst_BaseModel_Relations::load() const
{
    auto torrent = Torrent::find(2);

    QVERIFY(torrent->getRelations().isEmpty());

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
    QVector<quint64> fileIds {2, 3};
    for (auto *file : files) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), torrent->getAttribute("id"));
        QVERIFY(fileIds.contains(file->getAttribute("id").toULongLong()));
        QCOMPARE(typeid (TorrentPreviewableFile *), typeid (file));

        // No TorrentPreviewableFileProperty loaded
        QVERIFY_EXCEPTION_THROWN(
                    (file->getRelation<TorrentPreviewableFileProperty, One>(
                         "fileProperty")),
                    OrmRuntimeError);
    }
}

void tst_BaseModel_Relations::load_Failed() const
{
    auto torrent = Torrent::find(2);

    QVERIFY(torrent->getRelations().isEmpty());

    QVERIFY_EXCEPTION_THROWN(torrent->load("torrentFiles-NON_EXISTENT"),
                             RelationNotFoundError);
    QVERIFY(torrent->getRelations().isEmpty());
}

void tst_BaseModel_Relations::refresh_EagerLoad_OnlyRelations() const
{
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
            reinterpret_cast<uintptr_t>(&relations.find("torrentFiles").key());
    uintptr_t relationFilesValueOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentFiles").value());
    uintptr_t relationPeerKeyOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentPeer").key());
    uintptr_t relationPeerValueOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentPeer").value());

    torrent->refresh();

    QVERIFY(relations.size() == 2);
    /* Values in the QHash container can't be the same, because they were
       moved from the Model copy in the Model::load() method. */
    QVERIFY(relationFilesKeyOriginal
            != reinterpret_cast<uintptr_t>(&relations.find("torrentFiles").key()));
    QVERIFY(relationFilesValueOriginal
            != reinterpret_cast<uintptr_t>(&relations.find("torrentFiles").value()));
    QVERIFY(relationPeerKeyOriginal
            != reinterpret_cast<uintptr_t>(&relations.find("torrentPeer").key()));
    QVERIFY(relationPeerValueOriginal
            != reinterpret_cast<uintptr_t>(&relations.find("torrentPeer").value()));

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
    auto torrent = Torrent::find(3);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    auto &relations = torrent->getRelations();
    QVERIFY(relations.isEmpty());

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
            reinterpret_cast<uintptr_t>(&relations.find("torrentFiles").key());
    uintptr_t relationFilesValueOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentFiles").value());
    uintptr_t relationPeerKeyOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentPeer").key());
    uintptr_t relationPeerValueOriginal =
            reinterpret_cast<uintptr_t>(&relations.find("torrentPeer").value());

    torrent->refresh();

    QVERIFY(relations.size() == 2);
    /* Values in the QHash container can't be the same, because they were
       moved from the Model copy in the Model::load() method. */
    QVERIFY(relationFilesKeyOriginal
            != reinterpret_cast<uintptr_t>(&relations.find("torrentFiles").key()));
    QVERIFY(relationFilesValueOriginal
            != reinterpret_cast<uintptr_t>(&relations.find("torrentFiles").value()));
    QVERIFY(relationPeerKeyOriginal
            != reinterpret_cast<uintptr_t>(&relations.find("torrentPeer").key()));
    QVERIFY(relationPeerValueOriginal
            != reinterpret_cast<uintptr_t>(&relations.find("torrentPeer").value()));

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
    auto torrent = Torrent::find(2);

    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    QVERIFY(torrent->getRelations().isEmpty());

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

    QVERIFY(torrentVerify->getRelations().isEmpty());

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

QTEST_MAIN(tst_BaseModel_Relations)

#include "tst_basemodel_relations.moc"
